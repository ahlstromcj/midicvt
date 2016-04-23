#ifndef MIDIPP_STRINGMAP_HPP
#define MIDIPP_STRINGMAP_HPP

/**
 * \file          stringmap.hpp
 * \library       libmidipp
 * \author        Chris Ahlstrom
 * \date          2014-04-22
 * \updates       2014-05-21
 * \version       $Revision$
 * \license       $MIDIPP_SUITE_GPL_LICENSE$
 *
 *    This module defines an std::map template class using std::string as a
 *    key value.
 */

#include <algorithm>					      /* std::for_each                       */
#include <map>							      /* std::map                            */
#include <string>						      /* std::string                         */
#include <stdio.h>						   /* fprintf() and stdout !              */

/*
 * Global functions.
 */

extern void show (const std::string & tag, const std::string & s);

namespace midipp
{

/*
 * Global functions in the midipp namespace
 *
 *    Each is documented in the cpp file.
 */

extern bool iequal (const std::string & s1, const std::string & s2);

/**
 *    Provides an std::map wrapper geared towards using std::string as
 *    a key.
 *
 *    The problem with using integers is that they are not very easy to
 *    decipher while debugging, nor are they easy to understand in code
 *    (without using ugly define-macros).
 *
 *    Also, strings are a frequent lookup mechanism.  A wrapper is needed
 *    so that we can disable the automatic insertion of objects that occurs
 *    when operator [] is applied to a key that does not yet exist in the
 *    std::map container.
 *
 *    Efficiency?  Well, we should test that.  :-)  We're using copy
 *    semantic for the value part.  If you use pointers for the value, the
 *    management of them is up to you!
 *
 * \template
 *    This template class supports looking up a container of VALUETYPE
 *    objects by std::string.
 *
 *    The VALUETYPE must support and publicize the following operations:
 *
 *       -  Default constructor.  In addition, the default constructor
 *          should create an object that indicates an error state.
 *          This feature is necessary so that we can avoid throwing
 *          exceptions when doing map lookups.
 *       -  Copy constructor.
 *       -  Principal assignment operator.
 *       -  show().  The object must have its own overload of the global
 *          show() function.  See the stringmap.cpp module.
 *
 *    The type of container is given by the Container typedef.  The
 *    stringmap template is essentially a wrapper for this class.
 *
 * \todo
 *    -  Consider implementing lookup by integer index; right now, iterators
 *       suffice;
 *    -  It would be nice to be able to superimpose a numeric order on the
 *       container, somehow.
 */

template <class VALUETYPE>
class stringmap
{

public:

   /**
    *    Defines the type of container used by stringmap.
    *
    *    We want to be able to use array notation, yet be able to look up
    *    items (e.g. database fields) by name.
    */

   typedef std::map<std::string, VALUETYPE> Container;

   /**
    *    Provides a constant-iterator type for notational convenience.
    */

   typedef typename Container::const_iterator const_iterator;

public:

   /**
    *    Provides an iterator type for notational convenience.
    */

   typedef typename Container::iterator iterator;

   /**
    *    Provides a pair type for notational convenience.
    */

   typedef std::pair<std::string, VALUETYPE> pair;

private:

   /**
    *    Provides the name of the stringmap.  This name can be used to add
    *    the stringmap to a stringmap full of stringmaps.
    */

   std::string m_Name;

   /**
    *    Provides the actual container for which the stringmap template
    *    class is a wrapper.
    */

   Container m_Fields;

public:

   /**
    * \defaultctor
    *    Creates an empty and unnamed Container.
    */

   stringmap () : m_Name (), m_Fields ()
   {
      // done
   }

   /**
    * \defaultctor
    *    Creates an empty, but named Container.
    */

   stringmap (const std::string & name) : m_Name (name), m_Fields ()
   {
      // done
   }

   /**
    * \copyctor
    *    Copies the Container.
    *
    * \param source
    *    The stringmap to be copied.
    */

   stringmap (const stringmap & source)
    :
      m_Name   (source.m_Name),
      m_Fields (source.m_Fields)
   {
      // done
   }

   /**
    * \paoperator
    *    Copies one Container into the current object's Container.
    *
    * \param source
    *    The stringmap to be copied into the current stringmap.
    *
    * \return
    *    A reference to the destination object is returned, in order to
    *    support multiple assignments in one statement.
    */

   stringmap & operator = (const stringmap & source)
   {
      if (this != &source)
      {
         m_Name   = source.m_Name;
         m_Fields = source.m_Fields;
      }
      return *this;
   }

   /**
    * \destructor
    *    Provided as a virtual destructor so that we can derive from this
    *    class.
    */

   virtual ~stringmap ()
   {
      // That is it for now!
   }

   /**
    * \getter m_Name
    */

   const std::string & name () const
   {
      return m_Name;
   }

   /**
    *    Allows the insertion of a VALUETYPE object into the container.
    *
    * \param key
    *    The string that is to serve as the lookup value for the inserted
    *    object.
    *
    * \param value
    *    The value object to be added to the container.
    *
    * \return
    *    The size of the container after insertion is returned.  If
    *    important, the caller should check that the size is one larger.
    */

   int insert (const std::string & key, const VALUETYPE & value)
   {
      m_Fields.insert(std::make_pair(key, value));
      return int(m_Fields.size());
   }

   /**
    *    Manufacturers a new key based on the current size of the container,
    *    and inserts the given value with this key.
    *
    *    This function is meant to parallel the integer-key version of the
    *    insert() function.  It could also be called \e append().
    *
    * \param value
    *    The value object to be added to the container.
    *
    * \return
    *    The size of the container after insertion is returned.
    */

   int insert (const VALUETYPE & value)
   {
      int key = size() + 1;
      return insert(key, value);
   }

   /**
    *    Provides a way to look up a string key and return a value.
    *
    *    We can't just return "m_Fields[key]" because that causes an
    *    insertion if the key doesn't exist in the map.  That is not the
    *    semantics we want.
    *
    * \note
    *    This statement can change the container, so is not const.
    *
    *       return m_Fields[key];
    *
    * \param key
    *    Provides the string key to be looked up.
    *
    * \return
    *    Returns the VALUETYPE found.  If it was not found, then a
    *    default-constructor VALUETYPE is returned.  It is the author's
    *    responsibility to provide an error-checking facility in VALUETYPE.
    */

   VALUETYPE value (const std::string & key) const
   {
      const_iterator ci = m_Fields.find(key);
      if (ci == m_Fields.end())
         return VALUETYPE();
      else
         return ci->second;
   }

   /**
    *    Allows the container to be emptied of VALUETYPE objects.
    */

   void clear ()
   {
      m_Fields.clear();
   }

   /**
    * \accessor m_Fields.begin()
    *    This function makes this class look more like an STL container,
    *    good for using "for each" constructs.
    *
    * \warning
    *    Only a subset of std::map members are reimplemented in the
    *    stringmap class.
    *
    * \return
    *    Returns an iterator for the first element of the container, if any.
    *    Otherwise end() is returned.
    */

   iterator begin ()
   {
      return iterator(m_Fields.begin());
   }

   /**
    * \accessor m_Fields.begin() const
    *    Makes this class look more like an STL container.
    *
    * \return
    *    Returns a const iterator for the first element of the container, if
    *    any.  Otherwise end() is returned.
    */

   const_iterator begin () const
   {
      return const_iterator(m_Fields.begin());
   }

   /**
    * \accessor m_Fields.end()
    *    Makes this class look more like an STL container.
    *
    * \return
    *    Returns an iterator indicating the end of the container.
    */

   iterator end ()
   {
      return iterator(m_Fields.end());
   }

   /**
    * \accessor m_Fields.end() const
    *    Makes this class look more like an STL container.
    *
    * \return
    *    Returns a const iterator indicating the end of the container.
    */

   const_iterator end () const
   {
      return const_iterator(m_Fields.end());
   }

   /**
    * \accessor m_Fields.size()
    *
    * \return
    *    Returns the number of VALUETYPE objects in the container.
    */

   size_t size () const
   {
      return m_Fields.size();
   }

   /**
    * \accessor m_Fields.empty()
    *
    * \return
    *    Returns true if the container is empty.
    */

   bool empty () const
   {
      return m_Fields.empty();
   }

   /**
    * \accessor m_Fields.find()
    *    This function makes this class look more like an STL container.
    *
    * \return
    *    Returns an iterator for the found element of the container, if any.
    *    Otherwise end() is returned.
    */

   iterator find (const std::string & key)
   {
      return iterator(m_Fields.find(key));
   }

   /**
    * \accessor m_Fields.find() const
    *    Makes this class look more like an STL container.
    *
    * \return
    *    Returns a const iterator for the found element of the container, if
    *    any.  Otherwise end() is returned.
    */

   const_iterator find (const std::string & key) const
   {
      return const_iterator(m_Fields.find(key));
   }

   /*
    * Other functions to consider implementing:
    *
    *    -  erase()
    */

protected:

   /**
    * \setter m_Name
    */

   void name (const std::string & n)
   {
      m_Name = n;
   }

};                // class stringmap

/**
 *
 *    Writes out the contents of an string/VALUETYPE pair.
 *
 *    This function exists in order to be used in an std::for_each() call.
 *
 * \warning
 *    The VALUETYPE object must have its own overload of the global show()
 *    function.
 *
 * \param p
 *    The pair value to be shown.
 *
 */

template <class VALUETYPE>
void
show_pair (const typename stringmap<VALUETYPE>::pair & p)
{
   show("key", p.first);
   show("value", p.second);
}

/**
 *
 *    Writes out the contents of the property container.
 *
 *    This implementation is a for_each style of looping through the
 *    container.
 *
 * \usage
 *
\verbatim
         resultrow r;
         (void) r.insert("name", "value");
         show(std::string("Row"), r);
\endverbatim
 *
 * \param tag
 *    Identifies the object in the human-readable output.
 *
 * \param container
 *    The stringmap through which iteration is done for showing.
 *
 */

template <class VALUETYPE>
void
show
(
   const std::string & tag,
   const stringmap<VALUETYPE> & container
)
{
   fprintf
   (
      stdout,
      "- xpc::stringmap '%s':\n"
      "-    Name:                    '%s'\n"
      "-    Size:                     %d\n"
      ,
      tag.c_str(),
      container.name().c_str(),
      int(container.size())
   );
   std::for_each(container.begin(), container.end(), show_pair<VALUETYPE>);
}

}                 // namespace midipp

#endif            // MIDIPP_STRINGMAP_HPP

/******************************************************************************
 * stringmap.hpp
 *-----------------------------------------------------------------------------
 * Local Variables:
 * End:
 *-----------------------------------------------------------------------------
 * vim: ts=3 sw=3 et ft=cpp
 *----------------------------------------------------------------------------*/
