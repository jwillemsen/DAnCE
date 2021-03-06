#include "Deployment_Scheduler.h"
#include "dance/Logger/Log_Macros.h"

#include "ace/Auto_Ptr.h"

#if !defined (__ACE_INLINE__)
#include "dance/LocalityManager/Scheduler/Deployment_Scheduler.inl"
#endif

namespace DAnCE
{
  Deployment_Scheduler::Deployment_Scheduler (void)
    : multithread_ (false)
    {
    }

  Deployment_Scheduler::~Deployment_Scheduler (void)
    {
    }

  int
  Deployment_Scheduler::schedule_event (Deployment_Event *event)
  {
    int const retval = this->event_queue_.enqueue (event);

    if (retval == -1)
      {
        DANCE_ERROR (DANCE_LOG_ERROR,
                     (LM_ERROR, DLINFO
                      ACE_TEXT ("Deployment_Scheduler::schedule_event - ")
                      ACE_TEXT ("Error: Unable to schedule event for execution\n")));
      }

    return retval;
  }

  void
  Deployment_Scheduler::terminate_scheduler (void)
  {
    this->event_queue_.queue ()->close ();
  }

  int
  Deployment_Scheduler::svc (void)
  {
    for ( ; ; )
      {
        auto_ptr < ACE_Method_Request > de (this->event_queue_.dequeue ());

        if (de.get ())
          {
            DANCE_DEBUG (DANCE_LOG_DETAILED_TRACE,
                         (LM_TRACE, DLINFO
                          ACE_TEXT ("Deployment_Scheduler::svc - ")
                          ACE_TEXT ("Invoking a deployment event\n")));
            de->call ();
          }
        else
          {
            break;
          }
      }

    return 0;
  }

  void Deployment_Scheduler::perform_work (void)
    {
      auto_ptr < ACE_Method_Request > de (this->event_queue_.dequeue ());

      if (de.get ())
        {
          DANCE_DEBUG (DANCE_LOG_DETAILED_TRACE,
                       (LM_TRACE, DLINFO
                        ACE_TEXT ("Deployment_Scheduler::perform_work - ")
                        ACE_TEXT ("Invoking a deployment event\n")));
          de->call ();
        }
    }
}
