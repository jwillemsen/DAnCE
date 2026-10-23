#include "tao/AnyTypeCode/TypeCode.h"
#include "Comp_Intf_Descr_Handler.h"
#include "ace/XML_Utils/XMLSchema/Types.hpp"
#include "ace/XML_Utils/XML_Helper.h"
#include "Utils/Exceptions.h"
#include "Utils/Functors.h"
#include "DataType_Handler.h"
#include "Property_Handler.h"
#include "Deployment.hpp"
#include "Package_Handlers/CPD_Handler.h"
#include "tools/Config_Handlers/CPD_Handler.h"
#include "dance/Logger/Log_Macros.h"
namespace DAnCE
{
  namespace Config_Handlers
  {
    namespace Packaging
    {
        struct Comp_Prop_Handler
        {
          static void handle_cpd (const ComponentPropertyDescription &desc,
                               ::Deployment::ComponentPropertyDescription &toconfig)
          {
            DANCE_TRACE ("Comp_Prop_Handler::get_cpd");

            toconfig.name = desc.name ().c_str ();
            DataType_Handler::data_type (desc.type (),
                                         toconfig.type);
          }

          static ComponentPropertyDescription
          get_cpd (const ::Deployment::ComponentPropertyDescription &src)
          {
            DANCE_TRACE ("Comp_Prop_Handler::get_cpd - reverse");

            return ComponentPropertyDescription (ACE_TEXT_CHAR_TO_TCHAR (src.name.in ()),
                                                 DataType_Handler::data_type (src.type));
          }

        };

        typedef Sequence_Handler < ComponentPropertyDescription,
                                   ::Deployment::ComponentPropertyDescriptions,
                                   ::Deployment::ComponentPropertyDescription,
                                   Comp_Prop_Handler::handle_cpd > Comp_Prop_Functor;


      void
      Comp_Intf_Descr_Handler::comp_intf_descr (
                                                const ComponentInterfaceDescription &desc,
                                                ::Deployment::ComponentInterfaceDescription &toconfig)
      {
        DANCE_TRACE ("Comp_Intf_Descr_Handler::comp_intf_descr");

        const ComponentInterfaceDescription *cid = 0;
        std::auto_ptr <ComponentInterfaceDescription> safe_cid;


        ::XMLSchema::string<char> const* href_if_set_ptr = desc.href_if_set ();
        if (href_if_set_ptr)
          {
            safe_cid.reset (Comp_Intf_Descr_Handler::resolve_cid (href_if_set_ptr->c_str ()));
            cid = safe_cid.get ();
          }
        else
          cid = &desc;


        ::XMLSchema::string<char> const* label_if_set_ptr = cid->label_if_set ();
        if (label_if_set_ptr)
          toconfig.label = label_if_set_ptr->c_str ();


        ::XMLSchema::string<char> const* UUID_if_set_ptr = cid->UUID_if_set ();
        if (UUID_if_set_ptr)
          toconfig.UUID = UUID_if_set_ptr->c_str ();

        // MAJO: SpecificType should be required.
        ::XMLSchema::string<char> const* specificType_if_set_ptr = cid->specificType_if_set ();
        if (specificType_if_set_ptr)
          toconfig.specificType = specificType_if_set_ptr->c_str ();

        toconfig.supportedType.length (cid->supportedType ().size ());
        std::for_each (cid->supportedType ().begin (),
                       cid->supportedType ().end (),
                       String_Seq_Functor (toconfig.supportedType));

        toconfig.idlFile.length (cid->idlFile ().size ());
        std::for_each (cid->idlFile ().begin (),
                       cid->idlFile ().end (),
                       String_Seq_Functor (toconfig.idlFile));

        toconfig.configProperty.length (cid->configProperty ().size ());
        std::for_each (cid->configProperty ().begin (),
                       cid->configProperty ().end (),
                       Property_Functor (toconfig.configProperty));

        toconfig.port.length (cid->port ().size ());
        std::for_each (cid->port ().begin (),
                       cid->port ().end (),
                       DAnCE::Config_Handlers::CPD_Functor (toconfig.port));

        toconfig.property.length (cid->property ().size ());
        SEQ_HAND_GCC_BUG_WORKAROUND (Comp_Prop_Handler::handle_cpd,
                                     cid->property ().front (),
                                     toconfig.property);
        std::for_each (cid->property ().begin (),
                       cid->property ().end (),
                       Comp_Prop_Functor (toconfig.property));

        toconfig.infoProperty.length (cid->infoProperty ().size ());
        std::for_each (cid->infoProperty ().begin (),
                       cid->infoProperty ().end (),
                       Property_Functor (toconfig.infoProperty));
      }


      ComponentInterfaceDescription
      Comp_Intf_Descr_Handler::comp_intf_descr (
                                                const ::Deployment::ComponentInterfaceDescription &src)
      {
        DANCE_TRACE ("Comp_Intf_Descr_Handler::comp_intf_descr - reverse");
        ComponentInterfaceDescription retval;

        retval.label (ACE_TEXT_CHAR_TO_TCHAR (src.label.in ()));
        retval.UUID (ACE_TEXT_CHAR_TO_TCHAR  (src.UUID.in ()));
        retval.specificType (ACE_TEXT_CHAR_TO_TCHAR  (src.specificType.in ()));
#if 0
        for (CORBA::ULong i = 0; i < src.supportedType.length (); ++i)
          retval.add_supportedType (src.supportedType[i].in ());

        for (CORBA::ULong i = 0; i < src.idlFile.length (); ++i)
          retval.add_idlFile (src.idlFile[i].in ());

        for (CORBA::ULong i = 0; i < src.configProperty.length (); ++i)
          retval.add_configProperty (
                                     Property_Handler::get_property (src.configProperty[i]));

        for (CORBA::ULong i = 0; i < src.port.length (); ++i)
          retval.add_port
            (DAnCE::Config_Handlers::CPD_Handler::component_port_description (src.port[i]));

        for (CORBA::ULong i = 0; i < src.property.length (); ++i)
          retval.add_property (Comp_Prop_Handler::get_cpd (src.property[i]));

        for (CORBA::ULong i = 0; i < src.infoProperty.length (); ++i)
          retval.add_infoProperty (
                                   Property_Handler::get_property (src.infoProperty[i]));
#endif
        return retval;
      }

      ComponentInterfaceDescription *
      Comp_Intf_Descr_Handler::resolve_cid (const char *uri)
      {
        DANCE_TRACE ("Comp_Intf_Descr_Handler::resolve_cid");

        XML::XML_Helper<> xml_helper;
        xercesc::DOMDocument *dom = xml_helper.create_dom (ACE_TEXT_CHAR_TO_TCHAR (uri));

        if (!dom)
          throw Parse_Error ("Unable to create DOM for Component Interface Description.");

        try {
          return new ComponentInterfaceDescription
            (reader::componentInterfaceDescription (dom));
        }
        catch (...) {
          throw Parse_Error ("Unable to create XSC structure for Component Interface Description");
        }
      }
    }
  }}
