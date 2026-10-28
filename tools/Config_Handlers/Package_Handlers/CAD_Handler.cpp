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
    static void handle_spe (SubcomponentPortEndpoint const& desc,
                ::Deployment::SubcomponentPortEndpoint &toconfig)
    {
      DANCE_TRACE("SPE_Handler::get_spe");

      toconfig.portName = desc.portName ().c_str ();

      ::XMLSchema::IDREF<char> const* instanceRef = desc.instance ().idref_if_set ();
      if (!instanceRef)
        throw Config_Error ("", "Subcomponent port endpoint has no instance IDREF");

      std::string const str (instanceRef->id ().c_str ());
      CORBA::ULong pos {};

      SID_Handler::IDREF.find_ref (str, pos);
      toconfig.instanceRef = pos;
    }

    static SubcomponentPortEndpoint
    get_spe (::Deployment::SubcomponentPortEndpoint const& src)
    {
      DANCE_TRACE("SPE_Handler::get_spe - reverse");
      std::string str {};
      SID_Handler::IDREF.find_ref (src.instanceRef, str);

      XMLSchema::string<char> port_name (src.portName.in ());
      return SubcomponentPortEndpoint (
        port_name, IdRef (std::nullopt, XMLSchema::IDREF<char> (str.c_str ())));
    }
      };

      struct Packaging_Handlers_Export ACD_Handler
      {
    static void handle_acd (AssemblyConnectionDescription const& desc,
                ::Deployment::AssemblyConnectionDescription &toconfig)
    {
      DANCE_TRACE("ACD_Handler::get_acd");

      toconfig.name = desc.name ().c_str ();

      toconfig.deployRequirement.length (desc.deployRequirement ().size ());
      CORBA::ULong deployRequirement_index {};
      for (auto const& source_item : desc.deployRequirement ())
        Req_Handler::handle_requirement (source_item, toconfig.deployRequirement[deployRequirement_index++]);

      toconfig.internalEndpoint.length (desc.internalEndpoint ().size ());
      CORBA::ULong internalEndpoint_index {};
      for (auto const& source_item : desc.internalEndpoint ())
        SPE_Handler::handle_spe (source_item, toconfig.internalEndpoint[internalEndpoint_index++]);

      toconfig.externalEndpoint.length (desc.externalEndpoint ().size ());
      CORBA::ULong externalEndpoint_index {};
      for (auto const& source_item : desc.externalEndpoint ())
        CEPE_Handler::handle_external_port_endpoint (source_item, toconfig.externalEndpoint[externalEndpoint_index++]);

      toconfig.externalReference.length (desc.externalReference ().size ());
      CORBA::ULong externalReference_index {};
      for (auto const& source_item : desc.externalReference ())
        ERE_Handler::handle_external_ref_endpoint (source_item, toconfig.externalReference[externalReference_index++]);

    }

    static AssemblyConnectionDescription
    get_acd (::Deployment::AssemblyConnectionDescription const& src)
    {
      DANCE_TRACE("ACD_Handler::get_acd - reverse");

      AssemblyConnectionDescription::deployRequirement_container_type requirements;
      for (CORBA::ULong i {}; i < src.deployRequirement.length (); ++i)
        {
          Requirement const requirement (Req_Handler::get_requirement (src.deployRequirement[i]));
          requirements.push_back (requirement);
        }

      AssemblyConnectionDescription::internalEndpoint_container_type internal_endpoints;
      for (CORBA::ULong i {}; i < src.internalEndpoint.length (); ++i)
        {
          SubcomponentPortEndpoint const endpoint (SPE_Handler::get_spe (src.internalEndpoint[i]));
          internal_endpoints.push_back (endpoint);
        }

      AssemblyConnectionDescription::externalEndpoint_container_type external_endpoints;
      for (CORBA::ULong i {}; i < src.externalEndpoint.length (); ++i)
        {
          ComponentExternalPortEndpoint const endpoint (
            CEPE_Handler::external_port_endpoint (src.externalEndpoint[i]));
          external_endpoints.push_back (endpoint);
        }

      AssemblyConnectionDescription::externalReference_container_type external_references;
      for (CORBA::ULong i {}; i < src.externalReference.length (); ++i)
        {
          ExternalReferenceEndpoint const endpoint (
            ERE_Handler::external_ref_endpoint (src.externalReference[i]));
          external_references.push_back (endpoint);
        }

      return AssemblyConnectionDescription (src.name.in (), requirements, internal_endpoints,
                                            external_endpoints, external_references);
    }
      };

      struct Packaging_Handlers_Export SPR_Handler
      {
    static void handle_spr (SubcomponentPropertyReference const& desc,
                ::Deployment::SubcomponentPropertyReference &toconfig)
    {
      DANCE_TRACE("SPR_Handler::get_spr");

      toconfig.propertyName = desc.propertyName ().c_str ();

      ::XMLSchema::IDREF<char> const* instanceRef = desc.instance ().idref_if_set ();
      if (!instanceRef)
        throw Config_Error ("", "Subcomponent property reference has no instance IDREF");

      std::string const str (instanceRef->id ().c_str ());
      CORBA::ULong pos {};

      SID_Handler::IDREF.find_ref (str, pos);
      toconfig.instanceRef = pos;

    }

    static SubcomponentPropertyReference
    get_spr (::Deployment::SubcomponentPropertyReference const& src)
    {
      DANCE_TRACE("SPR_HAndler::get_spr - reverse");
      std::string str {};
      SID_Handler::IDREF.find_ref (src.instanceRef, str);

      return SubcomponentPropertyReference (
        src.propertyName.in (), IdRef (std::nullopt, XMLSchema::IDREF<char> (str.c_str ())));
    }

      };

      struct Packaging_Handlers_Export APM_Handler
      {
    static void handle_apm (AssemblyPropertyMapping const& desc,
                ::Deployment::AssemblyPropertyMapping &toconfig)
    {
      DANCE_TRACE("APM_Handler::get_apm");

      toconfig.name = desc.name ().c_str ();
      toconfig.externalName = desc.externalName ().c_str ();

      toconfig.delegatesTo.length (desc.delegatesTo ().size ());
      CORBA::ULong delegatesTo_index {};
      for (auto const& source_item : desc.delegatesTo ())
        SPR_Handler::handle_spr (source_item, toconfig.delegatesTo[delegatesTo_index++]);
    }

    static AssemblyPropertyMapping
    get_apm (::Deployment::AssemblyPropertyMapping const& src)
    {
      DANCE_TRACE("APM_Handler::get_apm - reverse");

      AssemblyPropertyMapping::delegatesTo_container_type delegates;
      for (CORBA::ULong i {}; i < src.delegatesTo.length (); ++i)
        {
          SubcomponentPropertyReference const delegate (SPR_Handler::get_spr (src.delegatesTo[i]));
          delegates.push_back (delegate);
        }

      return AssemblyPropertyMapping (src.name.in (), src.externalName.in (), delegates);
    }
      };

      void
      CAD_Handler::component_assem_descr (
        ComponentAssemblyDescription const& desc,
        ::Deployment::ComponentAssemblyDescription &toconfig)
      {
        DANCE_TRACE("CAD_Handler::component_assem_descr");

        toconfig.instance.length (desc.instance ().size ());
        CORBA::ULong instance_index {};
        for (auto const& source_item : desc.instance ())
          SID_Handler::handle_sub_comp_inst_descr (source_item, toconfig.instance[instance_index++]);

        /* @@ MAJO: Implement Locality */

        toconfig.connection.length (desc.connection ().size ());
        CORBA::ULong connection_index {};
        for (auto const& source_item : desc.connection ())
          ACD_Handler::handle_acd (source_item, toconfig.connection[connection_index++]);

        toconfig.externalProperty.length (desc.externalProperty ().size ());
        CORBA::ULong externalProperty_index {};
        for (auto const& source_item : desc.externalProperty ())
          APM_Handler::handle_apm (source_item, toconfig.externalProperty[externalProperty_index++]);
      }

      ComponentAssemblyDescription
      CAD_Handler::component_assem_descr (
        ::Deployment::ComponentAssemblyDescription const& src)
      {
        DANCE_TRACE("CAD_Handler::component_assem_descr - reverse");

        ComponentAssemblyDescription::instance_container_type instances;
        for (CORBA::ULong i {}; i < src.instance.length (); ++i)
          {
            SubcomponentInstantiationDescription const instance (
              SID_Handler::sub_comp_inst_descr (src.instance[i]));
            instances.push_back (instance);
          }

        ComponentAssemblyDescription::connection_container_type connections;
        for (CORBA::ULong i {}; i < src.connection.length (); ++i)
          {
            AssemblyConnectionDescription const connection (ACD_Handler::get_acd (src.connection[i]));
            connections.push_back (connection);
          }

        ComponentAssemblyDescription::externalProperty_container_type external_properties;
        for (CORBA::ULong i {}; i < src.externalProperty.length (); ++i)
          {
            AssemblyPropertyMapping const external_property (APM_Handler::get_apm (src.externalProperty[i]));
            external_properties.push_back (external_property);
          }

        ComponentAssemblyDescription::locality_container_type localities;
        for (CORBA::ULong i {}; i < src.localityConstraint.length (); ++i)
          {
            ::DAnCE::Config_Handlers::LocalityKind::Value kind;
            switch (src.localityConstraint[i].constraint)
              {
              case ::Deployment::SameNodeAnyProcess:
                kind = ::DAnCE::Config_Handlers::LocalityKind::SameNodeAnyProcess;
                break;
              case ::Deployment::SameNodeSameProcess:
                kind = ::DAnCE::Config_Handlers::LocalityKind::SameNodeSameProcess;
                break;
              case ::Deployment::SameNodeDifferentProcess:
                kind = ::DAnCE::Config_Handlers::LocalityKind::SameNodeDifferentProcess;
                break;
              case ::Deployment::DifferentNode:
                kind = ::DAnCE::Config_Handlers::LocalityKind::DifferentNode;
                break;
              case ::Deployment::DifferentProcess:
                kind = ::DAnCE::Config_Handlers::LocalityKind::DifferentProcess;
                break;
              case ::Deployment::NoConstraint:
                kind = ::DAnCE::Config_Handlers::LocalityKind::NoConstraint;
                break;
              default:
                throw Config_Error ("", "Unknown LocalityKind.");
              }

            ::DAnCE::Config_Handlers::LocalityKind const constraint (kind);
            for (CORBA::ULong j {}; j < src.localityConstraint[i].constrainedInstanceRef.length (); ++j)
              {
                std::string id;
                SID_Handler::IDREF.find_ref (src.localityConstraint[i].constrainedInstanceRef[j], id);
                ::DAnCE::Config_Handlers::IdRef const instance (
                  std::nullopt, XMLSchema::IDREF<char> (id.c_str ()));
                ::DAnCE::Config_Handlers::Locality const locality (constraint, instance);
                localities.push_back (locality);
              }
          }

        return ComponentAssemblyDescription (instances, connections, external_properties, localities);
      }

      ComponentAssemblyDescription * CAD_Handler::resolve_cad (std::string_view)
      {
        DANCE_TRACE("CAD_Handler::resolve_cad");

        return nullptr;
      }
    }
  }
}

