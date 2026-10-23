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
      ADD_Handler::artifact_deployment_descrs (deploymentPlan const& src,
                                               ::Deployment::ArtifactDeploymentDescriptions &dest)
      {
        DANCE_TRACE("ADD_Handler::atrifact_deployment_descrs");

        auto aci_e = src.artifact ().end ();
        dest.length (src.artifact ().size ());
        CORBA::ULong pos {};
        for (auto aci_b = src.artifact ().begin (); aci_e != aci_b; ++aci_b)
          {
            ADD_Handler::artifact_deployment_descr (*aci_b, dest[pos], pos);
            ++pos;
          }
      }

      void
      ADD_Handler::artifact_deployment_descr (
                                              ArtifactDeploymentDescription const& src,
                                              Deployment::ArtifactDeploymentDescription &dest,
                                              CORBA::ULong pos)
      {
        DANCE_TRACE("ADD_Handler::atrifact_deployment_descr");
        dest.name = src.name ().c_str ();

        dest.node = src.node ().c_str ();

        auto end = src.location ().end ();

        dest.location.length (src.location ().size ());
        CORBA::ULong len {};
        for (auto start = src.location ().begin (); start != end; ++start)
          {
            dest.location[len++] = start->c_str ();
          }

        auto sce = src.source ().end ();
        len = 0;
        dest.source.length (src.source ().size ());
        for (auto scb = src.source ().begin (); scb != sce; ++scb)
          {
            dest.source[len++] = scb->c_str ();
          }

        // @@TODO: See this loop is repeated
        auto adce = src.execParameter ().end ();
        len = 0;
        dest.execParameter.length (src.execParameter ().size ());
        for (auto adcb = src.execParameter ().begin (); adcb != adce; ++adcb)
          {
            Property_Handler::handle_property (*adcb, dest.execParameter[len++]);
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
                        "(%P|%t) Warning: ADD %C has no idref.\n",
                        src.name ().c_str ()));
          }

      }

      ArtifactDeploymentDescription
      ADD_Handler::artifact_deployment_descr (
                                              Deployment::ArtifactDeploymentDescription const& src)
      {
        DANCE_TRACE("ADD_Handler::atrifact_deployment_descr - reverse");
        //Get the name and node and store them in the add
        XMLSchema::string<char> name (src.name);
        XMLSchema::string<char> node (src.node);

        ArtifactDeploymentDescription add (name,node);

        // Generate a UUID to use for the IDREF.
        ACE_Utils::UUID uuid {};
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
