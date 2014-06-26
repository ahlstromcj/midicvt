#ifndef MIDIPP_CSVARRAY_HPP
#define MIDIPP_CSVARRAY_HPP

/**
 * \file          csvarray.hpp
 * \library       libmidipp
 * \author        Chris Ahlstrom
 * \date          2014-04-23
 * \updates       2014-05-21
 * \version       $Revision$
 * \license       $MIDIPP_SUITE_GPL_LICENSE$
 *
 *    Provides a way to convert a simple CSV (comma-separated value) file
 *    into an array of data, and into an INI format.
 *
 *    The input CSV file is an unadorned file.  The caller must know what
 *    the file is for, so that the results can be properly annotated.
 */

#include <string>
#include <vector>

namespace midipp
{

/**
 *    The csvarray class assist in parsing a file that has lines of
 *    comma-separated values.
 *
 *    It creates a vector of vectors out of this file.  The main vector
 *    contains one element for each usable line in the file.  Each element
 *    is itself a vector of strings.  A record is kept of the number of
 *    elements in each vector.  Generally, that number should be the same
 *    for every line in the file, but this is not required.
 */

class csvarray
{

public:

   /**
    *    Provides a handy type for the data structure that the csvarray class
    *    stores in a vector.  Each string in this vector is called a
    *    "field".
    */

   typedef std::vector<std::string> Fields;

   /**
    *    Provides a type that holds a vector of Field elements.
    */

   typedef std::vector<Fields> Rows;

private:

   /**
    *    Provides the value for the separator.  By default, this is a comma,
    *    but other characters, including a tab, can be used as well.  The
    *    separator character can never be included in a Field.
    */

   char m_separator;

   /**
    *    Provides the file-name of the file from which this data
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

   Rows m_csv_lines;

   /**
    *    Indicates the minimum number of fields found in a row.
    */

   int m_minimum_length;

   /**
    *    Indicates the maximum number of fields found in a row.
    */

   int m_maximum_length;

   /**
    *    Indicates if construction succeeded in all respects.
    *
    *    An empty csvarray is not valid.  Trying to read a non-existent or
    *    corrupt CSV file results in a csvarray that is not valid.
    *
    *    We're not big on throwing exceptions as a means of error handling.
    */

   bool m_is_valid;

public:

   csvarray ();                         // an empty, unnamed csvarray
   csvarray
   (
      const std::string & name,
      const std::string & filespec,
      char separator                = ','
   );
   csvarray (const csvarray & source);
   csvarray & operator = (const csvarray & source);

   /**
    * \destructor
    *    Provided as a virtual destructor so that we can derive from this
    *    class.
    */

   virtual ~csvarray ()
   {
      // That is it for now!
   }

   /**
    * @getter  m_is_valid;
    */

   bool is_valid () const
   {
      return m_is_valid;
   }

   /**
    * @getter m_csv_lines
    */

   const Rows & rows () const
   {
      return m_csv_lines;
   }

   /**
    * @getter m_source_file
    */

   const std::string & source_file () const
   {
      return m_source_file;
   }

   /**
    * @getter m_name
    */

   const std::string & name () const
   {
      return m_name;
   }

   /**
    * @setter m_name
    */

   void name (const std::string & n)
   {
      m_name = n;
   }

   /**
    * \accessor m_csv_lines.size()
    *
    * \return
    *    Returns the number of VALUETYPE objects in the container.
    */

   size_t size () const
   {
      return m_csv_lines.size();
   }

   /**
    * \accessor m_csv_lines.empty()
    *
    * \return
    *    Returns true if the container is empty.
    */

   bool empty () const
   {
      return m_csv_lines.empty();
   }

   /**
    * \getter m_minimum_length
    */

   int min_count () const
   {
      return m_minimum_length;
   }

   /**
    * \getter m_maximum_length
    */

   int max_count () const
   {
      return m_maximum_length;
   }

protected:

   bool readfile (const std::string & filespec);

   /**
    *    Checks for a comment character.
    *
    * \return
    *    Returns 'true' if the character is in the set <code> #; </code>
    */

   bool is_comment (char c)
   {
       return c == '#' || c == ';';
   }

   /**
    *    Allows the container to be emptied of Section objects.
    */

   void clear ()
   {
      m_csv_lines.clear();
   }

};                // class csvarray

}                 // namespace midipp

/*
 * Global functions.
 */

extern void show (const std::string & tag, const midipp::csvarray & container);

#endif            // MIDIPP_CSVARRAY_HPP

/*
 * csvarray.hpp
 *
 * vim: ts=3 sw=3 et ft=cpp
 */
