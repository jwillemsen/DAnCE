#include "ace/XML_Utils/XMLSchema/Types.hpp"
#include "SatisfierProperty_Handler.h"
#include "DD_Handler.h"
#include "Any_Handler.h"
#include "Deployment.hpp"
#include "dance/Deployment/Deployment_TargetDataC.h"
#include "dance/Logger/Log_Macros.h"

using namespace std;

namespace DAnCE
{
  namespace Config_Handlers
  {

    DD_Handler::DD_Handler (Domain *dmn):
      domain_ptr_ (dmn),
      domain_ (*dmn)
    {
      DANCE_TRACE("DP_PCD_Handler::constructor - Domain");
      if(!this->build_domain ())
        throw NoDomain ();
    }

    DD_Handler::DD_Handler (Domain &dmn):
      domain_ (dmn)
    {
      DANCE_TRACE("DP_PCD_Handler::constructor - Domain");
      if(!this->build_domain ())
        throw NoDomain ();
    }

    DD_Handler::~DD_Handler ()
    {
    }

    bool DD_Handler::build_domain ()
    {
      DANCE_TRACE("DP_PCD_Handler::build_domain");
      this->idl_domain_ = std::make_unique<::Deployment::Domain> ();

      // Read in the name
      // Check if the label is there or not
      ::XMLSchema::string<char> const* label_if_set_ptr = domain_.label_if_set ();
      if (label_if_set_ptr)
        this->idl_domain_->label = CORBA::string_dup (label_if_set_ptr->c_str ());

      ::XMLSchema::string<char> const* UUID_if_set_ptr = domain_.UUID_if_set ();
      if (UUID_if_set_ptr)
        this->idl_domain_->UUID = CORBA::string_dup (UUID_if_set_ptr->c_str ());

      CORBA::ULong const len = domain_.node ().size ();
      this->idl_domain_->node.length (len);

      CORBA::ULong node_index {};
      for (auto const& node : domain_.node ())
        {
          this->idl_domain_->node[node_index].name = CORBA::string_dup (node.name ().c_str ());

          ::XMLSchema::string<char> const* node_label_if_set_ptr = node.label_if_set ();
          if (node_label_if_set_ptr)
            this->idl_domain_->node[node_index].label = CORBA::string_dup (node_label_if_set_ptr->c_str ());

          // length is hard-coded for now ...
          // here iterate over the resources ...
          CORBA::ULong const res_len = node.resource ().size ();
          this->idl_domain_->node[node_index].resource.length (res_len);

          CORBA::ULong resource_index {};
          for (auto const& resource : node.resource ())
            {
              this->idl_domain_->node[node_index].resource[resource_index].name =
                CORBA::string_dup (resource.name ().c_str ());

              CORBA::ULong const resource_type_len = resource.resourceType ().size ();
              this->idl_domain_->node[node_index].resource[resource_index].resourceType.length (resource_type_len);

              CORBA::ULong resource_type_index {};
              for (auto const& resource_type : resource.resourceType ())
                {
                  this->idl_domain_->node[node_index].resource[resource_index].resourceType[resource_type_index++] =
                    CORBA::string_dup (resource_type.c_str ());
                }

              CORBA::ULong const property_len = resource.property ().size ();
              this->idl_domain_->node[node_index].resource[resource_index].property.length (property_len);

              CORBA::ULong property_index {};
              for (auto const& source_item : resource.property ())
                SatisfierProperty_Handler::handle_sat_property (source_item,
                  this->idl_domain_->node[node_index].resource[resource_index].property[property_index++]);
              ++resource_index;
            }
          ++node_index;
        }

      return true;
    }

    ::Deployment::Domain const *
    DD_Handler::domain_idl () const
    {
      if(!this->idl_domain_.get())
        throw NoDomain ();

      return this->idl_domain_.get();
    }

    ::Deployment::Domain * DD_Handler::domain_idl ()
    {
      if(!this->idl_domain_.get())
        throw NoDomain();

      return this->idl_domain_.release();
    }

    Domain const *
    DD_Handler::domain_xsc () const
    {
      if(!this->domain_ptr_.get())
        throw NoDomain ();

      return this->domain_ptr_.get();
    }

    Domain * DD_Handler::domain_xsc ()
    {
      if(!this->domain_ptr_.get())
        throw NoDomain ();

      return this->domain_ptr_.release();
    }
  }
}

