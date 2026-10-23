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
      idl_domain_ (nullptr),
      domain_ptr_ (dmn),
      domain_ (*dmn),
      retval_(false)
    {
      DANCE_TRACE("DP_PCD_Handler::constructor - Domain");
      if(!this->build_domain ())
        throw NoDomain ();
    }

    DD_Handler::DD_Handler (Domain &dmn):
      idl_domain_ (nullptr),
      domain_ (dmn),
      retval_(false)
    {
      DANCE_TRACE("DP_PCD_Handler::constructor - Domain");
      if(!this->build_domain ())
        throw NoDomain ();
    }

    /*
    DD_Handler::DD_Handler (::Deployment::Domain *dmn):
      idl_domain_(dmn),
      domain_ptr_ (dmn),
      retval_(false)
    {
      if(!this->build_xsc())
        throw NoDomain ();
    }
    */

    DD_Handler::~DD_Handler (void)
    {
    }

    bool DD_Handler::build_domain ()
    {
      DANCE_TRACE("DP_PCD_Handler::build_domain");
      this->idl_domain_.reset ( new ::Deployment::Domain );

      // Read in the name
      // Check if the label is there or not
      ::XMLSchema::string<char> const* label_if_set_ptr = domain_.label_if_set ();
      if (label_if_set_ptr)
        this->idl_domain_->label =
          CORBA::string_dup (label_if_set_ptr->c_str ());


      ::XMLSchema::string<char> const* UUID_if_set_ptr = domain_.UUID_if_set ();
      if (UUID_if_set_ptr)
        this->idl_domain_->UUID =
          CORBA::string_dup (UUID_if_set_ptr->c_str ());

      CORBA::ULong len = domain_.node ().size ();
      this->idl_domain_->node.length (len);

      //Resource _resource;
      int i =0;
      for (auto iter = domain_.node ().begin ();
           iter != domain_.node ().end ();
           ++iter,++i
           )
        {
          // Node* thisNode = (Node*)iter;
          this->idl_domain_->node[i].name =
            CORBA::string_dup (iter->name ().c_str ());


          ::XMLSchema::string<char> const* label_if_set_ptr_2 = iter->label_if_set ();
          if (label_if_set_ptr_2)
            this->idl_domain_->node[i].label =
              CORBA::string_dup (label_if_set_ptr_2->c_str ());

          // length is hard-coded for now ...
          // here iterate over the resources ...
          CORBA::ULong res_len = iter->resource ().size ();
          this->idl_domain_->node[i].resource.length (res_len);

          int res_id=0;
          // FOR EACH RESOURCE ....
          for (auto res_iter = iter->resource ().begin ();
               res_iter != iter->resource ().end ();
               res_iter++, res_id++)
            {

              this->idl_domain_->node[i].resource[res_id].name =
                CORBA::string_dup (res_iter->name ().c_str ());

              CORBA::ULong resource_type_len = res_iter->resourceType ().size ();
              this->idl_domain_->node[i].resource[res_id].resourceType.length (resource_type_len);

              int res_type_id =0;

              // FOR EACH RESOURCE TYPE
              for (auto res_type_iter =
                     res_iter->resourceType ().begin ();
                   res_type_iter != res_iter->resourceType ().end ();
                   res_type_iter++, res_type_id++)
                {
                  this->idl_domain_->node[i].resource[res_id].resourceType[res_type_id] =
                    CORBA::string_dup (res_type_iter->c_str ());
                }

              CORBA::ULong property_len = res_iter->property ().size ();
              this->idl_domain_->node[i].resource[res_id].property.length (property_len);

              //              int property_id =0;

              std::for_each (res_iter->property ().begin (),
                             res_iter->property ().end (),
                             SatisfierProperty_Functor (this->idl_domain_->node[i].resource[res_id].property));

              /*
              //FOR EACH PROPERTY
              for (auto property_iter =
              res_iter->property ().begin ();
              property_iter != res_iter->property ().end ();
              property_iter++, property_id++)
              {
              this->idl_domain_->node[i].resource[res_id].property[property_id ].name =
              CORBA::string_dup (property_iter->name ().c_str ());


              this->idl_domain_->node[i].resource[res_id].property[property_id].kind =
              ::Deployment::Quantity;

              Any the_any = property_iter->value ();
              CORBA::Any a_corba_any;
              Any_Handler::extract_into_any (the_any , a_corba_any);
              this->idl_domain_->node[i].resource[res_id].property[property_id].value
              = a_corba_any;
              }
              */
            }
        }

      return true;

    }

    bool DD_Handler::build_xsc ()
    {
      return true;
    }

    ::Deployment::Domain const *
    DD_Handler::domain_idl () const
    {
      if(!this->idl_domain_.get())
        throw NoDomain ();

      //else
      return this->idl_domain_.get();
    }

    ::Deployment::Domain * DD_Handler::domain_idl ()
    {
      if(!this->idl_domain_.get())
        throw NoDomain();

      //else
      return this->idl_domain_.release();
    }

    Domain const *
    DD_Handler::domain_xsc () const
    {
      if(!this->domain_ptr_.get())
        throw NoDomain ();

      //else
      return this->domain_ptr_.get();
    }

    Domain * DD_Handler::domain_xsc ()
    {
      if(!this->domain_ptr_.get())
        throw NoDomain ();

      //else
      return this->domain_ptr_.release();
    }
  }
}

