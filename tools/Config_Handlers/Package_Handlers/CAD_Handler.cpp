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

      toconfig.portName = desc.portName ().c_str ();

      ::XMLSchema::IDREF<char> const* instanceRef = desc.instance ().idref_if_set ();
      if (!instanceRef)
        throw Config_Error ("", "Subcomponent port endpoint has no instance IDREF");

      const std::string str (instanceRef->id ().c_str ());
      CORBA::ULong pos {};

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

      return SubcomponentPortEndpoint ( ACE_TEXT_CHAR_TO_TCHAR (src.portName.in ()), xid);
    }
      };

      struct Packaging_Handlers_Export ACD_Handler
      {
    static void handle_acd (const AssemblyConnectionDescription &desc,
                ::Deployment::AssemblyConnectionDescription &toconfig)
    {
      DANCE_TRACE("ACD_Handler::get_acd");

      toconfig.name = desc.name ().c_str ();

      toconfig.deployRequirement.length (desc.deployRequirement ().size ());
      CORBA::ULong deployRequirement_index = 0;
      for (auto const& source_item : desc.deployRequirement ())
        Req_Handler::handle_requirement (source_item, toconfig.deployRequirement[deployRequirement_index++]);

      toconfig.internalEndpoint.length (desc.internalEndpoint ().size ());
      CORBA::ULong internalEndpoint_index = 0;
      for (auto const& source_item : desc.internalEndpoint ())
        SPE_Handler::handle_spe (source_item, toconfig.internalEndpoint[internalEndpoint_index++]);

      toconfig.externalEndpoint.length (desc.externalEndpoint ().size ());
      CORBA::ULong externalEndpoint_index = 0;
      for (auto const& source_item : desc.externalEndpoint ())
        CEPE_Handler::handle_external_port_endpoint (source_item, toconfig.externalEndpoint[externalEndpoint_index++]);

      toconfig.externalReference.length (desc.externalReference ().size ());
      CORBA::ULong externalReference_index = 0;
      for (auto const& source_item : desc.externalReference ())
        ERE_Handler::handle_external_ref_endpoint (source_item, toconfig.externalReference[externalReference_index++]);

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

      struct Packaging_Handlers_Export SPR_Handler
      {
    static void handle_spr (const SubcomponentPropertyReference &desc,
                ::Deployment::SubcomponentPropertyReference &toconfig)
    {
      DANCE_TRACE("SPR_Handler::get_spr");

      toconfig.propertyName = desc.propertyName ().c_str ();

      ::XMLSchema::IDREF<char> const* instanceRef = desc.instance ().idref_if_set ();
      if (!instanceRef)
        throw Config_Error ("", "Subcomponent property reference has no instance IDREF");

      const std::string str (instanceRef->id ().c_str ());
      CORBA::ULong pos {};

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
      return SubcomponentPropertyReference (ACE_TEXT_CHAR_TO_TCHAR (src.propertyName.in ()), xid);
    }

      };

      struct Packaging_Handlers_Export APM_Handler
      {
    static void handle_apm (const AssemblyPropertyMapping &desc,
                ::Deployment::AssemblyPropertyMapping &toconfig)
    {
      DANCE_TRACE("APM_Handler::get_apm");

      toconfig.name = desc.name ().c_str ();
      toconfig.externalName = desc.externalName ().c_str ();

      toconfig.delegatesTo.length (desc.delegatesTo ().size ());
      CORBA::ULong delegatesTo_index = 0;
      for (auto const& source_item : desc.delegatesTo ())
        SPR_Handler::handle_spr (source_item, toconfig.delegatesTo[delegatesTo_index++]);
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

      void
      CAD_Handler::component_assem_descr (
        const ComponentAssemblyDescription &desc,
        ::Deployment::ComponentAssemblyDescription &toconfig)
      {
        DANCE_TRACE("CAD_Handler::component_assem_descr");

        toconfig.instance.length (desc.instance ().size ());
        CORBA::ULong instance_index = 0;
        for (auto const& source_item : desc.instance ())
          SID_Handler::handle_sub_comp_inst_descr (source_item, toconfig.instance[instance_index++]);

        /* @@ MAJO: Implement Locality */

        toconfig.connection.length (desc.connection ().size ());
        CORBA::ULong connection_index = 0;
        for (auto const& source_item : desc.connection ())
          ACD_Handler::handle_acd (source_item, toconfig.connection[connection_index++]);

        toconfig.externalProperty.length (desc.externalProperty ().size ());
        CORBA::ULong externalProperty_index = 0;
        for (auto const& source_item : desc.externalProperty ())
          APM_Handler::handle_apm (source_item, toconfig.externalProperty[externalProperty_index++]);
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
          retval.add_externalProperty ( APM_Handler::get_apm (src.externalProperty[i]));
#endif
        return retval;

      }

      ComponentAssemblyDescription * CAD_Handler::resolve_cad (std::string_view)
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

