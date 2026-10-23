#include "tao/AnyTypeCode/TypeCode.h"
#include "Package_Handlers/CAD_Handler.h"
#include "Basic_Deployment_Data.hpp"
#include "cid.hpp"
#include "dance/Deployment/Deployment_Packaging_DataC.h"
#include "Req_Handler.h"
#include "CEPE_Handler.h"
#include "ERE_Handler.h"
#include "Utils/Exceptions.h"

#include "Package_Handlers/SID_Handler.h"

namespace DAnCE
{
  namespace Config_Handlers
  {
    namespace Packaging
    {
      struct Packaging_Handlers_Export SPE_Handler
      {
    static void handle_spe (const SubcomponentPortEndpoint &desc,
                ::Deployment::SubcomponentPortEndpoint &toconfig)
    {
      DANCE_TRACE("SPE_Handler::get_spe");

      toconfig.portName =  desc.portName ().c_str ();

      ::XMLSchema::IDREF<char> const* instanceRef =
        desc.instance ().idref_if_set ();
      if (!instanceRef)
        throw Config_Error ("",
                            "Subcomponent port endpoint has no instance IDREF");

      std::string str (instanceRef->id ().c_str ());
      CORBA::ULong pos (0);

      SID_Handler::IDREF.find_ref (str, pos);
      toconfig.instanceRef = pos;
    }

    static SubcomponentPortEndpoint
    get_spe (const ::Deployment::SubcomponentPortEndpoint &src)
    {
      DANCE_TRACE("SPE_Handler::get_spe - reverse");
      std::string str;
      SID_Handler::IDREF.find_ref (src.instanceRef, str);

      IdRef xid;
      xid.idref (str.c_str ());

      return SubcomponentPortEndpoint (
        ACE_TEXT_CHAR_TO_TCHAR (src.portName.in ()),
                                       xid);
    }
      };

      typedef Sequence_Handler < SubcomponentPortEndpoint,
                 ::Deployment::SubcomponentPortEndpoints,
                 ::Deployment::SubcomponentPortEndpoint,
                 SPE_Handler::handle_spe > SPE_Functor;

      struct Packaging_Handlers_Export ACD_Handler
      {
    static void handle_acd (const AssemblyConnectionDescription &desc,
                ::Deployment::AssemblyConnectionDescription &toconfig)
    {
      DANCE_TRACE("ACD_Handler::get_acd");

      toconfig.name =  desc.name ().c_str ();

      toconfig.deployRequirement.length (desc.deployRequirement ().size ());
      std::for_each (desc.deployRequirement ().begin (),
             desc.deployRequirement ().end (),
             Requirement_Functor (toconfig.deployRequirement));

      toconfig.internalEndpoint.length (desc.internalEndpoint ().size ());
      SEQ_HAND_GCC_BUG_WORKAROUND (SPE_Handler::handle_spe,
                                   desc.internalEndpoint ().front (),
                                   toconfig.internalEndpoint);
      std::for_each (desc.internalEndpoint ().begin (),
             desc.internalEndpoint ().end (),
             SPE_Functor (toconfig.internalEndpoint));

      toconfig.externalEndpoint.length (desc.externalEndpoint ().size ());
      std::for_each (desc.externalEndpoint ().begin (),
                     desc.externalEndpoint ().end (),
                     CEPE_Functor (toconfig.externalEndpoint));

      toconfig.externalReference.length (desc.externalReference ().size ());
      std::for_each (desc.externalReference ().begin (),
             desc.externalReference ().end (),
             ERE_Functor (toconfig.externalReference));

    }

    static AssemblyConnectionDescription
    get_acd (const ::Deployment::AssemblyConnectionDescription &src)
    {
      DANCE_TRACE("ACD_Handler::get_acd - reverse");

      AssemblyConnectionDescription retval (ACE_TEXT_CHAR_TO_TCHAR (src.name.in ()));
#if 0
      for (CORBA::ULong i = 0; i < src.deployRequirement.length (); ++i)
        retval.add_deployRequirement (Req_Handler::get_requirement (src.deployRequirement[i]));

      for (CORBA::ULong i = 0; i < src.internalEndpoint.length (); ++i)
        retval.add_internalEndpoint
          (SPE_Handler::get_spe (src.internalEndpoint[i]));

      for (CORBA::ULong i = 0; i < src.externalEndpoint.length (); ++i)
        retval.add_externalEndpoint
          (CEPE_Handler::external_port_endpoint (src.externalEndpoint[i]));

      for (CORBA::ULong i = 0; i < src.externalReference.length (); ++i)
        retval.add_externalReference
          (ERE_Handler::external_ref_endpoint (src.externalReference[i]));
#endif
      return retval;
    }
      };

      typedef Sequence_Handler < AssemblyConnectionDescription,
                 ::Deployment::AssemblyConnectionDescriptions,
                 ::Deployment::AssemblyConnectionDescription,
                 ACD_Handler::handle_acd > ACD_Functor;


      struct Packaging_Handlers_Export SPR_Handler
      {
    static void handle_spr (const SubcomponentPropertyReference &desc,
                ::Deployment::SubcomponentPropertyReference &toconfig)
    {
      DANCE_TRACE("SPR_Handler::get_spr");

      toconfig.propertyName =  desc.propertyName ().c_str ();

      ::XMLSchema::IDREF<char> const* instanceRef =
        desc.instance ().idref_if_set ();
      if (!instanceRef)
        throw Config_Error ("",
                            "Subcomponent property reference has no instance IDREF");

      std::string str (instanceRef->id ().c_str ());
      CORBA::ULong pos (0);

      SID_Handler::IDREF.find_ref (str, pos);
      toconfig.instanceRef = pos;

    }

    static SubcomponentPropertyReference
    get_spr (const ::Deployment::SubcomponentPropertyReference &src)
    {
      DANCE_TRACE("SPR_HAndler::get_spr - reverse");
      std::string str;
      SID_Handler::IDREF.find_ref (src.instanceRef, str);

      IdRef xid;
      xid.idref (str.c_str ());
      return SubcomponentPropertyReference (ACE_TEXT_CHAR_TO_TCHAR (src.propertyName.in ()),
                                            xid);
    }

      };

      typedef Sequence_Handler < SubcomponentPropertyReference,
                 ::Deployment::SubcomponentPropertyReferences,
                 ::Deployment::SubcomponentPropertyReference,
                 SPR_Handler::handle_spr > SPR_Functor;

      struct Packaging_Handlers_Export APM_Handler
      {
    static void handle_apm (const AssemblyPropertyMapping &desc,
                ::Deployment::AssemblyPropertyMapping &toconfig)
    {
      DANCE_TRACE("APM_Handler::get_apm");

      toconfig.name =  desc.name ().c_str ();
      toconfig.externalName =
        desc.externalName ().c_str ();

      toconfig.delegatesTo.length (desc.delegatesTo ().size ());
      SEQ_HAND_GCC_BUG_WORKAROUND (SPR_Handler::handle_spr,
                                   desc.delegatesTo ().front (),
                                   toconfig.delegatesTo);
      std::for_each (desc.delegatesTo ().begin (),
             desc.delegatesTo ().end (),
             SPR_Functor (toconfig.delegatesTo));
    }

    static AssemblyPropertyMapping
    get_apm (const ::Deployment::AssemblyPropertyMapping &src)
    {
      DANCE_TRACE("APM_Handler::get_apm - reverse");

      AssemblyPropertyMapping retval (
        ACE_TEXT_CHAR_TO_TCHAR (src.name.in ()),
        ACE_TEXT_CHAR_TO_TCHAR (src.externalName.in ()));
#if 0
      for (CORBA::ULong i = 0; i < src.delegatesTo.length (); ++i)
        retval.add_delegatesTo (SPR_Handler::get_spr (src.delegatesTo[i]));
#endif
      return retval;
    }
      };

      typedef Sequence_Handler < AssemblyPropertyMapping,
                 ::Deployment::AssemblyPropertyMappings,
                 ::Deployment::AssemblyPropertyMapping,
                 APM_Handler::handle_apm > APM_Functor;

      void
      CAD_Handler::component_assem_descr (
        const ComponentAssemblyDescription &desc,
        ::Deployment::ComponentAssemblyDescription &toconfig)
      {
        DANCE_TRACE("CAD_Handler::component_assem_descr");

        toconfig.instance.length (desc.instance ().size ());
        std::for_each (desc.instance ().begin (),
                       desc.instance ().end (),
                       SID_Functor (toconfig.instance));

        /* @@ MAJO: Implement Locality */

        toconfig.connection.length (desc.connection ().size ());
        SEQ_HAND_GCC_BUG_WORKAROUND (ACD_Handler::handle_acd,
                                     desc.connection ().front (),
                                     toconfig.connection);
        std::for_each (desc.connection ().begin (),
                       desc.connection ().end (),
                       ACD_Functor (toconfig.connection));

        toconfig.externalProperty.length (desc.externalProperty ().size ());
        SEQ_HAND_GCC_BUG_WORKAROUND (APM_Handler::handle_apm,
                                     desc.externalProperty ().front (),
                                     toconfig.externalProperty);
        std::for_each (desc.externalProperty ().begin (),
                       desc.externalProperty ().end (),
                       APM_Functor (toconfig.externalProperty));
      }

      ComponentAssemblyDescription
      CAD_Handler::component_assem_descr (
        const ::Deployment::ComponentAssemblyDescription &/*src*/)
      {
        DANCE_TRACE("CAD_Handler::component_assem_descr - reverse");

        ComponentAssemblyDescription retval;
#if 0
        for (CORBA::ULong i = 0; i < src.instance.length (); ++i)
          retval.add_instance
            (SID_Handler::sub_comp_inst_descr (src.instance[i]));

        for (CORBA::ULong i = 0; i < src.connection.length (); ++i)
          retval.add_connection (ACD_Handler::get_acd (src.connection[i]));

        for (CORBA::ULong i = 0; i < src.externalProperty.length (); ++i)
          retval.add_externalProperty (
            APM_Handler::get_apm (src.externalProperty[i]));
#endif
        return retval;

      }

      ComponentAssemblyDescription *
      CAD_Handler::resolve_cad (const char *)
      {
        DANCE_TRACE("CAD_Handler::resolve_cad");

        /*      xercesc::DOMDocument *dom =
                this->xml_helper_->create_dom (uri);

                if (!dom)
                throw Parse_Error ("Unable to create DOM for CAD");

                try {
                return new ComponentAssemblyDescription
                (componentAssemblyDescription (dom));
                }
                catch (...) {
                throw Parse_Error ("Unable to create XSC structure for CAD");
        */
        return 0;
      }
    }
  }
}

