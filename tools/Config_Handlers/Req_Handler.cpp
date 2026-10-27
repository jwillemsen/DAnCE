#include "Req_Handler.h"
#include "Common.h"
#include "Property_Handler.h"
#include "dance/Deployment/Deployment_BaseC.h"
#include "dance/Logger/Log_Macros.h"
namespace DAnCE
{
  namespace Config_Handlers
  {

    Req_Handler::Req_Handler ()
    {
    }

    Req_Handler::~Req_Handler ()
    {
    }

    void Req_Handler::handle_requirement (Requirement const& desc, Deployment::Requirement& toconfig)
    {
      DANCE_TRACE("Req_Handler::get_Requirement");

      //Map the basic string types to their Deployment::Req
      //counterparts.
      toconfig.name = desc.name ().c_str ();
      toconfig.resourceType = desc.resourceType ().c_str ();

      toconfig.property.length (desc.property ().size ());
      CORBA::ULong property_index {};
      for (auto const& source_item : desc.property ())
        Property_Handler::handle_property (source_item, toconfig.property[property_index++]);
    }
#if DANCE_CONFIG_HANDLERS_CORBA_TO_XSC

    Requirement Req_Handler::get_requirement (Deployment::Requirement const& src)
    {
      DANCE_TRACE("Req_Handler::get_requirement - reverse");

      //Get the values for name and res
      XMLSchema::string<char> const name (src.name);
      XMLSchema::string<char> const res (src.resourceType);

      return Requirement (name, res);
    }
#endif // DANCE_CONFIG_HANDLERS_CORBA_TO_XSC
  }
}
