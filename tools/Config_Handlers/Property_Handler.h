//================================================
/**
 *  @file  Property_Handler.h
 *
 *  @author Jules White <jules@dre.vanderbilt.edu>
 */
//================================================

#ifndef CIAO_CONFIG_HANDLERS_PROPERTY_HANDLER_H
#define CIAO_CONFIG_HANDLERS_PROPERTY_HANDLER_H

#include /**/ "ace/pre.h"

#include "Config_Handlers_Common_Export.h"

#if !defined (ACE_LACKS_PRAGMA_ONCE)
# pragma once
#endif /* ACE_LACKS_PRAGMA_ONCE */

namespace Deployment
{
  struct Property;
  class Properties;
}

namespace DAnCE
{
  namespace Config_Handlers
    {
      class Property;

      /*
       * @class Property_Handler
       *
       * @brief Handler class for <CCMComponentPortDescription> types.
       *
       * This class defines handler methods to map values from
       * XSC objects, parsed from the descriptor files, to the
       * corresponding CORBA IDL type for the schema element.
       *
       */

      class Config_Handlers_Common_Export Property_Handler
    {
    public:
      Property_Handler ();
      virtual ~Property_Handler ();

      /// This method maps the values from the XSC object
      /// <DAnCE::Config_Handlers::Property> to the CORBA IDL type
      /// <Deployment::Property>.
      static void handle_property (Property const& desc, ::Deployment::Property& toconfig);
    };

    }
}

#include /**/ "ace/post.h"
#endif /* CIAO_CONFIG_HANDLERS_PROPERTY_HANDLER_H*/
