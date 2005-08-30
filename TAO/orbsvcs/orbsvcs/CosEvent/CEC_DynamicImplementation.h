/* -*- C++ -*- */
//=============================================================================
/**
 *  @file   CEC_DynamicImplementation.h
 *
 *  $Id$
 *
 *  @author Jon Astle (jon@astle45.fsnet.co.uk)
 */
//=============================================================================


#ifndef TAO_CEC_DYNAMICIMPLEMENTATION_H
#define TAO_CEC_DYNAMICIMPLEMENTATION_H

#include "CEC_TypedProxyPushConsumer.h"
#include "CEC_TypedEventChannel.h"
#include "tao/DynamicInterface/Server_Request.h"
#include "tao/DynamicInterface/Dynamic_Implementation.h"
#include "tao/ORB.h"

class TAO_CEC_DynamicImplementationServer : public TAO_DynamicImplementation
{
  // = TITLE
  //   DSI TAO_CEC_DynamicImplementationServer implementation
  //
  // = DESCRIPTION
  //   Implements the DSI impl object
  //
public:

  // Constructure
  TAO_CEC_DynamicImplementationServer (PortableServer::POA_ptr poa,
                                       TAO_CEC_TypedProxyPushConsumer *typed_pp_consumer,
                                       TAO_CEC_TypedEventChannel *typed_event_channel);

  //Destructor 
  virtual ~TAO_CEC_DynamicImplementationServer (void);

  // = The DynamicImplementation methods.
  virtual void invoke (CORBA::ServerRequest_ptr request
                       ACE_ENV_ARG_DECL)
      ACE_THROW_SPEC ((CORBA::SystemException));

  virtual CORBA::RepositoryId _primary_interface (
      const PortableServer::ObjectId &oid,
      PortableServer::POA_ptr poa
      ACE_ENV_ARG_DECL
    )
      ACE_THROW_SPEC (());

  virtual PortableServer::POA_ptr _default_POA (
      ACE_ENV_SINGLE_ARG_DECL
    );

  // Handles the _is_a call
  virtual void is_a (CORBA::ServerRequest_ptr request
                     ACE_ENV_ARG_DECL);

private:
  // The POA
  PortableServer::POA_var poa_;

  // The Typed Proxy Push Consumer Implementation
  TAO_CEC_TypedProxyPushConsumer *typed_pp_consumer_;

  // The Typed Event Channel Implementation
  TAO_CEC_TypedEventChannel *typed_event_channel_;

  // The RepositoryId
  CORBA::RepositoryId repository_id_;
};

#if defined(__ACE_INLINE__)
#include "CEC_DynamicImplementation.i"
#endif /* __ACE_INLINE__ */

#endif /* TAO_CEC_DYNAMICIMPLEMENTATION_H */
