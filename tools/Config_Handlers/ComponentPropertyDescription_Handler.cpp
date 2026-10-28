#include "ComponentPropertyDescription_Handler.h"
#include "Common.h"
#include "DataType_Handler.h"
#include "dance/Deployment/Deployment_DataC.h"
#include "dance/Logger/Log_Macros.h"
namespace DAnCE
{
  namespace Config_Handlers
  {
    void
    ComponentPropertyDescription_Handler::component_property_description (
                                                                          ComponentPropertyDescription const& desc,
                                                                          ::Deployment::ComponentPropertyDescription& toconfig)
    {
      DANCE_TRACE("ComponentPropertyDescription_Handler::component_property_description");
      toconfig.name = CORBA::string_dup (desc.name ().c_str ());

      // Delegate the DataType to the
      // DataType_Handler.

      // @@ There is a lurking bug here.
      CORBA::TypeCode_ptr tcptr = toconfig.type.in ();
      DataType_Handler::data_type (desc.type (), tcptr);
    }

  }
}
