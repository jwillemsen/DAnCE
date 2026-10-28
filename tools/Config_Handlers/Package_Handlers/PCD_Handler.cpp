#include "tao/AnyTypeCode/AnyTypeCode_methods.h"
#include "dance/Logger/Log_Macros.h"
#include "dance/Deployment/Deployment_Packaging_DataC.h"
#include "ace/XML_Utils/XMLSchema/Types.hpp"
#include "ace/XML_Utils/XML_Helper.h"
#include "ace/XML_Utils/XercesString.h"
#include "Utils/Exceptions.h"
#include "Package_Handlers/PCD_Handler.h"
#include "Package_Handlers/CPD_Handler.h"
#include "Package_Handlers/Comp_Intf_Descr_Handler.h"
#include "toplevel.hpp"
#include "Deployment.hpp"
#include "Property_Handler.h"
#include "Req_Handler.h"

#include <memory>
#include <string>

namespace DAnCE
{
  namespace Config_Handlers
  {
    namespace Packaging
    {
      void PCD_Handler::package_config (ACE_TCHAR const* uri, ::Deployment::PackageConfiguration &toconfig)
      {
        XML::XML_Helper<> xml_helper {};
        XERCES_CPP_NAMESPACE::DOMDocument *dom = xml_helper.create_dom (uri);

        if (dom == 0)
          {
            std::string error ("Unable to open file: ");
            error += ACE_TEXT_ALWAYS_CHAR (uri);
            throw Parse_Error (error);
          }

        XML::XStr const root = dom->getDocumentElement ()->getTagName ();

        if (root == XML::XStr (
                    ACE_TEXT ("Deployment:topLevelPackageDescription")))
          {
            TopLevelPackageDescription tpd (reader::topLevelPackageDescription (dom));

            if (tpd.package ().empty ())
              throw Plan_Error ("Top-level package description contains no package");

            PCD_Handler::package_config (tpd.package ().front (), toconfig);
          }
        else if (root == XML::XStr (ACE_TEXT ("Deployment:packageConfiguration")))
          {
            PackageConfiguration pcd (reader::packageConfiguration (dom));
            PCD_Handler::package_config (pcd, toconfig);
          }
        else
          {
            std::string error ("Invalid file passed to package_config, has base ");

            char *croot = xercesc::XMLString::transcode (root);

            error += croot;

            delete [] croot;

            throw Plan_Error (error);
          }
      }

      void PCD_Handler::package_config (PackageConfiguration const& desc, ::Deployment::PackageConfiguration &toconfig)
      {
        DANCE_TRACE ("PCD_Handler::package_config");
        std::unique_ptr<PackageConfiguration> xsc_pcd {};

        PackageConfiguration const* pcd {};

        ::XMLSchema::string<char> const* contentLocation_if_set_ptr = desc.contentLocation_if_set ();
        if (contentLocation_if_set_ptr)
          {
            // Take ownership of the resolved pcd
            xsc_pcd.reset (PCD_Handler::resolve_package_config (contentLocation_if_set_ptr->c_str ()));
            pcd = xsc_pcd.get ();
          }
        else
          pcd = &desc;

        ::XMLSchema::string<char> const* label_if_set_ptr = pcd->label_if_set ();
        if (label_if_set_ptr)
          toconfig.label = label_if_set_ptr->c_str ();

        ::XMLSchema::string<char> const* UUID_if_set_ptr = pcd->UUID_if_set ();
        if (UUID_if_set_ptr)
          toconfig.UUID = UUID_if_set_ptr->c_str ();

        ::DAnCE::Config_Handlers::ComponentPackageDescription const*
          basePackage_if_set_ptr = pcd->basePackage_if_set ();
        if (basePackage_if_set_ptr)
          {
            toconfig.basePackage.length (1);
            CPD_Handler::handle_component_package_descr (*basePackage_if_set_ptr, toconfig.basePackage [0]);
          }

        // @@ MAJO: Support other elements present here.

        toconfig.configProperty.length (pcd->configProperty ().size ());
        CORBA::ULong configProperty_index {};
        for (auto const& source_item : pcd->configProperty ())
          Property_Handler::handle_property (source_item, toconfig.configProperty[configProperty_index++]);

        toconfig.selectRequirement.length (pcd->selectRequirement ().size ());
        CORBA::ULong selectRequirement_index {};
        for (auto const& source_item : pcd->selectRequirement ())
          Req_Handler::handle_requirement (source_item, toconfig.selectRequirement[selectRequirement_index++]);

      }

      PackageConfiguration PCD_Handler::package_config (Deployment::PackageConfiguration const& src)
      {
        DANCE_TRACE ("PCD_Handler::package_config - reverse");
        XMLSchema::string<char> label (src.label.in () == nullptr ? "" : src.label.in ());
        XMLSchema::string<char> UUID (src.UUID.in () == nullptr ? "" : src.UUID.in ());
        XSCRT::OptionalArg<XMLSchema::string<char>> label_arg (std::nullopt);
        XSCRT::OptionalArg<XMLSchema::string<char>> UUID_arg (std::nullopt);
        if (src.label.in () != nullptr)
          label_arg = XSCRT::OptionalArg<XMLSchema::string<char>> (label);
        if (src.UUID.in () != nullptr)
          UUID_arg = XSCRT::OptionalArg<XMLSchema::string<char>> (UUID);

        XSCRT::OptionalArg<ComponentPackageDescription> base_package (std::nullopt);
        if (src.basePackage.length () > 1 || src.specializedConfig.length () > 1 ||
            src.importedPackage.length () > 1 || src.referencedPackage.length () > 1)
          throw Plan_Error ("PackageConfiguration has multiple values for a single-valued field");
        if (src.basePackage.length () == 1)
          {
            ComponentPackageDescription const value (
              CPD_Handler::component_package_descr (src.basePackage[0]));
            base_package = XSCRT::OptionalArg<ComponentPackageDescription> (value);
          }

        XSCRT::OptionalArg<PackageConfiguration> specialized_config (std::nullopt);
        if (src.specializedConfig.length () == 1)
          {
            PackageConfiguration const value (PCD_Handler::package_config (src.specializedConfig[0]));
            specialized_config = XSCRT::OptionalArg<PackageConfiguration> (value);
          }

        XSCRT::OptionalArg<ComponentPackageImport> imported_package (std::nullopt);
        if (src.importedPackage.length () == 1)
          {
            ComponentPackageImport::location_container_type locations;
            for (CORBA::ULong i {}; i < src.importedPackage[0].location.length (); ++i)
              locations.emplace_back (src.importedPackage[0].location[i]);
            ComponentPackageImport const value (locations);
            imported_package = XSCRT::OptionalArg<ComponentPackageImport> (value);
          }

        XSCRT::OptionalArg<ComponentPackageReference> referenced_package (std::nullopt);
        if (src.referencedPackage.length () == 1)
          {
            ::Deployment::ComponentPackageReference const& package_reference = src.referencedPackage[0];
            ComponentInterfaceDescription const required_type (
              Comp_Intf_Descr_Handler::comp_intf_descr (package_reference.requiredType));
            XMLSchema::string<char> const required_uuid (
              package_reference.requiredUUID.in () == nullptr ? "" : package_reference.requiredUUID.in ());
            XMLSchema::string<char> const required_name (
              package_reference.requiredName.in () == nullptr ? "" : package_reference.requiredName.in ());
            XSCRT::OptionalArg<XMLSchema::string<char>> required_uuid_arg (std::nullopt);
            XSCRT::OptionalArg<XMLSchema::string<char>> required_name_arg (std::nullopt);
            if (package_reference.requiredUUID.in () != nullptr)
              required_uuid_arg = XSCRT::OptionalArg<XMLSchema::string<char>> (required_uuid);
            if (package_reference.requiredName.in () != nullptr)
              required_name_arg = XSCRT::OptionalArg<XMLSchema::string<char>> (required_name);
            ComponentPackageReference const value (required_type, required_uuid_arg, required_name_arg);
            referenced_package = XSCRT::OptionalArg<ComponentPackageReference> (value);
          }

        PackageConfiguration::selectRequirement_container_type select_requirements;
        for (CORBA::ULong i {}; i < src.selectRequirement.length (); ++i)
          {
            Requirement const requirement (Req_Handler::get_requirement (src.selectRequirement[i]));
            select_requirements.push_back (requirement);
          }

        PackageConfiguration::configProperty_container_type config_properties;
        for (CORBA::ULong i {}; i < src.configProperty.length (); ++i)
          {
            Property const property (Property_Handler::get_property (src.configProperty[i]));
            config_properties.push_back (property);
          }

        return PackageConfiguration (label_arg, UUID_arg, base_package, specialized_config,
                                     imported_package, referenced_package,
                                     select_requirements, config_properties);
      }

      PackageConfiguration * PCD_Handler::resolve_package_config (std::string_view uri)
      {
        XML::XML_Helper<> xml_helper {};
        std::string const uri_string (uri);
        xercesc::DOMDocument* dom = xml_helper.create_dom (ACE_TEXT_CHAR_TO_TCHAR (uri_string.c_str()));

        if (!dom)
          throw Parse_Error ("Unable to create DOM for PackageConfiguration");

        try {
          return new PackageConfiguration (reader::packageConfiguration (dom));
        }
        catch (...) {
          throw Parse_Error ("Unable to create XSC structure for PackageConfiguration");
        }

      }

    }
  }
}
