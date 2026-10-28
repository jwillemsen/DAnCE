#include /**/ "ace/pre.h"

#include "dance/Logger/Log_Macros.h"
#include "DynAny_Handler.h"
#include "DynEnum_Handler.h"
#include "DynSequence_Handler.h"
#include "DynStruct_Handler.h"
#include "DynAlias_Handler.h"
#include "DynArray_Handler.h"
#include "DynString_Handler.h"
#include "DataType_Handler.h"
#include "Basic_Deployment_Data.hpp"
#include "Common.h"
#include <functional>

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

      auto require_one = [] (size_t count, char const* type_name)
      {
        if (count != 1)
          throw Config_Error ("", std::string ("Expected exactly one ") + type_name + " value");
      };

      switch (tkind)
        {
        case TCKind::tk_short: require_one (value._cxx_short ().size (), "short"); break;
        case TCKind::tk_long: require_one (value._cxx_long ().size (), "long"); break;
        case TCKind::tk_ushort: require_one (value.ushort ().size (), "unsigned short"); break;
        case TCKind::tk_ulong: require_one (value.ulong ().size (), "unsigned long"); break;
        case TCKind::tk_float: require_one (value._cxx_float ().size (), "float"); break;
        case TCKind::tk_double: require_one (value._cxx_double ().size (), "double"); break;
        case TCKind::tk_boolean: require_one (value.boolean ().size (), "boolean"); break;
        case TCKind::tk_octet: require_one (value.octet ().size (), "octet"); break;
        case TCKind::tk_longlong: require_one (value.longlong ().size (), "long long"); break;
        case TCKind::tk_ulonglong: require_one (value.ulonglong ().size (), "unsigned long long"); break;
        case TCKind::tk_string: require_one (value.string ().size (), "string"); break;
        case TCKind::tk_char:
        case TCKind::tk_wchar:
          require_one (value.string ().size (), "character");
          if (value.string ().front ().size () != 1)
            throw Config_Error ("", "Character values must contain exactly one character");
          break;
        default:
          break;
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

#if DANCE_CONFIG_HANDLERS_CORBA_TO_XSC
    Any DynAny_Handler::extract_from_dynany (CORBA::Any const& any)
    {
      DANCE_TRACE("DynAny_Handler::extract_from_dynany");

      DynamicAny::DynAny_var const dyn = this->daf_->create_dyn_any (any);
      CORBA::TypeCode_var const typecode (any.type ());
      using ExtractedValue = std::pair<DataType, DataValue>;
      std::function<ExtractedValue (CORBA::TypeCode_ptr, DynamicAny::DynAny_ptr)> convert;

      convert = [this, &convert] (CORBA::TypeCode_ptr tc, DynamicAny::DynAny_ptr value) -> ExtractedValue
      {
        switch (tc->kind ())
          {
          case CORBA::tk_null:
          case CORBA::tk_void:
            {
              DataType const type (tc->kind () == CORBA::tk_null ? TCKind::tk_null : TCKind::tk_void);
              return ExtractedValue (type, DataValue {});
            }
          case CORBA::tk_short:
            {
              DataType const type (TCKind::tk_short);
              DataValue::short_container_type values {value->get_short ()};
              return ExtractedValue (type, DataValue (values));
            }
          case CORBA::tk_long:
            {
              DataType const type (TCKind::tk_long);
              DataValue::long_container_type values {value->get_long ()};
              return ExtractedValue (type, DataValue ({}, values));
            }
          case CORBA::tk_ushort:
            {
              DataType const type (TCKind::tk_ushort);
              DataValue::ushort_container_type values {value->get_ushort ()};
              return ExtractedValue (type, DataValue ({}, {}, values));
            }
          case CORBA::tk_ulong:
            {
              DataType const type (TCKind::tk_ulong);
              DataValue::ulong_container_type values {value->get_ulong ()};
              return ExtractedValue (type, DataValue ({}, {}, {}, values));
            }
          case CORBA::tk_float:
            {
              DataType const type (TCKind::tk_float);
              DataValue::float_container_type values {value->get_float ()};
              return ExtractedValue (type, DataValue ({}, {}, {}, {}, values));
            }
          case CORBA::tk_double:
            {
              DataType const type (TCKind::tk_double);
              DataValue::double_container_type values {value->get_double ()};
              return ExtractedValue (type, DataValue ({}, {}, {}, {}, {}, values));
            }
          case CORBA::tk_boolean:
            {
              DataType const type (TCKind::tk_boolean);
              DataValue::boolean_container_type values {value->get_boolean ()};
              return ExtractedValue (type, DataValue ({}, {}, {}, {}, {}, {}, values));
            }
          case CORBA::tk_char:
            {
              DataType const type (TCKind::tk_char);
              DataValue::string_container_type values {std::string (1, value->get_char ())};
              return ExtractedValue (type, DataValue ({}, {}, {}, {}, {}, {}, {}, {}, {}, values));
            }
          case CORBA::tk_octet:
            {
              DataType const type (TCKind::tk_octet);
              DataValue::octet_container_type values {value->get_octet ()};
              return ExtractedValue (type, DataValue ({}, {}, {}, {}, {}, {}, {}, values));
            }
          case CORBA::tk_string:
            {
              CORBA::String_var string_value (value->get_string ());
              DataValue::string_container_type values {string_value.in ()};
              CORBA::ULong const bound = tc->length ();
              if (bound != 0)
                {
                  BoundedStringType::bound_container_type bounds {bound};
                  BoundedStringType const bounded (bounds);
                  ::XSCRT::OptionalArg<BoundedStringType> const bounded_arg (bounded);
                  DataType const type (TCKind::tk_string, std::nullopt, std::nullopt, std::nullopt,
                                       std::nullopt, std::nullopt, std::nullopt, bounded_arg);
                  return ExtractedValue (type, DataValue ({}, {}, {}, {}, {}, {}, {}, {}, {}, values));
                }
              DataType const type (TCKind::tk_string);
              return ExtractedValue (type, DataValue ({}, {}, {}, {}, {}, {}, {}, {}, {}, values));
            }
          case CORBA::tk_longlong:
            {
              DataValue::longlong_container_type values {value->get_longlong ()};
              DataType const type (TCKind::tk_longlong);
              return ExtractedValue (type, DataValue ({}, {}, {}, {}, {}, {}, {}, {}, {}, {}, values));
            }
          case CORBA::tk_ulonglong:
            {
              DataValue::ulonglong_container_type values {value->get_ulonglong ()};
              DataType const type (TCKind::tk_ulonglong);
              return ExtractedValue (type, DataValue ({}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, values));
            }
          case CORBA::tk_wchar:
            {
              CORBA::WChar const character = value->get_wchar ();
              if (character > 0x7f)
                throw Config_Error ("", "Non-ASCII wchar values cannot be represented in XSC string data");
              DataValue::string_container_type values {std::string (1, static_cast<char> (character))};
              DataType const type (TCKind::tk_wchar);
              return ExtractedValue (type, DataValue ({}, {}, {}, {}, {}, {}, {}, {}, {}, values));
            }
          case CORBA::tk_enum:
            {
              EnumType::member_container_type members;
              CORBA::ULong const count = tc->member_count ();
              for (CORBA::ULong i {}; i < count; ++i)
                {
                  CORBA::String_var member_name (tc->member_name (i));
                  members.emplace_back (member_name.in ());
                }
              CORBA::String_var name (tc->name ());
              CORBA::String_var id (tc->id ());
              EnumType const enumeration (name.in (), id.in (), members);
              ::XSCRT::OptionalArg<EnumType> const enumeration_arg (enumeration);
              DataType const type (TCKind::tk_enum, enumeration_arg);
              DynamicAny::DynEnum_var enumeration_value = DynamicAny::DynEnum::_narrow (value);
              CORBA::String_var value_name (enumeration_value->get_as_string ());
              DataValue::enum_container_type values {value_name.in ()};
              return ExtractedValue (type, DataValue ({}, {}, {}, {}, {}, {}, {}, {}, values));
            }
          case CORBA::tk_alias:
            {
              CORBA::TypeCode_var const content_type (tc->content_type ());
              ExtractedValue const content (convert (content_type.in (), value));
              CORBA::String_var name (tc->name ());
              CORBA::String_var id (tc->id ());
              AliasType const alias (name.in (), id.in (), content.first);
              ::XSCRT::OptionalArg<AliasType> const alias_arg (alias);
              DataType const type (TCKind::tk_alias, std::nullopt, std::nullopt, std::nullopt,
                                   std::nullopt, alias_arg);
              return ExtractedValue (type, content.second);
            }
          case CORBA::tk_sequence:
          case CORBA::tk_array:
            {
              CORBA::TypeCode_var const content_type (tc->content_type ());
              DynamicAny::DynAny_var const default_element (
                this->daf_->create_dyn_any_from_type_code (content_type.in ()));
              ExtractedValue const element_type (convert (content_type.in (), default_element.in ()));
              DataValue::element_container_type elements;
              DynamicAny::DynSequence_var sequence = DynamicAny::DynSequence::_narrow (value);
              DynamicAny::DynArray_var array = DynamicAny::DynArray::_narrow (value);
              DynamicAny::DynAnySeq_var values = tc->kind () == CORBA::tk_sequence
                ? sequence->get_elements_as_dyn_any () : array->get_elements_as_dyn_any ();
              std::string characters;
              for (CORBA::ULong i {}; i < values->length (); ++i)
                {
                  ExtractedValue const element (convert (content_type.in (), values[i]));
                  if (content_type->kind () == CORBA::tk_char || content_type->kind () == CORBA::tk_wchar)
                    characters += element.second.string ().front ();
                  else
                    elements.push_back (element.second);
                }

              if (content_type->kind () == CORBA::tk_char || content_type->kind () == CORBA::tk_wchar)
                {
                  DataValue::string_container_type string_value {characters};
                  DataValue const character_values ({}, {}, {}, {}, {}, {}, {}, {}, {}, string_value);
                  elements.clear ();
                  if (tc->kind () == CORBA::tk_sequence)
                    {
                      std::optional<XMLSchema::unsignedInt> bound;
                      CORBA::ULong const bound_value = tc->length ();
                      if (bound_value != 0)
                        bound.emplace (bound_value);
                      ::XSCRT::OptionalArg<XMLSchema::unsignedInt> const bound_arg (
                        bound ? ::XSCRT::OptionalArg<XMLSchema::unsignedInt> (*bound) : std::nullopt);
                      SequenceType const sequence_type (element_type.first, bound_arg);
                      ::XSCRT::OptionalArg<SequenceType> const sequence_arg (sequence_type);
                      DataType const result_type (TCKind::tk_sequence, std::nullopt, std::nullopt,
                                                  std::nullopt, sequence_arg);
                      return ExtractedValue (result_type, character_values);
                    }

                  XMLSchema::unsignedInt const length (tc->length ());
                  ArrayType const array_type (length, element_type.first);
                  ::XSCRT::OptionalArg<ArrayType> const array_arg (array_type);
                  DataType const result_type (TCKind::tk_array, std::nullopt, std::nullopt,
                                              std::nullopt, std::nullopt, std::nullopt, array_arg);
                  return ExtractedValue (result_type, character_values);
                }

              if (tc->kind () == CORBA::tk_sequence)
                {
                  std::optional<XMLSchema::unsignedInt> bound;
                  CORBA::ULong const bound_value = tc->length ();
                  if (bound_value != 0)
                    bound.emplace (bound_value);
                  ::XSCRT::OptionalArg<XMLSchema::unsignedInt> const bound_arg (
                    bound ? ::XSCRT::OptionalArg<XMLSchema::unsignedInt> (*bound) : std::nullopt);
                  SequenceType const sequence_type (element_type.first, bound_arg);
                  ::XSCRT::OptionalArg<SequenceType> const sequence_arg (sequence_type);
                  DataType const result_type (TCKind::tk_sequence, std::nullopt, std::nullopt,
                                              std::nullopt, sequence_arg);
                  return ExtractedValue (result_type,
                    DataValue ({}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, elements));
                }

              XMLSchema::unsignedInt const length (tc->length ());
              ArrayType const array_type (length, element_type.first);
              ::XSCRT::OptionalArg<ArrayType> const array_arg (array_type);
              DataType const result_type (TCKind::tk_array, std::nullopt, std::nullopt, std::nullopt,
                                          std::nullopt, std::nullopt, array_arg);
              return ExtractedValue (result_type,
                DataValue ({}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, elements));
            }
          case CORBA::tk_struct:
            {
              DynamicAny::DynStruct_var structure = DynamicAny::DynStruct::_narrow (value);
              DynamicAny::NameDynAnyPairSeq_var values = structure->get_members_as_dyn_any ();
              StructType::member_container_type members;
              DataValue::member_container_type member_values;
              CORBA::ULong const count = tc->member_count ();
              if (values->length () != count)
                throw Config_Error ("", "Struct member count does not match its typecode");

              for (CORBA::ULong i {}; i < count; ++i)
                {
                  CORBA::String_var member_name (tc->member_name (i));
                  CORBA::TypeCode_var const member_type (tc->member_type (i));
                  ExtractedValue const extracted (convert (member_type.in (), values[i].value.in ()));
                  XMLSchema::string<char> const schema_name (member_name.in ());
                  StructMemberType const member_type_desc (schema_name, extracted.first);
                  members.push_back (member_type_desc);
                  NamedValue const member_value (schema_name, extracted.second);
                  member_values.push_back (member_value);
                }

              CORBA::String_var name (tc->name ());
              CORBA::String_var id (tc->id ());
              StructType const structure_type (name.in (), id.in (), members);
              ::XSCRT::OptionalArg<StructType> const structure_arg (structure_type);
              DataType const result_type (TCKind::tk_struct, std::nullopt, structure_arg);
              return ExtractedValue (result_type,
                DataValue ({}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, member_values));
            }
          case CORBA::tk_longdouble:
          case CORBA::tk_wstring:
          case CORBA::tk_any:
          case CORBA::tk_TypeCode:
          case CORBA::tk_objref:
          case CORBA::tk_union:
          case CORBA::tk_except:
          case CORBA::tk_fixed:
          case CORBA::tk_value:
          case CORBA::tk_value_box:
          case CORBA::tk_native:
          case CORBA::tk_abstract_interface:
          case CORBA::tk_local_interface:
          case CORBA::tk_component:
          case CORBA::tk_home:
          case CORBA::tk_event:
          case CORBA::tk_Principal:
            throw Config_Error ("", "CORBA Any type is not supported by the XSC reverse conversion");
          default:
            throw Config_Error ("", "Unknown CORBA Any type");
          }
      };

      ExtractedValue const extracted (convert (typecode.in (), dyn.in ()));
      return Any (extracted.first, extracted.second);
    }
#endif // DANCE_CONFIG_HANDLERS_CORBA_TO_XSC

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
