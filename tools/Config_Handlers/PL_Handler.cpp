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

#if DANCE_CONFIG_HANDLERS_CORBA_TO_XSC
    PlanLocality PL_Handler::plan_locality (::Deployment::PlanLocality const& src)
    {
      PlanLocalityKind::Value value {};
      switch (src.constraint)
        {
        case Deployment::PlanSameProcess:
          value = PlanLocalityKind::SameProcess;
          break;
        case Deployment::PlanDifferentProcess:
          value = PlanLocalityKind::DifferentProcess;
          break;
        case Deployment::PlanNoConstraint:
          value = PlanLocalityKind::NoConstraint;
          break;
        default:
          throw Config_Error ("", "Unknown PlanLocality constraint");
        }

      PlanLocalityKind const constraint (value);
      PlanLocality::constrainedInstance_container_type instances;
      for (CORBA::ULong i {}; i < src.constrainedInstanceRef.length (); ++i)
        {
          std::string id;
          IDD_Handler::IDREF.find_ref (src.constrainedInstanceRef[i], id);
          XMLSchema::IDREF<char> instance_ref (id.c_str ());
          IdRef const instance (std::nullopt, instance_ref);
          instances.push_back (instance);
        }

      return PlanLocality (constraint, instances);
    }
#endif // DANCE_CONFIG_HANDLERS_CORBA_TO_XSC

  }
}
