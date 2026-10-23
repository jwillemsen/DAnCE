#ifndef IDREF_BASE_CPP
#define IDREF_BASE_CPP

#include "dance/Logger/Log_Macros.h"
#include "IDREF_Base.h"
#include <iostream>

namespace DAnCE
  {
    namespace Config_Handlers
    {
      template <typename T>
      void IDREF_Base<T>::bind_ref (std::string& id, T value)
      {
        DANCE_TRACE("IDREF_Base<T>::bind_ref");

        int retval = idref_map_.bind (id, value);

        pos_map_.bind (value,id);

        if (retval < 0)
          throw Config_Error (id.c_str (), "Failed to bind an IDRef.  This likely indicates a name clash.");
      }

      template <typename T>
      void IDREF_Base<T>::bind_next_available (std::string &id)
      {
        // Note:  For this function to compile and work properly,
        // T must have the postincrement operator defined.
        DANCE_TRACE ("IDREF_Base<T>::bind_next_available");

        int retval = idref_map_.bind (id, this->next_);

        int pos_retval = pos_map_.bind (this->next_, id);

        ++this->next_;

        if (retval < 0 || pos_retval < 0)
          throw Config_Error (id.c_str (), "Failed to bind an IDRef.  This likely indicates a name clash");

      }

      template <typename T>
      void IDREF_Base<T>::find_ref (std::string const& id, T& val)
      {
        DANCE_TRACE("IDREF_Base<T>::find_ref(C_String, T)");

        int retval = idref_map_.find (id, val);

        if (retval < 0)
          throw Config_Error (id.c_str (), "Unable to look up an IDRef.");
      }

      template <typename T>
      void IDREF_Base<T>::find_ref (T const& value, std::string& id)
      {
        DANCE_TRACE("IDREF_Base<T>::find_ref (T, CString)");

        int retval = pos_map_.find (value, id);

        if (retval < 0)
          throw Config_Error ("No location information for reverse IDREF lookup", id.c_str ());
      }

      template <typename T>
      bool IDREF_Base<T>::unbind_refs ()
      {
        DANCE_TRACE("IDREF_Base<T>::unbind_refs");

        int retval = idref_map_.unbind_all ();

        pos_map_.unbind_all ();

        this->next_ = 0;

        if (retval < 0)
          return false;

        return true;
      }
    }

  }

#endif /* IDREF_BASE_CPP */
