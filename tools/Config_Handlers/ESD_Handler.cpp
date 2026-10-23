#include "ESD_Handler.h"
#include "Basic_Deployment_Data.hpp"
#include "dance/Deployment/Deployment_DataC.h"
#include "cdp.hpp"

namespace DAnCE
{
  namespace Config_Handlers
  {
    void
    ESD_Handler::es_deployment_descrs (
        const DeploymentPlan &src,
        Deployment::EventServiceDeploymentDescriptions& dest)
    {
      DANCE_TRACE("ESD_Handler::instance_deployment_descrs");
      auto esd_e = src.instance ().end ();

      CORBA::ULong pos = 0;
      dest.length (src.instance ().size ());
      for (auto esd_b =
             src.instance ().begin ();
           esd_b != esd_e;
           ++esd_b)
        {
          ESD_Handler::instance_deployment_descr ((*esd_b),
                    dest[pos], pos);
          pos++;
        }
    }


    void
    ESD_Handler::es_deployment_descr (
        const EventServiceDeploymentDescription& src,
        Deployment::EventServiceDeploymentDescription& dest,
        CORBA::ULong pos)
    {
      DANCE_TRACE("IDD_Handler::es_deployment_descr");
      try
      {
        dest.name = src.name ().c_str ();
        dest.node = src.node ().c_str ();
        dest.svc_cfg_file = src.svc_cfg_file ().c_str ());

        switch (desc.type ().integral ())
        {
          case EventServiceType::EC_l:
            dest.type = Deployment::EC;
            break;

          case EventServiceType::RTEC_l:
            dest.type = Deployment::RTEC;
            break;

          case EventServiceType::NOTIFY_l:
            dest.type = Deployment::NOTIFY;
            break;

          case EventServiceType::RTNOTIFY_l:
            dest.type = Deployment::RTNOTIFY;
            break;

          default:
            throw Config_Error (desc.requirementName (),
                    "Unknown EventServiceType.");
            break;
          }
      }
      catch (Config_Error &ex)
      {
        ex.name_ = src.name ()  + ":" + ex.name_;
        throw ex;
      }
          // Done!
    }

    EventServiceDeploymentDescription
    ESD_Handler::es_deployment_descr (
        const Deployment::EventServiceDeploymentDescription& src)
    {
      DANCE_TRACE("ESD_Handler::es_deployment_descr - reverse");
      //Get all the string
      XMLSchema::string<char> name ((src.name));
      XMLSchema::string<char> node ((src.node));
      XMLSchema::string<char> svc_cfg_file ((src.svc_cfg_file));

      // Instantiate the IDD
      EventServiceDeploymentDescription esd (
        name,
        node,
        EventServiceType::RTEC_l,
        svc_cfg_file);

      switch (src.EventServiceType)
      {
      case Deployment::EC:
        esd.EventServiceType (EventServiceType::EC);
        break;

      case Deployment::RTEC:
        esd.EventServiceType (EventServiceType::RTEC);
        break;

      case Deployment::NOTIFY:
        esd.EventServiceType (EventServiceType::NOTIFY);
        break;

      case Deployment::RTNOTIFY:
        esd.EventServiceType (EventServiceType::RTNOTIFY);
        break;

      default:
        throw Config_Error (src.resourceName.in (),
                "Unknown EventServiceType.");
        break;
      }

      return esd;
    }

  }
}
