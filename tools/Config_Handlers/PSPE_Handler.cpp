#include "PSPE_Handler.h"
#include "IDD_Handler.h"
#include "Basic_Deployment_Data.hpp"
#include "Common.h"
#include "dance/Deployment/Deployment_DataC.h"

namespace DAnCE
{
  namespace Config_Handlers
  {
    void
    PSPE_Handler::sub_component_port_endpoints (
                                                PlanConnectionDescription const& src,
                                                ::Deployment::PlanSubcomponentPortEndpoints &dest)
    {
      DANCE_TRACE("PSPE_Handler::sub_component_port_endpoints");

      CORBA::ULong pos {};
      dest.length (src.internalEndpoint ().size ());

      for (auto const& endpoint : src.internalEndpoint ())
        {
          PSPE_Handler::sub_component_port_endpoint (endpoint, dest[pos++]);
        }
    }

    void
    PSPE_Handler::sub_component_port_endpoint (
      PlanSubcomponentPortEndpoint const& src,
      ::Deployment::PlanSubcomponentPortEndpoint &dest)
    {
      DANCE_TRACE("PSPE_Handler::sub_component_port_endpoint");
      dest.portName = src.portName ().c_str ();

      ::XMLSchema::string<char> const* provider_if_set_ptr = src.provider_if_set ();
      if (provider_if_set_ptr)
        {
          dest.provider = *provider_if_set_ptr == "true";
        }
      else
        {
          dest.provider = false;
        }

      CORBA::ULong tmp {};

      ::XMLSchema::IDREF<char> const* instanceRef = src.instance ().idref_if_set ();
      if (!instanceRef)
        throw Config_Error ("", "Plan subcomponent port endpoint has no instance IDREF");

      IDD_Handler::IDREF.find_ref (instanceRef->id ().c_str (), tmp);

      dest.instanceRef = tmp;

      switch (src.kind ().integral ())
        {
        case CCMComponentPortKind::Facet:
          dest.kind = Deployment::Facet;
          break;

        case CCMComponentPortKind::SimplexReceptacle:
          dest.kind = Deployment::SimplexReceptacle;
          break;

        case CCMComponentPortKind::MultiplexReceptacle:
          dest.kind = Deployment::MultiplexReceptacle;
          break;

        case CCMComponentPortKind::EventEmitter:
          dest.kind = Deployment::EventEmitter;
          break;

        case CCMComponentPortKind::EventPublisher:
          dest.kind = Deployment::EventPublisher;
          break;

        case CCMComponentPortKind::EventConsumer:
          dest.kind = Deployment::EventConsumer;
          break;

        default:
          throw Config_Error ("", "Invalid port kind in plan subcomponent endpoint");

        }
    }

  }
}
