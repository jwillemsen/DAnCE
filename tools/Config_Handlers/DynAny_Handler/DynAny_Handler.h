//==============================================================
/**
 *  @file  DynAny_Handler.h
 *
 *  @author Will Otte <wotte@dre.vanderbilt.edu>
 */
//================================================================

#ifndef CIAO_CONFIG_HANDLERS_DYNANY_HANDLER_H
#define CIAO_CONFIG_HANDLERS_DYNANY_HANDLER_H
#include /**/ "ace/pre.h"

#include "DynAny_Handler_Export.h"

#include "tao/AnyTypeCode/AnyTypeCode_methods.h"
#include "tao/ORB.h"
#include "tao/DynamicAny/DynamicAny.h"
#include "ace/Singleton.h"

#include <map>
#include <functional>
#include <string>
#include <string_view>

#if !defined (ACE_LACKS_PRAGMA_ONCE)
# pragma once
#endif /* ACE_LACKS_PRAGMA_ONCE */

namespace DAnCE
{
  namespace Config_Handlers
  {
    class Any;
    class DataType;
    class DataValue;

    /**
    * @class Any_Handler
    *
    * @brief Handler class for ComponentInterfaceDescription types.
    *
    * This class defines handler methods to map values from
    * XSC Any objects, parsed from the descriptor files, to the
    * corresponding CORBA IDL Any type.
    *
    */
    class DynAny_Handler_Export DynAny_Handler
    {
    public:
      DynAny_Handler ();

      ~DynAny_Handler (void);

      DynamicAny::DynAny_ptr  extract_into_dynany (DataType const& type,
                                                   DataValue const& value,
                                                   CORBA::TypeCode_ptr = CORBA::TypeCode::_nil ());

      Any extract_from_dynany (CORBA::Any const& any);

      CORBA::ORB_ptr orb ();

      DynamicAny::DynAnyFactory_ptr daf ();

      CORBA::TypeCode_ptr create_typecode (DataType const& type);

      void register_typecode (std::string_view typeID, CORBA::TypeCode_ptr tc);

      CORBA::TypeCode_ptr get_typecode (std::string_view typeID);

    private:

      CORBA::ORB_var orb_;

      DynamicAny::DynAnyFactory_var daf_;

      using TypecodeMap = std::map <std::string, CORBA::TypeCode_ptr, std::less<>>;
      TypecodeMap typecode_map_;
    };

    typedef ACE_Singleton<DynAny_Handler, ACE_Null_Mutex> DynAny_Singleton;
    #define DYNANY_HANDLER DynAny_Singleton::instance()
  }
}

#include /**/ "ace/post.h"
#endif /* CIAO_CONFIG_HANDLERS_ANY_HANDLER_H*/
