#include "Req_Handler.h"
#include "Property_Handler.h"
#include "dance/Deployment/Deployment_BaseC.h"
#include "dance/Logger/Log_Macros.h"
namespace DAnCE
{
  namespace Config_Handlers
  {

    Req_Handler::Req_Handler (void)
    {
    }

    Req_Handler::~Req_Handler (void)
    {
    }

    void Req_Handler::handle_requirement (const Requirement& desc, Deployment::Requirement& toconfig)
    {
      DANCE_TRACE("Req_Handler::get_Requirement");

      //Map the basic string types to their Deployment::Req
      //counterparts.
      toconfig.name =  desc.name ().c_str ();
      toconfig.resourceType =  desc.resourceType ().c_str ();

      toconfig.property.length (desc.property ().size ());
      std::for_each (desc.property ().begin (),
                     desc.property ().end (),
                     Property_Functor (toconfig.property));
    }

    Requirement Req_Handler::get_requirement (const Deployment::Requirement& src)
    {
      DANCE_TRACE("Req_Handler::get_requirement - reverse");

      //Get the values for name and res
      XMLSchema::string<char> name (src.name);
      XMLSchema::string<char> res (src.resourceType);

      //Instantiate the Requirement
      Requirement req (name,res);

      for (CORBA::ULong i = 0; i < src.property.length (); ++i)
        {
          //req.add_property (Property_Handler::get_property (src.property[i]));
        }

      return req;
    }
  }
}
