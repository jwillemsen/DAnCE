#include "CPD_Handler.h"
#include "Basic_Deployment_Data.hpp"
#include "Common.h"
#include "dance/Deployment/Deployment_DataC.h"
#include "dance/Logger/Log_Macros.h"

namespace DAnCE
{
  namespace Config_Handlers
  {
    void
    CPD_Handler::handle_component_port_description (
                                             ComponentPortDescription const& desc,
                                             Deployment::ComponentPortDescription& toconfig)
    {
      DANCE_TRACE("CPD_Handler::component_port_description");
      toconfig.name = desc.name ().c_str ();

      ::XMLSchema::string<char> const* specificType_if_set_ptr = desc.specificType_if_set ();
      if (specificType_if_set_ptr)
        toconfig.specificType = specificType_if_set_ptr->c_str ();

      toconfig.supportedType.length (desc.supportedType ().size ());
      CORBA::ULong supportedType_index {};
      for (auto const& source_item : desc.supportedType ())
        toconfig.supportedType[supportedType_index++] = source_item.c_str ();

      switch (desc.kind ().integral ())
        {
        case CCMComponentPortKind::Facet:
          toconfig.kind = Deployment::Facet;
          break;
        case CCMComponentPortKind::SimplexReceptacle:
          toconfig.kind = Deployment::SimplexReceptacle;
          break;
        case CCMComponentPortKind::MultiplexReceptacle:
          toconfig.kind = Deployment::MultiplexReceptacle;
          break;
        case CCMComponentPortKind::EventEmitter:
          toconfig.kind = Deployment::EventEmitter;
          break;
        case CCMComponentPortKind::EventPublisher:
          toconfig.kind = Deployment::EventPublisher;
          break;
        case CCMComponentPortKind::EventConsumer:
          toconfig.kind = Deployment::EventConsumer;
          break;

        default:
          DANCE_DEBUG (DANCE_LOG_TERMINAL_ERROR,
            (LM_ERROR, "Invalid port type in connection %C\n",
             desc.name ().c_str ()));
          throw Config_Error (desc.name ().c_str (), "Invalid port type");
        }

      toconfig.provider = desc.provider ();
      toconfig.exclusiveProvider = desc.exclusiveProvider ();
      toconfig.exclusiveUser = desc.exclusiveUser ();
      toconfig.optional = desc.optional ();
    }

  }
}
