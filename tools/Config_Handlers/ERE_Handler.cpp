#include "ERE_Handler.h"
#include "Basic_Deployment_Data.hpp"
#include "dance/Deployment/Deployment_DataC.h"
#include "dance/Logger/Log_Macros.h"
namespace DAnCE
{
  namespace Config_Handlers
  {
    void
    ERE_Handler::external_ref_endpoints (
                                         PlanConnectionDescription const& src,
                                         Deployment::ExternalReferenceEndpoints &dest)
    {
      DANCE_TRACE("ERE_Handler::external_ref_endpoints");
      auto erep_e = src.externalReference ().end ();

      CORBA::ULong pos {};
      dest.length (src.externalReference ().size ());
      for (auto erep_b = src.externalReference ().begin (); erep_b != erep_e; ++erep_b)
        {
          ERE_Handler::handle_external_ref_endpoint (*erep_b, dest[pos++]);
        }
    }

    void
    ERE_Handler::handle_external_ref_endpoint (
                                        ExternalReferenceEndpoint const& src,
                                        Deployment::ExternalReferenceEndpoint &dest)
    {
      DANCE_TRACE("ERE_Handler::external_ref_endpoint");
      dest.location = src.location ().c_str ();
      dest.provider = src.provider ();
      ::XMLSchema::string<char> const* portName_if_set_ptr = src.portName_if_set ();
      if (portName_if_set_ptr)
        dest.portName = portName_if_set_ptr->c_str ();
      dest.supportedType.length (src.supportedType ().size ());
      CORBA::ULong supportedType_index {};
      for (auto const& source_item : src.supportedType ())
        dest.supportedType[supportedType_index++] = source_item.c_str ();
    }

    ExternalReferenceEndpoint ERE_Handler::external_ref_endpoint (Deployment::ExternalReferenceEndpoint const& src)
    {
      DANCE_TRACE("ERE_Handler::external_ref_endpoint - reverse");
      XMLSchema::string<char> loc (src.location);
      //      XMLSchema::string< char > provider ((src.provider));
      XMLSchema::string<char> pn (src.portName);
      //      XMLSchema::string< char > st ((src.supportedType));
      ExternalReferenceEndpoint erp (loc, src.provider);
      return erp;
    }
  }
}
