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
        const SubcomponentInstantiationDescription &desc,
         ::Deployment::SubcomponentInstantiationDescription &toconfig)
      {
        DANCE_TRACE ("SID_Handler::sub_comp_inst_descr");
        toconfig.name = desc.name ().c_str ();

        ::DAnCE::Config_Handlers::PackageConfiguration const* specializedConfig =
          desc.specializedConfig_if_set ();
        ::DAnCE::Config_Handlers::ComponentPackageImport const* importedPackage =
          desc.importedPackage_if_set ();
        ::DAnCE::Config_Handlers::ComponentPackageReference const* referencedPackage =
          desc.referencedPackage_if_set ();

        ::DAnCE::Config_Handlers::ComponentPackageDescription const*
          basePackage_if_set_ptr = desc.basePackage_if_set ();
        if (basePackage_if_set_ptr)
          {
            toconfig.basePackage.length (1);
            CPD_Handler::handle_component_package_descr (*basePackage_if_set_ptr,
                                                  toconfig.basePackage[0]);
          }
        else if (specializedConfig)
          {
            toconfig.specializedConfig.length (1);
            PCD_Handler::package_config (*specializedConfig,
                                         toconfig.specializedConfig[0]);
          }
        else if (importedPackage)
          {
            toconfig.importedPackage.length (1);
            toconfig.importedPackage[0].location.length (
              importedPackage->location ().size ());

            std::for_each (importedPackage->location ().begin (),
                           importedPackage->location ().end (),
                           String_Seq_Functor (
                             toconfig.importedPackage[0].location));
          }
        else if (referencedPackage)
          {
            toconfig.referencedPackage.length (1);

            ::XMLSchema::string<char> const* requiredUUID =
              referencedPackage->requiredUUID_if_set ();
            if (requiredUUID)
              toconfig.referencedPackage[0].requiredUUID =
                requiredUUID->c_str ();

            ::XMLSchema::string<char> const* requiredName =
              referencedPackage->requiredName_if_set ();
            if (requiredName)
              toconfig.referencedPackage[0].requiredName =
                requiredName->c_str ();

            Comp_Intf_Descr_Handler::comp_intf_descr (
              referencedPackage->requiredType (),
              toconfig.referencedPackage[0].requiredType);

          }

        toconfig.selectRequirement.length (desc.selectRequirement ().size ());
        std::for_each (desc.selectRequirement ().begin (),
                       desc.selectRequirement ().end (),
                       Requirement_Functor (toconfig.selectRequirement));

        toconfig.configProperty.length (desc.configProperty ().size ());
        std::for_each (desc.configProperty ().begin (),
                       desc.configProperty ().end (),
                       Property_Functor (toconfig.configProperty));


        ::XMLSchema::ID<char> const* id_if_set_ptr = desc.id_if_set ();
        if (id_if_set_ptr)
          {
            std::string str (id_if_set_ptr->c_str ());
            SID_Handler::IDREF.bind_next_available (str);
          }
        else
          DANCE_ERROR (DANCE_LOG_WARNING,
            (LM_WARNING, ACE_TEXT ("Warning: SID With Name %s has no ID\n"),
             desc.name ().c_str ()));

      }

      SubcomponentInstantiationDescription
      SID_Handler::sub_comp_inst_descr (
        const Deployment::SubcomponentInstantiationDescription &src)
      {
        DANCE_TRACE ("SID_Handler::sub_comp_inst_descr - reverse");
        SubcomponentInstantiationDescription retval (
          ACE_TEXT_CHAR_TO_TCHAR (src.name.in ()));

        if (src.basePackage.length () == 1)
          retval.basePackage
            (CPD_Handler::component_package_descr (src.basePackage[0]));
        else if (src.specializedConfig.length () == 1)
          retval.specializedConfig
            (PCD_Handler::package_config (src.specializedConfig[0]));
        else if (src.importedPackage.length () == 1)
          {
            ComponentPackageImport ci;
#if 0
            for (CORBA::ULong i = 0;
              i < src.importedPackage[0].location.length (); ++i)
              ci.add_location (src.importedPackage[0].location[i].in ());
#endif
            retval.importedPackage (ci);
          }
        else if (src.referencedPackage.length () == 1)
          {
            ComponentPackageReference cpr (
              Comp_Intf_Descr_Handler::comp_intf_descr (
                src.referencedPackage[0].requiredType));
            cpr.requiredUUID (ACE_TEXT_CHAR_TO_TCHAR (
              src.referencedPackage[0].requiredUUID.in ()));
            cpr.requiredName (ACE_TEXT_CHAR_TO_TCHAR (
              src.referencedPackage[0].requiredName.in ()));

            retval.referencedPackage (cpr);
          }
#if 0
        for (CORBA::ULong i = 0; i < src.selectRequirement.length (); ++i)
          retval.add_selectRequirement
            (Req_Handler::get_requirement (src.selectRequirement[i]));

        for (CORBA::ULong i = 0; i < src.configProperty.length (); ++i)
          retval.add_configProperty
            (Property_Handler::get_property (src.configProperty[i]));
#endif
        // @@MAJO This is not a good way of binding reverse IDREFS.
        std::unique_ptr<ACE_Utils::UUID> safe_uuid (
          ACE_Utils::UUID_GENERATOR::instance ()->generate_UUID ());
        std::string uuid ( safe_uuid->to_string ()->c_str ());

        SID_Handler::IDREF.bind_next_available (uuid);

        return retval;
      }
    }
  }
}

