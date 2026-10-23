#include "Package_Handlers/IAD_Handler.h"
#include "Package_Handlers/NIA_Handler.h"
#include "Basic_Deployment_Data.hpp"
#include "Property_Handler.h"
#include "Req_Handler.h"
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
        CORBA::ULong location_index {};
        for (auto const& source_item : iad->location ())
          toconfig.location[location_index++] = source_item.c_str ();

        toconfig.dependsOn.length (iad->dependsOn ().size ());
        CORBA::ULong dependsOn_index {};
        for (auto const& source_item : iad->dependsOn ())
          NIA_Handler::handle_nia (source_item, toconfig.dependsOn[dependsOn_index++]);

        toconfig.execParameter.length (iad->execParameter ().size ());
        CORBA::ULong execParameter_index {};
        for (auto const& source_item : iad->execParameter ())
          Property_Handler::handle_property (source_item, toconfig.execParameter[execParameter_index++]);

        toconfig.infoProperty.length (iad->infoProperty ().size ());
        CORBA::ULong infoProperty_index {};
        for (auto const& source_item : iad->infoProperty ())
          Property_Handler::handle_property (source_item, toconfig.infoProperty[infoProperty_index++]);

        toconfig.deployRequirement.length (iad->deployRequirement ().size ());
        CORBA::ULong deployRequirement_index {};
        for (auto const& source_item : iad->deployRequirement ())
          Req_Handler::handle_requirement (source_item, toconfig.deployRequirement[deployRequirement_index++]);
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
