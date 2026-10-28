//==============================================================
/**
 *  @file  DP_Handler.h
 *
 *  @author Bala Natarajan <bala@dre.vanderbilt.edu>
 */
//================================================================

#ifndef CIAO_CONFIG_HANDLERS_DP_HANDLER_H
#define CIAO_CONFIG_HANDLERS_DP_HANDLER_H
#include /**/ "ace/pre.h"

#include "Config_Handlers_Export.h"
#include <memory>

#if !defined (ACE_LACKS_PRAGMA_ONCE)
# pragma once
#endif /* ACE_LACKS_PRAGMA_ONCE */

namespace Deployment
{
  struct DeploymentPlan ;
}

namespace DAnCE
{
  namespace Config_Handlers
    {
      class deploymentPlan;

      /*
       * @class DP_Handler
       *
       * @brief Handler for Deployment::DeploymentPlan
       */
      class Config_Handlers_Export DP_Handler
        {
        public:
          class NoPlan {};

          /// Use this constructor if you want to use
          /// the XSC->IDL mapping
          DP_Handler (deploymentPlan &dp);

          ~DP_Handler ()
            throw ();

          /// Generates the XSC->IDL mapping.  returns null
          /// if the method fails or the class was constructed
          ::Deployment::DeploymentPlan const *plan () const;

          ::Deployment::DeploymentPlan *plan ();

        private:

          bool resolve_plan (deploymentPlan &dp);


        private:
          std::unique_ptr<::Deployment::DeploymentPlan> idl_dp_ {};

        };
    }
}

#include /**/ "ace/post.h"
#endif /*CIAO_CONFIG_HANDLERS_DP_HANDLER_H*/
