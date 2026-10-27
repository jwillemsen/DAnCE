#include "SatisfierProperty_Handler.h"
#include "Any_Handler.h"
#include "Basic_Deployment_Data.hpp"
#include "dance/Deployment/Deployment_DataC.h"
#include "dance/Logger/Log_Macros.h"
#include "Utils/Exceptions.h"
#include "dance/Logger/Log_Macros.h"

namespace DAnCE
{
  namespace Config_Handlers
  {

    SatisfierProperty_Handler::SatisfierProperty_Handler ()
    {
      DANCE_TRACE("SatisfierProperty_Handler::constructor");
    }

    SatisfierProperty_Handler::~SatisfierProperty_Handler ()
    {
      DANCE_TRACE("SatisfierProperty_Handler::destructor");
    }

    void
    SatisfierProperty_Handler::handle_sat_property (
                                                 SatisfierProperty const& desc,
                                                 Deployment::SatisfierProperty& toconfig)
    {
      DANCE_TRACE("SatisfierProperty_Handler::get_sat_property");

      toconfig.name = desc.name ().c_str ();

      switch (desc.kind ().integral ())
        {
        case SatisfierPropertyKind::Quantity:
          toconfig.kind = Deployment::Quantity;
          break;

        case SatisfierPropertyKind::Capacity:
          toconfig.kind = Deployment::Capacity;
          break;

        case SatisfierPropertyKind::Minimum:
          toconfig.kind = Deployment::Minimum;
          break;

        case SatisfierPropertyKind::Maximum:
          toconfig.kind = Deployment::Maximum;
          break;

        case SatisfierPropertyKind::Attribute:
          toconfig.kind = Deployment::Attribute;
          break;

        case SatisfierPropertyKind::Selection:
          toconfig.kind = Deployment::Selection;
          break;

        default:
          DANCE_ERROR (DANCE_LOG_TERMINAL_ERROR, (LM_ERROR, ACE_TEXT ("Unknown SatisfierPropertyKind\n")));
          throw Config_Error (desc.name (), "Unknown SatisfierPropertyKind");

        }

      toconfig.dynamic = desc.dynamic ();

      try
        {
          Any_Handler::extract_into_any (desc.value (), toconfig.value);
        }
      catch (Config_Error &ex)
        {
          ex.name_ = desc.name ();
          throw ex;
        }
    }

    SatisfierProperty SatisfierProperty_Handler::get_sat_property (Deployment::SatisfierProperty const& src)
    {
      DANCE_TRACE("SatisfierProperty_Handler::get_sat_property - reverse");

      XMLSchema::string<char> name (src.name);

      SatisfierPropertyKind const* spk {};

      switch (src.kind)
        {
        case Deployment::Quantity:
          spk = &SatisfierPropertyKind::Quantity;
          break;

        case Deployment::Capacity:
          spk = &SatisfierPropertyKind::Capacity;
          break;

        case Deployment::Minimum:
          spk = &SatisfierPropertyKind::Minimum;
          break;

        case Deployment::Maximum:
          spk = &SatisfierPropertyKind::Maximum;
          break;

        default:
          throw Plan_Error ("Invalid SatisfierPropertyKind");
        }

      bool dynamic = src.dynamic;

      Any value (Any_Handler::get_any (src.value));

      return SatisfierProperty (name, *spk, dynamic, value);
    }

  }
}
