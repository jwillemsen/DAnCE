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
      : retval_ (true)
    {
      DANCE_TRACE("DP_Handler::constructor");
      if (!this->resolve_plan (dp))
        throw Config_Error ("", "Unable to convert the XML deployment plan");
    }
#if DANCE_CONFIG_HANDLERS_CORBA_TO_XSC

    DP_Handler::DP_Handler (::Deployment::DeploymentPlan const& plan)
      : retval_ (false)
    {
      DANCE_TRACE("DP_Handler::constructor");
      if (!this->build_xsc (plan))
        throw Config_Error ("", "Unable to convert the CORBA deployment plan");
    }
#endif // DANCE_CONFIG_HANDLERS_CORBA_TO_XSC

    DP_Handler::~DP_Handler ()
      throw ()
    {
      DANCE_TRACE("DP_Handler::destructor");
    }

    deploymentPlan const *
    DP_Handler::xsc () const
    {
      DANCE_TRACE("DP_Handler::xsc const");
      if (this->retval_ && this->xsc_dp_.get () != 0)
        return this->xsc_dp_.get ();

      throw NoPlan ();
    }

    deploymentPlan * DP_Handler::xsc ()
    {
      DANCE_TRACE("DP_Handler::xsc");
      if (this->retval_ && this->xsc_dp_.get () != 0)
        return this->xsc_dp_.release ();

      throw NoPlan ();
    }

    ::Deployment::DeploymentPlan const *
    DP_Handler::plan () const
    {
      DANCE_TRACE("DP_Handler::plan const");
      if (this->retval_ && this->idl_dp_.get () != 0)
        return this->idl_dp_.get ();

      throw NoPlan ();
    }

    ::Deployment::DeploymentPlan * DP_Handler::plan ()
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
#if DANCE_CONFIG_HANDLERS_CORBA_TO_XSC

    bool DP_Handler::build_xsc (::Deployment::DeploymentPlan const& plan)
    {
      DANCE_TRACE ("DP_Handler::build_xsc");

      // Initialize the UUID generator.
      ACE_Utils::UUID_GENERATOR::instance ()->init ();

      // Clear IDREF tables
      IDD_Handler::IDREF.unbind_refs ();
      MDD_Handler::IDREF.unbind_refs ();
      ADD_Handler::IDREF.unbind_refs ();

      std::optional<XMLSchema::string<char>> label;
      if (plan.label != 0)
        label.emplace (plan.label);

      std::optional<XMLSchema::string<char>> uuid;
      if (plan.UUID != 0)
        uuid.emplace (plan.UUID);

      ::XSCRT::OptionalArg<XMLSchema::string<char>> const label_arg (
        label ? ::XSCRT::OptionalArg<XMLSchema::string<char>> (*label) : std::nullopt);
      ::XSCRT::OptionalArg<XMLSchema::string<char>> const uuid_arg (
        uuid ? ::XSCRT::OptionalArg<XMLSchema::string<char>> (*uuid) : std::nullopt);

      ::DAnCE::Config_Handlers::ComponentInterfaceDescription const realizes (
        CCD_Handler::component_interface_descr (plan.realizes));

      deploymentPlan::artifact_container_type artifacts;
      for (CORBA::ULong i {}; i < plan.artifact.length (); ++i)
        {
          ArtifactDeploymentDescription const artifact (
            ADD_Handler::artifact_deployment_descr (plan.artifact[i]));
          artifacts.push_back (artifact);
        }

      deploymentPlan::implementation_container_type implementations;
      for (CORBA::ULong i {}; i < plan.implementation.length (); ++i)
        {
          MonolithicDeploymentDescription const implementation (
            MDD_Handler::mono_deployment_description (plan.implementation[i]));
          implementations.push_back (implementation);
        }

      deploymentPlan::instance_container_type instances;
      for (CORBA::ULong i {}; i < plan.instance.length (); ++i)
        {
          InstanceDeploymentDescription const instance (
            IDD_Handler::instance_deployment_descr (plan.instance[i]));
          instances.push_back (instance);
        }

      deploymentPlan::connection_container_type connections;
      for (CORBA::ULong i {}; i < plan.connection.length (); ++i)
        {
          PlanConnectionDescription const connection (
            PCD_Handler::get_PlanConnectionDescription (plan.connection[i]));
          connections.push_back (connection);
        }

      deploymentPlan::externalProperty_container_type external_properties;
      for (CORBA::ULong i {}; i < plan.externalProperty.length (); ++i)
        {
          ::Deployment::PlanPropertyMapping const& source = plan.externalProperty[i];
          XMLSchema::string<char> name (source.name);
          XMLSchema::string<char> external_name (source.externalName);
          PlanPropertyMapping::source_container_type sources;
          for (CORBA::ULong j {}; j < source.source.length (); ++j)
            sources.emplace_back (source.source[j]);

          PlanPropertyMapping::delegatesTo_container_type delegates;
          for (CORBA::ULong j {}; j < source.delegatesTo.length (); ++j)
            {
              CORBA::ULong const instance_index = source.delegatesTo[j].instanceRef;
              if (instance_index >= instances.size ())
                throw Config_Error (source.name.in (), "Property mapping references an unknown instance");
              auto instance_pos = instances.begin ();
              std::advance (instance_pos, instance_index);
              XMLSchema::string<char> property_name (source.delegatesTo[j].propertyName);
              PlanSubcomponentPropertyReference const delegate (property_name, *instance_pos);
              delegates.push_back (delegate);
            }

          PlanPropertyMapping const mapping (name, external_name, delegates, sources);
          external_properties.push_back (mapping);
        }

      deploymentPlan::dependsOn_container_type dependencies;
      for (CORBA::ULong i {}; i < plan.dependsOn.length (); ++i)
        {
          ImplementationDependency const dependency (ID_Handler::impl_dependency (plan.dependsOn[i]));
          dependencies.push_back (dependency);
        }

      deploymentPlan::infoProperty_container_type info_properties;
      for (CORBA::ULong i {}; i < plan.infoProperty.length (); ++i)
        {
          Property const property (Property_Handler::get_property (plan.infoProperty[i]));
          info_properties.push_back (property);
        }

      deploymentPlan::localityConstraint_container_type locality_constraints;
      for (CORBA::ULong i {}; i < plan.localityConstraint.length (); ++i)
        {
          PlanLocality const constraint (PL_Handler::plan_locality (plan.localityConstraint[i]));
          locality_constraints.push_back (constraint);
        }

      this->xsc_dp_ = std::make_unique<deploymentPlan> (
        label_arg, uuid_arg, realizes, implementations, instances, connections, external_properties,
        dependencies, artifacts, info_properties, locality_constraints);
      retval_ = true;
      return true;
    }
#endif // DANCE_CONFIG_HANDLERS_CORBA_TO_XSC
  }
}
