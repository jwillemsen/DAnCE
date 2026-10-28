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

  }
}
