#include "CEPE_Handler.h"
#include "Basic_Deployment_Data.hpp"
#include "dance/Deployment/Deployment_DataC.h"
#include "dance/Logger/Log_Macros.h"
namespace DAnCE
{
  namespace Config_Handlers
  {
    void
    CEPE_Handler::external_port_endpoints (
                                           const PlanConnectionDescription &src,
                                           ::Deployment::ComponentExternalPortEndpoints &dest)
    {
      DANCE_TRACE("CEOE_Handler::external_port_endpoints");
      auto eeci_e = src.externalEndpoint ().end ();

      CORBA::ULong pos = 0;
      dest.length (src.externalEndpoint ().size ());
      for (auto eeci_b = src.externalEndpoint ().begin (); eeci_b != eeci_e; ++eeci_b)
        {
          CEPE_Handler::handle_external_port_endpoint (*eeci_b, dest[pos++]);
        }
    }

    void
    CEPE_Handler::handle_external_port_endpoint (
                                          const ComponentExternalPortEndpoint &src,
                                          ::Deployment::ComponentExternalPortEndpoint &dest)
    {
      DANCE_TRACE("CEOE_Handler::external_port_endpoint");
      dest.portName = src.portName ().c_str ();
    }

    ComponentExternalPortEndpoint
    CEPE_Handler::external_port_endpoint (
                                          const ::Deployment::ComponentExternalPortEndpoint &src)
    {
      DANCE_TRACE("CEOE_Handler::external_port_endpoint - reverse");
      //MAJO Unfinished
      XMLSchema::string<char> portname (src.portName);
      return ComponentExternalPortEndpoint  (portname);
    }
  }
}
