//==============================================================
/**
 *  @file  Req_Handler.h
 *
 *  @author Jules White <jules@dre.vanderbilt.edu>
 */
//================================================================

#ifndef CIAO_CONFIG_HANDLERS_REQ_HANDLER_H
#define CIAO_CONFIG_HANDLERS_REQ_HANDLER_H
#include /**/ "ace/pre.h"

#include "Basic_Deployment_Data.hpp"
#include "Config_Handlers_Common_Export.h"

#if !defined (ACE_LACKS_PRAGMA_ONCE)
# pragma once
#endif /* ACE_LACKS_PRAGMA_ONCE */

namespace Deployment
{
  struct Requirement;
  class Requirements;
}

namespace DAnCE
{

  namespace Config_Handlers
    {
      /*
       * @class REQ_Handler
       *
       * @brief Handler class for <Requirement> types.
       *
       * This class defines handler methods to map values from
       * XSC Requirement objects, parsed from
       * the descriptor files, to the corresponding CORBA IDL type.
       *
       */

      class Config_Handlers_Common_Export Req_Handler
    {
    public:

      Req_Handler ();
      virtual ~Req_Handler ();

      ///This method takes a <Deployment::Requirement>
      ///and maps the values from the passed in XSC
      ///Requirement to its members.
      static void handle_requirement (Requirement const& desc, Deployment::Requirement& toconfig);
      static Requirement
        get_requirement (Deployment::Requirement const& src);
    };

    }
}

#include /**/ "ace/post.h"
#endif /* CIAO_CONFIG_HANDLERS_REQ_HANDLER_H*/
