#include "ID_Handler.h"
#include "Common.h"
#include "Basic_Deployment_Data.hpp"
#include "dance/Deployment/Deployment_DataC.h"
#include "dance/Logger/Log_Macros.h"
namespace DAnCE
{
  namespace Config_Handlers
  {
    ID_Handler::ID_Handler ()
    {
    }

    ID_Handler::~ID_Handler ()
    {
    }

    void
    ID_Handler::get_ImplementationDependency (ImplementationDependency const& desc,
                                              Deployment::ImplementationDependency& toconfig)
    {

      DANCE_TRACE("ID_Handler::get_ImplementationDependency");

      toconfig.requiredType= CORBA::string_dup (desc.requiredType ().c_str ());
    }
#if DANCE_CONFIG_HANDLERS_CORBA_TO_XSC

    ImplementationDependency ID_Handler::impl_dependency (::Deployment::ImplementationDependency const& src)
    {
      DANCE_TRACE("ID_Handler::get_ImplementationDependency - reverse");
      XMLSchema::string<char> reqtype (src.requiredType);

      return ImplementationDependency (reqtype);
    }
#endif // DANCE_CONFIG_HANDLERS_CORBA_TO_XSC

  }

}
