/**
 * @file CAD_Handler.h
 * @author William Otte <wotte@dre.vanderbilt.edu>
 */

#ifndef CIAO_PACKAGING_CAD_HANDLER_H
#define CIAO_PACKAGING_CAD_HANDLER_H

#include /**/ "ace/pre.h"

#include "ace/XML_Utils/XMLSchema/Types.hpp"
#include "dance/Deployment/DeploymentC.h"
#include "Package_Handlers/Packaging_Handlers_Export.h"
#include <string_view>

#if !defined (ACE_LACKS_PRAGMA_ONCE)
# pragma once
#endif /* ACE_LACKS_PRAGMA_ONCE */

namespace Deployment
{
  struct ComponentAssemblyDescription;
}

namespace DAnCE
{
  namespace Config_Handlers
  {
    class ComponentAssemblyDescription;

    namespace Packaging
    {

      /**
       * @class CAD_Handler
       *
       * @brief Handler class for ComponentAssemblyDescription.
       */
      class Packaging_Handlers_Export CAD_Handler
      {
      public:
        static void component_assem_descr (ComponentAssemblyDescription const& desc,
                                           ::Deployment::ComponentAssemblyDescription &toconfig);

        static ComponentAssemblyDescription
        component_assem_descr (::Deployment::ComponentAssemblyDescription const& src);

      private:
        static ComponentAssemblyDescription * resolve_cad (std::string_view uri);
      };

    }
  }
}

#include /**/ "ace/post.h"
#endif /* CIAO_PACKAGING_CAD_HANDLER_H */

