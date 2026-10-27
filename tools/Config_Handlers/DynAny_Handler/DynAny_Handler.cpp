#include /**/ "ace/pre.h"

#include "dance/Logger/Log_Macros.h"
#include "DynAny_Handler.h"
#include "DynEnum_Handler.h"
#include "DynSequence_Handler.h"
#include "DynStruct_Handler.h"
#include "DynAlias_Handler.h"
#include "DynArray_Handler.h"
#include "DynString_Handler.h"
#include "Basic_Deployment_Data.hpp"
#include "Common.h"

namespace DAnCE
{
  namespace Config_Handlers
  {
    DynAny_Handler::DynAny_Handler ()
    {
      DANCE_TRACE("DynAny_Handler::constructor");

      int argc {};
      char **argv {};

      orb_ = CORBA::ORB_init (argc, argv);

      CORBA::Object_var const temp = orb_->resolve_initial_references ("DynAnyFactory");
      daf_ = DynamicAny::DynAnyFactory::_narrow (temp.in ());

      if (CORBA::is_nil (daf_.in ()))
        {
          DANCE_ERROR (DANCE_LOG_TERMINAL_ERROR, (LM_ERROR, ACE_TEXT ("Unable to narrow Dynamic Any factory\n")));
          throw Config_Error ("", "Unable to narrow DynAny factory");
        }
    }

    DynAny_Handler::~DynAny_Handler ()
    {
      DANCE_TRACE("DynAny_Handler::destructor");
    }

    CORBA::ORB_ptr DynAny_Handler::orb ()
    {
      DANCE_TRACE("DynAny_Handler::orb");

      return orb_.in ();
    }

    DynamicAny::DynAnyFactory_ptr DynAny_Handler::daf ()
    {
      DANCE_TRACE("DynAny_Handler::daf");

      return daf_.in ();
    }

    DynamicAny::DynAny_ptr
    DynAny_Handler::extract_into_dynany (DataType const& type,
                                         DataValue const& value,
                                         CORBA::TypeCode_ptr req_tc)
    {
      DANCE_TRACE("DynAny_Handler::extract_into_dynany");

      DynamicAny::DynAny_var retval {};

      if (req_tc)
        retval = this->daf_->create_dyn_any_from_type_code (req_tc);

      TCKind::Value const tkind = type.kind ().integral ();

      if (value._cxx_short ().size () > 1 || value._cxx_long ().size () > 1 ||
          value.ushort ().size () > 1 || value.ulong ().size () > 1 ||
          value._cxx_float ().size () > 1 || value._cxx_double ().size () > 1 ||
          value.boolean ().size () > 1 || value.octet ().size () > 1 ||
          value.longlong ().size () > 1 || value.ulonglong ().size () > 1 ||
          value.longdouble ().size () > 1 || value._cxx_enum ().size () > 1 ||
          value.string ().size () > 1)
        {
          throw Config_Error ("", "Multiple scalar values provided for data type");
        }

      if ((!value._cxx_short ().empty () && tkind != TCKind::tk_short) ||
          (!value._cxx_long ().empty () && tkind != TCKind::tk_long) ||
          (!value.ushort ().empty () && tkind != TCKind::tk_ushort ) ||
          (!value.ulong ().empty () && tkind != TCKind::tk_ulong) ||
          (!value._cxx_float ().empty () && tkind != TCKind::tk_float) ||
          (!value._cxx_double ().empty () && tkind != TCKind::tk_double) ||
          (!value.boolean ().empty () && tkind != TCKind::tk_boolean) ||
          (!value.octet ().empty () && tkind != TCKind::tk_octet) ||
          (!value.longlong ().empty () && tkind != TCKind::tk_longlong) ||
          (!value.ulonglong ().empty () && tkind != TCKind::tk_ulonglong) ||
          (!value.string ().empty () && !(tkind == TCKind::tk_string ||
                                      tkind == TCKind::tk_char ||
                                      tkind == TCKind::tk_wchar)))
        {
          throw Config_Error ("", "Wrong value type for data type");
        }

      switch (tkind)
        {
          // ========== BASIC TYPES
        case TCKind::tk_null:
        case TCKind::tk_void:
          DANCE_ERROR (DANCE_LOG_TERMINAL_ERROR,
            (LM_ERROR,
            ACE_TEXT ("DynAny_Handler::extract_into_dynany - Don't know how to handle null or void types\n")));
          throw Config_Error ("", "Null or void types not supported");
          break;

        case TCKind::tk_short:
          {
            if (!req_tc)
              {
                retval = this->daf_->create_dyn_any_from_type_code (CORBA::_tc_short);
              }
            CORBA::Short s {};
            if (!value._cxx_short ().empty ())
              {
                s = value._cxx_short ().front ();
              }
            retval->insert_short (s);
          }
          break;

        case TCKind::tk_long:
          {
            if (!req_tc)
              {
                retval = this->daf_->create_dyn_any_from_type_code (CORBA::_tc_long);
              }
            CORBA::Long l {};
            if (!value._cxx_long ().empty ())
              {
                l = value._cxx_long ().front ();
              }
            retval->insert_long (l);
          }
          break;

        case TCKind::tk_ushort:
          {
            if (!req_tc)
              {
                retval = this->daf_->create_dyn_any_from_type_code (CORBA::_tc_ushort);
              }
            CORBA::UShort us {};
            if (!value.ushort ().empty ())
              {
                us = value.ushort ().front ();
              }
            retval->insert_ushort (us);
          }
          break;

        case TCKind::tk_ulong:
          {
            if (!req_tc)
              {
                retval = this->daf_->create_dyn_any_from_type_code (CORBA::_tc_ulong);
              }
            CORBA::ULong ul {};
            if (!value.ulong ().empty ())
              {
                ul = value.ulong ().front ();
              }
            retval->insert_ulong (ul);
          }
          break;

        case TCKind::tk_float:
          {
            if (!req_tc)
              {
                retval = this->daf_->create_dyn_any_from_type_code (CORBA::_tc_float);
              }
            CORBA::Float f {};
            if (!value._cxx_float ().empty ())
              {
                f = value._cxx_float ().front ();
              }
            retval->insert_float (f);
          }
          break;

        case TCKind::tk_double:
          {
            if (!req_tc)
              {
                retval = this->daf_->create_dyn_any_from_type_code (CORBA::_tc_double);
              }
            CORBA::Double d {};
            if (!value._cxx_double ().empty ())
              {
                d = value._cxx_double ().front ();
              }
            retval->insert_double (d);
          }
          break;

        case TCKind::tk_boolean:
          {
            if (!req_tc)
              {
                retval = this->daf_->create_dyn_any_from_type_code (CORBA::_tc_boolean);
              }
            CORBA::Boolean b {};
            if (!value.boolean ().empty ())
              {
                b = value.boolean ().front ();
              }
            retval->insert_boolean (b);
          }
          break;

        case TCKind::tk_octet:
          {
            if (!req_tc)
              {
                retval = this->daf_->create_dyn_any_from_type_code (CORBA::_tc_octet);
              }
            CORBA::Octet o {};
            if (!value.octet ().empty ())
              {
                o = value.octet ().front ();
              }
            retval->insert_octet (o);
          }
          break;

        case TCKind::tk_longlong:
          {
            if (!req_tc)
              {
                retval = this->daf_->create_dyn_any_from_type_code (CORBA::_tc_longlong);
              }
            CORBA::LongLong ll {};
            if (!value.longlong ().empty ())
              {
                ll = value.longlong ().front ();
              }
            retval->insert_longlong (ll);
          }
          break;

        case TCKind::tk_ulonglong:
          {
            if (!req_tc)
              {
                retval = this->daf_->create_dyn_any_from_type_code (CORBA::_tc_ulonglong);
              }
            CORBA::ULongLong ull {};
            if (!value.ulonglong ().empty ())
              {
                ull = value.ulonglong ().front ();
              }
            retval->insert_ulonglong (ull);
          }
          break;

        case TCKind::tk_string:
          return DynString_Handler::extract_into_dynany (type, value, req_tc);

        case TCKind::tk_char:
          {
            if (!req_tc)
              {
                retval = this->daf_->create_dyn_any_from_type_code (CORBA::_tc_char);
              }
            CORBA::Char c {};
            if (!value.string ().empty ())
              {
                c = *value.string ().front ().c_str ();
              }
            retval->insert_char (c);
          }
          break;

        case TCKind::tk_wchar:
          {
            if (!req_tc)
              {
                retval = this->daf_->create_dyn_any_from_type_code (CORBA::_tc_wchar);
              }
            CORBA::WChar wc {};
            if (!value.string ().empty ())
              {
                ::XMLSchema::string<char> const& character_value = value.string ().front ();
                if (character_value.size () != 1 ||
                    static_cast<unsigned char> (character_value.front ()) > 0x7f)
                  {
                    throw Config_Error ("", "Non-ASCII wchar values are not supported");
                  }
                wc = character_value.front ();
              }
            retval->insert_wchar (wc);
          }
          break;

        case TCKind::tk_enum:
          return DynEnum_Handler::extract_into_dynany (type, value, req_tc);

        case TCKind::tk_sequence:
          return DynSequence_Handler::extract_into_dynany (type, value, req_tc);

        case TCKind::tk_struct:
          return DynStruct_Handler::extract_into_dynany (type, value, req_tc);

        case TCKind::tk_alias:
          return DynAlias_Handler::extract_into_dynany (type, value, req_tc);

        case TCKind::tk_array:
          return DynArray_Handler::extract_into_dynany (type, value, req_tc);

        case TCKind::tk_longdouble:
          // Not supported since a longdouble is defined in the xsd as double.
          // We are then creating a long double initialized with a regular
          // double. This is a very tricky conversion and doesn't work in
          // combination with certain (versions of) compilers.
        case TCKind::tk_wstring:
        case TCKind::tk_fixed:
        case TCKind::tk_any:
        case TCKind::tk_TypeCode:
        case TCKind::tk_Principal:
        case TCKind::tk_objref:
        case TCKind::tk_union:
        case TCKind::tk_except:
        case TCKind::tk_value:
        case TCKind::tk_value_box:
        case TCKind::tk_native:
        case TCKind::tk_abstract_interface:
        case TCKind::tk_local_interface:
        case TCKind::tk_component:
        case TCKind::tk_home:
        case TCKind::tk_event:
          DANCE_ERROR (DANCE_LOG_TERMINAL_ERROR, (LM_ERROR,
            DLINFO ACE_TEXT ("DynAny_Handler::extract_into_dynany - Type not supported\n")));
          throw Config_Error ("", "Type not supported");
        }

      return retval._retn ();
    }

    Any DynAny_Handler::extract_from_dynany (CORBA::Any const& any)
    {
      DANCE_TRACE("DynAny_Handler::extract_from_dynany");

      DynamicAny::DynAny_var const dyn = this->daf_->create_dyn_any (any);

      DataValue const val {};
      Any retval (TCKind::tk_char, val);
      return retval;
    }

    CORBA::TypeCode_ptr DynAny_Handler::create_typecode (DataType const& type)
    {
      DANCE_TRACE("DynAny_Handler::create_typecode");

      switch (type.kind ().integral ())
        {
          // ========== BASIC TYPES
        case TCKind::tk_null:
        case TCKind::tk_void:
          DANCE_DEBUG (DANCE_LOG_TERMINAL_ERROR, (LM_WARNING,
            ACE_TEXT ("I don't know how to handle null or void types\n")));
          throw Config_Error ("", "Null or void types not supported");
          break;

        case TCKind::tk_short:
          return CORBA::_tc_short;

        case TCKind::tk_long:
          return CORBA::_tc_long;

        case TCKind::tk_ushort:
          return CORBA::_tc_ushort;

        case TCKind::tk_ulong:
          return CORBA::_tc_ulong;

        case TCKind::tk_float:
          return CORBA::_tc_float;

        case TCKind::tk_double:
          return CORBA::_tc_double;

        case TCKind::tk_boolean:
          return CORBA::_tc_boolean;

        case TCKind::tk_char:
          return CORBA::_tc_char;

        case TCKind::tk_octet:
          return CORBA::_tc_octet;

        case TCKind::tk_string:
          return DynString_Handler::create_typecode (type);

        case TCKind::tk_longlong:
          return CORBA::_tc_longlong;

        case TCKind::tk_ulonglong:
          return CORBA::_tc_ulonglong;

        case TCKind::tk_longdouble:
          // Disabled since a longdouble is defined in the xsd as double.
          // We are then creating a long double initialized from a regular
          // double. This is a very tricky conversion and doesn't work in
          // combination with certain (versions of) compilers.
          break;

        case TCKind::tk_wchar:
          return CORBA::_tc_wchar;

        case TCKind::tk_wstring:
          break;

        case TCKind::tk_enum:
          return DynEnum_Handler::create_typecode (type);

        case TCKind::tk_struct:
          return DynStruct_Handler::create_typecode (type);

        case TCKind::tk_sequence:
          return DynSequence_Handler::create_typecode (type);

        case TCKind::tk_alias:
          return DynAlias_Handler::create_typecode (type);

        case TCKind::tk_array:
          return DynArray_Handler::create_typecode (type);

        case TCKind::tk_fixed:
        case TCKind::tk_any:
        case TCKind::tk_TypeCode:
        case TCKind::tk_Principal:
        case TCKind::tk_objref:
        case TCKind::tk_union:
        case TCKind::tk_except:
        case TCKind::tk_value:
        case TCKind::tk_value_box:
        case TCKind::tk_native:
        case TCKind::tk_abstract_interface:
        case TCKind::tk_local_interface:
        case TCKind::tk_component:
        case TCKind::tk_home:
        case TCKind::tk_event:
          DANCE_DEBUG (DANCE_LOG_TERMINAL_ERROR,
            (LM_ERROR, DLINFO ACE_TEXT ("DynAny_Handler::create_typecode - Type not supported\n")));
          throw Config_Error ("", "Type not supported");
        }

      return 0;
    }

    void DynAny_Handler::register_typecode (std::string_view typeID, CORBA::TypeCode_ptr tc)
    {
      DANCE_TRACE("DynAny_Handler::register_typecode");

      this->typecode_map_.insert_or_assign (std::string (typeID), tc);
    }

    CORBA::TypeCode_ptr DynAny_Handler::get_typecode (std::string_view typeID)
    {
      DANCE_TRACE("DynAny_Handler::get_typecode");

      TypecodeMap::const_iterator const pos = this->typecode_map_.find (typeID);
      return pos == this->typecode_map_.end () ? 0 : pos->second;
    }
  }
}
#include /**/ "ace/post.h"
