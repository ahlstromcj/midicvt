/**
 * \file          stringmap.cpp
 * \library       libmidipp
 * \author        Chris Ahlstrom
 * \date          2014-04-22
 * \updates       2014-05-18
 * \version       $Revision$
 * \license       $XPC_SUITE_GPL_LICENSE$
 *
 *    This module defines an std::map that uses only std::string as a key
 *    value, for easy lookup.
 */

#include <stringmap.hpp>               /* midipp::stringmap<VALUETYPE>        */

namespace midipp
{

/**
 *    A simple case-insensitive comparison for simple string equality.
 *
 *    It is a bit similar to the cmp_nocase() function in section 20.3.8
 *    "Comparisons" of Stroustrop's C++ Programming Language book.
 *
 *    We just stick it in this existing module for convenience.
 *
 * \note
 *    If your application is already using Boost, you can use the
 *    boost::iequal() function instead.
 *
 * \param s1
 *    Provides the first string.
 *
 * \param s2
 *    Provides the second string.
 *
 * \return
 *    Returns true of the strings are identical except for case.
 */

bool
iequal (const std::string & s1, const std::string & s2)
{
   bool result = s1.size() == s2.size();
   if (result)
   {
      std::string::const_iterator p1 = s1.begin();
      std::string::const_iterator p2 = s2.begin();
      while (p1 != s1.end() && p2 != s2.end())
      {
         if (toupper(*p1) != toupper(*p2))
         {
            result = false;
            break;
         }
         else
         {
            p1++;
            p2++;
         }
      }
   }
   return result;
}

}        // namespace midipp

/**
 *    Writes out the contents of the string to standard output, in a
 *    stylized format.
 *
 *    Also see the show_pair() and show() functions in the stringmap.hpp
 *    module.
 *
 * \param tag
 *    A name for the string, to provide context for the human reader.
 *
 * \param s
 *    The string to be shown.
 */

void
show (const std::string & tag, const std::string & s)
{
   fprintf
   (
      stderr,
      "-    String ('%8s')       '%s'\n",
      tag.c_str(), s.c_str()
   );
}

/*
 * stringmap.cpp
 *
 * vim: ts=3 sw=3 et ft=cpp
 */
