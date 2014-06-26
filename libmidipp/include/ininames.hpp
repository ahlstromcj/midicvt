#ifndef MIDIPP_ININAMES_HPP
#define MIDIPP_ININAMES_HPP

/**
 * \file          ininames.hpp
 *
 *    Provides some free functions to write INI files, given a
 *    midipp::csvarray object.
 *
 * \library       libmidipp
 * \author        Chris Ahlstrom
 * \date          2014-05-06
 * \updates       2014-05-17
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

/**
 *    Provides a selection switch for the supported INI section types.
 *    Currently that includes "Drum" and "Patch" sections.
 */

typedef enum
{
   INI_SECTION_DRUM,          /**< Marks a "[ Drum ]" section.                */
   INI_SECTION_PATCH,         /**< Marks a "[ Patch ]" section.               */
   INI_SECTION_CHANNEL,       /**< Marks a "[ Channel ]" section.             */
   INI_SECTION_UNKNOWN        /**< Marks an unknown section name.             */

} gm_ini_section_t;

/**
 *    Provides the prefix for the initial part of the drum-section name.
 *    These names are of the form "Drum nnn" where nnn is the MIDI note
 *    number, ranging from 0 to 127.
 */

const std::string DRUM_SECTION            = "Drum";

/**
 *    Provides mnemonics for the expected indices into the vector of fields
 *    that occurs on each line of our Drum-map files.
 *
 * \var DRUM_INDEX_GM_NAME
 *    Provides the index needed to access the column containing the General
 *    MIDI drum name for the given MIDI note.  Note that this name does not
 *    have to correspond to General MIDI; it can correspond to another
 *    consumer-level MIDI device.  But the most common case will be mapping
 *    between one device and Genera MIDI.
 *
 * \var DRUM_INDEX_GM_NOTE
 *    Provides the index needed to access the column containing the MIDI
 *    note value for the given General MIDI drum name in the previous
 *    column.  This note value is the input value of the note that we want
 *    to remap.
 *
 * \var DRUM_INDEX_DEV_NAME
 *    Provides the index needed to access the column containing the
 *    consumer-level device's name for the drum that we want to map to
 *    General MIDI or another device.
 *
 * \var DRUM_INDEX_DEV_NOTE
 *    Provides the index needed to access the column containing the General
 *    MIDI (or other device) note number to which we want to remap.  This
 *    note value is the output value of the note, given the input value at
 *    DRUM_INDEX_GM_NOTE.
 *
 * \var DRUM_INDEX_GM_EQUIV
 *    This item can be optionally placed in the fifth column of the CSV
 *    to indicate the GM note name to which the device is being mapped.
 */

typedef enum
{
   DRUM_INDEX_GM_NAME,
   DRUM_INDEX_GM_NOTE,
   DRUM_INDEX_DEV_NAME,
   DRUM_INDEX_DEV_NOTE,
   DRUM_INDEX_GM_EQUIV

} gm_drum_field_index_t;

/**
 *    Provides the string for the name of the DRUM_INDEX_GM_NAME field as
 *    shown in the INI file.
 */

const std::string DRUM_LABEL_GM_NAME      =  "gm-name";

/**
 *    Provides the string for the name of the DRUM_INDEX_GM_NOTE field as
 *    shown in the INI file.
 */

const std::string DRUM_LABEL_GM_NOTE      =  "gm-note";

/**
 *    Provides the string for the name of the DRUM_INDEX_DEV_NAME field as
 *    shown in the INI file.
 */

const std::string DRUM_LABEL_DEV_NAME     =  "dev-name";

/**
 *    Provides the string for the name of the DRUM_INDEX_DEV_NAME field as
 *    shown in the INI file.
 */

const std::string DRUM_LABEL_DEV_NOTE     =  "dev-note";

/**
 *    Provides the string for the name of the DRUM_INDEX_GM_EQUIV field as
 *    shown in the INI file.
 */

const std::string DRUM_LABEL_GM_EQUIV     =  "gm-equiv";

/**
 *    Provides the prefix for the initial part of the patch-section name.
 *    These names are of the form "Patch nnn" where nnn is the MIDI program
 *    number, ranging from 0 to 127.
 */

const std::string PATCH_SECTION           = "Patch";

/**
 *    Provides mnemonics for the expected indices into the vector of fields
 *    that occurs on each line of our Patch-map files.
 *
 * \var PATCH_INDEX_GM_NAME
 *    Provides the index needed to access the column containing the General
 *    MIDI drum name for the given MIDI note.  Note that this name does not
 *    have to correspond to General MIDI; it can correspond to another
 *    consumer-level MIDI device.  But the most common case will be mapping
 *    between one device and Genera MIDI.
 *
 * \var PATCH_INDEX_GM_PATCH
 *    Provides the index needed to access the column containing the MIDI
 *    note value for the given General MIDI drum name in the previous
 *    column.  This note value is the input value of the note that we want
 *    to remap.
 *
 * \var PATCH_INDEX_DEV_NAME
 *    Provides the index needed to access the column containing the
 *    consumer-level device's name for the drum that we want to map to
 *    General MIDI or another device.
 *
 * \var PATCH_INDEX_DEV_PATCH
 *    Provides the index needed to access the column containing the General
 *    MIDI (or other device) note number to which we want to remap.  This
 *    note value is the output value of the note, given the input value at
 *    PATCH_INDEX_GM_PATCH.
 *
 * \var PATCH_INDEX_GM_EQUIV
 *    This item can be optionally placed in the fifth column of the CSV
 *    to show the GM patch name to which the device is being mapped.
 */

typedef enum
{
   PATCH_INDEX_GM_NAME,
   PATCH_INDEX_GM_PATCH,
   PATCH_INDEX_DEV_NAME,
   PATCH_INDEX_DEV_PATCH,
   PATCH_INDEX_GM_EQUIV

} gm_patch_field_index_t;

/**
 *    Provides the string for the name of the PATCH_INDEX_GM_NAME field as
 *    shown in the INI file.
 */

const std::string PATCH_LABEL_GM_NAME     =  "gm-name";

/**
 *    Provides the string for the name of the PATCH_INDEX_GM_PATCH field as
 *    shown in the INI file.
 */

const std::string PATCH_LABEL_GM_PATCH    =  "gm-patch";

/**
 *    Provides the string for the name of the PATCH_INDEX_DEV_NAME field as
 *    shown in the INI file.
 */

const std::string PATCH_LABEL_DEV_NAME    =  "dev-name";

/**
 *    Provides the string for the name of the PATCH_INDEX_DEV_NAME field as
 *    shown in the INI file.
 */

const std::string PATCH_LABEL_DEV_PATCH   =  "dev-patch";

/**
 *    Provides the string for the name of the PATCH_INDEX_GM_EQUIV field as
 *    shown in the INI file.
 */

const std::string PATCH_LABEL_GM_EQUIV    =  "gm-equiv";

/* --------------------------------------------------------------------
 *    Value-names for the unnamed section of the INI file.
 * -------------------------------------------------------------------- */

/**
 *    Provides the prefix for the channel-section name.
 */

const std::string CHANNEL_SECTION         = "Channels";

/**
 *    Provides the initial part of the name for each entry in the
 *    channel-section.
 */

const std::string CHANNEL_TOKEN           = "ch_";

/* --------------------------------------------------------------------
 *    Value-names for the unnamed section of the INI file.
 * -------------------------------------------------------------------- */

/**
 *    Provides a consistent string for the file-style attribute.
 */

const std::string GM_INI_FILE_STYLE       = "file-style";

/**
 *    Provides a consistent string for the setup-name attribute.
 */

const std::string GM_INI_SETUP_NAME       = "setup-name";

/**
 *    Provides a consistent string for the map-type attribute.
 */

const std::string GM_INI_MAP_TYPE         = "map-type";

/**
 *    Provides a consistent string for the GM-channel attribute.
 */

const std::string GM_INI_GM_CHANNEL       = "gm-channel";

/**
 *    Provides a consistent string for the device-channel attribute.
 */

const std::string GM_INI_DEV_CHANNEL      = "dev-channel";

/**
 *    Provides a consistent string for the extract-channel attribute.
 */

const std::string GM_INI_EXTRACT_CHANNEL  = "extract-channel";

/**
 *    Provides a consistent string for the reject-channel attribute.
 */

const std::string GM_INI_REJECT_CHANNEL   = "reject-channel";

/**
 *    Provides a consistent string for the reverse attribute.  This is a
 *    boolean attribute.
 */

const std::string GM_INI_REVERSE          = "reverse";

/**
 *    Provides a consistent string for the testing attribute.  This is a
 *    boolean attribute.
 */

const std::string GM_INI_TESTING          = "testing";

/**
 *    Provides a consistent string for the "none" value.
 */

const std::string GM_INI_NO_VALUE         = "none";

}                 // namespace midipp

#endif            //  MIDIPP_ININAMES_HPP

/*
 * ininames.hpp
 *
 * vim: ts=3 sw=3 et ft=cpp
 */
