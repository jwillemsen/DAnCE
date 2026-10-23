//==============================================================
/**
 *  @file  XML_File_Intf.h
 *
 *  @author Bala Natarajan <bala@dre.vanderbilt.edu>
 */
//================================================================

#ifndef CIAO_CONFIG_XML_FILE_INTF_H
#define CIAO_CONFIG_XML_FILE_INTF_H
#include /**/ "ace/pre.h"

#include "Config_Handlers_Export.h"
#include "ace/XML_Utils/XML_Helper.h"

#if !defined (ACE_LACKS_PRAGMA_ONCE)
# pragma once
#endif /* ACE_LACKS_PRAGMA_ONCE */

#include <memory>
#include <string>

namespace Deployment
{
  struct DeploymentPlan;
  struct Domain;
}

namespace DAnCE
{
  namespace Config_Handlers
  {
    class Config_Handlers_Export XML_File_Intf
    {
    public:
      XML_File_Intf (const ACE_TCHAR *file);

      ~XML_File_Intf (void);

      ::Deployment::DeploymentPlan const *get_plan (void);
      ::Deployment::DeploymentPlan *release_plan (void);

      ::Deployment::Domain const *get_domain (void);
      ::Deployment::Domain *release_domain (void);

      void add_search_path (const ACE_TCHAR *environment,
                            const ACE_TCHAR *relpath);

    protected:

      bool read_process_plan (const ACE_TCHAR *file);

      bool read_process_domain (const ACE_TCHAR *file);

    private:
      std::basic_string <ACE_TCHAR> file_;
      XML::XML_Helper<XML::XML_Schema_Resolver<XML::Environment_Resolver>> xml_helper_;
      std::unique_ptr<::Deployment::DeploymentPlan> idl_dp_;
      std::unique_ptr<::Deployment::Domain> idl_domain_;
    };
  }
}
#include /**/ "ace/post.h"
#endif /*CIAO_CONFIG_XML_FILE_INTF_H*/
