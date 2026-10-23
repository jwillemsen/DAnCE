#include "tao/AnyTypeCode/TypeCode.h"
#include "Comp_Intf_Descr_Handler.h"
#include "ace/XML_Utils/XMLSchema/Types.hpp"
#include "ace/XML_Utils/XML_Helper.h"
#include "Utils/Exceptions.h"
#include "DataType_Handler.h"
#include "Property_Handler.h"
#include "Deployment.hpp"
#include "Package_Handlers/CPD_Handler.h"
#include "tools/Config_Handlers/CPD_Handler.h"
#include "dance/Logger/Log_Macros.h"
#include <string>
namespace DAnCE
{
  namespace Config_Handlers
  {
    namespace Packaging
    {
        struct Comp_Prop_Handler
        {
          static void handle_cpd (ComponentPropertyDescription const& desc,
                               ::Deployment::ComponentPropertyDescription &toconfig)
          {
            DANCE_TRACE ("Comp_Prop_Handler::get_cpd");

            toconfig.name = desc.name ().c_str ();
            DataType_Handler::data_type (desc.type (), toconfig.type);
          }

          static ComponentPropertyDescription
          get_cpd (::Deployment::ComponentPropertyDescription const& src)
          {
            DANCE_TRACE ("Comp_Prop_Handler::get_cpd - reverse");

            return ComponentPropertyDescription (ACE_TEXT_CHAR_TO_TCHAR (src.name.in ()),
                                                 DataType_Handler::data_type (src.type));
          }

        };

      void
      Comp_Intf_Descr_Handler::comp_intf_descr (
                                                ComponentInterfaceDescription const& desc,
                                                ::Deployment::ComponentInterfaceDescription &toconfig)
      {
        DANCE_TRACE ("Comp_Intf_Descr_Handler::comp_intf_descr");

        ComponentInterfaceDescription const* cid {};
        std::unique_ptr<ComponentInterfaceDescription> safe_cid {};

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
        CORBA::ULong supportedType_index {};
        for (auto const& source_item : cid->supportedType ())
          toconfig.supportedType[supportedType_index++] = source_item.c_str ();

        toconfig.idlFile.length (cid->idlFile ().size ());
        CORBA::ULong idlFile_index {};
        for (auto const& source_item : cid->idlFile ())
          toconfig.idlFile[idlFile_index++] = source_item.c_str ();

        toconfig.configProperty.length (cid->configProperty ().size ());
        CORBA::ULong configProperty_index {};
        for (auto const& source_item : cid->configProperty ())
          Property_Handler::handle_property (source_item, toconfig.configProperty[configProperty_index++]);

        toconfig.port.length (cid->port ().size ());
        CORBA::ULong port_index {};
        for (auto const& source_item : cid->port ())
          ::DAnCE::Config_Handlers::CPD_Handler::handle_component_port_description (
            source_item, toconfig.port[port_index++]);

        toconfig.property.length (cid->property ().size ());
        CORBA::ULong property_index {};
        for (auto const& source_item : cid->property ())
          Comp_Prop_Handler::handle_cpd (source_item, toconfig.property[property_index++]);

        toconfig.infoProperty.length (cid->infoProperty ().size ());
        CORBA::ULong infoProperty_index {};
        for (auto const& source_item : cid->infoProperty ())
          Property_Handler::handle_property (source_item, toconfig.infoProperty[infoProperty_index++]);
      }

      ComponentInterfaceDescription
      Comp_Intf_Descr_Handler::comp_intf_descr (
                                                ::Deployment::ComponentInterfaceDescription const& src)
      {
        DANCE_TRACE ("Comp_Intf_Descr_Handler::comp_intf_descr - reverse");
        ComponentInterfaceDescription retval {};

        retval.label (ACE_TEXT_CHAR_TO_TCHAR (src.label.in ()));
        retval.UUID (ACE_TEXT_CHAR_TO_TCHAR  (src.UUID.in ()));
        retval.specificType (ACE_TEXT_CHAR_TO_TCHAR  (src.specificType.in ()));
#if 0
        for (CORBA::ULong i {}; i < src.supportedType.length (); ++i)
          retval.add_supportedType (src.supportedType[i].in ());

        for (CORBA::ULong i {}; i < src.idlFile.length (); ++i)
          retval.add_idlFile (src.idlFile[i].in ());

        for (CORBA::ULong i {}; i < src.configProperty.length (); ++i)
          retval.add_configProperty (Property_Handler::get_property (src.configProperty[i]));

        for (CORBA::ULong i {}; i < src.port.length (); ++i)
          retval.add_port
            (DAnCE::Config_Handlers::CPD_Handler::component_port_description (src.port[i]));

        for (CORBA::ULong i {}; i < src.property.length (); ++i)
          retval.add_property (Comp_Prop_Handler::get_cpd (src.property[i]));

        for (CORBA::ULong i {}; i < src.infoProperty.length (); ++i)
          retval.add_infoProperty (Property_Handler::get_property (src.infoProperty[i]));
#endif
        return retval;
      }

      ComponentInterfaceDescription * Comp_Intf_Descr_Handler::resolve_cid (std::string_view uri)
      {
        DANCE_TRACE ("Comp_Intf_Descr_Handler::resolve_cid");

        XML::XML_Helper<> xml_helper {};
        std::string const uri_string (uri);
        xercesc::DOMDocument *dom = xml_helper.create_dom (ACE_TEXT_CHAR_TO_TCHAR (uri_string.c_str()));

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
