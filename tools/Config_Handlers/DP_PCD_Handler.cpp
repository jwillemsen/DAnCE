#include "DP_PCD_Handler.h"
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
      auto cci_e = src.connection ().end ();

      CORBA::ULong pos {};
      dest.length (src.connection ().size ());
      for (auto cci_b = src.connection ().begin (); cci_b != cci_e; ++cci_b)
        {
          DP_PCD_Handler::plan_connection_descr (*(cci_b), dest[pos++]);
        }
    }

    PlanConnectionDescription DP_PCD_Handler::plan_connection_descr (Deployment::PlanConnectionDescription const& src)
    {
      DANCE_TRACE("DP_PCD_Handler::plan_connection_descr");
      PlanConnectionDescription pcd = PCD_Handler::get_PlanConnectionDescription(src);
      return pcd;
    }

    void
    DP_PCD_Handler::plan_connection_descr (
        PlanConnectionDescription const& src,
        Deployment::PlanConnectionDescription &dest)
    {
      DANCE_TRACE("DP_PCD_Handler::plan_connection_descr - reverse");
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
