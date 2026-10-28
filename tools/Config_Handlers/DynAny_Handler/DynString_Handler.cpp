#include "DynString_Handler.h"
#include "DynAny_Handler.h"
#include "dance/Logger/Log_Macros.h"
#include "Basic_Deployment_Data.hpp"
#include "Common.h"

#include "ace/Null_Mutex.h"

#include "tao/AnyTypeCode/TypeCode.h"
#include "tao/TypeCodeFactory/TypeCodeFactory_Adapter_Impl.h"
#include "tao/AnyTypeCode/AnyTypeCode_methods.h"
#include "tao/AnyTypeCode/Sequence_TypeCode.h"

namespace DAnCE
{
  namespace Config_Handlers
  {
    DynamicAny::DynAny_ptr
    DynString_Handler::extract_into_dynany (DataType const& type,
                                            DataValue const& value,
                                            CORBA::TypeCode_ptr req_tc)
    {
      DANCE_TRACE("DynString_Handler::extract_into_dynany");

      CORBA::TypeCode_var tc {};
      if (req_tc)
        tc = req_tc;
      else
        tc = DynString_Handler::create_typecode (type);

      // Make the actual DynAny
      DynamicAny::DynAny_var retval = DYNANY_HANDLER->daf ()->create_dyn_any_from_type_code (tc);

      if (value.string ().size () != 1)
        throw Config_Error ("", "Expected exactly one string value");

      retval->insert_string (value.string ().front ().c_str ());

      return retval._retn ();
    }

    CORBA::TypeCode_ptr DynString_Handler::create_typecode (DataType const& type)
    {
      DANCE_TRACE("DynString_Handler::create_typecode");

      if (type.kind ().integral () != TCKind::tk_string)
        {
          DANCE_DEBUG (DANCE_LOG_TERMINAL_ERROR, (LM_ERROR, ACE_TEXT ("ERROR: String type description required")));
          throw Config_Error ("", "Expected <string> element, incorrect tc_kind.");
        }

      CORBA::ULong bound {};
      ::DAnCE::Config_Handlers::BoundedStringType const* boundedString_if_set_ptr = type.boundedString_if_set ();
      if (boundedString_if_set_ptr)
        {
          if (!boundedString_if_set_ptr->bound ().empty ())
          {
            bound = boundedString_if_set_ptr->bound ().front ();
          }
        }

      CORBA::TypeCode_var tc = DYNANY_HANDLER->orb ()->create_string_tc (bound);

      return tc._retn ();
    }
  }
}

