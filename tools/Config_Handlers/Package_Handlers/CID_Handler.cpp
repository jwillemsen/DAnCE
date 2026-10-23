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


namespace DAnCE
{
  namespace Config_Handlers
  {
    namespace Packaging
    {

      IDREF_Base<CORBA::ULong> CID_Handler::IDREF;

      struct Capability_Handler
      {
    static void handle_capability (const Capability &desc,
                       ::Deployment::Capability &toconfig)
    {
      DANCE_TRACE ("Capability_Handler::get_capability");

      toconfig.name =  desc.name ().c_str ();

      toconfig.resourceType.length (desc.resourceType ().size ());
      std::for_each (desc.resourceType ().begin (),
             desc.resourceType ().end (),
             String_Seq_Functor (toconfig.resourceType));

      toconfig.property.length (desc.property ().size ());
      std::for_each (desc.property ().begin (),
             desc.property ().end (),
             SatisfierProperty_Functor (toconfig.property));
    }

    static Capability get_capability (const ::Deployment::Capability &src)
    {
      DANCE_TRACE ("Capability_Handler::get_capability - reverse");

      Capability retval (ACE_TEXT_CHAR_TO_TCHAR (src.name.in ()));
#if 0
      for (CORBA::ULong i = 0; i < src.resourceType.length (); ++i)
        retval.add_resourceType (src.resourceType[i].in ());

      for (CORBA::ULong i = 0; i < src.property.length (); ++i)
        retval.add_property (SatisfierProperty_Handler::get_sat_property (
          src.property[i]));
#endif
      return retval;
    }
      };

      typedef Sequence_Handler < Capability,
                 ::Deployment::Capabilities,
                 ::Deployment::Capability,
                 Capability_Handler::handle_capability > Capability_Functor;

      struct IR_Handler
      {
    static void handle_ir (const ImplementationRequirement &desc,
                   ::Deployment::ImplementationRequirement &toconfig)
    {
      DANCE_TRACE ("IR_Handler::get_ir");


      ::DAnCE::Config_Handlers::ResourceUsageKind const* resourceUsage_if_set_ptr = desc.resourceUsage_if_set ();
      if (resourceUsage_if_set_ptr)
        {
          switch (resourceUsage_if_set_ptr->integral ())
        {
        case ResourceUsageKind::None_l:
          toconfig.resourceUsage = Deployment::None;
          break;

        case ResourceUsageKind::InstanceUsesResource_l:
          toconfig.resourceUsage = Deployment::InstanceUsesResource;
          break;

        case ResourceUsageKind::ResourceUsesInstance_l:
          toconfig.resourceUsage = Deployment::ResourceUsesInstance;
          break;

        case ResourceUsageKind::PortUsesResource_l:
          toconfig.resourceUsage = Deployment::PortUsesResource;
          break;

        case ResourceUsageKind::ResourceUsesPort_l:
          toconfig.resourceUsage = Deployment::ResourceUsesPort;
          break;

        default:
          throw Config_Error (desc.name (),
                              ACE_TEXT ("Unknown ResourceUsageKind."));
          break;
        }
        }


      ::XMLSchema::string<char> const* resourcePort_if_set_ptr = desc.resourcePort_if_set ();
      if (resourcePort_if_set_ptr)
        toconfig.resourcePort = resourcePort_if_set_ptr->c_str ();


      ::XMLSchema::string<char> const* componentPort_if_set_ptr = desc.componentPort_if_set ();
      if (componentPort_if_set_ptr)
        toconfig.componentPort = componentPort_if_set_ptr->c_str ();

      toconfig.resourceType =  desc.resourceType ().c_str ();
      toconfig.name =  desc.name ().c_str ();

      std::for_each (desc.property ().begin (),
             desc.property ().end (),
             Property_Functor (toconfig.property ));
    }

    static ImplementationRequirement
    get_ir (const ::Deployment::ImplementationRequirement &src)
    {
      DANCE_TRACE ("IR_Handler::get_ir - reverse");

      ImplementationRequirement retval (ACE_TEXT_CHAR_TO_TCHAR (src.resourceType.in ()),
                                        ACE_TEXT_CHAR_TO_TCHAR (src.name.in ()));

      if (src.resourceUsage.length () == 1)
        switch (src.resourceUsage[0])
          {
          case Deployment::None:
        retval.resourceUsage (ResourceUsageKind::None);
        break;

          case Deployment::InstanceUsesResource:
        retval.resourceUsage (ResourceUsageKind::InstanceUsesResource);
        break;

          case Deployment::ResourceUsesInstance:
        retval.resourceUsage (ResourceUsageKind::ResourceUsesInstance);
        break;

          case Deployment::PortUsesResource:
        retval.resourceUsage (ResourceUsageKind::PortUsesResource);
        break;

          case Deployment::ResourceUsesPort:
        retval.resourceUsage (ResourceUsageKind::ResourceUsesPort);
        break;

          default:
            throw Config_Error (ACE_TEXT_CHAR_TO_TCHAR (src.name.in ()),
                            ACE_TEXT ("Unknown ResourceUsageKind."));
        break;
          }

      retval.resourcePort (ACE_TEXT_CHAR_TO_TCHAR (src.resourcePort.in ()));

      retval.componentPort (ACE_TEXT_CHAR_TO_TCHAR (src.componentPort.in ()));
#if 0
      for (CORBA::ULong i = 0; i < src.property.length (); ++i)
        retval.add_property (
          Property_Handler::get_property (src.property[i]));
#endif
      return retval;
    }
      };

      typedef Sequence_Handler < ImplementationRequirement,
                 ::Deployment::ImplementationRequirements,
                 ::Deployment::ImplementationRequirement,
                 IR_Handler::handle_ir > IR_Functor;

      struct MID_Handler
      {
    static void handle_mid (const MonolithicImplementationDescription &desc,
                ::Deployment::MonolithicImplementationDescription &toconfig)
    {
      DANCE_TRACE ("MID_Handler::get_mid");

      toconfig.nodeExecParameter.length (desc.nodeExecParameter ().size ());
      std::for_each (desc.nodeExecParameter ().begin (),
             desc.nodeExecParameter ().end (),
             Property_Functor (toconfig.nodeExecParameter));

      toconfig.componentExecParameter.length (
        desc.componentExecParameter ().size ());
      std::for_each (desc.componentExecParameter ().begin (),
             desc.componentExecParameter ().end (),
             Property_Functor (toconfig.componentExecParameter));

      toconfig.deployRequirement.length (desc.deployRequirement ().size ());
      while (0)
        {
          IR_Handler::handle_ir (desc.deployRequirement ().front (),
                                 toconfig.deployRequirement[0]);
        }
      std::for_each (desc.deployRequirement ().begin (),
             desc.deployRequirement ().end (),
             IR_Functor (toconfig.deployRequirement));

      toconfig.primaryArtifact.length (desc.primaryArtifact ().size ());
      SEQ_HAND_GCC_BUG_WORKAROUND (NIA_Handler::handle_nia,
                                   desc.primaryArtifact ().front (),
                                   toconfig.primaryArtifact);
      std::for_each (desc.primaryArtifact ().begin (),
             desc.primaryArtifact ().end (),
             NIA_Functor (toconfig.primaryArtifact));
    }

    static MonolithicImplementationDescription
    get_mid (const ::Deployment::MonolithicImplementationDescription &/*src*/)
    {
      DANCE_TRACE ("MID_Handler::get_mid - reverse");

      MonolithicImplementationDescription retval;
#if 0
      for (CORBA::ULong i = 0; i < src.nodeExecParameter.length (); ++i)
        retval.add_nodeExecParameter
          (Property_Handler::get_property (src.nodeExecParameter[i]));

      for (CORBA::ULong i = 0; i < src.componentExecParameter.length (); ++i)
        retval.add_componentExecParameter
          (Property_Handler::get_property (src.componentExecParameter[i]));

      for (CORBA::ULong i = 0; i < src.deployRequirement.length (); ++i)
        retval.add_deployRequirement
          (IR_Handler::get_ir (src.deployRequirement[i]));

      for (CORBA::ULong i = 0; i < src.primaryArtifact.length (); ++i)
        retval.add_primaryArtifact
          (NIA_Handler::get_nia (src.primaryArtifact[i]));
#endif
      return retval;
    }

      };

      typedef Sequence_Handler < MonolithicImplementationDescription,
                 ::Deployment::MonolithicImplementationDescriptions,
                 ::Deployment::MonolithicImplementationDescription,
                 MID_Handler::handle_mid > MID_Functor;



      void
      CID_Handler::component_impl_descr (
        const ComponentImplementationDescription &desc,
        ::Deployment::ComponentImplementationDescription &toconfig)
      {
        DANCE_TRACE ("CID_Handler::component_impl_descr");

        const ComponentImplementationDescription *cid = 0;
        std::auto_ptr <ComponentImplementationDescription> xsc_cid;


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
            Comp_Intf_Descr_Handler::comp_intf_descr (*implements_if_set_ptr,
                                                      toconfig.implements);
          }


        ::DAnCE::Config_Handlers::ComponentAssemblyDescription const*
          assemblyImpl_if_set_ptr = cid->assemblyImpl_if_set ();
        ::DAnCE::Config_Handlers::MonolithicImplementationDescription const* monolithicImpl =
          cid->monolithicImpl_if_set ();
        if (assemblyImpl_if_set_ptr)
          {
            toconfig.assemblyImpl.length (1);
            CAD_Handler::component_assem_descr (*assemblyImpl_if_set_ptr,
                                                toconfig.assemblyImpl[0]);
          }
        else if (monolithicImpl)
          {
            toconfig.monolithicImpl.length (1);
            MID_Handler::handle_mid (*monolithicImpl,
                     toconfig.monolithicImpl[0]);
          }
        else
          throw Plan_Error (
            ACE_TEXT ("ComponentImplementationDescription must have either"\
                      "assemblyImpl or monolithicImpl"));

        // configProperty
        toconfig.configProperty.length (cid->configProperty ().size ());
        std::for_each (cid->configProperty ().begin (),
                       cid->configProperty ().end (),
                       Property_Functor (toconfig.configProperty));

        // capability
        toconfig.capability.length (cid->capability ().size ());
        SEQ_HAND_GCC_BUG_WORKAROUND (Capability_Handler::handle_capability,
                                     cid->capability ().front (),
                                     toconfig.capability);
        std::for_each (cid->capability ().begin (),
                       cid->capability ().end (),
                       Capability_Functor (toconfig.capability));

        // dependsOn
        toconfig.dependsOn.length (cid->dependsOn ().size ());
        CORBA::ULong pos = 0;
        for (auto i =
               cid->dependsOn ().begin ();
             i != cid->dependsOn ().end ();
             ++i)
          toconfig.dependsOn[pos++].requiredType =
             i->requiredType ().c_str ();

        // infoProperty
        toconfig.infoProperty.length (cid->infoProperty ().size ());
        std::for_each (cid->infoProperty ().begin (),
                       cid->infoProperty ().end (),
                       Property_Functor (toconfig.infoProperty));

      }

      ComponentImplementationDescription
      CID_Handler::component_impl_descr (
        const Deployment::ComponentImplementationDescription& src)
      {
        DANCE_TRACE ("CID_Handler::component_impl_descr - reverse");

        ComponentImplementationDescription retval;

        retval.label (ACE_TEXT_CHAR_TO_TCHAR (src.label.in ()));
        retval.UUID (ACE_TEXT_CHAR_TO_TCHAR (src.UUID.in ()));

        {
          retval.implements (Comp_Intf_Descr_Handler::comp_intf_descr (
            src.implements));
        }

        if (src.assemblyImpl.length () == 1)
          {
            retval.assemblyImpl (CAD_Handler::component_assem_descr (
              src.assemblyImpl[0]));
          }
        else if (src.monolithicImpl.length () == 1)
          {
            retval.monolithicImpl (MID_Handler::get_mid (
              src.monolithicImpl[0]));
          }
        else
          DANCE_ERROR (DANCE_LOG_WARNING,
            (LM_WARNING, "Warning: ComponentImplementationDescription lacks "
                         "either a required assemblyImpl or monolithicImpl, "\
                         "or has too many"));
#if 0
        for (CORBA::ULong i = 0; i < src.configProperty.length (); ++i)
          {
            retval.add_configProperty (
                                       Property_Handler::get_property (
                                         src.configProperty[i]));
          }

        for (CORBA::ULong i = 0; i < src.dependsOn.length (); ++i)
          {
            retval.add_dependsOn (ImplementationDependency (
              src.dependsOn[i].requiredType.in ()));
          }

        for (CORBA::ULong i = 0; i < src.infoProperty.length (); ++i)
          {
            retval.add_infoProperty (
                                     Property_Handler::get_property (
                                       src.infoProperty[i]));
          }
#endif
        return retval;
      }

      ComponentImplementationDescription *
      CID_Handler::resolve_cid (const ACE_TCHAR *uri)
      {
        DANCE_TRACE ("CID_Handler::resolve_cid");

        XML::XML_Helper<> xml_helper;
        xercesc::DOMDocument *dom = xml_helper.create_dom (uri);

        if (!dom)
          throw Parse_Error (ACE_TEXT ("Unable to create DOM for CID"));

        try {
          return new ComponentImplementationDescription
            (reader::componentImplementationDescription (dom));
        }
        catch (...) {
          throw Parse_Error (
            ACE_TEXT ("Unable to create XSC structure for CID"));
        }
      }
    }

  }
}
