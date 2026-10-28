//======================================
/**
*   @file    DD_Handler.h
*
*   @brief Contains the Domain Descriptior
*   Handler
*
*   @author Nilabja Roy <nilabjar@dre.vanderbilt.edu>
*
*/
//======================================

#ifndef CIAO_CONFIG_HANDLERS_DD_HANDLER_H
#define CIAO_CONFIG_HANDLERS_DD_HANDLER_H
#include /**/ "ace/pre.h"

#include "Config_Handlers_Export.h"
#include <memory>

#if !defined (ACE_LACKS_PRAGMA_ONCE)
# pragma once
#endif /* ACE_LACKS_PRAGMA_ONCE */

namespace Deployment
{
struct Domain;
}

namespace DAnCE
{
namespace Config_Handlers
{
class Domain;

/*
* @class DD_Handler
*
* @brief Handler class for <Domain>
*/

class Config_Handlers_Export DD_Handler
{
public:
class NoDomain {};

/**
* @param dmn The Domain structure
*/
DD_Handler(Domain *dmn);

DD_Handler(Domain &dmn);

/**
*/
~DD_Handler ();

/**
* @brief Returns the const IDL domain
* @return Domain* the IDL domain pointer
*/
 ::Deployment::Domain const *domain_idl () const;

/**
* @brief Returns the IDL domain
* @return Domain* the IDL domain pointer
*/
 ::Deployment::Domain *domain_idl ();

/**
* @brief Returns the const XSC domain
* @return Domain* the XSC domain pointer
*/
 Domain const *domain_xsc () const;

/**
* @brief Returns the XSC domain
* @return Domain* the XSC domain pointer
*/
 Domain *domain_xsc ();

private:
/**
*  @brief builds the domain structure
*/
bool build_domain ();

/// The Deployment Domain structure
std::unique_ptr<::Deployment::Domain> idl_domain_ {};

/// The XSC Domain structure
std::unique_ptr<Domain> domain_ptr_ {};

 Domain &domain_;
/// The return value used
};
}
}

#include /**/ "ace/post.h"
#endif /* DD_HANDLER_H */
