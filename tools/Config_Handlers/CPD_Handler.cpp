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
#if DANCE_CONFIG_HANDLERS_CORBA_TO_XSC

    ComponentPortDescription CPD_Handler::component_port_description (Deployment::ComponentPortDescription const& src)
    {
      DANCE_TRACE("CPD_Handler::component_port_description - reverse");
      XMLSchema::string<char> name (src.name);
      XMLSchema::boolean provider (src.provider);
      XMLSchema::boolean exclusiveProvider (src.exclusiveProvider);
      XMLSchema::boolean exclusiveUser (src.exclusiveUser);
      XMLSchema::boolean optional (src.optional);
      XMLSchema::string<char> specific_type_value (
        src.specificType.in () == nullptr ? "" : src.specificType.in ());
      XSCRT::OptionalArg<XMLSchema::string<char>> specific_type (std::nullopt);
      if (src.specificType.in () != nullptr)
        specific_type = XSCRT::OptionalArg<XMLSchema::string<char>> (specific_type_value);

      ComponentPortDescription::supportedType_container_type supported_types;
      for (CORBA::ULong i {}; i < src.supportedType.length (); ++i)
        supported_types.emplace_back (src.supportedType[i]);

      ComponentPortDescription::templateParam_container_type template_parameters;
      for (CORBA::ULong i {}; i < src.templateParam.length (); ++i)
        template_parameters.emplace_back (src.templateParam[i]);

      CCMComponentPortKind::Value kind (CCMComponentPortKind::Facet);

      switch (src.kind)
        {
        case ::Deployment::Facet:
          kind = CCMComponentPortKind::Facet;
          break;

        case ::Deployment::SimplexReceptacle:
          kind = CCMComponentPortKind::SimplexReceptacle;
          break;

        case ::Deployment::MultiplexReceptacle:
          kind = CCMComponentPortKind::MultiplexReceptacle;
          break;

        case ::Deployment::EventEmitter:
          kind = CCMComponentPortKind::EventEmitter;
          break;

        case ::Deployment::EventPublisher:
          kind = CCMComponentPortKind::EventPublisher;
          break;

        case ::Deployment::EventConsumer:
          kind = CCMComponentPortKind::EventConsumer;
          break;

        default:
          DANCE_DEBUG (DANCE_LOG_NONFATAL_ERROR,
            (LM_ERROR, ACE_TEXT("Invalid port kind in connection %C\n"),
            name.c_str ()));
          throw Config_Error (name.c_str (), "Invalid port kind in connection");
        }

      return ComponentPortDescription (name, provider, exclusiveProvider, exclusiveUser, optional,
                                       CCMComponentPortKind (kind), specific_type,
                                       supported_types, template_parameters);
    }
#endif // DANCE_CONFIG_HANDLERS_CORBA_TO_XSC

  }
}
