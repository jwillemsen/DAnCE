#include "IRDD_Handler.h"
#include "Common.h"
#include "Basic_Deployment_Data.hpp"
#include "dance/Deployment/Deployment_DataC.h"
#include "Property_Handler.h"
#include "dance/Logger/Log_Macros.h"

namespace DAnCE
{
  namespace Config_Handlers
  {

    IRDD_Handler::IRDD_Handler ()
    {
    }

    IRDD_Handler::~IRDD_Handler ()
    {
    }

    void
    IRDD_Handler::handle_irdd (InstanceResourceDeploymentDescription const& desc,
                               Deployment::InstanceResourceDeploymentDescription& toconfig)
    {
      DANCE_TRACE("IRDD_Handler::instance_resource_deployment_descr");

      switch (desc.resourceUsage ().integral ())
        {
        case ResourceUsageKind::None:
          toconfig.resourceUsage = Deployment::None;
          break;

        case ResourceUsageKind::InstanceUsesResource:
          toconfig.resourceUsage = Deployment::InstanceUsesResource;
          break;

        case ResourceUsageKind::ResourceUsesInstance:
          toconfig.resourceUsage = Deployment::ResourceUsesInstance;
          break;

        case ResourceUsageKind::PortUsesResource:
          toconfig.resourceUsage = Deployment::PortUsesResource;
          break;

        case ResourceUsageKind::ResourceUsesPort:
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
#if DANCE_CONFIG_HANDLERS_CORBA_TO_XSC

    InstanceResourceDeploymentDescription
    IRDD_Handler::instance_resource_deployment_descr (Deployment::InstanceResourceDeploymentDescription const& src)
    {
      DANCE_TRACE("IRDD_Handler::instance_resource_deployment_descr - reverse");

      XMLSchema::string<char> reqname (src.requirementName);
      XMLSchema::string<char> resname (src.resourceName);

      ResourceUsageKind::Value usage_value {};

      switch (src.resourceUsage)
        {
        case Deployment::None:
          usage_value = ResourceUsageKind::None;
          break;

        case Deployment::InstanceUsesResource:
          usage_value = ResourceUsageKind::InstanceUsesResource;
          break;

        case Deployment::ResourceUsesInstance:
          usage_value = ResourceUsageKind::ResourceUsesInstance;
          break;

        case Deployment::PortUsesResource:
          usage_value = ResourceUsageKind::PortUsesResource;
          break;

        case Deployment::ResourceUsesPort:
          usage_value = ResourceUsageKind::ResourceUsesPort;
          break;

        default:
          throw Config_Error (src.resourceName.in (), "Unknown ResourceUsageKind.");
        }

      ResourceUsageKind const usage (usage_value);
      InstanceResourceDeploymentDescription::property_container_type properties;
      for (CORBA::ULong i {}; i < src.property.length (); ++i)
        {
          Property const property (Property_Handler::get_property (src.property[i]));
          properties.push_back (property);
        }

      return InstanceResourceDeploymentDescription (usage, reqname, resname, properties);
    }
#endif // DANCE_CONFIG_HANDLERS_CORBA_TO_XSC
  }

}
