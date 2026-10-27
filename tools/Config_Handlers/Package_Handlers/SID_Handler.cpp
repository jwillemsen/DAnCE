#include "tao/AnyTypeCode/AnyTypeCode_methods.h"
#include "Package_Handlers/SID_Handler.h"
#include "cid.hpp"
#include "dance/Deployment/DeploymentC.h"
#include "Package_Handlers/CPD_Handler.h"
#include "Package_Handlers/PCD_Handler.h"
#include "Package_Handlers/Comp_Intf_Descr_Handler.h"
#include "Req_Handler.h"
#include "Property_Handler.h"

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

        if (src.basePackage.length () == 1)
          return SubcomponentInstantiationDescription (
            name, CPD_Handler::component_package_descr (src.basePackage[0]));

        if (src.specializedConfig.length () == 1)
          return SubcomponentInstantiationDescription (
            name, std::nullopt, PCD_Handler::package_config (src.specializedConfig[0]));

        if (src.importedPackage.length () == 1)
          {
            ComponentPackageImport const imported_package {};
            return SubcomponentInstantiationDescription (
              name, std::nullopt, std::nullopt, {}, {}, std::nullopt, imported_package);
          }

        if (src.referencedPackage.length () == 1)
          {
            Deployment::ComponentPackageReference const& package_reference = src.referencedPackage[0];
            ComponentInterfaceDescription required_type (
              Comp_Intf_Descr_Handler::comp_intf_descr (package_reference.requiredType));
            XMLSchema::string<char> required_uuid (package_reference.requiredUUID.in ());
            XMLSchema::string<char> required_name (package_reference.requiredName.in ());
            ComponentPackageReference const referenced_package (
              required_type, required_uuid, required_name);
            return SubcomponentInstantiationDescription (
              name, std::nullopt, std::nullopt, {}, {}, referenced_package);
          }

        return SubcomponentInstantiationDescription (name);
      }
    }
  }
}

