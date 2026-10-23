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
    void create_type_map (const DataType &type,
                          std::map <std::string,
                          DataType const *> &dt_map)
    {
      DANCE_TRACE("Config_Handlers::create_type_map");

      ::DAnCE::Config_Handlers::StructType const* structType =
        type.struct__if_set ();
      if (!structType)
        throw Config_Error (ACE_TEXT (""),
                            ACE_TEXT ("Expected struct type information"));

      for (auto i =
             structType->member_values ().begin ();
           i != structType->member_values ().end (); ++i)
        {
          dt_map[i->name ()] = &(i->type ());
        }
    }

    DynamicAny::DynAny_ptr
    DynStruct_Handler::extract_into_dynany (const DataType &type,
                                            const DataValue &value,
                                            CORBA::TypeCode_ptr req_tc)
    {
      DANCE_TRACE("DynStruct_Handler::extract_into_dynany");

      ::DAnCE::Config_Handlers::StructType const* structType =
        type.struct__if_set ();
      if (!structType)
        throw Config_Error (ACE_TEXT (""),
                            ACE_TEXT ("Expected struct type information"));

      try
        {
          CORBA::TypeCode_var tc;

          if (req_tc)
            tc = req_tc;
          else
            tc = DynStruct_Handler::create_typecode (type);

          std::map <std::string, DataType const*> dt_map;
          create_type_map (type, dt_map);

          // Make the actual DynStruct
          DynamicAny::DynAny_var temp =
            DYNANY_HANDLER->daf ()->create_dyn_any_from_type_code (tc);
          DynamicAny::DynStruct_var retval =
            DynamicAny::DynStruct::_narrow (temp.in ());

          CORBA::ULong pos (0);
          DynamicAny::NameDynAnyPairSeq values;
          values.length (value.member_values ().size ());

          for (auto i = value.member_values ().begin ();
               i != value.member_values ().end (); ++i)
            {
              values[pos].id =
                i->name ().c_str ();
              values[pos].value =
                DYNANY_HANDLER->extract_into_dynany (*dt_map[i->name ()],
                                                     i->value ());
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
          throw Config_Error (structType->typeId (),
                              ACE_TEXT ("Unknown exception"));
        }
    }


    CORBA::TypeCode_ptr
    DynStruct_Handler::create_typecode (const DataType &type)
    {
      DANCE_TRACE("DynStruct_Handler::create_typecode");


      ::DAnCE::Config_Handlers::StructType const* struct__if_set_ptr_2 = type.struct__if_set ();
      if (!struct__if_set_ptr_2)
        {
          DANCE_DEBUG (DANCE_LOG_TERMINAL_ERROR,
            (LM_ERROR,
             ACE_TEXT ("ERROR: Struct type description required\n")));
          throw Config_Error (ACE_TEXT (""),
            ACE_TEXT ("Expected struct type information, tc_kind may be"\
                      "incorrect\n"));
        }

      std::string rid (struct__if_set_ptr_2->typeId ());
      std::string name (struct__if_set_ptr_2->name ());

      CORBA::StructMemberSeq members;
      members.length (struct__if_set_ptr_2->member_values ().size ());
      CORBA::ULong pos (0);

      for (auto i =
        struct__if_set_ptr_2->member_values ().begin ();
           i != struct__if_set_ptr_2->member_values ().end (); ++i)
        {
          members[pos].name = i->name ().c_str ();
          members[pos].type = DYNANY_HANDLER->create_typecode (i->type ());
          ++pos;
        }

      CORBA::TypeCode_var tc =
        DYNANY_HANDLER->orb ()->create_struct_tc (
          rid.c_str (),
          name.c_str (),
          members);

      DYNANY_HANDLER->register_typecode (struct__if_set_ptr_2->typeId (), tc.in ());

      return tc._retn ();
    }
  }
}



