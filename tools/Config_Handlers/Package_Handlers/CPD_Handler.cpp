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

      CID_Handler::component_impl_descr (desc.referencedImplementation (),
                         toconfig.referencedImplementation);
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

      typedef Sequence_Handler < PackagedComponentImplementation,
                 ::Deployment::PackagedComponentImplementations,
                 ::Deployment::PackagedComponentImplementation,
                 PCI_Handler::handle_pci > PCI_Functor;


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
          Comp_Intf_Descr_Handler::comp_intf_descr (*realizes_if_set_ptr,
                                                    toconfig.realizes);

        // Config Properties
        toconfig.configProperty.length (desc.configProperty_values ().size ());
        std::for_each (cpd->infoProperty_values ().begin (),
                       cpd->infoProperty_values ().end (),
                       Property_Functor (toconfig.configProperty));

        // Packaged Component Implementations
        toconfig.implementation.length ( cpd->implementation_values ().size ());
        SEQ_HAND_GCC_BUG_WORKAROUND (PCI_Handler::handle_pci,
                                     (*cpd->implementation_values ().begin ()),
                                     toconfig.implementation);
        std::for_each (cpd->implementation_values ().begin (),
                       cpd->implementation_values ().end (),
                       PCI_Functor (toconfig.implementation));

        // Info Properties
        toconfig.infoProperty.length (cpd->infoProperty_values ().size ());
        std::for_each (cpd->infoProperty_values ().begin (),
                       cpd->infoProperty_values ().end (),
                       Property_Functor (toconfig.infoProperty));
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
            toconfig.add_configProperty (
                     Property_Handler::get_property (src.configProperty[i]));
          }

        { // Packaged Component Implementations
          for (size_t i = 0; i < src.implementation.length (); ++i)
            toconfig.add_implementation (
                     PCI_Handler::get_pci (src.implementation[i]));
        }

        for (size_t i = 0; i < src.infoProperty.length (); ++i)
          {
            toconfig.add_infoProperty (
                       Property_Handler::get_property (src.infoProperty[i]));
          }
#endif
        return toconfig;
      }

      ComponentPackageDescription * CPD_Handler::resolve_cpd (
        const ACE_TCHAR *uri)
      {
        DANCE_TRACE ("CPD_Handler::resolve_cpd");
        XML::XML_Helper<> xml_helper;
        if (!xml_helper.is_initialized ())
          return 0;

        xercesc::DOMDocument* dom =
          xml_helper.create_dom (uri);

        if (!dom)
          throw Parse_Error ( ACE_TEXT (
            "Unable to create DOM for component package description"));

        try {
          return new ComponentPackageDescription (reader::componentPackageDescription (dom));
        }
        catch (...) {
          throw Parse_Error (ACE_TEXT (
            "Unable to create XSC structure for CID"));
        }
      }
    }
  }
}
