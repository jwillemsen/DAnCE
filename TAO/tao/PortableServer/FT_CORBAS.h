/* -*- C++ -*- $Id$ */

// ******  Code generated by the The ACE ORB (TAO) IDL Compiler *******
// TAO and the TAO IDL Compiler have been developed by the Center for
// Distributed Object Computing at Washington University, St. Louis.
//
// Information about TAO is available at:
//                 http://www.cs.wustl.edu/~schmidt/TAO.html

#ifndef TAO_FT_CORBAS_H_
#define TAO_FT_CORBAS_H_
#include "ace/pre.h"

#include "POA_CORBA.h"
#include "tao/ftpoafwd.h"
#include "tao/FT_CORBAC.h"
#include "tao/PolicyC.h"

#if (TAO_HAS_FT_CORBA == 1)

#if !defined (ACE_LACKS_PRAGMA_ONCE)
# pragma once
#endif /* ACE_LACKS_PRAGMA_ONCE */

#if defined(_MSC_VER)
#if (_MSC_VER >= 1200)
#pragma warning(push)
#endif /* _MSC_VER >= 1200 */
#pragma warning(disable:4250)
#endif /* _MSC_VER */


class TAO_PortableServer_Export FT_POA_RequestDurationPolicy : public virtual POA_CORBA::Policy
{
protected:
  FT_POA_RequestDurationPolicy (void);

public:
  FT_POA_RequestDurationPolicy (const FT_POA_RequestDurationPolicy& rhs);
  virtual ~FT_POA_RequestDurationPolicy (void);


  virtual CORBA::Boolean _is_a (
      const char* logical_type_id,
      CORBA::Environment &ACE_TRY_ENV =
      TAO_default_environment ()
      );

    virtual void* _downcast (
        const char* logical_type_id
      );

    static void _is_a_skel (
        CORBA::ServerRequest &req,
        void *obj,
        void *context,
        CORBA::Environment &ACE_TRY_ENV =
          TAO_default_environment ()
      );

    static void _non_existent_skel (
        CORBA::ServerRequest &req,
        void *obj,
        void *context,
        CORBA::Environment &ACE_TRY_ENV =
          TAO_default_environment ()
      );

    virtual void _dispatch (
        CORBA::ServerRequest &_tao_req,
        void *_tao_context,
        CORBA::Environment &ACE_TRY_ENV =
          TAO_default_environment ()
      );

    ::FT::RequestDurationPolicy *_this (
        CORBA::Environment &ACE_TRY_ENV =
          TAO_default_environment ()
      );

    virtual const char* _interface_repository_id (void) const;

    virtual TimeBase::TimeT request_duration_value (
        CORBA::Environment &ACE_TRY_ENV =
          TAO_default_environment ()
      )
      ACE_THROW_SPEC ((
        CORBA::SystemException
      )) = 0;

    static void _get_request_duration_value_skel (
        CORBA::ServerRequest &_tao_req,
        void *_tao_obj,
        void *_tao_context,
        CORBA::Environment &ACE_TRY_ENV =
          TAO_default_environment ()
      );

    static void _get_policy_type_skel (
        CORBA::ServerRequest &req,
        void *obj,
        void *context,
        CORBA::Environment &ACE_TRY_ENV =
          TAO_default_environment ()
      );

    static void copy_skel (
        CORBA::ServerRequest &req,
        void *obj,
        void *context,
        CORBA::Environment &ACE_TRY_ENV =
          TAO_default_environment ()
      );

    static void destroy_skel (
        CORBA::ServerRequest &req,
        void *obj,
        void *context,
        CORBA::Environment &ACE_TRY_ENV =
          TAO_default_environment ()
      );
};


class TAO_PortableServer_Export
FT_POA_tao_thru_poa_collocated_RequestDurationPolicy :
  public virtual FT::RequestDurationPolicy,
  public virtual POA_CORBA::_tao_thru_poa_collocated_Policy
{
public:
  FT_POA_tao_thru_poa_collocated_RequestDurationPolicy (
      TAO_Stub *stub
      );
  virtual CORBA::Boolean _is_a(
      const CORBA::Char *logical_type_id,
      CORBA_Environment &ACE_TRY_ENV = TAO_default_environment ()
      );

  virtual CORBA::Boolean _non_existent(
      CORBA_Environment &ACE_TRY_ENV = TAO_default_environment ()
      );

  virtual TimeBase::TimeT request_duration_value (
      CORBA::Environment &ACE_TRY_ENV =
        TAO_default_environment ()
    )
    ACE_THROW_SPEC ((
      CORBA::SystemException
    ));

};


#if defined (ACE_HAS_USING_KEYWORD)
# if 0
// This wouldn't work. We need to find a way around for these
// guys. If somebody wants to use it, we may have to do something
// about it.

// TIE class: Refer to CORBA v2.2, Section 20.34.4
template <class T>
class TAO_PortableServer_Export FT_POA_RequestDurationPolicy_tie :
  public FT_POA_RequestDurationPolicy
{
public:
  FT_POA_RequestDurationPolicy_tie (T &t);
  // the T& ctor
  FT_POA_RequestDurationPolicy_tie (T &t, PortableServer::POA_ptr poa);
  // ctor taking a POA
  FT_POA_RequestDurationPolicy_tie (T *tp, CORBA::Boolean release=1);
  // ctor taking pointer and an ownership flag
  FT_POA_RequestDurationPolicy_tie (T *tp, PortableServer::POA_ptr poa, CORBA::Boolean release=1);
  // ctor with T*, ownership flag and a POA
  ~FT_POA_RequestDurationPolicy_tie (void);
  // dtor

  // TIE specific functions
  T *_tied_object (void);
  // return the underlying object
  void _tied_object (T &obj);
  // set the underlying object
  void _tied_object (T *obj, CORBA::Boolean release=1);
  // set the underlying object and the ownership flag
  CORBA::Boolean _is_owner (void);
  // do we own it
  void _is_owner (CORBA::Boolean b);
  // set the ownership

  // overridden ServantBase operations
  PortableServer::POA_ptr _default_POA (
      CORBA::Environment &ACE_TRY_ENV =
        TAO_default_environment ()
    );
  TimeBase::TimeT request_duration_value (
      CORBA::Environment &ACE_TRY_ENV =
        TAO_default_environment ()
    )
    ACE_THROW_SPEC ((
      CORBA::SystemException
    ));

  CORBA::PolicyType policy_type (
      CORBA::Environment &ACE_TRY_ENV =
        TAO_default_environment ()
    )
    ACE_THROW_SPEC ((
      CORBA::SystemException
    ));

   CORBA::Policy_ptr copy (
      CORBA::Environment &ACE_TRY_ENV =
        TAO_default_environment ()
    )
     ACE_THROW_SPEC ((
        CORBA::SystemException
      ));

  void destroy (
      CORBA::Environment &ACE_TRY_ENV =
        TAO_default_environment ()
    )
    ACE_THROW_SPEC ((
      CORBA::SystemException
    ));

private:
  T *ptr_;
  PortableServer::POA_var poa_;
  CORBA::Boolean rel_;

  // copy and assignment are not allowed
  FT_POA_RequestDurationPolicy_tie (const FT_POA_RequestDurationPolicy_tie &);
  void operator= (const FT_POA_RequestDurationPolicy_tie &);
};


#endif /*if 0*/

#endif /* ACE_HAS_USING_KEYWORD */

class TAO_PortableServer_Export FT_POA_HeartbeatPolicy : public virtual POA_CORBA::Policy
{
protected:
  FT_POA_HeartbeatPolicy (void);

public:
  FT_POA_HeartbeatPolicy (const FT_POA_HeartbeatPolicy& rhs);
  virtual ~FT_POA_HeartbeatPolicy (void);


  virtual CORBA::Boolean _is_a (
      const char* logical_type_id,
      CORBA::Environment &ACE_TRY_ENV =
        TAO_default_environment ()
    );

  virtual void* _downcast (
      const char* logical_type_id
    );

  static void _is_a_skel (
      CORBA::ServerRequest &req,
      void *obj,
      void *context,
      CORBA::Environment &ACE_TRY_ENV =
        TAO_default_environment ()
    );

  static void _non_existent_skel (
      CORBA::ServerRequest &req,
      void *obj,
      void *context,
      CORBA::Environment &ACE_TRY_ENV =
        TAO_default_environment ()
    );

  virtual void _dispatch (
      CORBA::ServerRequest &_tao_req,
      void *_tao_context,
      CORBA::Environment &ACE_TRY_ENV =
        TAO_default_environment ()
    );

  FT::HeartbeatPolicy *_this (
      CORBA::Environment &ACE_TRY_ENV =
        TAO_default_environment ()
    );

  virtual const char* _interface_repository_id (void) const;

  virtual FT::HeartbeatPolicyValue heartbeat_policy_value (
      CORBA::Environment &ACE_TRY_ENV =
      TAO_default_environment ()
    )
    ACE_THROW_SPEC ((
      CORBA::SystemException
    )) = 0;

  static void _get_heartbeat_policy_value_skel (
      CORBA::ServerRequest &_tao_req,
      void *_tao_obj,
      void *_tao_context,
      CORBA::Environment &ACE_TRY_ENV =
        TAO_default_environment ()
    );

  static void _get_policy_type_skel (
      CORBA::ServerRequest &req,
      void *obj,
      void *context,
      CORBA::Environment &ACE_TRY_ENV =
        TAO_default_environment ()
    );

  static void copy_skel (
      CORBA::ServerRequest &req,
      void *obj,
      void *context,
      CORBA::Environment &ACE_TRY_ENV =
        TAO_default_environment ()
    );

  static void destroy_skel (
      CORBA::ServerRequest &req,
      void *obj,
      void *context,
      CORBA::Environment &ACE_TRY_ENV =
        TAO_default_environment ()
    );
};


class TAO_PortableServer_Export FT_POA_tao_thru_poa_collocated_HeartbeatPolicy:
  public virtual FT::HeartbeatPolicy,
  public virtual POA_CORBA::_tao_thru_poa_collocated_Policy
{
public:
  FT_POA_tao_thru_poa_collocated_HeartbeatPolicy (
      TAO_Stub *stub
    );
  virtual CORBA::Boolean _is_a(
      const CORBA::Char *logical_type_id,
      CORBA_Environment &ACE_TRY_ENV = TAO_default_environment ()
    );

  virtual CORBA::Boolean _non_existent(
      CORBA_Environment &ACE_TRY_ENV = TAO_default_environment ()
    );

  virtual FT::HeartbeatPolicyValue heartbeat_policy_value (
      CORBA::Environment &ACE_TRY_ENV =
        TAO_default_environment ()
    )
    ACE_THROW_SPEC ((
      CORBA::SystemException
    ));
};


#if defined (ACE_HAS_USING_KEYWORD)
# if 0
// This wouldn't work. We need to find a way around for these
// guys. If somebody wants to use it, we may have to do something
// about it.
// TIE class: Refer to CORBA v2.2, Section 20.34.4
template <class T>
class TAO_PortableServer_Export FT_POA_HeartbeatPolicy_tie : public FT_POA_HeartbeatPolicy
{
public:
  FT_POA_HeartbeatPolicy_tie (T &t);
  // the T& ctor
  FT_POA_HeartbeatPolicy_tie (T &t, PortableServer::POA_ptr poa);
  // ctor taking a POA
  FT_POA_HeartbeatPolicy_tie (T *tp, CORBA::Boolean release=1);
    // ctor taking pointer and an ownership flag
  FT_POA_HeartbeatPolicy_tie (T *tp,
                              PortableServer::POA_ptr poa,
                              CORBA::Boolean release=1);

  // ctor with T*, ownership flag and a POA
  ~FT_POA_HeartbeatPolicy_tie (void);
  // dtor

  // TIE specific functions
  T *_tied_object (void);
  // return the underlying object
  void _tied_object (T &obj);
  // set the underlying object
  void _tied_object (T *obj, CORBA::Boolean release=1);
  // set the underlying object and the ownership flag
  CORBA::Boolean _is_owner (void);
  // do we own it
  void _is_owner (CORBA::Boolean b);
  // set the ownership

    // overridden ServantBase operations
  PortableServer::POA_ptr _default_POA (
      CORBA::Environment &ACE_TRY_ENV =
        TAO_default_environment ()
      );
  FT::HeartbeatPolicyValue heartbeat_policy_value (
      CORBA::Environment &ACE_TRY_ENV =
        TAO_default_environment ()
    )
    ACE_THROW_SPEC ((
      CORBA::SystemException
    ));

  CORBA::PolicyType policy_type (
      CORBA::Environment &ACE_TRY_ENV =
        TAO_default_environment ()
    )
    ACE_THROW_SPEC ((
      CORBA::SystemException
    ));

  CORBA::Policy_ptr copy (
      CORBA::Environment &ACE_TRY_ENV =
        TAO_default_environment ()
    )
    ACE_THROW_SPEC ((
      CORBA::SystemException
    ));

  void destroy (CORBA::Environment &ACE_TRY_ENV =
                TAO_default_environment ())
      ACE_THROW_SPEC ((CORBA::SystemException));

private:
  T *ptr_;
  PortableServer::POA_var poa_;
  CORBA::Boolean rel_;

  // copy and assignment are not allowed
  FT_POA_HeartbeatPolicy_tie (const FT_POA_HeartbeatPolicy_tie &);
  void operator= (const FT_POA_HeartbeatPolicy_tie &);
};


#endif /*if 0*/
#endif /* ACE_HAS_USING_KEYWORD */


class TAO_PortableServer_Export FT_POA_HeartbeatEnabledPolicy : public virtual POA_CORBA::Policy
{
protected:
  FT_POA_HeartbeatEnabledPolicy (void);

public:
  FT_POA_HeartbeatEnabledPolicy (const FT_POA_HeartbeatEnabledPolicy& rhs);
  virtual ~FT_POA_HeartbeatEnabledPolicy (void);


  virtual CORBA::Boolean _is_a (
      const char* logical_type_id,
      CORBA::Environment &ACE_TRY_ENV =
        TAO_default_environment ()
    );

  virtual void* _downcast (
      const char* logical_type_id
    );

  static void _is_a_skel (
      CORBA::ServerRequest &req,
      void *obj,
      void *context,
      CORBA::Environment &ACE_TRY_ENV =
        TAO_default_environment ()
    );

  static void _non_existent_skel (
      CORBA::ServerRequest &req,
      void *obj,
      void *context,
      CORBA::Environment &ACE_TRY_ENV =
        TAO_default_environment ()
    );

  virtual void _dispatch (
      CORBA::ServerRequest &_tao_req,
      void *_tao_context,
      CORBA::Environment &ACE_TRY_ENV =
        TAO_default_environment ()
    );

  FT::HeartbeatEnabledPolicy *_this (
      CORBA::Environment &ACE_TRY_ENV =
          TAO_default_environment ()
    );

  virtual const char* _interface_repository_id (void) const;

  virtual CORBA::Boolean heartbeat_enabled_policy_value (
      CORBA::Environment &ACE_TRY_ENV =
        TAO_default_environment ()
    )
    ACE_THROW_SPEC ((
      CORBA::SystemException
    )) = 0;

  static void _get_heartbeat_enabled_policy_value_skel (
      CORBA::ServerRequest &_tao_req,
      void *_tao_obj,
      void *_tao_context,
      CORBA::Environment &ACE_TRY_ENV =
        TAO_default_environment ()
    );

  static void _get_policy_type_skel (
      CORBA::ServerRequest &req,
      void *obj,
      void *context,
      CORBA::Environment &ACE_TRY_ENV =
        TAO_default_environment ()
    );

  static void copy_skel (
      CORBA::ServerRequest &req,
      void *obj,
      void *context,
      CORBA::Environment &ACE_TRY_ENV =
        TAO_default_environment ()
    );

  static void destroy_skel (
      CORBA::ServerRequest &req,
      void *obj,
      void *context,
      CORBA::Environment &ACE_TRY_ENV =
        TAO_default_environment ()
    );
};


class TAO_PortableServer_Export FT_POA_tao_thru_poa_collocated_HeartbeatEnabledPolicy
  : public virtual FT::HeartbeatEnabledPolicy,
    public virtual POA_CORBA::_tao_thru_poa_collocated_Policy
{
public:
  FT_POA_tao_thru_poa_collocated_HeartbeatEnabledPolicy (
      TAO_Stub *stub
    );
  virtual CORBA::Boolean _is_a(
      const CORBA::Char *logical_type_id,
      CORBA_Environment &ACE_TRY_ENV = TAO_default_environment ()
    );

  virtual CORBA::Boolean _non_existent(
      CORBA_Environment &ACE_TRY_ENV = TAO_default_environment ()
    );

  virtual CORBA::Boolean heartbeat_enabled_policy_value (
      CORBA::Environment &ACE_TRY_ENV =
        TAO_default_environment ()
    )
    ACE_THROW_SPEC ((
      CORBA::SystemException
    ));

};



#if defined (ACE_HAS_USING_KEYWORD)

# if 0
// This wouldn't work. We need to find a way around for these
// guys. If somebody wants to use it, we may have to do something
// about it.
// TIE class: Refer to CORBA v2.2, Section 20.34.4
template <class T>
class TAO_PortableServer_Export FT_POA_HeartbeatEnabledPolicy_tie :
  public FT_POA_HeartbeatEnabledPolicy
{
public:
  FT_POA_HeartbeatEnabledPolicy_tie (T &t);
  // the T& ctor
  FT_POA_HeartbeatEnabledPolicy_tie (T &t, PortableServer::POA_ptr poa);
  // ctor taking a POA
  FT_POA_HeartbeatEnabledPolicy_tie (T *tp, CORBA::Boolean release=1);
  // ctor taking pointer and an ownership flag
  FT_POA_HeartbeatEnabledPolicy_tie (T *tp, PortableServer::POA_ptr poa, CORBA::Boolean release=1);
  // ctor with T*, ownership flag and a POA
  ~FT_POA_HeartbeatEnabledPolicy_tie (void);
  // dtor

  // TIE specific functions
  T *_tied_object (void);
  // return the underlying object
  void _tied_object (T &obj);
  // set the underlying object
  void _tied_object (T *obj, CORBA::Boolean release=1);
  // set the underlying object and the ownership flag
  CORBA::Boolean _is_owner (void);
  // do we own it
  void _is_owner (CORBA::Boolean b);
  // set the ownership

  // overridden ServantBase operations
  PortableServer::POA_ptr _default_POA (
      CORBA::Environment &ACE_TRY_ENV =
        TAO_default_environment ()
    );
  CORBA::Boolean heartbeat_enabled_policy_value (
      CORBA::Environment &ACE_TRY_ENV =
        TAO_default_environment ()
    )
    ACE_THROW_SPEC ((
      CORBA::SystemException
    ));

  CORBA::PolicyType policy_type (
      CORBA::Environment &ACE_TRY_ENV =
        TAO_default_environment ()
    )
    ACE_THROW_SPEC ((
      CORBA::SystemException
    ));

  CORBA::Policy_ptr copy (
      CORBA::Environment &ACE_TRY_ENV =
        TAO_default_environment ()
    )
    ACE_THROW_SPEC ((
      CORBA::SystemException
    ));

  void destroy (
      CORBA::Environment &ACE_TRY_ENV =
        TAO_default_environment ()
    )
    ACE_THROW_SPEC ((
      CORBA::SystemException
    ));

private:
  T *ptr_;
  PortableServer::POA_var poa_;
  CORBA::Boolean rel_;

  // copy and assignment are not allowed
  FT_POA_HeartbeatEnabledPolicy_tie (const FT_POA_HeartbeatEnabledPolicy_tie &);
  void operator= (const FT_POA_HeartbeatEnabledPolicy_tie &);
};

#endif /*if 0 */
#endif /* ACE_HAS_USING_KEYWORD */



#if defined (__ACE_INLINE__)
#include "FT_CORBAS.i"
#endif /* defined INLINE */

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
#pragma warning(pop)
#endif /* _MSC_VER */

#endif /*TAO_HAS_FT_CORBA == 1*/

#include "ace/post.h"
#endif /*TAO_FT_CORBAS_H */
