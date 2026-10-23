#include "DP_Handler.h"
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

namespace DAnCE
{
  namespace Config_Handlers
  {
    DP_Handler::DP_Handler (deploymentPlan &dp)
      : xsc_dp_ {}
      , idl_dp_ {}
      , retval_ (true)
    {
      DANCE_TRACE("DP_Handler::constructor");
      if (!this->resolve_plan (dp))
        throw;
    }

    DP_Handler::DP_Handler (const ::Deployment::DeploymentPlan &plan)
      : xsc_dp_ (std::make_unique<deploymentPlan> ()),
        idl_dp_ {},
        retval_ (0)
    {
      DANCE_TRACE("DP_Handler::constructor");
      if (!this->build_xsc (plan))
        throw;
    }

    DP_Handler::~DP_Handler (void)
      throw ()
    {
      DANCE_TRACE("DP_Handler::destructor");
    }

    deploymentPlan const *
    DP_Handler::xsc (void) const
    {
      DANCE_TRACE("DP_Handler::xsc const");
      if (this->retval_ && this->xsc_dp_.get () != 0)
        return this->xsc_dp_.get ();

      throw NoPlan ();
    }

    deploymentPlan * DP_Handler::xsc (void)
    {
      DANCE_TRACE("DP_Handler::xsc");
      if (this->retval_ && this->xsc_dp_.get () != 0)
        return this->xsc_dp_.release ();

      throw NoPlan ();
    }

    ::Deployment::DeploymentPlan const *
    DP_Handler::plan (void) const
    {
      DANCE_TRACE("DP_Handler::plan const");
      if (this->retval_ && this->idl_dp_.get () != 0)
        return this->idl_dp_.get ();

      throw NoPlan ();
    }

    ::Deployment::DeploymentPlan * DP_Handler::plan (void)
    {
      DANCE_TRACE("DP_Handler::plan");
      if (this->retval_ && this->idl_dp_.get () != 0)
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
          this->idl_dp_->label =
            CORBA::string_dup (
              label_if_set_ptr->c_str ());
        }

      // Read in the UUID, if present
      ::XMLSchema::string<char> const* UUID_if_set_ptr = xsc_dp.UUID_if_set ();
      if (UUID_if_set_ptr)
        {
          this->idl_dp_->UUID =
            CORBA::string_dup (
              UUID_if_set_ptr->c_str ());
        }

      // Similar thing for dependsOn
      for (auto dstart
                  = xsc_dp.dependsOn ().begin ();
            dstart != xsc_dp.dependsOn ().end ();
            ++dstart)
        {
          CORBA::ULong len = this->idl_dp_->dependsOn.length ();
          this->idl_dp_->dependsOn.length (len + 1);
          ID_Handler::get_ImplementationDependency (*dstart,
            this->idl_dp_->dependsOn [len]);

        }

      // ... An the property stuff
      for (auto pstart
                  = xsc_dp.infoProperty ().begin ();
            pstart != xsc_dp.infoProperty ().end ();
            ++pstart)
        {
          CORBA::ULong len =
            this->idl_dp_->infoProperty.length ();

          this->idl_dp_->infoProperty.length (len + 1);

          Property_Handler::handle_property (*pstart,
                            this->idl_dp_->infoProperty [len]);
        }

      // Read in the realizes, if present
      ::DAnCE::Config_Handlers::ComponentInterfaceDescription const* realizes_if_set_ptr = xsc_dp.realizes_if_set ();
      if (realizes_if_set_ptr)
        {
          CCD_Handler::component_interface_descr (
                            *realizes_if_set_ptr,
                            this->idl_dp_->realizes);
        }

      ADD_Handler::artifact_deployment_descrs (xsc_dp,
                            this->idl_dp_->artifact);

      MDD_Handler::mono_deployment_descriptions (xsc_dp,
                            this->idl_dp_->implementation);

      IDD_Handler::instance_deployment_descrs (xsc_dp,
                            this->idl_dp_->instance);

      this->idl_dp_->connection.length (xsc_dp.connection ().size ());
      std::for_each (xsc_dp.connection ().begin (),
                      xsc_dp.connection ().end (),
                      PCD_Functor (this->idl_dp_->connection));

      this->idl_dp_->localityConstraint.length (
        xsc_dp.localityConstraint ().size ());

      std::for_each (xsc_dp.localityConstraint ().begin (),
                      xsc_dp.localityConstraint ().end (),
                      PL_Functor (this->idl_dp_->localityConstraint));

      //PCD_Handler::get_PlanConnectionDescription (xsc_dp,
      // this->idl_dp_->connection);

      return true;
    }

    bool DP_Handler::build_xsc (const ::Deployment::DeploymentPlan &plan)
    {
      DANCE_TRACE ("DP_Handler::build_xsc");

      // Initialize the UUID generator.
      ACE_Utils::UUID_GENERATOR::instance ()->init ();

      // Clear IDREF tables
      IDD_Handler::IDREF.unbind_refs ();
      MDD_Handler::IDREF.unbind_refs ();
      ADD_Handler::IDREF.unbind_refs ();

      size_t len; //Used for checking the length of struct data members

      // Read in the label, if present, since minoccurs = 0
      if (plan.label != 0)
        {
          XMLSchema::string<char> i (
            plan.label);
          this->xsc_dp_->label(i);
        }

      // Read in the UUID, if present
      if (plan.UUID != 0)
        {
          XMLSchema::string<char> j (
            plan.UUID);
          this->xsc_dp_->UUID(j);
        }

      // Similar thing for dependsOn
      len = plan.dependsOn.length();
      for (size_t j = 0;
            j < len;
            ++j)
        {
          //this->xsc_dp_->add_dependsOn(ID_Handler::
          //impl_dependency(plan.dependsOn[j]));
        }

      // ... And the property stuff
      len = plan.infoProperty.length();
      for (size_t q = 0;
            q < len;
            q++)
        {
          if (ACE_OS::strcmp (plan.infoProperty[q].name.in (),
                              "CIAOServerResources") == 0)
            {
              DANCE_DEBUG (DANCE_LOG_NONFATAL_ERROR, (LM_ERROR,
                          "(%P|%t) DP_Handler: Dumping of ServerResources"\
                          " not currently supported."));
              continue;
            }

          //this->xsc_dp_->add_infoProperty (Property_Handler::
          // get_property (plan.infoProperty[q]));
        }

      // We are assuming there is a realizes for the moment
      // @@ We may want to change this at a later date
      // @@ by creating a sequence of
      // @@ ComponentInterfaceDescriptions in the DeploymentPlan
      // @@ in ../dance/Deployment/Deployment_Data.idl
      // @@ so we can check for length
      this->xsc_dp_->realizes(CCD_Handler::component_interface_descr(plan.realizes));
      ::DAnCE::Config_Handlers::ComponentInterfaceDescription const*
        realizes_if_set_ptr_2 = this->xsc_dp_->realizes_if_set ();
      if (!realizes_if_set_ptr_2)
        {
          DANCE_DEBUG (DANCE_LOG_TERMINAL_ERROR, (LM_ERROR,
                      "(%P|%t) DP_Handler: "
                      "Error parsing Component Interface Descriptor."));
          return false;
        }

      //Take care of the artifact(s) if they exist
      len = plan.artifact.length();
      for(size_t k = 0;
          k < len;
          k++)
        {
          //this->xsc_dp_->add_artifact
          //(ADD_Handler::artifact_deployment_descr (plan.artifact[k]));
        }

      //Take care of the implementation(s) if they exist
      len = plan.implementation.length();
      for(size_t l = 0;
          l < len;
          l++)
        {
          //this->xsc_dp_->add_implementation
          //(MDD_Handler::mono_deployment_description (plan.implementation[l]));
        }

      //Ditto for the instance(s)
      len = plan.instance.length();
      for(size_t m = 0;
          m < len;
          m++)
        {
          //this->xsc_dp_->add_instance
          //(IDD_Handler::instance_deployment_descr (plan.instance[m]));
        }

      //Finally, take care of the Connection Planning
      len = plan.connection.length();
      for(size_t n = 0; n < len; n++)
        {
          //this->xsc_dp_->add_connection
          //(PCD_Handler::get_PlanConnectionDescription (plan.connection[n]));
        }

      retval_ = true;
      return true;
    }
  }
}
