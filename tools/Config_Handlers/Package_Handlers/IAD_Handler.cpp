#include "Package_Handlers/IAD_Handler.h"
#include "Package_Handlers/NIA_Handler.h"
#include "Basic_Deployment_Data.hpp"
#include "Property_Handler.h"
#include "Req_Handler.h"
#include "Utils/Functors.h"
#include "Utils/Exceptions.h"
#include "ace/XML_Utils/XMLSchema/Types.hpp"
#include "ace/XML_Utils/XML_Helper.h"

#include "Deployment.hpp"
#include "dance/Logger/Log_Macros.h"
#include <string>

namespace DAnCE
{
  namespace Config_Handlers
  {
    namespace Packaging
    {

      void
      IAD_Handler::impl_artifact_descr (const ImplementationArtifactDescription &desc,
                                        ::Deployment::ImplementationArtifactDescription &toconfig)
      {
        DANCE_TRACE ("IAD_Handler::impl_artifact_descr");

        const ImplementationArtifactDescription *iad = 0;
        std::unique_ptr<ImplementationArtifactDescription> safe_iad;

        ::XMLSchema::string<char> const* href_if_set_ptr = desc.href_if_set ();
        if (href_if_set_ptr)
          {
            safe_iad.reset (IAD_Handler::resolve_iad (href_if_set_ptr->c_str ()));
            iad = safe_iad.get ();
          }
        else
          iad = &desc;

        ::XMLSchema::string<char> const* label_if_set_ptr = iad->label_if_set ();
        if (label_if_set_ptr)
          toconfig.label = label_if_set_ptr->c_str ();

        ::XMLSchema::string<char> const* UUID_if_set_ptr = iad->UUID_if_set ();
        if (UUID_if_set_ptr)
          toconfig.UUID = UUID_if_set_ptr->c_str ();

        toconfig.location.length (iad->location ().size ());
        std::for_each (iad->location ().begin (),
                       iad->location ().end (),
                       String_Seq_Functor (toconfig.location));

        toconfig.dependsOn.length (iad->dependsOn ().size ());
        SEQ_HAND_GCC_BUG_WORKAROUND (NIA_Handler::handle_nia,
                                     desc.dependsOn ().front (),
                                     toconfig.dependsOn);
        std::for_each (iad->dependsOn ().begin (),
                       iad->dependsOn ().end (),
                       NIA_Functor (toconfig.dependsOn));

        toconfig.execParameter.length (iad->execParameter ().size ());
        std::for_each (iad->execParameter ().begin (),
                       iad->execParameter ().end (),
                       Property_Functor (toconfig.execParameter));

        toconfig.infoProperty.length (iad->infoProperty ().size ());
        std::for_each (iad->infoProperty ().begin (),
                       iad->infoProperty ().end (),
                       Property_Functor (toconfig.infoProperty));

        toconfig.deployRequirement.length (iad->deployRequirement ().size ());
        std::for_each (iad->deployRequirement ().begin (),
                       iad->deployRequirement ().end (),
                       Requirement_Functor (toconfig.deployRequirement ));
      }

      ImplementationArtifactDescription
      IAD_Handler::impl_artifact_descr (const ::Deployment::ImplementationArtifactDescription &src)
      {
        DANCE_TRACE ("IAD_Handler::impl_artifact_descr - reverse");
        ImplementationArtifactDescription retval;

        retval.label (ACE_TEXT_CHAR_TO_TCHAR (src.label.in ()));
        retval.UUID (ACE_TEXT_CHAR_TO_TCHAR  (src.UUID.in ()));
#if 0
        for (CORBA::ULong i = 0; i < src.location.length (); ++i)
          retval.add_location (src.location[i].in ());

        for (CORBA::ULong i = 0; i < src.dependsOn.length (); ++i)
          retval.add_dependsOn (NIA_Handler::get_nia (src.dependsOn[i]));

        for (CORBA::ULong i = 0; i < src.execParameter.length (); ++i)
          retval.add_execParameter
            (Property_Handler::get_property (src.execParameter[i]));

        for (CORBA::ULong i = 0; i < src.infoProperty.length (); ++i)
          retval.add_infoProperty
            (Property_Handler::get_property ( src.infoProperty[i]));

        for (CORBA::ULong i = 0; i < src.deployRequirement.length (); ++i)
          retval.add_deployRequirement
            (Req_Handler::get_requirement (src.deployRequirement[i]));
#endif
        return retval;
      }

      ImplementationArtifactDescription * IAD_Handler::resolve_iad (std::string_view uri)
      {
        DANCE_TRACE ("IAD_Handler::resolve_iad");

        XML::XML_Helper<> xml_helper;
        const std::string uri_string (uri);
        xercesc::DOMDocument *dom = xml_helper.create_dom (ACE_TEXT_CHAR_TO_TCHAR (uri_string.c_str()));

        if (!dom)
          throw Parse_Error ("Unable to create DOM for IAD");

        try {
          return new ImplementationArtifactDescription
            (reader::implementationArtifactDescription (dom));
        }
        catch (...) {
          throw Parse_Error ("Unable to create XSC structure for IAD");
        }
      }

    }
  }
}
