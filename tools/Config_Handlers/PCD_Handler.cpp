#include "PCD_Handler.h"
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

      toconfig.name =(CORBA::string_dup (desc.name ().c_str ()));

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

    PlanConnectionDescription
    PCD_Handler::get_PlanConnectionDescription (Deployment::PlanConnectionDescription const& src)
    {
      DANCE_TRACE("PCD_Handler::get_PlanConnectionDescription");

      XMLSchema::string<char> name (src.name);

      PlanConnectionDescription pcd(name);

      //Get the source if it exists
      if(src.source.length() != 0)
        {
          XMLSchema::string<char> source(src.source[0]);
          pcd.source(source);
        }

      return pcd;
    }
  }
}
