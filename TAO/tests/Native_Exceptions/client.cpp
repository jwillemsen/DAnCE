// $Id$

#include "ace/Get_Opt.h"
#include "testC.h"

ACE_RCSID(Native_Exceptions, client, "$Id$")

const char *ior = "file://test.ior";
int niterations = 100;

int
parse_args (int argc, char *argv[])
{
  ACE_Get_Opt get_opts (argc, argv, "k:i:");
  int c;

  while ((c = get_opts ()) != -1)
    switch (c)
      {
      case 'k':
        ior = get_opts.optarg;
        break;

      case 'i':
        niterations = ACE_OS::atoi (get_opts.optarg);
        break;

      case '?':
      default:
        ACE_ERROR_RETURN ((LM_ERROR,
                           "usage:  %s "
                           "-k <ior> "
                           "-i <niterations> "
                           "\n",
                           argv [0]),
                          -1);
      }
  // Indicates sucessful parsing of the command line
  return 0;
}

int
main (int argc, char *argv[])
{
  try
    {
      CORBA::ORB_var orb =
        CORBA::ORB_init (argc, argv, "");

      if (parse_args (argc, argv) != 0)
        return 1;

      CORBA::Object_var object =
        orb->string_to_object (ior);

      Simple_Server_var server =
        Simple_Server::_narrow (object.in ());

      if (CORBA::is_nil (server.in ()))
        {
          ACE_ERROR_RETURN ((LM_ERROR,
                             "Object reference <%s> is nil\n",
                             ior),
                            1);
        }

      for (int i = 0; i != niterations; ++i)
        {
          CORBA::Long number = i;

          if (server->test_method (i) != i)
            {
              ACE_ERROR ((LM_ERROR,
                          "Unexpected result from test_method\n"));
            }

          try
            {
              server->test_raise (i);
              ACE_ERROR ((LM_ERROR,
                          "The test_raise call didn't raise\n"));
            }
          catch (Simple_Server::Failure &fail)
            {
              // Do nothing, this is the normal behavior...
            }
        }

      server->shutdown ();
    }
  catch (CORBA::SystemException &ex)
    {
      ACE_PRINT_EXCEPTION (ex, "Catched exception:");
      return 1;
    }

  return 0;
}
