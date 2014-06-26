#ifndef MIDIPP_INIWRITING_HPP
#define MIDIPP_INIWRITING_HPP

/**
 * \file          iniwriting.hpp
 *
 *    Provides some free functions to write INI files, given a
 *    midipp::csvarray object.
 *
 * \library       libmidipp
 * \author        Chris Ahlstrom
 * \date          2014-04-23
 * \updates       2014-05-08
 * \version       $Revision$
 * \license       $XPC_SUITE_GPL_LICENSE$
 *
 *    Provides a way to write various kinds of MIDI-related INI files.
 *
 *    Also provides some names and macros.
 */

#include <string>

namespace midipp
{

/*
 * Other declarations
 */

class csvarray;                        // forward reference

/*
 * Global functions
 */

extern bool write_simple_drum_file     // deprecated, will be unsupported
(
   const std::string & filespec,
   const csvarray & rows
);
extern bool write_sectioned_drum_file
(
   const std::string & filespec,
   const csvarray & rows,
   bool writefooter = true
);
extern bool write_sectioned_patch_file
(
   const std::string & filespec,
   const csvarray & rows,
   bool writeheader = true
);

}                 // namespace midipp

#endif            //  MIDIPP_INIWRITING_HPP

/*
 * iniwriting.hpp
 *
 * vim: ts=3 sw=3 et ft=cpp
 */
