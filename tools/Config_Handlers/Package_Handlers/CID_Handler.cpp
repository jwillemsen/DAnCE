#include "tao/AnyTypeCode/AnyTypeCode_methods.h"
#include "Package_Handlers/CID_Handler.h"
#include "Package_Handlers/CPD_Handler.h"
#include "Package_Handlers/IAD_Handler.h"
#include "Package_Handlers/CAD_Handler.h"
#include "Package_Handlers/Comp_Intf_Descr_Handler.h"
#include "Package_Handlers/NIA_Handler.h"
#include "SatisfierProperty_Handler.h"
#include "Req_Handler.h"

#include "Basic_Deployment_Data.hpp"
#include "dance/Deployment/Deployment_DataC.h"
#include "ace/XML_Utils/XMLSchema/Types.hpp"
#include "ace/XML_Utils/XML_Helper.h"
#include "Utils/Exceptions.h"

#include "Deployment.hpp"
#include "Property_Handler.h"
#include <string>

namespace DAnCE
{
  namespace Config_Handlers
  {
    namespace Packaging
    {

      IDREF_Base<CORBA::ULong> CID_Handler::IDREF;

      struct Capability_Handler
      {
    static void handle_capability (Capability const& desc,
                       ::Deployment::Capability &toconfig)
    {
      DANCE_TRACE ("Capability_Handler::get_capability");

      toconfig.name = desc.name ().c_str ();

      toconfig.resourceType.length (desc.resourceType ().size ());
      CORBA::ULong resourceType_index {};
      for (auto const& source_item : desc.resourceType ())
        toconfig.resourceType[resourceType_index++] = source_item.c_str ();

      toconfig.property.length (desc.property ().size ());
      CORBA::ULong property_index {};
      for (auto const& source_item : desc.property ())
        SatisfierProperty_Handler::handle_sat_property (source_item, toconfig.property[property_index++]);
    }

    static Capability get_capability (::Deployment::Capability const& src)
    {
      DANCE_TRACE ("Capability_Handler::get_capability - reverse");

      Capability::resourceType_container_type resource_types;
      for (CORBA::ULong i {}; i < src.resourceType.length (); ++i)
        resource_types.emplace_back (src.resourceType[i]);

      Capability::property_container_type properties;
      for (CORBA::ULong i {}; i < src.property.length (); ++i)
        {
          SatisfierProperty const property (SatisfierProperty_Handler::get_sat_property (src.property[i]));
          properties.push_back (property);
        }

      return Capability (src.name.in (), resource_types, properties);
    }
      };

      struct IR_Handler
      {
    static void handle_ir (ImplementationRequirement const& desc,
                   ::Deployment::ImplementationRequirement &toconfig)
    {
      DANCE_TRACE ("IR_Handler::get_ir");

      ::DAnCE::Config_Handlers::ResourceUsageKind const* resourceUsage_if_set_ptr = desc.resourceUsage_if_set ();
      if (resourceUsage_if_set_ptr)
        {
          switch (resourceUsage_if_set_ptr->integral ())
        {
        case ResourceUsageKind::None:
          toconfig.resourceUsage = Deployment::None;
          break;
        case ResourceUsageKind::InstanceUsesResource:
          toconfig.resourceUsage = Deployment::InstanceUsesResource;
          break;
        case ResourceUsageKind::ResourceUsesInstance:
          toconfig.resourceUsage = Deployment::ResourceUsesInstance;
          break;
        case ResourceUsageKind::PortUsesResource:
          toconfig.resourceUsage = Deployment::PortUsesResource;
          break;
        case ResourceUsageKind::ResourceUsesPort:
          toconfig.resourceUsage = Deployment::ResourceUsesPort;
          break;

        default:
          throw Config_Error (desc.name (), "Unknown ResourceUsageKind.");
          break;
        }
        }

      ::XMLSchema::string<char> const* resourcePort_if_set_ptr = desc.resourcePort_if_set ();
      if (resourcePort_if_set_ptr)
        toconfig.resourcePort = resourcePort_if_set_ptr->c_str ();

      ::XMLSchema::string<char> const* componentPort_if_set_ptr = desc.componentPort_if_set ();
      if (componentPort_if_set_ptr)
        toconfig.componentPort = componentPort_if_set_ptr->c_str ();

      toconfig.resourceType = desc.resourceType ().c_str ();
      toconfig.name = desc.name ().c_str ();

      toconfig.property.length (desc.property ().size ());
      CORBA::ULong property_index {};
      for (auto const& source_item : desc.property ())
        Property_Handler::handle_property (source_item, toconfig.property[property_index++]);
    }

    static ImplementationRequirement
    get_ir (::Deployment::ImplementationRequirement const& src)
    {
      DANCE_TRACE ("IR_Handler::get_ir - reverse");

      ResourceUsageKind::Value usage_value (ResourceUsageKind::None);

      if (src.resourceUsage.length () == 1)
        switch (src.resourceUsage[0])
          {
          case Deployment::None:
            usage_value = ResourceUsageKind::None;
            break;
          case Deployment::InstanceUsesResource:
            usage_value = ResourceUsageKind::InstanceUsesResource;
            break;
          case Deployment::ResourceUsesInstance:
            usage_value = ResourceUsageKind::ResourceUsesInstance;
            break;
          case Deployment::PortUsesResource:
            usage_value = ResourceUsageKind::PortUsesResource;
            break;
          case Deployment::ResourceUsesPort:
            usage_value = ResourceUsageKind::ResourceUsesPort;
            break;

          default:
            throw Config_Error (src.name.in (), "Unknown ResourceUsageKind.");
          }

      ResourceUsageKind const usage (usage_value);
      XSCRT::OptionalArg<ResourceUsageKind> usage_arg (std::nullopt);
      if (src.resourceUsage.length () == 1)
        usage_arg = XSCRT::OptionalArg<ResourceUsageKind> (usage);

      XMLSchema::string<char> resource_port_value (
        src.resourcePort.in () == nullptr ? "" : src.resourcePort.in ());
      XMLSchema::string<char> component_port_value (
        src.componentPort.in () == nullptr ? "" : src.componentPort.in ());
      XSCRT::OptionalArg<XMLSchema::string<char>> resource_port (std::nullopt);
      XSCRT::OptionalArg<XMLSchema::string<char>> component_port (std::nullopt);
      if (src.resourcePort.in () != nullptr)
        resource_port = XSCRT::OptionalArg<XMLSchema::string<char>> (resource_port_value);
      if (src.componentPort.in () != nullptr)
        component_port = XSCRT::OptionalArg<XMLSchema::string<char>> (component_port_value);

      ImplementationRequirement::property_container_type properties;
      for (CORBA::ULong i {}; i < src.property.length (); ++i)
        {
          Property const property (Property_Handler::get_property (src.property[i]));
          properties.push_back (property);
        }

      return ImplementationRequirement (
        src.resourceType.in (), src.name.in (), usage_arg, resource_port, component_port, properties);
    }
      };

      struct MID_Handler
      {
    static void handle_mid (MonolithicImplementationDescription const& desc,
                ::Deployment::MonolithicImplementationDescription &toconfig)
    {
      DANCE_TRACE ("MID_Handler::get_mid");

      toconfig.nodeExecParameter.length (desc.nodeExecParameter ().size ());
      CORBA::ULong nodeExecParameter_index {};
      for (auto const& source_item : desc.nodeExecParameter ())
        Property_Handler::handle_property (source_item, toconfig.nodeExecParameter[nodeExecParameter_index++]);

      toconfig.componentExecParameter.length (desc.componentExecParameter ().size ());
      CORBA::ULong componentExecParameter_index {};
      for (auto const& source_item : desc.componentExecParameter ())
        Property_Handler::handle_property (
          source_item, toconfig.componentExecParameter[componentExecParameter_index++]);

      toconfig.deployRequirement.length (desc.deployRequirement ().size ());
      CORBA::ULong deployRequirement_index {};
      for (auto const& source_item : desc.deployRequirement ())
        IR_Handler::handle_ir (source_item, toconfig.deployRequirement[deployRequirement_index++]);

      toconfig.primaryArtifact.length (desc.primaryArtifact ().size ());
      CORBA::ULong primaryArtifact_index {};
      for (auto const& source_item : desc.primaryArtifact ())
        NIA_Handler::handle_nia (source_item, toconfig.primaryArtifact[primaryArtifact_index++]);
    }

    static MonolithicImplementationDescription
    get_mid (::Deployment::MonolithicImplementationDescription const& src)
    {
      DANCE_TRACE ("MID_Handler::get_mid - reverse");

      MonolithicImplementationDescription::nodeExecParameter_container_type node_parameters;
      for (CORBA::ULong i {}; i < src.nodeExecParameter.length (); ++i)
        {
          Property const property (Property_Handler::get_property (src.nodeExecParameter[i]));
          node_parameters.push_back (property);
        }

      MonolithicImplementationDescription::componentExecParameter_container_type component_parameters;
      for (CORBA::ULong i {}; i < src.componentExecParameter.length (); ++i)
        {
          Property const property (Property_Handler::get_property (src.componentExecParameter[i]));
          component_parameters.push_back (property);
        }

      MonolithicImplementationDescription::deployRequirement_container_type requirements;
      for (CORBA::ULong i {}; i < src.deployRequirement.length (); ++i)
        {
          ImplementationRequirement const requirement (IR_Handler::get_ir (src.deployRequirement[i]));
          requirements.push_back (requirement);
        }

      MonolithicImplementationDescription::primaryArtifact_container_type artifacts;
      for (CORBA::ULong i {}; i < src.primaryArtifact.length (); ++i)
        {
          NamedImplementationArtifact const artifact (NIA_Handler::get_nia (src.primaryArtifact[i]));
          artifacts.push_back (artifact);
        }

      return MonolithicImplementationDescription (node_parameters, component_parameters,
                                                  requirements, artifacts);
    }

      };

      void
      CID_Handler::component_impl_descr (
        ComponentImplementationDescription const& desc,
        ::Deployment::ComponentImplementationDescription &toconfig)
      {
        DANCE_TRACE ("CID_Handler::component_impl_descr");

        ComponentImplementationDescription const* cid {};
        std::unique_ptr<ComponentImplementationDescription> xsc_cid {};

        ::XMLSchema::string<char> const* href_if_set_ptr = desc.href_if_set ();
        if (href_if_set_ptr)
          {
            xsc_cid.reset (CID_Handler::resolve_cid (href_if_set_ptr->c_str ()));
            cid = xsc_cid.get ();
          }
        else
          cid = &desc;

        ::XMLSchema::string<char> const* label_if_set_ptr = cid->label_if_set ();
        if (label_if_set_ptr)
          toconfig.label = label_if_set_ptr->c_str ();

        ::XMLSchema::string<char> const* UUID_if_set_ptr = cid->UUID_if_set ();
        if (UUID_if_set_ptr)
          toconfig.UUID = UUID_if_set_ptr->c_str ();

        ::DAnCE::Config_Handlers::ComponentInterfaceDescription const*
          implements_if_set_ptr = cid->implements_if_set ();
        if (implements_if_set_ptr)
          {
            // MAJO: We should be able to assume this exists,
            //fix broken interpeters..
            Comp_Intf_Descr_Handler::comp_intf_descr (*implements_if_set_ptr, toconfig.implements);
          }

        ::DAnCE::Config_Handlers::ComponentAssemblyDescription const*
          assemblyImpl_if_set_ptr = cid->assemblyImpl_if_set ();
        ::DAnCE::Config_Handlers::MonolithicImplementationDescription const* monolithicImpl =
          cid->monolithicImpl_if_set ();
        if (assemblyImpl_if_set_ptr)
          {
            toconfig.assemblyImpl.length (1);
            CAD_Handler::component_assem_descr (*assemblyImpl_if_set_ptr, toconfig.assemblyImpl[0]);
          }
        else if (monolithicImpl)
          {
            toconfig.monolithicImpl.length (1);
            MID_Handler::handle_mid (*monolithicImpl, toconfig.monolithicImpl[0]);
          }
        else
          throw Plan_Error (
            ACE_TEXT ("ComponentImplementationDescription must have either"\
                      "assemblyImpl or monolithicImpl"));

        // configProperty
        toconfig.configProperty.length (cid->configProperty ().size ());
        CORBA::ULong configProperty_index {};
        for (auto const& source_item : cid->configProperty ())
          Property_Handler::handle_property (source_item, toconfig.configProperty[configProperty_index++]);

        // capability
        toconfig.capability.length (cid->capability ().size ());
        CORBA::ULong capability_index {};
        for (auto const& source_item : cid->capability ())
          Capability_Handler::handle_capability (source_item, toconfig.capability[capability_index++]);

        // dependsOn
        toconfig.dependsOn.length (cid->dependsOn ().size ());
        CORBA::ULong pos {};
        for (auto const& dependency : cid->dependsOn ())
          toconfig.dependsOn[pos++].requiredType = dependency.requiredType ().c_str ();

        // infoProperty
        toconfig.infoProperty.length (cid->infoProperty ().size ());
        CORBA::ULong infoProperty_index {};
        for (auto const& source_item : cid->infoProperty ())
          Property_Handler::handle_property (source_item, toconfig.infoProperty[infoProperty_index++]);

      }

      ComponentImplementationDescription
      CID_Handler::component_impl_descr (
        Deployment::ComponentImplementationDescription const& src)
      {
        DANCE_TRACE ("CID_Handler::component_impl_descr - reverse");

        XMLSchema::string<char> label (src.label.in () == nullptr ? "" : src.label.in ());
        XMLSchema::string<char> UUID (src.UUID.in () == nullptr ? "" : src.UUID.in ());
        XSCRT::OptionalArg<XMLSchema::string<char>> label_arg (std::nullopt);
        XSCRT::OptionalArg<XMLSchema::string<char>> UUID_arg (std::nullopt);
        if (src.label.in () != nullptr)
          label_arg = XSCRT::OptionalArg<XMLSchema::string<char>> (label);
        if (src.UUID.in () != nullptr)
          UUID_arg = XSCRT::OptionalArg<XMLSchema::string<char>> (UUID);

        ComponentInterfaceDescription implements (
          Comp_Intf_Descr_Handler::comp_intf_descr (src.implements));

        ComponentImplementationDescription::configProperty_container_type config_properties;
        for (CORBA::ULong i {}; i < src.configProperty.length (); ++i)
          {
            Property const property (Property_Handler::get_property (src.configProperty[i]));
            config_properties.push_back (property);
          }

        ComponentImplementationDescription::capability_container_type capabilities;
        for (CORBA::ULong i {}; i < src.capability.length (); ++i)
          {
            Capability const capability (Capability_Handler::get_capability (src.capability[i]));
            capabilities.push_back (capability);
          }

        ComponentImplementationDescription::dependsOn_container_type dependencies;
        for (CORBA::ULong i {}; i < src.dependsOn.length (); ++i)
          {
            ImplementationDependency const dependency (src.dependsOn[i].requiredType.in ());
            dependencies.push_back (dependency);
          }

        ComponentImplementationDescription::infoProperty_container_type info_properties;
        for (CORBA::ULong i {}; i < src.infoProperty.length (); ++i)
          {
            Property const property (Property_Handler::get_property (src.infoProperty[i]));
            info_properties.push_back (property);
          }

        if (src.assemblyImpl.length () + src.monolithicImpl.length () != 1)
          throw Plan_Error (
            "ComponentImplementationDescription must contain exactly one implementation kind");

        if (src.assemblyImpl.length () == 1)
          return ComponentImplementationDescription (
            label_arg, UUID_arg, implements,
            CAD_Handler::component_assem_descr (src.assemblyImpl[0]), std::nullopt,
            config_properties, capabilities, dependencies, info_properties);
        else if (src.monolithicImpl.length () == 1)
          {
            return ComponentImplementationDescription (
              label_arg, UUID_arg, implements, std::nullopt,
              MID_Handler::get_mid (src.monolithicImpl[0]),
              config_properties, capabilities, dependencies, info_properties);
          }

        throw Plan_Error ("ComponentImplementationDescription has an invalid implementation kind");
      }

      ComponentImplementationDescription * CID_Handler::resolve_cid (std::string_view uri)
      {
        DANCE_TRACE ("CID_Handler::resolve_cid");

        XML::XML_Helper<> xml_helper {};
        std::string const uri_string (uri);
        xercesc::DOMDocument *dom = xml_helper.create_dom (ACE_TEXT_CHAR_TO_TCHAR (uri_string.c_str()));

        if (!dom)
          throw Parse_Error ("Unable to create DOM for CID");

        try {
          return new ComponentImplementationDescription
            (reader::componentImplementationDescription (dom));
        }
        catch (...) {
          throw Parse_Error ("Unable to create XSC structure for CID");
        }
      }
    }

  }
}
