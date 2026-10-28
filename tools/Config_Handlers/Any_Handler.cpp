#include /**/ "ace/pre.h"
#include "Common.h"

#include "Any_Handler.h"
#include "DataType_Handler.h"
#include "Basic_Deployment_Data.hpp"
#include "tao/AnyTypeCode/Any.h"
#include "dance/Logger/Log_Macros.h"
#include "DynAny_Handler/DynAny_Handler.h"

#include "ace/Null_Mutex.h"

namespace DAnCE
{
  namespace Config_Handlers
  {

    Any_Handler::Any_Handler ()
    {
      DANCE_TRACE("Any_Handler::constructor");
    }

    Any_Handler::~Any_Handler ()
    {
      DANCE_TRACE("Any_Handler::destructor");
    }

    void Any_Handler::extract_into_any (Any const& desc, CORBA::Any& toconfig)
    {
      DANCE_TRACE("Any_Handler::extract_into_any");
      try
        {
          DynamicAny::DynAny_var const dyn = DYNANY_HANDLER->extract_into_dynany (desc.type (), desc.value ());
          CORBA::Any_var any_safe (dyn->to_any ());
          toconfig = *any_safe;

          dyn->destroy ();
        }
      catch (CORBA::Exception const& ex)
        {
          DANCE_ERROR (DANCE_LOG_TERMINAL_ERROR, (LM_ERROR, DLINFO
                           ACE_TEXT ("Any_Handler::extract_into_any -")
                           ACE_TEXT (" Caught CORBA Exception while extracting into")
                           ACE_TEXT (" dynany: %C\n"),
                           ex._info ().c_str ()));
          throw Config_Error ("", "CORBA Exception while extracting into dynany");
        }
      catch (Config_Error const& ex)
        {
          throw ex;
        }
      catch (...)
        {
          throw Config_Error ("", "Caught error whilst parsing XML into Any");
        }
    }

  }
}
#include /**/ "ace/post.h"
