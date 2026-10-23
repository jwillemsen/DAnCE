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

      for (StructType::member_const_iterator i =
             (*type.struct__if_set ()).begin_member ();
           i != (*type.struct__if_set ()).end_member (); ++i)
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
          values.length (value.count_member ());

          for (DataValue::member_const_iterator i = value.begin_member ();
               i != value.end_member (); ++i)
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
          if (type.struct__if_set ())
            ex.add_name ((*type.struct__if_set ()).typeId ());
          throw ex;
        }
      catch (...)
        {
          throw Config_Error ((*type.struct__if_set ()).typeId (),
                              ACE_TEXT ("Unknown exception"));
        }
    }


    CORBA::TypeCode_ptr
    DynStruct_Handler::create_typecode (const DataType &type)
    {
      DANCE_TRACE("DynStruct_Handler::create_typecode");

      if (!type.struct__if_set ())
        {
          DANCE_DEBUG (DANCE_LOG_TERMINAL_ERROR,
            (LM_ERROR,
             ACE_TEXT ("ERROR: Struct type description required\n")));
          throw Config_Error (ACE_TEXT (""),
            ACE_TEXT ("Expected struct type information, tc_kind may be"\
                      "incorrect\n"));
        }

      std::string rid ((*type.struct__if_set ()).typeId ());
      std::string name ((*type.struct__if_set ()).name ());

      CORBA::StructMemberSeq members;
      members.length ((*type.struct__if_set ()).count_member ());
      CORBA::ULong pos (0);

      for (StructType::member_const_iterator i =
        (*type.struct__if_set ()).begin_member ();
           i != (*type.struct__if_set ()).end_member (); ++i)
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

      DYNANY_HANDLER->register_typecode ((*type.struct__if_set ()).typeId (), tc.in ());

      return tc._retn ();
    }
  }
}



