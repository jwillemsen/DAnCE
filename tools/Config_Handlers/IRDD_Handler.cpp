#include "IRDD_Handler.h"
#include "Basic_Deployment_Data.hpp"
#include "dance/Deployment/Deployment_DataC.h"
#include "Property_Handler.h"
#include "dance/Logger/Log_Macros.h"

namespace DAnCE
{
  namespace Config_Handlers
  {

    IRDD_Handler::IRDD_Handler (void)
    {
    }

    IRDD_Handler::~IRDD_Handler (void)
    {
    }

    void
    IRDD_Handler::handle_irdd (InstanceResourceDeploymentDescription const& desc,
                               Deployment::InstanceResourceDeploymentDescription& toconfig)
    {
      DANCE_TRACE("IRDD_Handler::instance_resource_deployment_descr");

      switch (desc.resourceUsage ().integral ())
        {
        case ResourceUsageKind::None_l:
          toconfig.resourceUsage = Deployment::None;
          break;

        case ResourceUsageKind::InstanceUsesResource_l:
          toconfig.resourceUsage = Deployment::InstanceUsesResource;
          break;

        case ResourceUsageKind::ResourceUsesInstance_l:
          toconfig.resourceUsage = Deployment::ResourceUsesInstance;
          break;

        case ResourceUsageKind::PortUsesResource_l:
          toconfig.resourceUsage = Deployment::PortUsesResource;
          break;

        case ResourceUsageKind::ResourceUsesPort_l:
          toconfig.resourceUsage = Deployment::ResourceUsesPort;
          break;

        default:
          throw Config_Error (desc.requirementName (), "Unknown ResourceUsageKind.");
          break;
        }

      toconfig.requirementName = desc.requirementName ().c_str ();

      toconfig.resourceName = desc.resourceName ().c_str ();

      toconfig.property.length (desc.property ().size ());
      CORBA::ULong property_index {};
      for (auto const& source_item : desc.property ())
        Property_Handler::handle_property (source_item, toconfig.property[property_index++]);
    }

    InstanceResourceDeploymentDescription
    IRDD_Handler::instance_resource_deployment_descr (Deployment::InstanceResourceDeploymentDescription const& src)
    {
      DANCE_TRACE("IRDD_Handler::instance_resource_deployment_descr - reverse");

      XMLSchema::string<char> reqname (src.requirementName);
      XMLSchema::string<char> resname (src.resourceName);

      InstanceResourceDeploymentDescription irdd (ResourceUsageKind::None, reqname, resname);

      switch (src.resourceUsage)
        {
        case Deployment::None:
          irdd.resourceUsage (ResourceUsageKind::None);
          break;

        case Deployment::InstanceUsesResource:
          irdd.resourceUsage (ResourceUsageKind::InstanceUsesResource);
          break;

        case Deployment::ResourceUsesInstance:
          irdd.resourceUsage (ResourceUsageKind::ResourceUsesInstance);
          break;

        case Deployment::PortUsesResource:
          irdd.resourceUsage (ResourceUsageKind::PortUsesResource);
          break;

        case Deployment::ResourceUsesPort:
          irdd.resourceUsage (ResourceUsageKind::ResourceUsesPort);
          break;

        default:
          throw Config_Error (src.resourceName.in (), "Unknown ResourceUsageKind.");
          break;
        }

      for (CORBA::ULong i = 0; i < src.property.length (); ++i)
        {
          //irdd.add_property (Property_Handler::get_property (src.property[i]));
        }

      return irdd;
    }
  }

}
