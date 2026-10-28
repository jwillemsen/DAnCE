#include "tao/AnyTypeCode/AnyTypeCode_methods.h"
#include "Package_Handlers/SID_Handler.h"
#include "cid.hpp"
#include "dance/Deployment/DeploymentC.h"
#include "Package_Handlers/CPD_Handler.h"
#include "Package_Handlers/PCD_Handler.h"
#include "Package_Handlers/Comp_Intf_Descr_Handler.h"
#include "Req_Handler.h"
#include "Property_Handler.h"
#include "Utils/Exceptions.h"

#include "ace/UUID.h"

namespace DAnCE
{
  namespace Config_Handlers
  {
    namespace Packaging
    {
      IDREF_Base<CORBA::ULong> SID_Handler::IDREF;

      void
      SID_Handler::handle_sub_comp_inst_descr (
        SubcomponentInstantiationDescription const& desc,
         ::Deployment::SubcomponentInstantiationDescription &toconfig)
      {
        DANCE_TRACE ("SID_Handler::sub_comp_inst_descr");
        toconfig.name = desc.name ().c_str ();

        ::DAnCE::Config_Handlers::PackageConfiguration const* specializedConfig = desc.specializedConfig_if_set ();
        ::DAnCE::Config_Handlers::ComponentPackageImport const* importedPackage = desc.importedPackage_if_set ();
        ::DAnCE::Config_Handlers::ComponentPackageReference const* referencedPackage = desc.referencedPackage_if_set ();

        ::DAnCE::Config_Handlers::ComponentPackageDescription const*
          basePackage_if_set_ptr = desc.basePackage_if_set ();
        if (basePackage_if_set_ptr)
          {
            toconfig.basePackage.length (1);
            CPD_Handler::handle_component_package_descr (*basePackage_if_set_ptr, toconfig.basePackage[0]);
          }
        else if (specializedConfig)
          {
            toconfig.specializedConfig.length (1);
            PCD_Handler::package_config (*specializedConfig, toconfig.specializedConfig[0]);
          }
        else if (importedPackage)
          {
            toconfig.importedPackage.length (1);
            toconfig.importedPackage[0].location.length (importedPackage->location ().size ());

            CORBA::ULong location_index {};
            for (auto const& source_item : importedPackage->location ())
              toconfig.importedPackage[0].location[location_index++] = source_item.c_str ();
          }
        else if (referencedPackage)
          {
            toconfig.referencedPackage.length (1);

            ::XMLSchema::string<char> const* requiredUUID = referencedPackage->requiredUUID_if_set ();
            if (requiredUUID)
              toconfig.referencedPackage[0].requiredUUID = requiredUUID->c_str ();

            ::XMLSchema::string<char> const* requiredName = referencedPackage->requiredName_if_set ();
            if (requiredName)
              toconfig.referencedPackage[0].requiredName = requiredName->c_str ();

            Comp_Intf_Descr_Handler::comp_intf_descr (
              referencedPackage->requiredType (),
              toconfig.referencedPackage[0].requiredType);

          }

        toconfig.selectRequirement.length (desc.selectRequirement ().size ());
        CORBA::ULong selectRequirement_index {};
        for (auto const& source_item : desc.selectRequirement ())
          Req_Handler::handle_requirement (source_item, toconfig.selectRequirement[selectRequirement_index++]);

        toconfig.configProperty.length (desc.configProperty ().size ());
        CORBA::ULong configProperty_index {};
        for (auto const& source_item : desc.configProperty ())
          Property_Handler::handle_property (source_item, toconfig.configProperty[configProperty_index++]);

        ::XMLSchema::ID<char> const* id_if_set_ptr = desc.id_if_set ();
        if (id_if_set_ptr)
          {
            std::string str (id_if_set_ptr->c_str ());
            SID_Handler::IDREF.bind_next_available (str);
          }
        else
          DANCE_ERROR (DANCE_LOG_WARNING,
            (LM_WARNING, ACE_TEXT ("Warning: SID With Name %C has no ID\n"),
             desc.name ().c_str ()));

      }

      SubcomponentInstantiationDescription
      SID_Handler::sub_comp_inst_descr (
        Deployment::SubcomponentInstantiationDescription const& src)
      {
        DANCE_TRACE ("SID_Handler::sub_comp_inst_descr - reverse");
        XMLSchema::string<char> name (src.name.in ());

        // @@MAJO This is not a good way of binding reverse IDREFS.
        std::unique_ptr<ACE_Utils::UUID> safe_uuid (ACE_Utils::UUID_GENERATOR::instance ()->generate_UUID ());
        std::string uuid (safe_uuid->to_string ()->c_str ());

        SID_Handler::IDREF.bind_next_available (uuid);

        XMLSchema::ID<char> id (uuid.c_str ());
        XSCRT::OptionalArg<XMLSchema::ID<char>> id_arg (id);

        SubcomponentInstantiationDescription::selectRequirement_container_type select_requirements;
        for (CORBA::ULong i {}; i < src.selectRequirement.length (); ++i)
          {
            Requirement const requirement (Req_Handler::get_requirement (src.selectRequirement[i]));
            select_requirements.push_back (requirement);
          }

        SubcomponentInstantiationDescription::configProperty_container_type config_properties;
        for (CORBA::ULong i {}; i < src.configProperty.length (); ++i)
          {
            Property const property (Property_Handler::get_property (src.configProperty[i]));
            config_properties.push_back (property);
          }

        CORBA::ULong package_kind_count {};
        package_kind_count += src.basePackage.length ();
        package_kind_count += src.specializedConfig.length ();
        package_kind_count += src.importedPackage.length ();
        package_kind_count += src.referencedPackage.length ();
        if (package_kind_count != 1)
          throw Plan_Error ("SubcomponentInstantiationDescription must contain exactly one package kind");

        if (src.basePackage.length () == 1)
          return SubcomponentInstantiationDescription (
            name, CPD_Handler::component_package_descr (src.basePackage[0]), std::nullopt,
            select_requirements, config_properties, std::nullopt, std::nullopt, id_arg);

        if (src.specializedConfig.length () == 1)
          return SubcomponentInstantiationDescription (
            name, std::nullopt, PCD_Handler::package_config (src.specializedConfig[0]),
            select_requirements, config_properties, std::nullopt, std::nullopt, id_arg);

        if (src.importedPackage.length () == 1)
          {
            ComponentPackageImport::location_container_type locations;
            for (CORBA::ULong i {}; i < src.importedPackage[0].location.length (); ++i)
              locations.emplace_back (src.importedPackage[0].location[i]);
            ComponentPackageImport const imported_package (locations);
            return SubcomponentInstantiationDescription (
              name, std::nullopt, std::nullopt, select_requirements, config_properties,
              std::nullopt, imported_package, id_arg);
          }

        if (src.referencedPackage.length () == 1)
          {
            Deployment::ComponentPackageReference const& package_reference = src.referencedPackage[0];
            ComponentInterfaceDescription required_type (
              Comp_Intf_Descr_Handler::comp_intf_descr (package_reference.requiredType));
            XMLSchema::string<char> required_uuid_value (
              package_reference.requiredUUID.in () == nullptr ? "" : package_reference.requiredUUID.in ());
            XMLSchema::string<char> required_name_value (
              package_reference.requiredName.in () == nullptr ? "" : package_reference.requiredName.in ());
            XSCRT::OptionalArg<XMLSchema::string<char>> required_uuid (std::nullopt);
            XSCRT::OptionalArg<XMLSchema::string<char>> required_name (std::nullopt);
            if (package_reference.requiredUUID.in () != nullptr)
              required_uuid = XSCRT::OptionalArg<XMLSchema::string<char>> (required_uuid_value);
            if (package_reference.requiredName.in () != nullptr)
              required_name = XSCRT::OptionalArg<XMLSchema::string<char>> (required_name_value);
            ComponentPackageReference const referenced_package (required_type, required_uuid, required_name);
            return SubcomponentInstantiationDescription (
              name, std::nullopt, std::nullopt, select_requirements, config_properties,
              referenced_package, std::nullopt, id_arg);
          }

        return SubcomponentInstantiationDescription (
          name, std::nullopt, std::nullopt, select_requirements, config_properties,
          std::nullopt, std::nullopt, id_arg);
      }
    }
  }
}

