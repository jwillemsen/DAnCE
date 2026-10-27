/**
 * @file Common.h
 * @author Will Otte <wotte@dre.vanderbilt.edu>
 *
 * Some common definitions for all config_handlers.
 */
#ifndef CONFIG_HANDLERS_COMMON_H
#define CONFIG_HANDLERS_COMMON_H

#ifndef DANCE_CONFIG_HANDLERS_CORBA_TO_XSC
#define DANCE_CONFIG_HANDLERS_CORBA_TO_XSC 1
#endif

#include <string>
#include <string_view>
#include <utility>

namespace DAnCE
{
  namespace Config_Handlers
  {
    /**
     * @class Config_Error
     * @brief Exception to communicate a config_handlers error
     */
    class Config_Error
    {
    public:
      Config_Error (std::string name, std::string error) : name_ (std::move (name)), error_ (std::move (error)) {};

      void add_name (std::string_view name)
      {
        name_ = std::string (name) + ":" + name_;
      }

      /// Some identifying information about the element that
      /// caused the error
      std::string name_;

      /// A human readable error message
      std::string error_;

    };
  }
}
#endif /* CONFIG_HANDLERS_COMMON_H */

