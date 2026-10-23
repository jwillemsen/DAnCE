#include "tao/AnyTypeCode/AnyTypeCode_methods.h"
#include "Package_Handlers/CPD_Handler.h"
#include "Package_Handlers/CID_Handler.h"
#include "Package_Handlers/Comp_Intf_Descr_Handler.h"
#include "Basic_Deployment_Data.hpp"
#include "dance/Deployment/Deployment_Packaging_DataC.h"
#include "Deployment.hpp"
#include "Utils/Exceptions.h"
#include "ace/XML_Utils/XML_Helper.h"
#include "Property_Handler.h"
#include <string>

namespace DAnCE
{
  namespace Config_Handlers
  {
    namespace Packaging
    {
      struct Packaging_Handlers_Export PCI_Handler
      {
    static void handle_pci (const PackagedComponentImplementation &desc,
                ::Deployment::PackagedComponentImplementation &toconfig)
    {
      DANCE_TRACE ("PCI_Handler::get_pci");
      toconfig.name = desc.name ().c_str ();

      CID_Handler::component_impl_descr (desc.referencedImplementation (), toconfig.referencedImplementation);
    }

    static PackagedComponentImplementation
    get_pci (const ::Deployment::PackagedComponentImplementation &src)
    {
      DANCE_TRACE ("PCI_Handler::get_pci - reverse");
      return PackagedComponentImplementation (
        ACE_TEXT_CHAR_TO_TCHAR (src.name.in ()),
        CID_Handler::component_impl_descr (src.referencedImplementation));
    }
      };

      void
      CPD_Handler::handle_component_package_descr (
        const ComponentPackageDescription &desc,
        ::Deployment::ComponentPackageDescription &toconfig)
      {
        DANCE_TRACE ("CPD_Handler::component_package_descr");

        std::unique_ptr<ComponentPackageDescription> xsc_cpd;
        const ComponentPackageDescription *cpd = 0;

        ::XMLSchema::string<char> const* href_if_set_ptr = desc.href_if_set ();
        if (href_if_set_ptr)
          {
            xsc_cpd.reset (CPD_Handler::resolve_cpd (href_if_set_ptr->c_str ()));
            cpd = xsc_cpd.get ();
          }
        else
          cpd = &desc;

        ::XMLSchema::string<char> const* label_if_set_ptr = cpd->label_if_set ();
        if (label_if_set_ptr)
          toconfig.label = label_if_set_ptr->c_str ();

        ::XMLSchema::string<char> const* UUID_if_set_ptr = cpd->UUID_if_set ();
        if (UUID_if_set_ptr)
          toconfig.UUID = UUID_if_set_ptr->c_str ();

        // CID
        ::DAnCE::Config_Handlers::ComponentInterfaceDescription const* realizes_if_set_ptr = cpd->realizes_if_set ();
        if (realizes_if_set_ptr)
          Comp_Intf_Descr_Handler::comp_intf_descr (*realizes_if_set_ptr, toconfig.realizes);

        // Config Properties
        toconfig.configProperty.length (cpd->configProperty ().size ());
        CORBA::ULong configProperty_index {};
        for (auto const& source_item : cpd->configProperty ())
          Property_Handler::handle_property (source_item, toconfig.configProperty[configProperty_index++]);

        // Packaged Component Implementations
        toconfig.implementation.length ( cpd->implementation ().size ());
        CORBA::ULong implementation_index {};
        for (auto const& source_item : cpd->implementation ())
          PCI_Handler::handle_pci (source_item, toconfig.implementation[implementation_index++]);

        // Info Properties
        toconfig.infoProperty.length (cpd->infoProperty ().size ());
        CORBA::ULong infoProperty_index {};
        for (auto const& source_item : cpd->infoProperty ())
          Property_Handler::handle_property (source_item, toconfig.infoProperty[infoProperty_index++]);
      }

      ComponentPackageDescription
      CPD_Handler::component_package_descr (
        const Deployment::ComponentPackageDescription& src)
      {
        DANCE_TRACE ("CPD_Handler::component_package_descr - reverse");
        ComponentPackageDescription toconfig;

        if (src.label.in () != 0)
          toconfig.label (ACE_TEXT_CHAR_TO_TCHAR (src.label.in ()));

        if (src.UUID.in () != 0)
          toconfig.UUID (ACE_TEXT_CHAR_TO_TCHAR (src.UUID.in ()));

        {
          toconfig.realizes
            (Comp_Intf_Descr_Handler::comp_intf_descr (src.realizes));
        }
#if 0
        for (size_t i = 0; i < src.configProperty.length (); ++i)
          {
            toconfig.add_configProperty ( Property_Handler::get_property (src.configProperty[i]));
          }

        { // Packaged Component Implementations
          for (size_t i = 0; i < src.implementation.length (); ++i)
            toconfig.add_implementation ( PCI_Handler::get_pci (src.implementation[i]));
        }

        for (size_t i = 0; i < src.infoProperty.length (); ++i)
          {
            toconfig.add_infoProperty ( Property_Handler::get_property (src.infoProperty[i]));
          }
#endif
        return toconfig;
      }

      ComponentPackageDescription * CPD_Handler::resolve_cpd (
        std::string_view uri)
      {
        DANCE_TRACE ("CPD_Handler::resolve_cpd");
        XML::XML_Helper<> xml_helper;
        if (!xml_helper.is_initialized ())
          return 0;

        const std::string uri_string (uri);
        xercesc::DOMDocument* dom = xml_helper.create_dom (ACE_TEXT_CHAR_TO_TCHAR (uri_string.c_str()));

        if (!dom)
          throw Parse_Error ( "Unable to create DOM for component package description");

        try {
          return new ComponentPackageDescription (reader::componentPackageDescription (dom));
        }
        catch (...) {
          throw Parse_Error ("Unable to create XSC structure for CID");
        }
      }
    }
  }
}
