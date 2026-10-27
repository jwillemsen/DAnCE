#include "tao/AnyTypeCode/AnyTypeCode_methods.h"
#include "dance/Logger/Log_Macros.h"
#include "dance/Deployment/Deployment_Packaging_DataC.h"
#include "ace/XML_Utils/XMLSchema/Types.hpp"
#include "ace/XML_Utils/XML_Helper.h"
#include "ace/XML_Utils/XercesString.h"
#include "Utils/Exceptions.h"
#include "Package_Handlers/PCD_Handler.h"
#include "Package_Handlers/CPD_Handler.h"
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
        if (src.basePackage.length () > 0)
          return PackageConfiguration (label_arg, UUID_arg,
                                       CPD_Handler::component_package_descr (src.basePackage[0]));

        return PackageConfiguration (label_arg, UUID_arg);
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
