#include "ace/UUID.h"
#include "IDD_Handler.h"
#include "IRDD_Handler.h"
#include "MDD_Handler.h"
#include "Property_Handler.h"
#include "Any_Handler.h"
#include "Basic_Deployment_Data.hpp"
#include "dance/Deployment/Deployment_DataC.h"
#include "cdp.hpp"

namespace DAnCE
{
  namespace Config_Handlers
  {
    IDREF_Base<CORBA::ULong> IDD_Handler::IDREF;

    void
    IDD_Handler::instance_deployment_descrs (deploymentPlan const& src,
                                             Deployment::InstanceDeploymentDescriptions& dest)
    {
      DANCE_TRACE("IDD_Handler::instance_deployment_descrs");
      CORBA::ULong pos {};
      dest.length (src.instance ().size ());
      for (auto const& instance : src.instance ())
        {
          IDD_Handler::instance_deployment_descr (instance, dest[pos], pos);
          ++pos;
        }
    }

    void
    IDD_Handler::instance_deployment_descr (
                                            InstanceDeploymentDescription const& src,
                                            Deployment::InstanceDeploymentDescription& dest,
                                            CORBA::ULong pos)
    {
      DANCE_TRACE("IDD_Handler::instance_deployment_descr");
      try
        {
          dest.name = src.name ().c_str ();
          dest.node = src.node ().c_str ();

          ::XMLSchema::ID<char> const* id_if_set_ptr = src.id_if_set ();
          if (id_if_set_ptr)
            {
              std::string cstr (id_if_set_ptr->c_str ());
              IDD_Handler::IDREF.bind_ref (cstr, pos);
            }
          else
            {
              DANCE_ERROR (DANCE_LOG_NONFATAL_ERROR, (LM_WARNING,
                               "(%P|%t) Warning:  IDD %C has no idref\n",
                               src.name ().c_str ()));
            }

          // We know there should be only one element
          dest.source.length (1);
          dest.source [0] = src.source ().c_str ();

          CORBA::ULong tmp {};
          ::XMLSchema::IDREF<char> const* implementationRef = src.implementation ().idref_if_set ();
          if (!implementationRef)
            throw Config_Error ("", "Instance deployment has no implementation IDREF");

          MDD_Handler::IDREF.find_ref (implementationRef->id ().c_str (), tmp);

          dest.implementationRef = tmp;

          dest.configProperty.length (src.configProperty ().size ());
          CORBA::ULong configProperty_index {};
          for (auto const& source_item : src.configProperty ())
            Property_Handler::handle_property (source_item, dest.configProperty[configProperty_index++]);

          dest.deployedResource.length (src.deployedResource ().size ());
          CORBA::ULong deployedResource_index {};
          for (auto const& source_item : src.deployedResource ())
            IRDD_Handler::handle_irdd (source_item, dest.deployedResource[deployedResource_index++]);

          ::DAnCE::Config_Handlers::InstanceResourceDeploymentDescription const*
            deployedSharedResource_if_set_ptr = src.deployedSharedResource_if_set ();
          if (deployedSharedResource_if_set_ptr)
            {
              dest.deployedSharedResource.length (1);
              IRDD_Handler::handle_irdd (*deployedSharedResource_if_set_ptr, dest.deployedSharedResource[0]);
            }

        }
      catch (Config_Error &ex)
        {
          ex.name_ = src.name ()  + ":" + ex.name_;
          throw ex;
        }
      catch (...)
        {
          DANCE_ERROR (DANCE_LOG_TERMINAL_ERROR,
            (LM_ERROR, ACE_TEXT ("Unknown exception at IDD:%C"),
            src.name ().c_str ()));
          throw Config_Error (src.name (), "Unknown exception");
        }

      // Done!
    }

    InstanceDeploymentDescription
    IDD_Handler::instance_deployment_descr (
                     Deployment::InstanceDeploymentDescription const& src)
    {
      DANCE_TRACE("IDD_Handler::instance_deployment_descr - reverse");
      //Get all the string/IDREFs
      XMLSchema::string<char> name (src.name);
      XMLSchema::string<char> node (src.node);
      XMLSchema::string<char> source ("");
      if (src.source.length () > 0)
        {
          XMLSchema::string<char> source_detail (src.source[0]);
          source = source_detail;
        }
      std::string temp {};
      MDD_Handler::IDREF.find_ref(src.implementationRef, temp);
      XMLSchema::IDREF<char> implementation ((temp.c_str()));

      // Instantiate the IDD
      IdRef xid {};
      xid.idref (implementation);
      InstanceDeploymentDescription idd (name, node, source, xid);

      //Check if there is a deployedSharedResource, if so store it
      if(src.deployedSharedResource.length() != 0)
        idd. deployedSharedResource(IRDD_Handler::instance_resource_deployment_descr(src.deployedSharedResource[0]));

      // Generate a UUID to use for the IDREF.
      ACE_Utils::UUID uuid {};
      ACE_Utils::UUID_GENERATOR::instance ()->generate_UUID (uuid);
      std::string idd_id ("_");
      idd_id += uuid.to_string ()->c_str ();

      XMLSchema::ID<char> xml_id (idd_id.c_str ());

      // Bind the ref and set it in the IDD
      IDD_Handler::IDREF.bind_next_available (idd_id);

      idd.id (xml_id);

      return idd;
    }

  }
}
