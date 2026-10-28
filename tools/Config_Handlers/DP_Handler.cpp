#include "DP_Handler.h"
#include "Common.h"
#include "ace/UUID.h"
#include "dance/Deployment/Deployment_DataC.h"

#include "CCD_Handler.h"
#include "ADD_Handler.h"
#include "MDD_Handler.h"
#include "IDD_Handler.h"
#include "ID_Handler.h"
#include "PL_Handler.h"
#include "Property_Handler.h"
#include "cdp.hpp"

#include "PCD_Handler.h"
#include <iterator>

namespace DAnCE
{
  namespace Config_Handlers
  {
    DP_Handler::DP_Handler (deploymentPlan &dp)
    {
      DANCE_TRACE("DP_Handler::constructor");
      if (!this->resolve_plan (dp))
        throw Config_Error ("", "Unable to convert the XML deployment plan");
    }

    DP_Handler::~DP_Handler ()
      throw ()
    {
      DANCE_TRACE("DP_Handler::destructor");
    }

    ::Deployment::DeploymentPlan const *
    DP_Handler::plan () const
    {
      DANCE_TRACE("DP_Handler::plan const");
      if (this->idl_dp_)
        return this->idl_dp_.get ();

      throw NoPlan ();
    }

    ::Deployment::DeploymentPlan * DP_Handler::plan ()
    {
      DANCE_TRACE("DP_Handler::plan");
      if (this->idl_dp_)
        return this->idl_dp_.release ();

      throw NoPlan ();
    }

    bool DP_Handler::resolve_plan (deploymentPlan &xsc_dp)
    {
      DANCE_TRACE ("DP_Handler::resolve_plan");

      this->idl_dp_ = std::make_unique<Deployment::DeploymentPlan> ();

      // Read in the label, if present, since minoccurs = 0
      ::XMLSchema::string<char> const* label_if_set_ptr = xsc_dp.label_if_set ();
      if (label_if_set_ptr)
        {
          this->idl_dp_->label = CORBA::string_dup (label_if_set_ptr->c_str ());
        }

      // Read in the UUID, if present
      ::XMLSchema::string<char> const* UUID_if_set_ptr = xsc_dp.UUID_if_set ();
      if (UUID_if_set_ptr)
        {
          this->idl_dp_->UUID = CORBA::string_dup (UUID_if_set_ptr->c_str ());
        }

      this->idl_dp_->dependsOn.length (xsc_dp.dependsOn ().size ());
      CORBA::ULong dependsOn_index {};
      for (auto const& source_item : xsc_dp.dependsOn ())
        ID_Handler::get_ImplementationDependency (source_item,
                                                  this->idl_dp_->dependsOn[dependsOn_index++]);

      this->idl_dp_->infoProperty.length (xsc_dp.infoProperty ().size ());
      CORBA::ULong infoProperty_index {};
      for (auto const& source_item : xsc_dp.infoProperty ())
        Property_Handler::handle_property (source_item,
                                           this->idl_dp_->infoProperty[infoProperty_index++]);

      // Read in the realizes, if present
      ::DAnCE::Config_Handlers::ComponentInterfaceDescription const* realizes_if_set_ptr = xsc_dp.realizes_if_set ();
      if (realizes_if_set_ptr)
        {
          CCD_Handler::component_interface_descr (*realizes_if_set_ptr, this->idl_dp_->realizes);
        }

      ADD_Handler::artifact_deployment_descrs (xsc_dp, this->idl_dp_->artifact);

      MDD_Handler::mono_deployment_descriptions (xsc_dp, this->idl_dp_->implementation);

      IDD_Handler::instance_deployment_descrs (xsc_dp, this->idl_dp_->instance);

      this->idl_dp_->connection.length (xsc_dp.connection ().size ());
      CORBA::ULong connection_index {};
      for (auto const& source_item : xsc_dp.connection ())
        PCD_Handler::handle_PlanConnectionDescription (source_item, this->idl_dp_->connection[connection_index++]);

      this->idl_dp_->localityConstraint.length (xsc_dp.localityConstraint ().size ());

      CORBA::ULong localityConstraint_index {};
      for (auto const& source_item : xsc_dp.localityConstraint ())
        PL_Handler::get_PlanLocality (source_item, this->idl_dp_->localityConstraint[localityConstraint_index++]);

      return true;
    }

  }
}
