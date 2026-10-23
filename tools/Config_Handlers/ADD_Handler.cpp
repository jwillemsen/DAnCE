#include "ADD_Handler.h"
#include "cdp.hpp"
#include "dance/Deployment/Deployment_DataC.h"
#include "Property_Handler.h"
#include "ace/UUID.h"

namespace DAnCE
{
  namespace Config_Handlers
  {
    IDREF_Base<CORBA::ULong> ADD_Handler::IDREF;

      void
      ADD_Handler::artifact_deployment_descrs (const deploymentPlan &src,
                                               ::Deployment::ArtifactDeploymentDescriptions &dest)
      {
        DANCE_TRACE("ADD_Handler::atrifact_deployment_descrs");

        auto aci_e =
          src.artifact ().end ();
        dest.length (src.artifact ().size ());
        CORBA::ULong pos = 0;
        for (auto aci_b =
               src.artifact ().begin ();
             aci_e != aci_b;
             ++aci_b)
          {
            ADD_Handler::artifact_deployment_descr (*aci_b,
                                                    dest[pos],
                                                    pos);
            pos++;
          }
      }

      void
      ADD_Handler::artifact_deployment_descr (
                                              const ArtifactDeploymentDescription &src,
                                              Deployment::ArtifactDeploymentDescription &dest,
                                              CORBA::ULong pos)
      {
        DANCE_TRACE("ADD_Handler::atrifact_deployment_descr");
        dest.name =  src.name ().c_str ();

        dest.node =  src.node ().c_str ();

        auto end =
          src.location ().end ();

        dest.location.length (src.location ().size ());
        CORBA::ULong len = 0;
        for (auto
               start = src.location ().begin ();
             start != end;
             ++start)
          {
            dest.location[len++] =  start->c_str ();
          }

        auto sce =
          src.source ().end ();
        len = 0;
        dest.source.length (src.source ().size ());
        for (auto
               scb = src.source ().begin ();
             scb != sce;
             ++scb)
          {
            dest.source[len++] =  scb->c_str ();
          }

        // @@TODO: See this loop is repeated
        auto adce =
          src.execParameter ().end ();
        len = 0;
        dest.execParameter.length (src.execParameter ().size ());
        for (auto adcb =
               src.execParameter ().begin ();
             adcb != adce;
             ++adcb)
          {
            Property_Handler::handle_property (*adcb,
                                               dest.execParameter[len++]);
          }

        ::XMLSchema::ID<char> const* id_if_set_ptr = src.id_if_set ();
        if (id_if_set_ptr)
          {
            std::string cstr (id_if_set_ptr->c_str ());

            ADD_Handler::IDREF.bind_ref (cstr,pos);
          }
        else
          {
            DANCE_DEBUG (DANCE_LOG_NONFATAL_ERROR, (LM_ERROR,
                        "(%P|%t) Warning: ADD %s has no idref.\n",
                        src.name ().c_str ()));
          }

#if 0
        // @@ MAJO: Don't know how to handle this.
        if (src.deployRequirement_if_set ())
          {
            Req_Handler handler;
            add.deployRequirement.length (
                                          add.deployRequirement.length () + 1);
            handler.get_Requirement (
                                     add.deployRequirement[add.deployRequirement.length () - 1],
                                     src.deployRequirement ());
          }

        if (src.deployedResource_if_set ())
          {
            RDD_Handler handler;
            add.deployedResource.length (
                                         add.deployedResource.length () + 1);
            handler.get_ResourceDeploymentDescription (
                                                       add.deployedResource[add.deployedResource.length () - 1],
                                                       src.deployedResource ());
          }
#endif /* 0  */
      }

      ArtifactDeploymentDescription
      ADD_Handler::artifact_deployment_descr (
                                              const Deployment::ArtifactDeploymentDescription &src)
      {
        DANCE_TRACE("ADD_Handler::atrifact_deployment_descr - reverse");
        //Get the name and node and store them in the add
        XMLSchema::string<char> name (src.name);
        XMLSchema::string<char> node (src.node);

        ArtifactDeploymentDescription add (name,node);

        //Get the location(s) and store it/them in the add
        size_t total = src.location.length ();
        for (size_t i = 0; i < total; ++i)
          {
            XMLSchema::string<char> curr (src.location[i]);
            //add.add_location (curr);
          }

        //As above, for the source(s)
        total = src.source.length ();
        for (size_t j = 0; j < total; ++j)
          {
            XMLSchema::string<char> curr (src.source[j]);
            //add.add_source (curr);
          }

        //As above for the execParameter(s)
        total = src.execParameter.length ();
        for (size_t k = 0; k < total; ++k)
          {
            //add.add_execParameter (Property_Handler::get_property (src.execParameter[k]));
          }

        // Generate a UUID to use for the IDREF.
        ACE_Utils::UUID uuid;
        ACE_Utils::UUID_GENERATOR::instance ()->generate_UUID (uuid);
        std::string add_id ("_");
        add_id += uuid.to_string ()->c_str ();

        XMLSchema::ID<char> xml_id (add_id.c_str ());

        // Bind the ref and set it in the IDD
        ADD_Handler::IDREF.bind_next_available (add_id);

        add.id (xml_id);

        return add;
      }
    }
  }
