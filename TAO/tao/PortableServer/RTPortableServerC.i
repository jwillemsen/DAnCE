// -*- C++ -*-
//
// $Id$

// ****  Code generated by the The ACE ORB (TAO) IDL Compiler ****
// TAO and the TAO IDL Compiler have been developed by:
//       Center for Distributed Object Computing
//       Washington University
//       St. Louis, MO
//       USA
//       http://www.cs.wustl.edu/~schmidt/doc-center.html
// and
//       Distributed Object Computing Laboratory
//       University of California at Irvine
//       Irvine, CA
//       USA
//       http://doc.ece.uci.edu/
//
// Information about TAO is available at:
//     http://www.cs.wustl.edu/~schmidt/TAO.html


#if !defined (_RTPORTABLESERVER_POA___VAR_CI_)
#define _RTPORTABLESERVER_POA___VAR_CI_

// *************************************************************
// Inline operations for class RTPortableServer::POA_var
// *************************************************************

ACE_INLINE
RTPortableServer::POA_var::POA_var (void) // default constructor
  : ptr_ (POA::_nil ())
{}

ACE_INLINE ::RTPortableServer::POA_ptr
RTPortableServer::POA_var::ptr (void) const
{
  return this->ptr_;
}

ACE_INLINE
RTPortableServer::POA_var::POA_var (const ::RTPortableServer::POA_var &p) // copy constructor
  : TAO_Base_var (),
    ptr_ (POA::_duplicate (p.ptr ()))
{}

ACE_INLINE
RTPortableServer::POA_var::~POA_var (void) // destructor
{
  CORBA::release (this->ptr_);
}

ACE_INLINE RTPortableServer::POA_var &
RTPortableServer::POA_var::operator= (POA_ptr p)
{
  CORBA::release (this->ptr_);
  this->ptr_ = p;
  return *this;
}

ACE_INLINE RTPortableServer::POA_var &
RTPortableServer::POA_var::operator= (const ::RTPortableServer::POA_var &p)
{
  if (this != &p)
  {
    CORBA::release (this->ptr_);
    this->ptr_ = ::RTPortableServer::POA::_duplicate (p.ptr ());
  }
  return *this;
}

ACE_INLINE 
RTPortableServer::POA_var::operator const ::RTPortableServer::POA_ptr &() const // cast
{
  return this->ptr_;
}

ACE_INLINE 
RTPortableServer::POA_var::operator ::RTPortableServer::POA_ptr &() // cast 
{
  return this->ptr_;
}

ACE_INLINE ::RTPortableServer::POA_ptr
RTPortableServer::POA_var::operator-> (void) const
{
  return this->ptr_;
}

ACE_INLINE ::RTPortableServer::POA_ptr
RTPortableServer::POA_var::in (void) const
{
  return this->ptr_;
}

ACE_INLINE ::RTPortableServer::POA_ptr &
RTPortableServer::POA_var::inout (void)
{
  return this->ptr_;
}

ACE_INLINE ::RTPortableServer::POA_ptr &
RTPortableServer::POA_var::out (void)
{
  CORBA::release (this->ptr_);
  this->ptr_ = ::RTPortableServer::POA::_nil ();
  return this->ptr_;
}

ACE_INLINE ::RTPortableServer::POA_ptr
RTPortableServer::POA_var::_retn (void)
{
  // yield ownership of managed obj reference
  ::RTPortableServer::POA_ptr val = this->ptr_;
  this->ptr_ = ::RTPortableServer::POA::_nil ();
  return val;
}


#endif /* end #if !defined */


#if !defined (_RTPORTABLESERVER_POA___OUT_CI_)
#define _RTPORTABLESERVER_POA___OUT_CI_

// *************************************************************
// Inline operations for class RTPortableServer::POA_out
// *************************************************************

ACE_INLINE
RTPortableServer::POA_out::POA_out (POA_ptr &p)
  : ptr_ (p)
{
  this->ptr_ = ::RTPortableServer::POA::_nil ();
}

ACE_INLINE
RTPortableServer::POA_out::POA_out (POA_var &p) // constructor from _var
  : ptr_ (p.out ())
{
  CORBA::release (this->ptr_);
  this->ptr_ = ::RTPortableServer::POA::_nil ();
}

ACE_INLINE
RTPortableServer::POA_out::POA_out (const ::RTPortableServer::POA_out &p) // copy constructor
  : ptr_ (ACE_const_cast (POA_out &, p).ptr_)
{}

ACE_INLINE ::RTPortableServer::POA_out &
RTPortableServer::POA_out::operator= (const ::RTPortableServer::POA_out &p)
{
  this->ptr_ = ACE_const_cast (POA_out&, p).ptr_;
  return *this;
}

ACE_INLINE RTPortableServer::POA_out &
RTPortableServer::POA_out::operator= (const ::RTPortableServer::POA_var &p)
{
  this->ptr_ = ::RTPortableServer::POA::_duplicate (p.ptr ());
  return *this;
}

ACE_INLINE RTPortableServer::POA_out &
RTPortableServer::POA_out::operator= (POA_ptr p)
{
  this->ptr_ = p;
  return *this;
}

ACE_INLINE 
RTPortableServer::POA_out::operator ::RTPortableServer::POA_ptr &() // cast
{
  return this->ptr_;
}

ACE_INLINE ::RTPortableServer::POA_ptr &
RTPortableServer::POA_out::ptr (void) // ptr
{
  return this->ptr_;
}

ACE_INLINE ::RTPortableServer::POA_ptr
RTPortableServer::POA_out::operator-> (void)
{
  return this->ptr_;
}


#endif /* end #if !defined */

