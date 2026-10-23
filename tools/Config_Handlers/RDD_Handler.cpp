#include "RDD_Handler.h"
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
                                            const ResourceDeploymentDescription &src,
                                            ::Deployment::ResourceDeploymentDescription &dest)
    {
      DANCE_TRACE("RDD_Handler::resource_deployment_descr");

      dest.requirementName = src.requirementName ().c_str ();

      dest.resourceName= src.resourceName ().c_str ();

      std::for_each (src.property_values ().begin (),
                     src.property_values ().end (),
                     Property_Functor (dest.property));

    }

    ResourceDeploymentDescription
    RDD_Handler::resource_deployment_descr (
                                            const ::Deployment::ResourceDeploymentDescription &src)
    {
      DANCE_TRACE("RDD_Handler::resource_deployment_descr - reverse");

      XMLSchema::string<char> reqname (src.requirementName);
      XMLSchema::string<char> resname (src.resourceName.in ());

      ResourceDeploymentDescription rdd (reqname,resname);

      for (CORBA::ULong i = 0;
           i != src.property.length ();
           ++i)
        {
          //rdd.add_property (Property_Handler::get_property (src.property[i]));
        }

      return rdd;
    }


  }

}
