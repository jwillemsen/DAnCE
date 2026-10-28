/**
 * @file IDREF_Base.h
 * @author Will Otte <wotte@dre.vanderbilt.edu>
 *
 * Defines IDREF_Base, which maps identifiers to indexes and back.
 */

#ifndef CIAO_CONFIG_HANDLERS_IDREF_BASE_H
#define CIAO_CONFIG_HANDLERS_IDREF_BASE_H

#include /**/ "ace/pre.h"

#include "Common.h"
#include "Config_Handlers_Export.h"
#include "ace/Hash_Map_Manager.h"
#include "ace/Null_Mutex.h"
#include <string>

#if !defined (ACE_LACKS_PRAGMA_ONCE)
#  pragma once
#endif /* ACE_LACKS_PRAGMA_ONCE */

namespace DAnCE
{
  namespace Config_Handlers
  {
    /**
     * @class IDREF_Base
     * @brief Base class for any handler that must process IDREFs.
     */
    template <typename T>
    class IDREF_Base
    {
    public:
      using IDREF_MAP = ACE_Hash_Map_Manager<std::string, T, ACE_Null_Mutex>;
      using POS_MAP = ACE_Hash_Map_Manager<T, std::string, ACE_Null_Mutex>;

      IDREF_Base () = default;
      explicit IDREF_Base (T start_point);

      /// Bind an IDREF to an index.
      void bind_ref (std::string const& id, T const& value);

      void bind_next_available (std::string const& id);

      /// Get the index associated with an IDREF.
      void find_ref (std::string const& id, T& value);

      /// Get the IDREF associated with an index.
      void find_ref (T const& value, std::string& id);

      /// Unbind all IDREFs.
      bool unbind_refs ();

    protected:
      /// Indexes stored by IDREF.
      IDREF_MAP idref_map_ {};

      /// IDREFs stored by index.
      POS_MAP pos_map_ {};

      T next_ {};
    };
  }
}

#include "IDREF_Base.cpp"

#include /**/ "ace/post.h"
#endif /* CIAO_CONFIG_HANDLERS_IDREF_BASE_H */
