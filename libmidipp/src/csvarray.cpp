/**
 * \file          csvarray.cpp
 *
 *    Primitive, but useful, CSV-file input parser.
 *
 * \library       libmidipp
 * \author        Chris Ahlstrom
 * \date          2014-04-23
 * \updates       2014-05-21
 * \version       $Revision$
 * \license       $XPC_SUITE_GPL_LICENSE$
 *
 *    Provides a way to convert a simple CSV (comma-separated value) file
 *    into an array of data.
 *
 * \warning
 *    -# This module assumes only ASCII text.  This is a big limitation for
 *       internationalization.
 */

#include <fstream>

#include <csvarray.hpp>                /* the functions in this module        */

/**
 *    Provides a consistent definition of the whitespace characters.  This
 *    define does not bother to try to trap all the characters that could be
 *    considered whitespace or outside the range of legal section and option
 *    names.
 */

#define TOKEN_SPACES    " \t\0"

namespace midipp
{

/**
 *    Creates an unnamed and empty csvarray.
 *
 *    The name can be added later with a call to name(), and sections can be
 *    added with the insert() function.
 */

csvarray::csvarray ()
 :
   m_separator          (','),
   m_source_file        (),
   m_name               (),
   m_csv_lines          (),
   m_minimum_length     (999999),
   m_maximum_length     (-1),
   m_is_valid           (false)        // for now
{
   // no other code
}

/**
 *    Creates an named, but empty, csvarray.  The name can be added or
 *    modified later with a call to name(), and sections can be added with
 *    the std::vector::push_back() function.
 *
 *    If a file-name is given, the file is read to fill in the lines.
 *    unnamed section.  If it isn't used, the overhead isn't significant.
 *
 * \param name
 *    Provides the name of the csvarray object.
 *
 * \param filespec
 *    Provides the optional full path file-specification for the file to be
 *    opened and read to create all of the lines specified in that file.
 *
 * \param separator
 *    Provides the separator character that demarcates fields.  The default
 *    value of this parameter is the comma.
 */

csvarray::csvarray
(
   const std::string & name,
   const std::string & filespec,
   char separator
) :
   m_separator          (separator),
   m_source_file        (filespec),
   m_name               (name),
   m_csv_lines          (),
   m_minimum_length     (999999),
   m_maximum_length     (-1),
   m_is_valid           (false)
{
   if (! m_source_file.empty())
      m_is_valid = readfile(filespec);
}

/**
 *    Copy constructor for csvarray.
 *
 * \param source
 *    Provides the original object to be copied.
 */

csvarray::csvarray (const csvarray & source)
 :
   m_separator          (source.m_separator),
   m_source_file        (source.m_source_file),
   m_name               (source.m_name),
   m_csv_lines          (source.m_csv_lines),
   m_minimum_length     (source.m_minimum_length),
   m_maximum_length     (source.m_maximum_length)
{
   // no other code
}

/**
 *    Principal assignment operator for csvarray.
 *
 * \param source
 *    Provides the original object to be assigned.
 */

csvarray &
csvarray::operator = (const csvarray & source)
{
   if (this != &source)
   {
      m_separator          = source.m_separator;
      m_source_file        = source.m_source_file;
      m_name               = source.m_name;
      m_csv_lines          = source.m_csv_lines;
      m_minimum_length     = source.m_minimum_length;
      m_maximum_length     = source.m_maximum_length;
   }
   return *this;
}

/**
 *    Opens a file, and tries to construct an csvarray object, and a number
 *    of section objects, in it.
 *
 *    This function reads a file line by line.
 *
 *       -  Blank lines are skipped.
 *       -  Lines that start with ";", "#", "!", "'", or """ are considered
 *          to be blank lines.
 *       -  Lines that start with "[" are potentially section-names; if not,
 *          then they are considered errors and processing fails.
 *       -  Lines that start with alphabetic characters (case-sensitive)
 *          are potentially options.
 *          -  If an equal sign "=" follows the first characters, unquoted,
 *             then the line is an option+value line.
 *          -  If there is no equal sign, then the option will be treated
 *             like a bare flag.
 *       -  Values are anything following the equals.  Leading and trailing
 *          spaces are stripped, unless quoted.
 *
 * \param filespec
 *    Provides the full path to the file to be processed.
 *
 * \return
 *    Returns 'true' if any legal option was found, and 'false' if anything
 *    bad was found.
 */

bool
csvarray::readfile (const std::string & filespec)
{
   std::ifstream input(filespec.c_str());
   bool result = input.good();
   if (result)
   {
      std::string current_section;     // starts out empty
      for (std::string s; getline(input, s);)
      {
         if (input.good())
         {
            if (s.empty())             // skip blank lines
               continue;
            else
            {
               std::string::size_type p = s.find_first_not_of(TOKEN_SPACES);
               if (p == std::string::npos)
                  continue;
               else
               {
                  if (is_comment(s[p]))
                     continue;
                  else
                  {
                     Fields f;
                     std::string::size_type psep;
                     int count = 0;
                     do
                     {
                        std::string::size_type slength = std::string::npos;
                        psep = s.find_first_of(m_separator, p);
                        if (psep != std::string::npos)
                           slength = psep - p;

                        std::string sub(s.substr(p, slength));
                        f.push_back(sub);          // add field to the Fields
                        ++count;
                        p = psep + 1;

                     } while (psep != std::string::npos);

                     if (count > m_maximum_length)
                        m_maximum_length = count;

                     if (count < m_minimum_length)
                        m_minimum_length = count;

                     m_csv_lines.push_back(f);     // add the line of Fields
                  }
               }
            }
         }
         else
            break;
      }
   }
   return result;
}

}                 // namespace midipp

/**
 *    Writes out the contents of the csvarray container to stderr.
 *
 *    This implementation is a for_each style of looping through the
 *    container.
 *
 * \param tag
 *    Identifies the object in the human-readable output.
 *
 * \param container
 *    The stringmap through which iteration is done for showing.
 */

void
show (const std::string & tag, const midipp::csvarray & container)
{
   fprintf
   (
      stderr,
      "- xpc::csvarray '%s':\n"
      "-    Name:                    '%s'\n"
      "-    Size:                     %d\n"
      ,
      tag.c_str(),
      container.name().c_str(),
      int(container.size())
   );
   int count = 0;
   midipp::csvarray::Rows::const_iterator ci;
   for (ci = container.rows().begin(); ci != container.rows().end(); ci++)
   {
      fprintf(stderr, "Row %d:\n", count);
      ++count;
      midipp::csvarray::Fields::const_iterator fi;
      for (fi = ci->begin(); fi != ci->end(); fi++)
      {
         const std::string & s = *fi;
         fprintf(stderr, "-    Field:                   '%s'\n", s.c_str());
      }
   }
}

/*
 * csvarray.cpp
 *
 * vim: ts=3 sw=3 et ft=cpp
 */
