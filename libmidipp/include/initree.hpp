#ifndef MIDIPP_INITREE_HPP
#define MIDIPP_INITREE_HPP

/**
 * \file          initree.hpp
 *
 *    Provides a "tree" that represents an INI file with named and unnamed
 *    sections, and name/value pairs.
 *
 * \library       libmidipp
 * \author        Chris Ahlstrom
 * \date          2014-04-22
 * \updates       2014-05-21
 * \version       $Revision$
 * \license       $MIDIPP_SUITE_GPL_LICENSE$
 *
 *    Provides a way to create an options tree using a DOS/Windows INI-style
 *    configuration file.
 */

#include <stringmap.hpp>               /* midipp::stringmap<> template class  */

namespace midipp
{

/**
 *    This class provides a way to read and represent an INI file as a kind
 *    of tree structure that can be navigate to look up information.
 *
 *    An initree consists of Sections.  A Section is a map of string values,
 *    keyed by the name of each value.  A section can be named or un-named.
 *    There can be only one un-named section in an initree.
 */

class initree
{

public:

   /**
    *    Provides a handy name for the data structure that the initree class
    *    stores in a map.  Makes the code quite a bit easier to grok.
    *
    *    Note that Sections will generally have a name.  However, an unnamed
    *    section is useful for representing INI files that have no section
    *    information.
    */

   typedef midipp::stringmap<std::string> Section;

   /**
    *    Provides a type that holds a map of strings, keyed by strings.
    *
    *    The string key is a section name.  A section name comes from the
    *    "[Name]" token in an INI file.
    *
    *    The stringmap value is itself a map of strings, keyed by strings,
    *    where the key is the name of an option, and the value is the
    *    option's value.  This pair comes from an entry in the INI file of
    *    the form "Name = Value".
    */

   typedef std::map <std::string, Section> Container;

   /**
    *    Provides a constant-iterator type for notational convenience.
    *
    *    typedef typename Container::const_iterator const_iterator;
    */

   typedef Container::const_iterator const_iterator;

public:

   /**
    *    Provides an iterator type for notational convenience.
    *
    *    typedef typename Container::iterator iterator;
    */

   typedef Container::iterator iterator;

   /**
    *    Provides a pair type for notational convenience.
    */

   typedef std::pair<std::string, Section> pair;

private:

   /**
    *    Provides the file-name of the file from which  this tree of data
    *    was instantiated.
    */

   std::string m_source_file;

   /**
    *    Provides the name of this object, for future reference.
    */

   std::string m_name;

   /**
    *    Provides a section container.  All of the sections together specify
    *    all of the existing sections in an INI file.
    */

   Container m_sections;

   /**
    *    Indicates if any named sections were added to the tree.
    */

   bool m_has_named_section;

   /**
    *    Provides an empty section to use as a return value.  This dummy
    *    value helps us avoid the need for pointers.
    */

   static Section sm_dummy_section;

public:

   initree ();                         // an empty, unnamed initree
   initree
   (
      const std::string & name,
      const std::string & filespec
   );
   initree (const initree & source);
   initree & operator = (const initree & source);

   /**
    * \destructor
    *    Provided as a virtual destructor so that we can derive from this
    *    class.
    */

   virtual ~initree ()
   {
      // That is it for now!
   }

   /**
    * \getter m_name
    */

   const std::string & name () const
   {
      return m_name;
   }

   /**
    * \setter m_name
    */

   void name (const std::string & n)
   {
      m_name = n;
   }

   /**
    *    Allows the insertion of an stringmap object into the container.
    *
    * \param section
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

   int insert (const std::string & sectionname, const Section & section)
   {
      m_sections.insert(std::make_pair(sectionname, section));
      return int(m_sections.size());
   }

   /**
    *    Provides a way to look up a section name and return a Section value,
    *    as a reference.  This is the const version, meant for outsiders to
    *    use.
    *
    * \param sectionname
    *    Provides the section name to be looked up.
    *
    * \return
    *    Returns a constant reference to the Section found.
    */

   const Section & section (const std::string & sectionname) const
   {
      return section(sectionname);
   }

   /**
    * \accessor m_sections.begin() const
    *    Makes this class look more like an STL container.
    *
    * \return
    *    Returns a const iterator for the first element of the container, if
    *    any.  Otherwise end() is returned.
    */

   const_iterator begin () const
   {
      return const_iterator(m_sections.begin());
   }

   /**
    * \accessor m_sections.end() const
    *    Makes this class look more like an STL container.
    *
    * \return
    *    Returns a const iterator indicating the end of the container.
    */

   const_iterator end () const
   {
      return const_iterator(m_sections.end());
   }

   /**
    * \accessor m_sections.begin()
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
      return iterator(m_sections.begin());
   }

   /**
    * \accessor m_sections.end()
    *    Makes this class look more like an STL container.
    *
    * \return
    *    Returns an iterator indicating the end of the container.
    */

   iterator end ()
   {
      return iterator(m_sections.end());
   }

   /**
    * \accessor m_sections.size()
    *
    * \return
    *    Returns the number of VALUETYPE objects in the container.
    */

   size_t size () const
   {
      return m_sections.size();
   }

   /**
    * \accessor m_sections.empty()
    *
    * \return
    *    Returns true if the container is empty.
    */

   bool empty () const
   {
      return m_sections.empty();
   }

   /**
    * \accessor m_sections.find() const
    *    Makes this class look more like an STL container.
    *
    * \return
    *    Returns a const iterator for the found element of the container, if
    *    any.  Otherwise end() is returned.
    */

   const_iterator find (const std::string & sectionname) const
   {
      return const_iterator(m_sections.find(sectionname));
   }

   /*
    * Other functions to consider implementing:
    *
    *    -  erase()
    */

protected:

   bool readfile (const std::string & filespec);

   /**
    *    Provides a way to look up a section name and return a Section value,
    *    as a reference.
    *
    * \param sectionname
    *    Provides the section name to be looked up.
    *
    * \return
    *    Returns a reference to the Section found.  If it was not found,
    *    then a default-constructor (empty) Section is returned.  It is the
    *    author's responsibility to check if the Section is useful or not.
    */

   Section & section (const std::string & sectionname)
   {
      iterator ci = m_sections.find(sectionname);
      if (ci == m_sections.end())
         return sm_dummy_section;
      else
         return ci->second;
   }

   /**
    *    Allows the container to be emptied of Section objects.
    */

   void clear ()
   {
      m_sections.clear();
   }

   /**
    * \accessor m_sections.find()
    *    This function makes this class look more like an STL container.
    *
    * \return
    *    Returns an iterator for the found element of the container, if any.
    *    Otherwise end() is returned.
    */

   iterator find (const std::string & sectionname)
   {
      return iterator(m_sections.find(sectionname));
   }

   /**
    *    Checks for a comment character.
    *
    *    Comment characters are:
    *
    *       -  Hash mark "#". The comment character for shell scripts.
    *       -  Semicolon ";". The comment character for assembly language.
    *       -  Exclamation point "!" The comment character for Fluxbox
    *          configuration files..
    *       -  Single quote. The comment character for Visual BASIC.
    *       -  Double quote. The comment character for vim scripts.
    *
    * \param c
    *    The character to be checked.
    *
    * \return
    *    Returns 'true' if the character is in the comment-character set.
    */

   bool is_comment (char c)
   {
       return c == '#' || c == ';' || c == '!' || c == '\'' || c == '"';
   }

   std::string process_section_name
   (
      const std::string & s,
      std::string::size_type p
   );
   bool process_option
   (
      const std::string & s,
      std::string::size_type p,
      const std::string & sectionname
   );
   bool make_section (const std::string & sectionname);

};                // class initree

}                 // namespace midipp

/*
 * Global functions.
 */

extern void show (const std::string & tag, const midipp::initree & container);

#endif            // MIDIPP_INITREE_HPP

/*
 * initree.hpp
 *
 * vim: ts=3 sw=3 et ft=cpp
 */
