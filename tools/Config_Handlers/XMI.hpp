/*
 * This code was generated by the XML Schema Compiler.
 *
 * Changes made to this code will most likely be overwritten
 * when the handlers are recompiled.
 *
 * If you find errors or feel that there are bugfixes to be made,
 * please report this to the XSC project at
 * https://github.com/DOCGroup/XSC
 */
#ifndef XMI_HPP
#define XMI_HPP

#include "XSC_XML_Handlers_Export.h"
// Forward declarations.
namespace XMI
{
  class Extension;
}

#include <memory>
#include <string>
#include <list>
#include "ace/XML_Utils/XMLSchema/Types.hpp"
#include "ace/XML_Utils/XMLSchema/id_map.hpp"
#include "ace/Refcounted_Auto_Ptr.h"
#include "ace/Null_Mutex.h"
#include "ace/ace_wchar.h"

#if !defined(XML_XSC_SMART_PTR)
# if defined(ACE_HAS_CPP11)
#   define XML_XSC_SMART_PTR(X) std::unique_ptr<X>
# else
#   define XML_XSC_SMART_PTR(X) std::auto_ptr<X>
# endif
#endif

namespace XMI
{
  class XSC_XML_Handlers_Export Extension : public ::XSCRT::Type
  {
    typedef ::XSCRT::Type Base;

    public:
    typedef ACE_Refcounted_Auto_Ptr < ::XMI::Extension, ACE_Null_Mutex> _ptr;

    // id
    public:
    bool id_p () const;
    ::XMLSchema::ID<ACE_TCHAR> const& id () const;
    ::XMLSchema::ID<ACE_TCHAR>& id ();
    void id (::XMLSchema::ID<ACE_TCHAR> const& );

    protected:
    typedef XML_XSC_SMART_PTR( ::XMLSchema::ID<ACE_TCHAR> ) id_auto_ptr_type;
    id_auto_ptr_type id_;

    // label
    public:
    bool label_p () const;
    ::XMLSchema::string<ACE_TCHAR> const& label () const;
    ::XMLSchema::string<ACE_TCHAR>& label ();
    void label (::XMLSchema::string<ACE_TCHAR> const& );

    protected:
    typedef XML_XSC_SMART_PTR( ::XMLSchema::string<ACE_TCHAR> ) label_auto_ptr_type;
    label_auto_ptr_type label_;

    // uuid
    public:
    bool uuid_p () const;
    ::XMLSchema::string<ACE_TCHAR> const& uuid () const;
    ::XMLSchema::string<ACE_TCHAR>& uuid ();
    void uuid (::XMLSchema::string<ACE_TCHAR> const& );

    protected:
    typedef XML_XSC_SMART_PTR( ::XMLSchema::string<ACE_TCHAR> ) uuid_auto_ptr_type;
    uuid_auto_ptr_type uuid_;

    // href
    public:
    bool href_p () const;
    ::XMLSchema::string<ACE_TCHAR> const& href () const;
    ::XMLSchema::string<ACE_TCHAR>& href ();
    void href (::XMLSchema::string<ACE_TCHAR> const& );

    protected:
    typedef XML_XSC_SMART_PTR( ::XMLSchema::string<ACE_TCHAR> ) href_auto_ptr_type;
    href_auto_ptr_type href_;

    // idref
    public:
    bool idref_p () const;
    ::XMLSchema::IDREF<ACE_TCHAR> const& idref () const;
    ::XMLSchema::IDREF<ACE_TCHAR>& idref ();
    void idref (::XMLSchema::IDREF<ACE_TCHAR> const& );
    ::XSCRT::Type* get_idref_ptr ();

    void set_idref_ptr (const std::basic_string<ACE_TCHAR>& idref);

    protected:
    typedef XML_XSC_SMART_PTR( ::XMLSchema::IDREF<ACE_TCHAR> ) idref_auto_ptr_type;
    idref_auto_ptr_type idref_;

    // version
    public:
    bool version_p () const;
    ::XMLSchema::string<ACE_TCHAR> const& version () const;
    ::XMLSchema::string<ACE_TCHAR>& version ();
    void version (::XMLSchema::string<ACE_TCHAR> const& );

    protected:
    typedef XML_XSC_SMART_PTR( ::XMLSchema::string<ACE_TCHAR> ) version_auto_ptr_type;
    version_auto_ptr_type version_;

    // extender
    public:
    bool extender_p () const;
    ::XMLSchema::string<ACE_TCHAR> const& extender () const;
    ::XMLSchema::string<ACE_TCHAR>& extender ();
    void extender (::XMLSchema::string<ACE_TCHAR> const& );

    protected:
    typedef XML_XSC_SMART_PTR( ::XMLSchema::string<ACE_TCHAR> ) extender_auto_ptr_type;
    extender_auto_ptr_type extender_;

    // extenderID
    public:
    bool extenderID_p () const;
    ::XMLSchema::string<ACE_TCHAR> const& extenderID () const;
    ::XMLSchema::string<ACE_TCHAR>& extenderID ();
    void extenderID (::XMLSchema::string<ACE_TCHAR> const& );

    protected:
    typedef XML_XSC_SMART_PTR( ::XMLSchema::string<ACE_TCHAR> ) extenderID_auto_ptr_type;
    extenderID_auto_ptr_type extenderID_;

    public:
    Extension ();

    explicit Extension (::XSCRT::XML::Element<ACE_TCHAR> const&);
    Extension (Extension const& s);
    Extension& operator= (Extension const& s);
  };
}

namespace XMI
{
  namespace reader
  {
    XSC_XML_Handlers_Export
    ::XMI::Extension
    extension (xercesc::DOMDocument const*);
  }
}

#endif // XMI_HPP
