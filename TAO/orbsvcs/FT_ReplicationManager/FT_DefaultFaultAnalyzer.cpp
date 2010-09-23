/* -*- C++ -*- */
//=============================================================================
/**
 *  @file    FT_DefaultFaultAnalyzer.cpp
 *
 *  $Id$
 *
 *  This file is part of TAO's implementation of Fault Tolerant CORBA.
 *
 *  @author Steve Totten <totten_s@ociweb.com>
 */
//=============================================================================

#include "FT_DefaultFaultAnalyzer.h"
#include "orbsvcs/CosNotifyCommC.h"
#include "orbsvcs/FT_NotifierC.h"
#include "orbsvcs/FT_FaultDetectorFactoryC.h"
#include "orbsvcs/FT_ReplicationManagerC.h"
#include "orbsvcs/FT_ReplicationManager/FT_FaultEventDescriptor.h"
#include "tao/debug.h"
#include "ace/OS_NS_string.h"

TAO_BEGIN_VERSIONED_NAMESPACE_DECL

/// Default constructor.
TAO::FT_DefaultFaultAnalyzer::FT_DefaultFaultAnalyzer ()
{
}

/// Destructor.
TAO::FT_DefaultFaultAnalyzer::~FT_DefaultFaultAnalyzer ()
{
}

// Validate the event to make sure it is one we can handle.
// If it is not an event we can handle, this function logs the error
// and returns -1.
int TAO::FT_DefaultFaultAnalyzer::validate_event_type (
  const CosNotification::StructuredEvent & event)
{
  int result = 0;

  // CORBA 3.0.2, section 23.4.5.1 states:
  //
  //   The fault management specification defines one event type:
  //   ObjectCrashFault. As the name suggests, this event is
  //   generated by a Fault Detector when it detects that an object
  //   has crashed.

  // So, the event header's event_type.domain_name must be "FT_CORBA"
  // and the event header's event_type.type_name must be "ObjectCrashFault".
  // @@ why make string dups just to do a strcmp?
  CORBA::String_var domain_name = CORBA::string_dup (
    event.header.fixed_header.event_type.domain_name);
  CORBA::String_var type_name = CORBA::string_dup (
      event.header.fixed_header.event_type.type_name);
  CORBA::String_var event_name = CORBA::string_dup (
      event.header.fixed_header.event_name);

  if (result == 0)
  {
    if (ACE_OS::strcmp (domain_name.in(), FT::FT_EVENT_TYPE_DOMAIN) != 0 ||
        ACE_OS::strcmp (type_name.in(), FT::FT_EVENT_TYPE_NAME) != 0)
    {
      if (TAO_debug_level > 6)
      {
        ACE_ERROR ((LM_ERROR,
          ACE_TEXT ("TAO::FT_DefaultFaultAnalyzer::validate_event_type: ")
          ACE_TEXT ("Received invalid event type.\n")
          ACE_TEXT ("EventType domain: <%s>\n")
          ACE_TEXT ("EventType type: <%s>\n")
          ACE_TEXT ("EventName: <%s>\n"),
          domain_name.in(),
          type_name.in(),
          event_name.in()
        ));
      }
      result = -1;
    }
  }

  // CORBA 3.0.2, section 23.4.5.1 also states:
  //
  //   The filterable_data part of the event body contains the
  //   identity of the crashed object as four name-value pairs: the
  //   fault tolerance domain identifier, the member's location
  //   identifier, the repository identifier and the object group
  //   identifier. The Fault Notifier filters events based on the
  //   domain_name, the type_name, and the four identifiers.  All
  //   other fields of the structured event may be set to null.
  //
  //   The Fault Detector always sets the following fault event
  //   fields: domain_name, type_name, FTDomainId, and Location.
  //
  // So, at least "FTDomainId" and "Location" must be present:
  if (result == 0)
  {
    if (event.filterable_data.length () >= 2)
    {
      // Check for FTDomainId.
      if (ACE_OS::strcmp (
            event.filterable_data[0].name.in(), FT::FT_DOMAIN_ID) != 0)
      {
        if (TAO_debug_level > 6)
        {
          ACE_ERROR ((LM_ERROR,
            ACE_TEXT ("TAO::FT_DefaultFaultAnalyzer::validate_event_type: ")
            ACE_TEXT ("Received invalid structured event.\n")
            ACE_TEXT ("filterable_data[0] must be \"FTDomainId\", not \"%s\"\n"),
            event.filterable_data[0].name.in()
          ));
        }
        result = -1;
      }
      else if (ACE_OS::strcmp (
            event.filterable_data[1].name.in(), FT::FT_LOCATION) != 0)
      {
        if (TAO_debug_level > 6)
        {
          ACE_ERROR ((LM_ERROR,
            ACE_TEXT ("TAO::FT_DefaultFaultAnalyzer::validate_event_type: ")
            ACE_TEXT ("Received invalid structured event.\n")
            ACE_TEXT ("filterable_data[1] must be \"Location\", not \"%s\"\n"),
            event.filterable_data[1].name.in()
          ));
        }
        result = -1;
      }
    }
    else
    {
      if (TAO_debug_level > 6)
      {
        ACE_ERROR ((LM_ERROR,
          ACE_TEXT ("TAO::FT_DefaultFaultAnalyzer::validate_event_type: ")
          ACE_TEXT ("Received invalid structured event.\n")
          ACE_TEXT ("There must be at least two name/value pairs in ")
          ACE_TEXT ("the filterable_data field, for \"FTDomainId\" and \"Location\".\n")
        ));
      }
      result = -1;
    }
  }

  return result;
}

/// Analyze a fault event.
int TAO::FT_DefaultFaultAnalyzer::analyze_fault_event (
  const CosNotification::StructuredEvent & event)
{
  ACE_UNUSED_ARG (event);
  if (TAO_debug_level > 6)
  {
    ACE_DEBUG ((LM_DEBUG,
      ACE_TEXT (
        "In TAO::FT_DefaultFaultAnalyzer::analyze_fault_event.\n")
    ));
  }

  // no-op
  return 0;
}

TAO_END_VERSIONED_NAMESPACE_DECL
