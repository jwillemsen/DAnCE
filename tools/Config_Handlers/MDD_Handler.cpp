#include "MDD_Handler.h"
#include "Basic_Deployment_Data.hpp"
#include "dance/Deployment/Deployment_DataC.h"
#include "ADD_Handler.h"
#include "Property_Handler.h"
#include "Req_Handler.h"
#include "cdp.hpp"
#include "ace/UUID.h"

namespace DAnCE
{
  namespace Config_Handlers
  {
    IDREF_Base<CORBA::ULong> MDD_Handler::IDREF;

    void
    MDD_Handler::mono_deployment_descriptions (deploymentPlan const& src,
                                               Deployment::MonolithicDeploymentDescriptions& dest)
    {
      DANCE_TRACE("MDD_Handler::mono_deployment_descriptions");

      auto imp_e = src.implementation ().end ();
      CORBA::ULong pos {};
      dest.length (src.implementation ().size ());
      for (auto imp_b = src.implementation ().begin (); imp_b != imp_e; ++imp_b)
        {
          MDD_Handler::mono_deployment_description (*imp_b, dest[pos], pos);
          ++pos;
        }
    }

    void
    MDD_Handler::mono_deployment_description (MonolithicDeploymentDescription const& desc,
                                              Deployment::MonolithicDeploymentDescription& toconfig,
                                              CORBA::ULong pos)
    {
      DANCE_TRACE("MDD_Handler::mono_deployment_description");

      toconfig.name = CORBA::string_dup (desc.name ().c_str ());

      auto me = desc.source ().end ();

      CORBA::ULong len {};
      toconfig.source.length (desc.source ().size ());
      for (auto se = desc.source ().begin (); se != me; ++se)
        {
          toconfig.source[len++] = CORBA::string_dup (se->c_str ());
        }

      auto ae = desc.artifact ().end ();
      len = 0;
      toconfig.artifactRef.length (desc.artifact ().size ());
      for (auto ab = desc.artifact ().begin (); ae != ab; ++ab)
        {
          CORBA::ULong tmp {};

          ::XMLSchema::IDREF<char> const* artifactRef = ab->idref_if_set ();
          if (!artifactRef)
            throw Config_Error ("", "Artifact reference has no IDREF");

          ADD_Handler::IDREF.find_ref (artifactRef->id ().c_str (), tmp);

          toconfig.artifactRef[len++] = tmp;
        }

      auto epce = desc.execParameter ().end ();
      len = 0;
      toconfig.execParameter.length (desc.execParameter ().size ());
      for (auto epcb = desc.execParameter ().begin (); epcb != epce; ++epcb)
        {
          Property_Handler::handle_property (*epcb, toconfig.execParameter[len++]);
        }

      toconfig.deployRequirement.length (desc.deployRequirement ().size ());
      CORBA::ULong deployRequirement_index {};
      for (auto const& source_item : desc.deployRequirement ())
        Req_Handler::handle_requirement (source_item, toconfig.deployRequirement[deployRequirement_index++]);

      // Handle the idref
      ::XMLSchema::ID<char> const* id_if_set_ptr = desc.id_if_set ();
      if (id_if_set_ptr)
        {
          std::string cstr (id_if_set_ptr->c_str ());

          MDD_Handler::IDREF.bind_ref (cstr, pos);
        }
      else
        {
          DANCE_ERROR(DANCE_LOG_NONFATAL_ERROR, (LM_WARNING,
                     "(%P|%t) Warning:  MDD %C has no idref\n",
                     desc.name ().c_str ()));
        }
    }

    MonolithicDeploymentDescription
    MDD_Handler::mono_deployment_description(
        Deployment::MonolithicDeploymentDescription const& src)
    {
      DANCE_TRACE("mono_deployment_description - reverse");

      //Get the name and instantiate the mdd
      XMLSchema::string<char> name (src.name);
      MonolithicDeploymentDescription mdd (name);

      // Generate a UUID to use for the IDREF.
      ACE_Utils::UUID uuid {};
      ACE_Utils::UUID_GENERATOR::instance ()->generate_UUID (uuid);
      std::string mdd_id ("_");
      mdd_id += uuid.to_string ()->c_str ();

      XMLSchema::ID<char> xml_id (mdd_id.c_str ());

      // Bind the ref and set it in the IDD
      MDD_Handler::IDREF.bind_next_available (mdd_id);

      mdd.id (xml_id);

      return mdd;
    }
  }

}

