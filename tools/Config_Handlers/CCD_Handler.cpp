#include "tao/AnyTypeCode/AnyTypeCode_methods.h"
#include "Common.h"

#include "dance/Logger/Log_Macros.h"
#include "CCD_Handler.h"
#include "ccd.hpp"
#include "Property_Handler.h"
#include "CPD_Handler.h"
#include "ComponentPropertyDescription_Handler.h"
#include "dance/Deployment/Deployment_DataC.h"

namespace DAnCE
{
  namespace Config_Handlers
  {
    void
    CCD_Handler::component_interface_descr (
                                            ComponentInterfaceDescription const& desc,
                                            Deployment::ComponentInterfaceDescription& toconfig)
    {
      DANCE_TRACE("CCD_Handler::component_interface_descr");
      ::XMLSchema::string<char> const* UUID_if_set_ptr = desc.UUID_if_set ();
      if (UUID_if_set_ptr)
        toconfig.UUID = CORBA::string_dup (UUID_if_set_ptr->c_str ());

      ::XMLSchema::string<char> const* label_if_set_ptr = desc.label_if_set ();
      if (label_if_set_ptr)
        {
          toconfig.label = CORBA::string_dup (label_if_set_ptr->c_str ());
        }

      ::XMLSchema::string<char> const* specificType_if_set_ptr = desc.specificType_if_set ();
      if (specificType_if_set_ptr)
        {
          toconfig.specificType = CORBA::string_dup (specificType_if_set_ptr->c_str ());
        }

      CORBA::ULong pos {};
      toconfig.supportedType.length (desc.supportedType ().size ());
      for (auto const& source_item : desc.supportedType ())
        toconfig.supportedType[pos++] = source_item.c_str ();

      pos = 0;
      toconfig.idlFile.length (desc.idlFile ().size ());
      for (auto const& source_item : desc.idlFile ())
        toconfig.idlFile[pos++] = source_item.c_str ();

      pos = 0;
      toconfig.configProperty.length (desc.configProperty ().size ());
      for (auto const& source_item : desc.configProperty ())
        Property_Handler::handle_property (source_item, toconfig.configProperty[pos++]);

      pos = 0;
      toconfig.port.length (desc.port ().size ());
      for (auto const& source_item : desc.port ())
        CPD_Handler::handle_component_port_description (source_item, toconfig.port[pos++]);

      pos = 0;
      CORBA::ULong const len = desc.property ().size ();
      toconfig.property.length (len );
      for (auto const& source_item : desc.property ())
        ComponentPropertyDescription_Handler::component_property_description (source_item, toconfig.property[pos++]);

      pos = 0;
      toconfig.infoProperty.length (desc.infoProperty ().size ());
      for (auto const& source_item : desc.infoProperty ())
        Property_Handler::handle_property (source_item, toconfig.infoProperty[pos++]);
    }
#if DANCE_CONFIG_HANDLERS_CORBA_TO_XSC

    ComponentInterfaceDescription
    CCD_Handler::component_interface_descr (
                                            ::Deployment::ComponentInterfaceDescription const&  src)
    {
      DANCE_TRACE("CCD_Handler::component_interface_descr - reverse");
      XMLSchema::string<char> const uuid (src.UUID);
      XMLSchema::string<char> const label (src.label);
      XMLSchema::string<char> const specifict (src.specificType);
      return ComponentInterfaceDescription (label, uuid, specifict);
    }
#endif // DANCE_CONFIG_HANDLERS_CORBA_TO_XSC
  }
}
