#include "CRDD_Handler.h"
#include "Any_Handler.h"
#include "Basic_Deployment_Data.hpp"
#include "dance/Deployment/Deployment_DataC.h"
#include "Property_Handler.h"

#include "dance/Logger/Log_Macros.h"

namespace DAnCE
{
  namespace Config_Handlers
  {

    CRDD_Handler::CRDD_Handler ()
    {
    }

    CRDD_Handler::~CRDD_Handler ()
    {
    }

    ///This method takes a <Deployment::ConnectionResourceDeploymentDescription>
    ///and maps the values from the passed in XSC
    ///ConnectionResourceDeploymentDescription to its members.
    void CRDD_Handler::get_ConnectionResourceDeploymentDescription (
      Deployment::ConnectionResourceDeploymentDescription& toconfig,
      ConnectionResourceDeploymentDescription const& desc)
    {
      DANCE_TRACE("CRDD_Handler::get_ConnectionResourceDD");

      toconfig.targetName = desc.targetName ().c_str ();
      toconfig.requirementName = desc.requirementName ().c_str ();
      toconfig.resourceName = desc.resourceName ().c_str ();

      toconfig.property.length (desc.property ().size ());
      CORBA::ULong property_index {};
      for (auto const& source_item : desc.property ())
        Property_Handler::handle_property (source_item, toconfig.property[property_index++]);
    }

    ConnectionResourceDeploymentDescription
    CRDD_Handler::connection_resource_depl_desc (
                                                 ::Deployment::ConnectionResourceDeploymentDescription const& src)
    {
      DANCE_TRACE("CRDD_Handler::get_ConnectionResourceDeploymentDescription- reverse");
      XMLSchema::string<char> tname (src.targetName);
      XMLSchema::string<char> reqname (src.requirementName);
      XMLSchema::string<char> resname (src.resourceName);

      ConnectionResourceDeploymentDescription crdd (tname, reqname, resname);

      for (CORBA::ULong i {}; i != src.property.length (); ++i)
        {
          //crdd.add_property (Property_Handler::get_property (src.property[i]));
        }

      return crdd;
    }

  }
}
