#include "Property_Handler.h"
#include "Any_Handler.h"
#include "Basic_Deployment_Data.hpp"
#include "dance/Deployment/Deployment_DataC.h"
#include "dance/Logger/Log_Macros.h"
namespace DAnCE
{
  namespace Config_Handlers
  {

    Property_Handler::Property_Handler ()
    {
    }

    Property_Handler::~Property_Handler ()
    {
    }

    void Property_Handler::handle_property (Property const& desc, Deployment::Property& toconfig)
    {
      DANCE_TRACE("Property_Handler::get_property");

      try
        {
          toconfig.name = CORBA::string_dup (desc.name ().c_str ());

          Any_Handler::extract_into_any (desc.value (), toconfig.value);
        }
      catch (Config_Error &ex)
        {
          ex.name_ = desc.name ();
          throw ex;
        }
    }

    Property Property_Handler::get_property (Deployment::Property const& src)
    {
      DANCE_TRACE("Property_Handler::get_property - reverse");

      XMLSchema::string<char> name (src.name);
      Any value (Any_Handler::get_any (src.value));

      Property prop (name,value);

      return prop;
    }

  }
}
