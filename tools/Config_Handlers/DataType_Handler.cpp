#include "DataType_Handler.h"
#include "Common.h"
#include "tao/AnyTypeCode/TypeCode.h"
#include "tao/AnyTypeCode/TypeCode_Constants.h"
#include "dance/Logger/Log_Macros.h"
#include <functional>
namespace DAnCE
{
  namespace Config_Handlers
  {

    DataType_Handler::DataType_Handler ()
    {
      DANCE_TRACE("DataType_Handler::constructor");
    }
    DataType_Handler::~DataType_Handler ()
    {
      DANCE_TRACE("DataType_Handler::destructor");
    }

    ///This method takes a DAnCE::Config_Handlers::DataType
    ///and returns the corresponding CORBA::TypeCode.
    void
    DataType_Handler::data_type (const DataType& desc,
                                 CORBA::TypeCode_ptr& type)
    {
      DANCE_TRACE("DataType_Handler::data_type");
      TCKind kind (desc.kind ());

      switch (kind.integral ())
        {
        case  TCKind::tk_null:
          type = CORBA::TypeCode::_duplicate (CORBA::_tc_null);
          break;

        case  TCKind::tk_short:
          type = CORBA::TypeCode::_duplicate (CORBA::_tc_short);
          break;

        case  TCKind::tk_long:
          type = CORBA::TypeCode::_duplicate (CORBA::_tc_long);
          break;

        case  TCKind::tk_ushort:
          type = CORBA::TypeCode::_duplicate (CORBA::_tc_ushort);
          break;

        case  TCKind::tk_ulong:
          type = CORBA::TypeCode::_duplicate (CORBA::_tc_ulong);
          break;

        case  TCKind::tk_float:
          type = CORBA::TypeCode::_duplicate (CORBA::_tc_float);
          break;

        case  TCKind::tk_double:
          type = CORBA::TypeCode::_duplicate (CORBA::_tc_double);
          break;

        case  TCKind::tk_boolean:
          type = CORBA::TypeCode::_duplicate (CORBA::_tc_boolean);
          break;

        case  TCKind::tk_char:
          type = CORBA::TypeCode::_duplicate (CORBA::_tc_char);
          break;

        case  TCKind::tk_octet:
          type = CORBA::TypeCode::_duplicate (CORBA::_tc_octet);
          break;

        case  TCKind::tk_string:
          type = CORBA::TypeCode::_duplicate (CORBA::_tc_string);
          break;

        case  TCKind::tk_longlong:
          type = CORBA::TypeCode::_duplicate (CORBA::_tc_longlong);
          break;

        case  TCKind::tk_ulonglong:
          type = CORBA::TypeCode::_duplicate (CORBA::_tc_ulonglong);
          break;

        case  TCKind::tk_longdouble:
          type = CORBA::TypeCode::_duplicate (CORBA::_tc_longdouble);
          break;

        case  TCKind::tk_wchar:
          type = CORBA::TypeCode::_duplicate (CORBA::_tc_wchar);
          break;

        case  TCKind::tk_wstring:
          type = CORBA::TypeCode::_duplicate (CORBA::_tc_wstring);
          break;

        case  TCKind::tk_any:
          type = CORBA::TypeCode::_duplicate (CORBA::_tc_any);
          break;

        case  TCKind::tk_TypeCode:
          type = CORBA::TypeCode::_duplicate (CORBA::_tc_TypeCode);
          break;

        default:
          DANCE_DEBUG (DANCE_LOG_TERMINAL_ERROR,
            (LM_ERROR,
            "DataType_Handler::data_type, Invalid typecode in any\n"));
          throw Config_Error ("", "Invalid typecode in data type description");
        }

      //   This case used to be supported...is it not in the schema?

    }
#if DANCE_CONFIG_HANDLERS_CORBA_TO_XSC

    DataType
    DataType_Handler::data_type (const CORBA::TypeCode_ptr& src)
    {
      DANCE_TRACE("DataType_Handler::data_type - reverse");
      std::function<DataType (CORBA::TypeCode_ptr)> convert;
      convert = [&convert] (CORBA::TypeCode_ptr typecode) -> DataType
      {
        switch (typecode->kind ())
          {
          case CORBA::tk_string:
            {
              CORBA::ULong const bound = typecode->length ();
              if (bound == 0)
                return DataType (TCKind::tk_string);
              BoundedStringType::bound_container_type bounds {bound};
              BoundedStringType const bounded (bounds);
              ::XSCRT::OptionalArg<BoundedStringType> const bounded_arg (bounded);
              return DataType (TCKind::tk_string, std::nullopt, std::nullopt, std::nullopt,
                               std::nullopt, std::nullopt, std::nullopt, bounded_arg);
            }
          case CORBA::tk_enum:
            {
              EnumType::member_container_type members;
              CORBA::ULong const count = typecode->member_count ();
              for (CORBA::ULong i {}; i < count; ++i)
                {
                  CORBA::String_var member_name (typecode->member_name (i));
                  members.emplace_back (member_name.in ());
                }
              CORBA::String_var name (typecode->name ());
              CORBA::String_var id (typecode->id ());
              EnumType const enumeration (name.in (), id.in (), members);
              ::XSCRT::OptionalArg<EnumType> const enumeration_arg (enumeration);
              return DataType (TCKind::tk_enum, enumeration_arg);
            }
          case CORBA::tk_struct:
            {
              StructType::member_container_type members;
              CORBA::ULong const count = typecode->member_count ();
              for (CORBA::ULong i {}; i < count; ++i)
                {
                  CORBA::String_var member_name (typecode->member_name (i));
                  CORBA::TypeCode_var const member_type (typecode->member_type (i));
                  XMLSchema::string<char> const name (member_name.in ());
                  StructMemberType const member (name, convert (member_type.in ()));
                  members.push_back (member);
                }
              CORBA::String_var name (typecode->name ());
              CORBA::String_var id (typecode->id ());
              StructType const structure (name.in (), id.in (), members);
              ::XSCRT::OptionalArg<StructType> const structure_arg (structure);
              return DataType (TCKind::tk_struct, std::nullopt, structure_arg);
            }
          case CORBA::tk_sequence:
            {
              CORBA::TypeCode_var const content_type (typecode->content_type ());
              DataType const element_type (convert (content_type.in ()));
              std::optional<XMLSchema::unsignedInt> bound;
              CORBA::ULong const bound_value = typecode->length ();
              if (bound_value != 0)
                bound.emplace (bound_value);
              ::XSCRT::OptionalArg<XMLSchema::unsignedInt> const bound_arg (
                bound ? ::XSCRT::OptionalArg<XMLSchema::unsignedInt> (*bound) : std::nullopt);
              SequenceType const sequence (element_type, bound_arg);
              ::XSCRT::OptionalArg<SequenceType> const sequence_arg (sequence);
              return DataType (TCKind::tk_sequence, std::nullopt, std::nullopt, std::nullopt,
                               sequence_arg);
            }
          case CORBA::tk_array:
            {
              CORBA::TypeCode_var const content_type (typecode->content_type ());
              DataType const element_type (convert (content_type.in ()));
              XMLSchema::unsignedInt const length (typecode->length ());
              ArrayType const array (length, element_type);
              ::XSCRT::OptionalArg<ArrayType> const array_arg (array);
              return DataType (TCKind::tk_array, std::nullopt, std::nullopt, std::nullopt,
                               std::nullopt, std::nullopt, array_arg);
            }
          case CORBA::tk_alias:
            {
              CORBA::TypeCode_var const content_type (typecode->content_type ());
              DataType const element_type (convert (content_type.in ()));
              CORBA::String_var name (typecode->name ());
              CORBA::String_var id (typecode->id ());
              AliasType const alias (name.in (), id.in (), element_type);
              ::XSCRT::OptionalArg<AliasType> const alias_arg (alias);
              return DataType (TCKind::tk_alias, std::nullopt, std::nullopt, std::nullopt,
                               std::nullopt, alias_arg);
            }
          case CORBA::tk_null: return DataType (TCKind::tk_null);
          case CORBA::tk_void: return DataType (TCKind::tk_void);
          case CORBA::tk_short: return DataType (TCKind::tk_short);
          case CORBA::tk_long: return DataType (TCKind::tk_long);
          case CORBA::tk_ushort: return DataType (TCKind::tk_ushort);
          case CORBA::tk_ulong: return DataType (TCKind::tk_ulong);
          case CORBA::tk_float: return DataType (TCKind::tk_float);
          case CORBA::tk_double: return DataType (TCKind::tk_double);
          case CORBA::tk_boolean: return DataType (TCKind::tk_boolean);
          case CORBA::tk_char: return DataType (TCKind::tk_char);
          case CORBA::tk_octet: return DataType (TCKind::tk_octet);
          case CORBA::tk_longlong: return DataType (TCKind::tk_longlong);
          case CORBA::tk_ulonglong: return DataType (TCKind::tk_ulonglong);
          case CORBA::tk_longdouble: return DataType (TCKind::tk_longdouble);
          case CORBA::tk_wchar: return DataType (TCKind::tk_wchar);
          case CORBA::tk_wstring: return DataType (TCKind::tk_wstring);
          case CORBA::tk_any: return DataType (TCKind::tk_any);
          case CORBA::tk_TypeCode: return DataType (TCKind::tk_TypeCode);
          case CORBA::tk_objref: return DataType (TCKind::tk_objref);
          case CORBA::tk_fixed: return DataType (TCKind::tk_fixed);
          case CORBA::tk_union:
          case CORBA::tk_except:
          case CORBA::tk_value:
          case CORBA::tk_value_box:
          case CORBA::tk_native:
          case CORBA::tk_abstract_interface:
          case CORBA::tk_local_interface:
          case CORBA::tk_component:
          case CORBA::tk_home:
          case CORBA::tk_event:
          case CORBA::tk_Principal:
            throw Config_Error ("", "Unsupported CORBA typecode in data type conversion");
          default:
            throw Config_Error ("", "Unknown CORBA typecode in data type conversion");
          }
      };

      return convert (src);
    }
#endif // DANCE_CONFIG_HANDLERS_CORBA_TO_XSC
  }
}
