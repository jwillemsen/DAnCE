#include "DynSequence_Handler.h"
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
    DynSequence_Handler::extract_into_dynany (DataType const& type,
                                              DataValue const& value,
                                              CORBA::TypeCode_ptr req_tc)
    {
      DANCE_TRACE("DynSequence_Handler::extract_into_dynany");

      ::DAnCE::Config_Handlers::SequenceType const* sequenceType = type.sequence_if_set ();
      if (!sequenceType)
        throw Config_Error ("", "Expected sequence type information");

      CORBA::TypeCode_var tc {};
      if (req_tc)
        tc = req_tc;
      else
        tc = DynSequence_Handler::create_typecode (type);

      // Make the actual DynSequence
      DynamicAny::DynAny_var const temp = DYNANY_HANDLER->daf ()->create_dyn_any_from_type_code (tc);
      DynamicAny::DynSequence_var retval = DynamicAny::DynSequence::_narrow (temp.in ());

      DynamicAny::DynAnySeq dynseq {};
      CORBA::ULong pos {};

      switch (sequenceType->elementType ().kind ().integral ())
        {
          // ========== BASIC TYPES
        case TCKind::tk_null:
        case TCKind::tk_void:
        case TCKind::tk_objref:
        case TCKind::tk_fixed:
        case TCKind::tk_component:
        case TCKind::tk_home:

          DANCE_DEBUG (DANCE_LOG_TERMINAL_ERROR,
            (LM_WARNING, "Null and void element types are not supported in sequences\n"));
          throw Config_Error ("", "Null or void element types are not supported in sequences");

        case TCKind::tk_short:
        case TCKind::tk_long:
        case TCKind::tk_ushort:
        case TCKind::tk_ulong:
        case TCKind::tk_float:
        case TCKind::tk_double:
        case TCKind::tk_boolean:
        case TCKind::tk_octet:
        case TCKind::tk_string:
        case TCKind::tk_longlong:
        case TCKind::tk_ulonglong:
        case TCKind::tk_longdouble:
        case TCKind::tk_wstring:
        case TCKind::tk_enum:
        case TCKind::tk_struct:
        case TCKind::tk_sequence:
        case TCKind::tk_array:
        case TCKind::tk_alias:
          try
            {
              dynseq.length (value.element ().size ());
              retval->set_length (value.element ().size ());

              for (auto const& element : value.element ())
                {
                  DynamicAny::DynAny_var const dynany (
                    DYNANY_HANDLER->extract_into_dynany (sequenceType->elementType (), element));
                  dynseq[pos++] = dynany->copy ();
                }

              retval->set_elements_as_dyn_any (dynseq);
              return retval._retn ();
            }
          catch (DynamicAny::DynAny::InvalidValue const&)
            {
              DANCE_DEBUG (DANCE_LOG_TERMINAL_ERROR,
                (LM_ERROR,
                 ACE_TEXT ("Invalid value provided in XML when trying to ")
                 ACE_TEXT ("populate %ith element of a sequence\n"),
                 pos));
              throw Config_Error ("", "Invalid value whilst populating the sequence.");
            }

        case TCKind::tk_char:
        case TCKind::tk_wchar:
          {
            if (!value.element ().empty () || value.string ().size () != 1)
              throw Config_Error ("", "Character sequences require one string value");

            std::string const& characters = value.string ().front ();
            ::XMLSchema::unsignedInt const* bound_if_set_ptr = sequenceType->bound_if_set ();
            if (bound_if_set_ptr && characters.size () > *bound_if_set_ptr)
              throw Config_Error ("", "Character sequence exceeds its bound");
            dynseq.length (characters.size ());
            retval->set_length (characters.size ());
            for (char character : characters)
              {
                if (sequenceType->elementType ().kind ().integral () == TCKind::tk_wchar &&
                    static_cast<unsigned char> (character) > 0x7f)
                  throw Config_Error ("", "Non-ASCII wchar values are not supported");

                DataValue::string_container_type character_value {std::string (1, character)};
                DataValue const element_value ({}, {}, {}, {}, {}, {}, {}, {}, {}, character_value);
                DynamicAny::DynAny_var const dynany (
                  DYNANY_HANDLER->extract_into_dynany (sequenceType->elementType (), element_value));
                dynseq[pos++] = dynany->copy ();
              }
            retval->set_elements_as_dyn_any (dynseq);
            return retval._retn ();
          }

        case TCKind::tk_any:
        case TCKind::tk_TypeCode:
        case TCKind::tk_Principal:
        case TCKind::tk_union:
        case TCKind::tk_except:
        case TCKind::tk_value:
        case TCKind::tk_value_box:
        case TCKind::tk_native:
        case TCKind::tk_abstract_interface:
        case TCKind::tk_local_interface:
        case TCKind::tk_event:
          // Special case where element association in datavalue contains another datavalue.

          DANCE_DEBUG (DANCE_LOG_TERMINAL_ERROR,
            (LM_ERROR,
             DLINFO ACE_TEXT ("DynSequence_Handler::extract_into_dynany - Type not supported\n")));
          throw Config_Error ("", "Type not supported");
        }

      return retval._retn ();
    }

    CORBA::TypeCode_ptr DynSequence_Handler::create_typecode (DataType const& type)
    {
      DANCE_TRACE("DynSequence_Handler::create_typecode");

      ::DAnCE::Config_Handlers::SequenceType const* sequence_if_set_ptr = type.sequence_if_set ();
      if (!sequence_if_set_ptr)
        {
          DANCE_DEBUG (DANCE_LOG_TERMINAL_ERROR, (LM_ERROR, ACE_TEXT ("ERROR: Sequence type description required")));
          throw Config_Error ("", "Expected <sequence> element, incorrect tc_kind.");
        }

      CORBA::TypeCode_var etc = DYNANY_HANDLER->create_typecode (sequence_if_set_ptr->elementType ());

      CORBA::ULong bound {};
      ::XMLSchema::unsignedInt const* bound_if_set_ptr = sequence_if_set_ptr->bound_if_set ();
      if (bound_if_set_ptr)
        bound = *bound_if_set_ptr;

      CORBA::TypeCode_var tc = DYNANY_HANDLER->orb ()->create_sequence_tc (bound, etc);

      return tc._retn ();
    }
  }
}

