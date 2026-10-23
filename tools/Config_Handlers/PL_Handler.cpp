#include "PL_Handler.h"
#include "IDD_Handler.h"
#include "Basic_Deployment_Data.hpp"
#include "dance/Deployment/Deployment_DataC.h"
#include "dance/Logger/Log_Macros.h"

namespace DAnCE
{
  namespace Config_Handlers
  {
    void PL_Handler::get_PlanLocality (const PlanLocality &desc, Deployment::PlanLocality &toconfig)
    {
      DANCE_TRACE ("PL_Handler::get_PlanLocality");

      switch (desc.constraint ().integral ())
        {
        case PlanLocalityKind::SameProcess_l:
          toconfig.constraint = Deployment::PlanSameProcess;
          break;

        case PlanLocalityKind::DifferentProcess_l:
          toconfig.constraint = Deployment::PlanDifferentProcess;
          break;

        case PlanLocalityKind::NoConstraint_l:
          toconfig.constraint = Deployment::PlanNoConstraint;
          break;
        }

      size_t pos = 0;
      toconfig.constrainedInstanceRef.length (desc.constrainedInstance ().size ());

      for (auto i = desc.constrainedInstance ().begin (); i != desc.constrainedInstance ().end (); ++i)
        {
          CORBA::ULong tmp = 0;
          ::XMLSchema::IDREF<char> const* instanceRef = i->idref_if_set ();
          if (!instanceRef)
            throw Config_Error ("", "Constrained instance has no IDREF");

          IDD_Handler::IDREF.find_ref (instanceRef->id ().c_str (), tmp);

          toconfig.constrainedInstanceRef[pos] = tmp;
          ++pos;
        }
    }

  }
}
