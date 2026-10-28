/**
 * @file NIA_Handler.h
 * @author William Otte <wotte@dre.vanderbilt.edu>
 */

#ifndef CIAO_PACKAGING_NIA_HANDLER_H
#define CIAO_PACKAGING_NIA_HANDLER_H

#include /**/ "ace/pre.h"
#include "Package_Handlers/Packaging_Handlers_Export.h"
#include "dance/Deployment/Deployment_Packaging_DataC.h"
#include "iad.hpp"

namespace Deployment
{
  struct NamedImplementationArtifact;
  class NamedImplementationArtifacts;
}

namespace DAnCE
{
  namespace Config_Handlers
  {
    class NamedImplementationArtifact;

    namespace Packaging
    {
      /**
       * @class NIA_Handler
       * @brief Handler class for NamedImplementationArtifact types.
       */
      class Packaging_Handlers_Export NIA_Handler
      {
      public:
        static void handle_nia (NamedImplementationArtifact const& desc,
                             ::Deployment::NamedImplementationArtifact &toconfig)
        {
          toconfig.name = desc.name ().c_str ();
          IAD_Handler::impl_artifact_descr (desc.referencedArtifact (), toconfig.referencedArtifact);
        }

        static NamedImplementationArtifact get_nia (::Deployment::NamedImplementationArtifact const& src)
        {
          return NamedImplementationArtifact (src.name.in (),
                                              IAD_Handler::impl_artifact_descr (src.referencedArtifact));
        }

      };

    }
  }
}

#include /**/ "ace/post.h"

#endif /* CIAO_PACKAGING_NIA_HANDLER_H */
