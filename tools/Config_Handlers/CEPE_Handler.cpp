#include "CEPE_Handler.h"
#include "Common.h"
#include "Basic_Deployment_Data.hpp"
#include "dance/Deployment/Deployment_DataC.h"
#include "dance/Logger/Log_Macros.h"
namespace DAnCE
{
  namespace Config_Handlers
  {
    void
    CEPE_Handler::external_port_endpoints (
                                           PlanConnectionDescription const& src,
                                           ::Deployment::ComponentExternalPortEndpoints &dest)
    {
      DANCE_TRACE("CEOE_Handler::external_port_endpoints");
      CORBA::ULong pos {};
      dest.length (src.externalEndpoint ().size ());
      for (auto const& source_item : src.externalEndpoint ())
        CEPE_Handler::handle_external_port_endpoint (source_item, dest[pos++]);
    }

    void
    CEPE_Handler::handle_external_port_endpoint (
                                          ComponentExternalPortEndpoint const& src,
                                          ::Deployment::ComponentExternalPortEndpoint &dest)
    {
      DANCE_TRACE("CEOE_Handler::external_port_endpoint");
      dest.portName = src.portName ().c_str ();
    }

  }
}
