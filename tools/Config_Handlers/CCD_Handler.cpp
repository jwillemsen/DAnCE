#include "tao/AnyTypeCode/AnyTypeCode_methods.h"

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
                                            const ComponentInterfaceDescription &desc,
                                            Deployment::ComponentInterfaceDescription& toconfig)
    {
      DANCE_TRACE("CCD_Handler::component_interface_descr");
      ::XMLSchema::string<char> const* UUID_if_set_ptr = desc.UUID_if_set ();
      if (UUID_if_set_ptr)
        toconfig.UUID =
          CORBA::string_dup (UUID_if_set_ptr->c_str ());


      ::XMLSchema::string<char> const* label_if_set_ptr = desc.label_if_set ();
      if (label_if_set_ptr)
        {
          toconfig.label =
            CORBA::string_dup (label_if_set_ptr->c_str ());
        }


      ::XMLSchema::string<char> const* specificType_if_set_ptr = desc.specificType_if_set ();
      if (specificType_if_set_ptr)
        {
          toconfig.specificType =
            CORBA::string_dup (specificType_if_set_ptr->c_str ());
        }

      auto
        end = desc.supportedType_values ().end ();

      CORBA::ULong pos = 0;
      toconfig.supportedType.length (desc.supportedType_values ().size ());
      for (auto s =
             desc.supportedType_values ().begin ();
           s != end;
           ++s)
        {
          toconfig.supportedType[pos++] = s->c_str ();
        }

      auto
        eidl = desc.idlFile_values ().end ();
      pos = 0;
      toconfig.idlFile.length (desc.idlFile_values ().size ());
      for (auto sidl=
             desc.idlFile_values ().begin ();
           sidl != eidl;
           ++sidl)
        {
          toconfig.idlFile [pos++] = sidl->c_str ();
        }

      auto pend =
        desc.configProperty_values ().end ();
      pos = 0;
      toconfig.configProperty.length (desc.configProperty_values ().size ());
      for (auto pstart =
             desc.configProperty_values ().begin ();
           pstart != pend;
           ++pstart)
        {
          Property_Handler::handle_property (*pstart,
                                             toconfig.configProperty [pos++]);
        }

      pos = 0;
      toconfig.port.length (desc.port_values ().size ());
      for (auto
             port (desc.port_values ().begin ());
           port != desc.port_values ().end ();
           ++port)
        {
          CPD_Handler::handle_component_port_description (*port,
                                                          toconfig.port[pos++]);
        }

      pos = 0;
      CORBA::ULong len = desc.property_values ().size ();
      toconfig.property.length ( len );
      for(auto
            prop (desc.property_values ().begin ());
          prop != desc.property_values ().end ();
          prop++)
        {
          ComponentPropertyDescription_Handler::component_property_description (*prop,
                                                                                toconfig.property[pos++]);
        }

      pos = 0;
      toconfig.infoProperty.length (desc.infoProperty_values ().size ());
      for ( auto
              infoProp (desc.infoProperty_values ().begin ());
            infoProp != desc.infoProperty_values ().end ();
            infoProp++)
        {
          Property_Handler::handle_property (*infoProp,
                                             toconfig.infoProperty[pos]);
        }
    }

    ComponentInterfaceDescription
    CCD_Handler::component_interface_descr (
                                            const ::Deployment::ComponentInterfaceDescription&  src)
    {
      DANCE_TRACE("CCD_Handler::component_interface_descr - reverse");
      ComponentInterfaceDescription cid;

      //Load up the basic string members
      XMLSchema::string<char> uuid (src.UUID);
      XMLSchema::string<char> label (src.label);
      XMLSchema::string<char> specifict (src.specificType);

      //Then put them in the CID
      cid.UUID (uuid);
      cid.label (label);
      cid.specificType (specifict);

      //Get the supported type(s) and load them into cid
      ::CORBA::ULong total = src.supportedType.length ();
      for (size_t i = 0; i < total; ++i)
        {
          XMLSchema::string<char> curr (src.supportedType[i]);
          //cid.add_supportedType (new XMLSchema::string<char> (curr));
        }

      //Get the idlFile(s) and store them into the cid
      total = src.idlFile.length ();
      for (size_t j = 0; j < total; ++j)
        {
          XMLSchema::string<char> curr (src.idlFile[j]);
          //cid.add_idlFile (curr);
        }

      //Get the configProperty(ies) and store them into the cid
      total = src.configProperty.length ();
      for (size_t k = 0; k < total; ++k)
        {
          //cid.add_configProperty (Property_Handler::get_property (src.configProperty[k]));
        }

      //Same drill for the component port description(s)
      total = src.port.length ();
      for (size_t l = 0; l < total; ++l)
        {
          //cid.add_port (CPD_Handler::component_port_description (src.port[l]));
        }


      //Load up the property field
      total = src.property.length();
      for(size_t m = 0;
          m < total;
          m++)
        {
          //cid.add_property(ComponentPropertyDescription_Handler::component_property_description (src.property[m]));
        }

      //Load up the infoProperty(s)
      total = src.infoProperty.length();
      for(size_t n = 0;
          n < total;
          n++)
        {
          //cid.add_infoProperty(Property_Handler::get_property (src.infoProperty[n]));
        }

      return cid;
    }
  }
}
