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

      for (auto const& source_item : src.internalEndpoint ())
        PSPE_Handler::sub_component_port_endpoint (source_item, dest[pos++]);
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
#if DANCE_CONFIG_HANDLERS_CORBA_TO_XSC

    PlanSubcomponentPortEndpoint
    PSPE_Handler::sub_component_port_endpoint (
      Deployment::PlanSubcomponentPortEndpoint const& src)
    { // @@MAJO
      DANCE_TRACE("PSPE_Handler::sub_component_port_endpoint - reverse");
      XMLSchema::string<char> pname (src.portName);
      XMLSchema::string<char> tval  ("true");
      XMLSchema::string<char> prov  ("");
      std::string id {};
      IDD_Handler::IDREF.find_ref(src.instanceRef, id);
      XMLSchema::IDREF<char> idref(id.c_str());

      if (src.provider)
        prov = tval;

      IdRef idr {};
      idr.idref (idref);

      PlanSubcomponentPortEndpoint pspe (pname, CCMComponentPortKind::Facet, idr);
      pspe.provider (prov);

      switch (src.kind)
        {
        case ::Deployment::Facet:
          pspe.kind (CCMComponentPortKind::Facet);
          break;

        case ::Deployment::SimplexReceptacle:
          pspe.kind (CCMComponentPortKind::SimplexReceptacle);
          break;

        case ::Deployment::MultiplexReceptacle:
          pspe.kind (CCMComponentPortKind::MultiplexReceptacle);
          break;

        case ::Deployment::EventEmitter:
          pspe.kind (CCMComponentPortKind::EventEmitter);
          break;

        case ::Deployment::EventPublisher:
          pspe.kind (CCMComponentPortKind::EventPublisher);
          break;

        case ::Deployment::EventConsumer:
          pspe.kind (CCMComponentPortKind::EventConsumer);
          break;

        default:
          DANCE_ERROR (DANCE_LOG_NONFATAL_ERROR, (LM_ERROR, "Invalid port kind in PSPE\n"));
        }

      return pspe;
    }
#endif // DANCE_CONFIG_HANDLERS_CORBA_TO_XSC
  }
}
