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
      void PCD_Handler::package_config (const ACE_TCHAR *uri, ::Deployment::PackageConfiguration &toconfig)
      {
        XML::XML_Helper<> xml_helper;
        XERCES_CPP_NAMESPACE::DOMDocument *dom = xml_helper.create_dom (uri);

        if (dom == 0)
          {
            std::string error ("Unable to open file: ");
            error += ACE_TEXT_ALWAYS_CHAR (uri);
            throw Parse_Error (error);
          }

        XML::XStr root = dom->getDocumentElement ()->getTagName ();

        if (root == XML::XStr (
                    ACE_TEXT ("Deployment:topLevelPackageDescription")))
          {
            TopLevelPackageDescription tpd;

            tpd = reader::topLevelPackageDescription (dom);

            if (tpd.package ().empty ())
              throw Plan_Error ("Top-level package description contains no package");

            PCD_Handler::package_config (tpd.package ().front (), toconfig);
          }
        else if (root == XML::XStr (ACE_TEXT ("Deployment:packageConfiguration")))
          {
            PackageConfiguration pcd;
            pcd = reader::packageConfiguration (dom);
            PCD_Handler::package_config (pcd, toconfig);
          }
        else
          {
            std::string error ( "Invalid file passed to package_config, has base ");

            char *croot = xercesc::XMLString::transcode (root);

            error += croot;

            delete [] croot;

            throw Plan_Error ("Invalid file passed to package_config, had base");
          }
      }

      void PCD_Handler::package_config (const PackageConfiguration &desc, ::Deployment::PackageConfiguration &toconfig)
      {
        DANCE_TRACE ("PCD_Handler::package_config");
        std::unique_ptr<PackageConfiguration> xsc_pcd;

        const PackageConfiguration *pcd;

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

      PackageConfiguration PCD_Handler::package_config (const Deployment::PackageConfiguration& src)
      {
        DANCE_TRACE ("PCD_Handler::package_config - reverse");
        PackageConfiguration pcd = PackageConfiguration ();

        if (src.label.in () != 0)
          pcd.label (ACE_TEXT_CHAR_TO_TCHAR (src.label.in ()));

        if (src.UUID.in () != 0)
          pcd.UUID (ACE_TEXT_CHAR_TO_TCHAR (src.UUID.in ()));

        if (src.basePackage.length () > 0)
          {
            pcd.basePackage (CPD_Handler::component_package_descr (src.basePackage[0]));
          }
#if 0
        // @@ MAJO: Support other elements present here.
        for (CORBA::ULong i = 0; i < src.configProperty.length (); ++i)
          pcd.add_configProperty (Property_Handler::get_property (src.configProperty[i]));

        for (CORBA::ULong i = 0; i < src.selectRequirement.length (); ++i)
          pcd.add_selectRequirement (Req_Handler::get_requirement (src.selectRequirement[i]));
#endif
        return pcd;

      }

      PackageConfiguration * PCD_Handler::resolve_package_config (std::string_view uri)
      {
        XML::XML_Helper<> xml_helper;
        const std::string uri_string (uri);
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
