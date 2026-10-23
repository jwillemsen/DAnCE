#include <cstring>

#include "DynStruct_Handler.h"
#include "DynAny_Handler.h"
#include "dance/Logger/Log_Macros.h"
#include "Basic_Deployment_Data.hpp"
#include "Common.h"

#include "ace/Null_Mutex.h"
#include "tao/AnyTypeCode/Struct_TypeCode.h"
#include "tao/IFR_Client/IFR_BasicC.h"

namespace DAnCE
{
  namespace Config_Handlers
  {
    void create_type_map (DataType const& type,
                          std::map <std::string,
                          DataType const *> &dt_map)
    {
      DANCE_TRACE("Config_Handlers::create_type_map");

      ::DAnCE::Config_Handlers::StructType const* structType = type._cxx_struct_if_set ();
      if (!structType)
        throw Config_Error ("", "Expected struct type information");

      for (auto i = structType->member ().begin (); i != structType->member ().end (); ++i)
        {
          dt_map[i->name ()] = &(i->type ());
        }
    }

    DynamicAny::DynAny_ptr
    DynStruct_Handler::extract_into_dynany (DataType const& type,
                                            DataValue const& value,
                                            CORBA::TypeCode_ptr req_tc)
    {
      DANCE_TRACE("DynStruct_Handler::extract_into_dynany");

      ::DAnCE::Config_Handlers::StructType const* structType = type._cxx_struct_if_set ();
      if (!structType)
        throw Config_Error ("", "Expected struct type information");

      try
        {
          CORBA::TypeCode_var tc {};

          if (req_tc)
            tc = req_tc;
          else
            tc = DynStruct_Handler::create_typecode (type);

          std::map <std::string, DataType const*> dt_map {};
          create_type_map (type, dt_map);

          // Make the actual DynStruct
          DynamicAny::DynAny_var const temp = DYNANY_HANDLER->daf ()->create_dyn_any_from_type_code (tc);
          DynamicAny::DynStruct_var retval = DynamicAny::DynStruct::_narrow (temp.in ());

          CORBA::ULong pos {};
          DynamicAny::NameDynAnyPairSeq values {};
          values.length (value.member ().size ());

          for (auto i = value.member ().begin (); i != value.member ().end (); ++i)
            {
              values[pos].id = i->name ().c_str ();
              values[pos].value = DYNANY_HANDLER->extract_into_dynany (*dt_map[i->name ()], i->value ());
              ++pos;
            }

          retval->set_members_as_dyn_any (values);

          return retval._retn ();
        }
      catch (Config_Error &ex)
        {
          ex.add_name (structType->typeId ());
          throw ex;
        }
      catch (...)
        {
          throw Config_Error (structType->typeId (), "Unknown exception");
        }
    }

    CORBA::TypeCode_ptr DynStruct_Handler::create_typecode (DataType const& type)
    {
      DANCE_TRACE("DynStruct_Handler::create_typecode");

      ::DAnCE::Config_Handlers::StructType const* struct_type = type._cxx_struct_if_set ();
      if (!struct_type)
        {
          DANCE_DEBUG (DANCE_LOG_TERMINAL_ERROR, (LM_ERROR, ACE_TEXT ("ERROR: Struct type description required\n")));
          throw Config_Error ("", ACE_TEXT ("Expected struct type information, tc_kind may be" "incorrect\n"));
        }

      std::string const rid (struct_type->typeId ());
      std::string const name (struct_type->name ());

      CORBA::StructMemberSeq members {};
      members.length (struct_type->member ().size ());
      CORBA::ULong pos {};

      for (auto i = struct_type->member ().begin (); i != struct_type->member ().end (); ++i)
        {
          members[pos].name = i->name ().c_str ();
          members[pos].type = DYNANY_HANDLER->create_typecode (i->type ());
          ++pos;
        }

      CORBA::TypeCode_var tc = DYNANY_HANDLER->orb ()->create_struct_tc (rid.c_str (), name.c_str (), members);

      DYNANY_HANDLER->register_typecode (struct_type->typeId (), tc.in ());

      return tc._retn ();
    }
  }
}

