#include <cstring>

#include "DynEnum_Handler.h"
#include "DynAny_Handler.h"
#include "dance/Logger/Log_Macros.h"
#include "Basic_Deployment_Data.hpp"
#include "Common.h"

#include "tao/AnyTypeCode/TypeCode.h"
#include "tao/TypeCodeFactory/TypeCodeFactory_Adapter_Impl.h"
#include "tao/AnyTypeCode/AnyTypeCode_methods.h"
#include "tao/AnyTypeCode/Enum_TypeCode.h"
#include "tao/IFR_Client/IFR_BasicC.h"

namespace DAnCE
{
  namespace Config_Handlers
  {
    DynamicAny::DynAny_ptr
    DynEnum_Handler::extract_into_dynany (DataType const& type,
                                          DataValue const& value,
                                          CORBA::TypeCode_ptr req_tc)
    {
      DANCE_TRACE("DynEnum_Handler::extract_into_dynany");

      ::DAnCE::Config_Handlers::EnumType const* enumType = type._cxx_enum_if_set ();
      if (!enumType)
        throw Config_Error ("", "Expected enum type information");

      ::DAnCE::Config_Handlers::DataValue::enum_container_type const& enum_values = value._cxx_enum ();
      if (enum_values.empty ())
        throw Config_Error (enumType->typeId (), "Enum value required");

      try
        {
          CORBA::TypeCode_var tc {};

          if (req_tc)
            tc = req_tc;
          else
            tc = DynEnum_Handler::create_typecode (type);

          // Make the actual DynEnum
          DynamicAny::DynAny_var const temp = DYNANY_HANDLER->daf ()->create_dyn_any_from_type_code (tc);
          DynamicAny::DynEnum_var retval = DynamicAny::DynEnum::_narrow (temp.in ());

          retval->set_as_string (enum_values.front ().c_str ());

          return retval._retn ();
        }
      catch (DynamicAny::DynAny::InvalidValue const&)
        {
          DANCE_DEBUG (DANCE_LOG_TERMINAL_ERROR,
            (LM_ERROR,
              ACE_TEXT ("Invalid value provided in XML when trying to ")
              ACE_TEXT ("initialize an instance of enumerated type %C\n"),
              enumType->typeId ().c_str ()));
          throw Config_Error (enumType->typeId (), "Invalid value provided in XML");
        }
      catch (Config_Error &ex)
        {
          DANCE_DEBUG (DANCE_LOG_TERMINAL_ERROR, (LM_ERROR, ACE_TEXT ("DynEnum_Handler caught Config_Error\n")));
          ex.add_name (enumType->typeId ());

          throw ex;
        }
      catch (...)
        {
          DANCE_DEBUG (DANCE_LOG_TERMINAL_ERROR, (LM_ERROR, ACE_TEXT ("DynEnum_Handler caught unknown exception\n")));
          throw Config_Error (enumType->typeId (), "Unknown exception");
        }
    }

    CORBA::TypeCode_ptr DynEnum_Handler::create_typecode (DataType const& type)
    {
      DANCE_TRACE("DynEnum_Handler::create_typecode");

      ::DAnCE::Config_Handlers::EnumType const* enum_type = type._cxx_enum_if_set ();
      if (!enum_type)
        {
          DANCE_DEBUG (DANCE_LOG_TERMINAL_ERROR, (LM_ERROR, ACE_TEXT ("ERROR: Enum type description required")));
          throw Config_Error ("", ACE_TEXT ("Did not find expected enum type description, tk_kind may be wrong."));
        }

      // Construct TypeCode for the enum
      CORBA::EnumMemberSeq members {};
      members.length (enum_type->member ().size ());
      CORBA::ULong index {};

      for (auto i = enum_type->member ().begin (); i != enum_type->member ().end (); ++i)
        {
          members[index++] = CORBA::string_dup (i->c_str ());
        }

      CORBA::TypeCode_var tc =
        DYNANY_HANDLER->orb ()->create_enum_tc (
          enum_type->typeId ().c_str (),
          enum_type->name ().c_str (),
          members);

      DYNANY_HANDLER->register_typecode (enum_type->typeId (), tc.in ());

      return tc._retn ();
    }

  }
}

