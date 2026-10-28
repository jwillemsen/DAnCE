#include "DP_PCD_Handler.h"
#include "Common.h"
#include "CEPE_Handler.h"
#include "PSPE_Handler.h"
#include "ERE_Handler.h"
#include "CRDD_Handler.h"
#include "PCD_Handler.h"
#include "Basic_Deployment_Data.hpp"
#include "cdp.hpp"
#include "dance/Deployment/Deployment_DataC.h"
#include "dance/Logger/Log_Macros.h"
namespace DAnCE
{
  namespace Config_Handlers
  {
    void
    DP_PCD_Handler::plan_connection_descrs (
        DeploymentPlan const& src,
        Deployment::PlanConnectionDescriptions& dest)
    {
      DANCE_TRACE("DP_PCD_Handler::plan_connection_descrs");
      CORBA::ULong pos {};
      dest.length (src.connection ().size ());
      for (auto const& source_item : src.connection ())
        DP_PCD_Handler::plan_connection_descr (source_item, dest[pos++]);
    }

    void
    DP_PCD_Handler::plan_connection_descr (
        PlanConnectionDescription const& src,
        Deployment::PlanConnectionDescription &dest)
    {
      DANCE_TRACE("DP_PCD_Handler::plan_connection_descr");
      dest.name = src.name ().c_str ();

      ::XMLSchema::string<char> const* source_if_set_ptr = src.source_if_set ();
      if (source_if_set_ptr)
        {
          // There will be only one as per the schema
          dest.source.length (1);
          dest.source[0] = source_if_set_ptr->c_str ();
        }

      CEPE_Handler::external_port_endpoints (src, dest.externalEndpoint);
      ERE_Handler::external_ref_endpoints (src, dest.externalReference);

      PSPE_Handler::sub_component_port_endpoints (src, dest.internalEndpoint);

    }
  }
}
