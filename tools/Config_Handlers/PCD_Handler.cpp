#include "PCD_Handler.h"
#include "Common.h"
#include "Req_Handler.h"
#include "CEPE_Handler.h"
#include "PSPE_Handler.h"
#include "ERE_Handler.h"
#include "CRDD_Handler.h"
#include "Basic_Deployment_Data.hpp"
#include "dance/Deployment/Deployment_DataC.h"
#include "dance/Logger/Log_Macros.h"

namespace DAnCE
{
  namespace Config_Handlers
  {

    PCD_Handler::PCD_Handler ()
    {
    }

    PCD_Handler::~PCD_Handler ()
    {
    }

    ///This method takes a <Deployment::PlanConnectionDescription>
    ///and maps the values from the passed in XSC
    ///PlanConnectionDescription to its members.
    void PCD_Handler::handle_PlanConnectionDescription (PlanConnectionDescription const& desc,
                                                     Deployment::PlanConnectionDescription& toconfig)
    {
      DANCE_TRACE("PCD_Handler::get_PlanConnectionDescription");

      toconfig.name = CORBA::string_dup (desc.name ().c_str ());

      //Source is mapped to a string in the schema and a sequence
      //in the IDL. We just assign the source string from the xml
      //to the first position in the IDL type's sequence. We
      //make sure that the source is present before lengthening
      //the sequence.
      ::XMLSchema::string<char> const* source_if_set_ptr = desc.source_if_set ();
      if (source_if_set_ptr)
        {
          // Only one.....
          toconfig.source.length (1);
          toconfig.source[0] = source_if_set_ptr->c_str ();
        }

      toconfig.deployRequirement.length (desc.deployRequirement ().size ());
      CORBA::ULong deployRequirement_index {};
      for (auto const& source_item : desc.deployRequirement ())
        Req_Handler::handle_requirement (source_item, toconfig.deployRequirement[deployRequirement_index++]);

      //Create the ComponentExternalPortEndpoint handler.
      CEPE_Handler::external_port_endpoints (desc, toconfig.externalEndpoint);

      //Configure the PlanSubcomponentPortEndpoint's.
      PSPE_Handler::sub_component_port_endpoints (desc, toconfig.internalEndpoint);

      //Configure the ExternalReferenceEndpoint's.
      ERE_Handler::external_ref_endpoints (desc, toconfig.externalReference);

      //Configure the resource value.
      CRDD_Handler crddhandler {};
      CORBA::ULong pos {};
      toconfig.deployedResource.length (desc.deployedResource ().size ());
      for (auto const& resource : desc.deployedResource ())
        crddhandler.get_ConnectionResourceDeploymentDescription (toconfig.deployedResource[pos++], resource);

    }

#if DANCE_CONFIG_HANDLERS_CORBA_TO_XSC
    PlanConnectionDescription
    PCD_Handler::get_PlanConnectionDescription (Deployment::PlanConnectionDescription const& src)
    {
      DANCE_TRACE("PCD_Handler::get_PlanConnectionDescription");

      XMLSchema::string<char> name (src.name);
      if (src.source.length () > 1)
        throw Config_Error (src.name.in (), "Multiple source entries cannot be represented by the schema");

      std::optional<XMLSchema::string<char>> source;
      if (src.source.length () == 1)
        source.emplace (src.source[0]);
      ::XSCRT::OptionalArg<XMLSchema::string<char>> const source_arg (
        source ? ::XSCRT::OptionalArg<XMLSchema::string<char>> (*source) : std::nullopt);

      PlanConnectionDescription::deployRequirement_container_type requirements;
      for (CORBA::ULong i {}; i < src.deployRequirement.length (); ++i)
        {
          Requirement const requirement (Req_Handler::get_requirement (src.deployRequirement[i]));
          requirements.push_back (requirement);
        }

      PlanConnectionDescription::externalEndpoint_container_type external_endpoints;
      for (CORBA::ULong i {}; i < src.externalEndpoint.length (); ++i)
        {
          ComponentExternalPortEndpoint const endpoint (
            CEPE_Handler::external_port_endpoint (src.externalEndpoint[i]));
          external_endpoints.push_back (endpoint);
        }

      PlanConnectionDescription::internalEndpoint_container_type internal_endpoints;
      for (CORBA::ULong i {}; i < src.internalEndpoint.length (); ++i)
        {
          PlanSubcomponentPortEndpoint const endpoint (
            PSPE_Handler::sub_component_port_endpoint (src.internalEndpoint[i]));
          internal_endpoints.push_back (endpoint);
        }

      PlanConnectionDescription::externalReference_container_type external_references;
      for (CORBA::ULong i {}; i < src.externalReference.length (); ++i)
        {
          ExternalReferenceEndpoint const endpoint (
            ERE_Handler::external_ref_endpoint (src.externalReference[i]));
          external_references.push_back (endpoint);
        }

      PlanConnectionDescription::deployedResource_container_type resources;
      for (CORBA::ULong i {}; i < src.deployedResource.length (); ++i)
        {
          ConnectionResourceDeploymentDescription const resource (
            CRDD_Handler::connection_resource_depl_desc (src.deployedResource[i]));
          resources.push_back (resource);
        }

      return PlanConnectionDescription (name, source_arg, requirements, external_endpoints,
                                        internal_endpoints, external_references, resources);
    }
#endif // DANCE_CONFIG_HANDLERS_CORBA_TO_XSC
  }
}
