#include "RDD_Handler.h"
#include "Common.h"
#include "Basic_Deployment_Data.hpp"
#include "dance/Deployment/Deployment_DataC.h"
#include "Any_Handler.h"
#include "dance/Logger/Log_Macros.h"
#include "Property_Handler.h"

namespace DAnCE
{
  namespace Config_Handlers
  {
    void
    RDD_Handler::resource_deployment_descr (
                                            ResourceDeploymentDescription const& src,
                                            ::Deployment::ResourceDeploymentDescription &dest)
    {
      DANCE_TRACE("RDD_Handler::resource_deployment_descr");

      dest.requirementName = src.requirementName ().c_str ();

      dest.resourceName= src.resourceName ().c_str ();

      CORBA::ULong property_index {};
      for (auto const& source_item : src.property ())
        Property_Handler::handle_property (source_item, dest.property[property_index++]);

    }
#if DANCE_CONFIG_HANDLERS_CORBA_TO_XSC

    ResourceDeploymentDescription
    RDD_Handler::resource_deployment_descr (
                                            ::Deployment::ResourceDeploymentDescription const& src)
    {
      DANCE_TRACE("RDD_Handler::resource_deployment_descr - reverse");

      XMLSchema::string<char> reqname (src.requirementName);
      XMLSchema::string<char> resname (src.resourceName.in ());

      ResourceDeploymentDescription rdd (reqname,resname);

      return rdd;
    }
#endif // DANCE_CONFIG_HANDLERS_CORBA_TO_XSC

  }

}
