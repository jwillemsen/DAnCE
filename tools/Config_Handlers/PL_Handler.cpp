#include "PL_Handler.h"
#include "IDD_Handler.h"
#include "Basic_Deployment_Data.hpp"
#include "dance/Deployment/Deployment_DataC.h"
#include "dance/Logger/Log_Macros.h"

namespace DAnCE
{
  namespace Config_Handlers
  {
    void PL_Handler::get_PlanLocality (PlanLocality const& desc, Deployment::PlanLocality &toconfig)
    {
      DANCE_TRACE ("PL_Handler::get_PlanLocality");

      switch (desc.constraint ().integral ())
        {
        case PlanLocalityKind::SameProcess:
          toconfig.constraint = Deployment::PlanSameProcess;
          break;
        case PlanLocalityKind::DifferentProcess:
          toconfig.constraint = Deployment::PlanDifferentProcess;
          break;
        case PlanLocalityKind::NoConstraint:
          toconfig.constraint = Deployment::PlanNoConstraint;
          break;

        default:
          throw Config_Error ("", "Unknown PlanLocalityKind.");
        }

      size_t pos {};
      toconfig.constrainedInstanceRef.length (desc.constrainedInstance ().size ());

      for (auto const& instance : desc.constrainedInstance ())
        {
          CORBA::ULong tmp {};
          ::XMLSchema::IDREF<char> const* instanceRef = instance.idref_if_set ();
          if (!instanceRef)
            throw Config_Error ("", "Constrained instance has no IDREF");

          IDD_Handler::IDREF.find_ref (instanceRef->id ().c_str (), tmp);

          toconfig.constrainedInstanceRef[pos] = tmp;
          ++pos;
        }
    }

  }
}
