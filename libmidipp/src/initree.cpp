/**
 * \file          initree.cpp
 *
 *    Primitive INI-file input parser derived from the XPC library.
 *
 * \library       libmidipp
 * \author        Chris Ahlstrom
 * \date          2014-04-22
 * \updates       2014-05-21
 * \version       $Revision$
 * \license       $XPC_SUITE_GPL_LICENSE$
 *
 *    Provides a way to set the application options from a Windows-style
 *    "INI" file, by parsing that configuration file and making it available
 *    as if it were a set of command-line parameters.
 *
 *    The file is in simple INI format.  Each line is one of the following:
 *
 *       -  <i>Blank.  Blank lines are ignored.
 *       -  <i>Comment.  Lines whose first non-white-space character is one
 *          of hash-mark, semi-colon, exclamation-point, single-quote, or
 *          double-quote, are ignored.
 *       -  <i>Section.  "[ Section Name ]".
 *          If present, this item is used to indicate which group of
 *          initializations are acceptable to the caller.
 *       -  <i>Option + value</i>.
 *          <code>optioname = optionvalue</code>.
 *       -  <i>Option without value</i>.
 *          This item is treated like a simple flag, since there is no value
 *          specified.
 *
 * \warning
 *    -# This module assumes only ASCII text.  This is a big limitation and
 *       severely hurts internationalization.
 *    -# The code cannot really handle mixes of quotes and comment
 *       characters on an option line.  So, generally prefer blocks of
 *       comments between options, rather than trailing comments on an
 *       option line.  See the tests/initree.ini file.
 *    -# Comments are not saved anywhere, so when we get around to writing
 *       out an INI file, they will get dropped.
 */

#include <cctype>                      /* toupper(), isalpha(), etc. macros   */
#include <fstream>

#include <initree.hpp>                 /* the functions in this module        */

/**
 * \internal
 *    Provides a consistent definition of the whitespace characters.  This
 *    define does not bother to try to trap all the characters that could be
 *    considered whitespace or outside the range of legal section and option
 *    names.
 */

#define TOKEN_SPACES    " \t\0"

/**
 * \internal
 *    Provides a consistent definition of the characters that can end a
 *    token.  These characters consist of token-spaces and the ']', '=',
 *    ";", and "#" characters.
 */

#define TOKEN_ENDERS    TOKEN_SPACES "]=;#"

/**
 * \internal
 *    Provides a consistent definition of the characters that can end an
 *    option.  These characters consist of token-spaces and the '='
 *    character.
 */

#define OPTION_ENDERS   TOKEN_SPACES "="

namespace midipp
{

/**
 *    Must provide an initialization for this static member of initree.
 */

initree::Section initree::sm_dummy_section;

/**
 *    Creates an unnamed and empty initree.
 *
 *    The name can be added later with a call to name(), and sections can be
 *    added with the insert() function.
 *
 *    However, to simplify the code, the constructor always creates an
 *    unnamed section.
 */

initree::initree ()
 :
   m_source_file        (),
   m_name               (),
   m_sections           (),
   m_has_named_section  (false)
{
   (void) make_section(std::string(""));
}

/**
 *    Creates a named, but empty, initree.  The name can be added or
 *    modified later with a call to name(), and sections can be added with
 *    the insert() function.
 *
 *    If a file-name is given, the file is read to fill in the sections.
 *
 *    To simplify the code, the constructor always creates an
 *    unnamed section.  If it isn't used, the overhead isn't significant.
 *
 * \param name
 *    Provides the name of the initree object.
 *
 * \param filespec
 *    Provides the optional full path file-specification for the file to be
 *    opened and read to create all of the sections specified in that file.
 */

initree::initree
(
   const std::string & name,
   const std::string & filespec
) :
   m_source_file        (filespec),
   m_name               (name),
   m_sections           (),
   m_has_named_section  (false)
{
   (void) make_section(std::string(""));
   if (! m_source_file.empty())
   {
      (void) readfile(filespec);
   }
}

/**
 *    Creates an unnamed and empty initree.  The name can be added later
 *    with a call to name(), and sections can be added with the insert()
 *    function.
 */

initree::initree (const initree & source)
 :
   m_source_file        (source.m_source_file),
   m_name               (source.m_name),
   m_sections           (source.m_sections),
   m_has_named_section  (source.m_has_named_section)
{
   // no other code
}

/**
 *    Creates an unnamed and empty initree.  The name can be added later
 *    with a call to name(), and sections can be added with the insert()
 *    function.
 */

initree &
initree::operator = (const initree & source)
{
   if (this != &source)
   {
      m_source_file        = source.m_source_file;
      m_name               = source.m_name;
      m_sections           = source.m_sections;
      m_has_named_section  = source.m_has_named_section;
   }
   return *this;
}

/**
 *    Opens a file, and tries to construct an initree object, and a number
 *    of section objects, in it.
 *
 *    This function reads a file line by line.
 *
 *       -  Blank lines are skipped.
 *       -  Lines that start with ";", "#", "!", "'", or double-quote are
 *          considered to be blank lines.
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
initree::readfile (const std::string & filespec)
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
               /**
                * The code checks only for white-space characters; it is
                * possible to embed control characters and have them treated
                * as tokens.  At your own risk, baby!
                */

               std::string::size_type p = s.find_first_not_of(TOKEN_SPACES);
               if (p == std::string::npos)
                  continue;
               else
               {
                  if (is_comment(s[p]))
                     continue;
                  else
                  {
                     if (s[p] == '[')
                     {
                        current_section = process_section_name(s, p);
                        if (current_section.empty())
                        {
                           result = false;
                           break;
                        }
                        else
                        {
                           result = make_section(current_section);
                           if (! result)
                              break;   // same section name found again
                        }
                     }
                     else if (isalpha(s[p]))
                     {
                        result = process_option(s, p, current_section);
                        if (! result)
                           break;
                     }
                     else
                     {
                        result = false;
                        break;
                     }
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

/**
 *    Extracts a section name, which must start with an alphabetic
 *    character, and can contain embedded spaces.
 *
 *    This function processes a line of the follow formats to extract the
 *    section name:
 *
\verbatim
         [Name]
         [ Name ]
         [ Section Name ]
\endverbatim
 *
 *    Leading and trailing spaces are not included in the section name.
 *    Embedded spaces are included.  Note that the trailing ']' is required,
 *    or the line is considered to be malformed.
 *
 * \param s
 *    Provides the current line of text, which starts with '['.
 *
 * \param p
 *    Provides the position of the '[' character.
 *
 * \return
 *    Returns the name of the newly-created section.  If this name is
 *    empty, then an error occurred.
 */

std::string
initree::process_section_name
(
   const std::string & s,
   std::string::size_type p
)
{
   std::string result;
   std::string::size_type pbegin = p + 1;
   if (isspace(s[pbegin]))
      pbegin = s.find_first_not_of(TOKEN_SPACES, pbegin);

   if (pbegin != std::string::npos)
   {
      if (isalpha(s[pbegin]))
      {
         std::string::size_type prbracket = s.find_first_of("]", pbegin+1);
         if (prbracket != std::string::npos)
         {
            std::string::size_type pend = s.find_last_not_of
            (
               TOKEN_ENDERS, prbracket-1
            );
            size_t length = pend - pbegin + 1;
            result = s.substr(pbegin, length);
         }
      }
   }
   return result;
}

/**
 *    Extracts an option from a line of text, where an option name must
 *    start with an alphabetic character, cannot have embedded spaces, and
 *    might not have a value.
 *
 *    This function handles options of the following forms:
 *
\verbatim
         option
         option = value
         option = "multiple-token value"
         option = ""
\endverbatim
 *
 *    The legal characters in an option name are letters, digits, a hyphen
 *    (minus), and underscore.
 *
 *    The "=" is mandatory, though the spaces around it are optional.
 *
 *    Every option has a string value.  If the value is not provided, then
 *    it is set to empty.  If the string value is in double-quotes, it is
 *    translated via snprintf().  Use double quites if spaces and special
 *    characters are part of the string.
 *
 *    Comment characters other than single- or double-quotes end a value.
 *
 * \param s
 *    Provides the current line of text, which starts with a letter.
 *
 * \param p
 *    Provides the position of the first letter.
 *
 * \param currentsection
 *    Points to the current Section (which might be an unnamed section).
 *    Option entries are added to this section.  This pointer is checked
 *    elsewhere, and will not be null here unless we haven't yet hit a
 *    section.  In that case, this function will created an unnamed section,
 *    if one does not yet exist.
 *
 * \return
 *    Returns 'true' if the option was proper and properly processed.
 *    Otherwise, 'false' is returned.
 */

bool
initree::process_option
(
   const std::string & s,
   std::string::size_type p,
   const std::string & sectionname
)
{
   std::string::size_type linelength = s.size();
   bool result = s.size() > 0 && p != std::string::npos;
   if (result)
   {
      std::string option;
      std::string value;
      std::string::size_type pbegin = p;
      if (isspace(s[pbegin]))
         pbegin = s.find_first_not_of(TOKEN_SPACES, pbegin);

      result = pbegin != std::string::npos;
      if (result)
      {
         bool no_value = false;

         /*
          * It is easiest to get the value first, if flagged by '='.
          */

         std::string::size_type pvalue = s.find_first_of("=");
         if (pvalue == std::string::npos)
            no_value = true;

         if (! no_value)
         {
            std::string::size_type pquote1 = s.find_first_of("\"");
            if (pquote1 != std::string::npos)
            {
               std::string::size_type pquote2 = s.find_last_of("\"");
               if (pquote2 > pquote1)
               {
                  if (pquote2 > (pquote1 + 1))           // anything in quotes?
                  {
                     pquote1++;                          // skip 1st quote
                     size_t sublength = pquote2 - pquote1;
                     value = s.substr(pquote1, sublength);
                  }
               }
               else
                  result = false;                        // no closing quote
            }
            else                                         // no quotes
            {
               pvalue++;                                 // move one past "="
               while (isspace(s[pvalue]) && pvalue < s.size())
                  pvalue++;

               if (pvalue < s.size())
               {
                  while
                  (
                     ! isspace(s[pvalue]) &&
                     s[pvalue] != '#' &&
                     s[pvalue] != ';' &&
                     pvalue < linelength
                  )
                  {
                     value += s[pvalue];
                     pvalue++;
                  }
               }
            }
         }

         /*
          * We now know the disposition of the value (do we have one, what
          * is it)?  Now to peel out the option.  If there is no value, the
          * end of the string or start of spaces ends the option.  If there
          * is a value, the "=" can also end the option.  The pbegin index
          * points to the first non-space character.
          */

         if (result)
         {
            while
            (
               (isalnum(s[pbegin]) || (s[pbegin] == '_') || (s[pbegin] == '-'))
                  && (pbegin < linelength)
            )
            {
               option += s[pbegin];
               pbegin++;
            }
            result = ! option.empty();
         }
      }
      if (result)
      {
         /*
          * Now create an option/value pair and insert it in the Section.
          */

         iterator ci = find(sectionname);
         if (ci != end())
         {
            int currsize = ci->second.size();
            int newsize = ci->second.insert(option, value);
            result = newsize == (currsize + 1);
         }
      }
   }
   return result;
}

/**
 *    Created a new section, with the given name, and inserts it into the
 *    initree.
 *
 * \param sectionname
 *    Provides the name of new section that is to be created.
 *
 * \return
 *    Returns 'true' if the section was successfully inserted into the
 *    initree.
 *
 * \sideeffect
 *    If the created section has a name, then m_has_named_section is set to
 *    'true'.
 */

bool
initree::make_section (const std::string & sectionname)
{
   Section sec(sectionname);
   int treecount = int(m_sections.size());
   int newcount = insert(sectionname, sec);  // copy new section into tree
   bool result = newcount == (treecount + 1);
   if (result)
   {
      if (! sectionname.empty())
         m_has_named_section = true;
   }
   return result;
}

}                 // namespace midipp

/**
 *    Writes out the contents of the stringmap container.
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
show (const std::string & tag, const midipp::initree & container)
{
   fprintf
   (
      stderr,
      "- midipp::initree '%s':\n"
      "-    Name:                    '%s'\n"
      "-    Size:                     %d\n"
      ,
      tag.c_str(),
      container.name().c_str(),
      int(container.size())
   );
   midipp::initree::const_iterator ci;
   for (ci = container.begin(); ci != container.end(); ci++)
   {
      show("midipp::initree::Section", ci->second);
   }
}

/*
 * initree.cpp
 *
 * vim: ts=3 sw=3 et ft=cpp
 */
