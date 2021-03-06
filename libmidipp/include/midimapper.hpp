#ifndef MIDIPP_MIDIMAPPER_HPP
#define MIDIPP_MIDIMAPPER_HPP

/*
 * midicvtpp - A MIDI-text-MIDI translater
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 * Free Software Foundation, Inc.,
 * 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

/**
 * \file          midimapper.hpp
 *
 *    This module provides functions for advanced MIDI/text conversions.
 *
 * \library       libmidipp
 * \author        Chris Ahlstrom
 * \date          2014-04-24
 * \updates       2016-04-23
 * \version       $Revision$
 * \license       GNU GPL
 *
 *    The mapping process works though static functions that reference a
 *    global midimapper object.
 *
 *    This object gets its setup from an INI file.  This INI file has an
 *    unnamed section with the following format:
 *
\verbatim
         file-style = sectioned
         setup-name = midicvtpp
         record-count = 51
         map-type = Drum
         gm_-hannel = 10
         device-channel = 16
\endverbatim
 *
 *    The "drum" sections are named for the GM note that is to be
 *    remapped, and the "patch" sections are name for the GM patch/program
 *    number that is to be remapped:
 *
\verbatim
         [ Drum 35 ]
         gm-name  = Acoustic Bass Drum
         gm-note  = 35
         dev-name = N/A
         dev-note = 35
         gm-equiv = Acoustic Base Drum

         [ Patch 1 ]
         gm-name = Bright Acoustic Piano
         gm-patch = 1
         dev-name = Jazz Organ 1
         dev-patch = 16
         gm-equiv = Drawbar Organ
\endverbatim
 *
 *    The gm-equiv fields are currently just a way to see how good the
 *    mapping is.  If it matches the gm-name, that mapping is probably
 *    pretty good.
 *
 *    Finally, a facility is provided to move channels around.  This is
 *    needed, for example, when converting drums to channel 10, but the
 *    MIDI file already has other music on channel 10.  If you don't need
 *    this section, don't define it; it saves processing time.
 *
\verbatim
         [ Channels ]
         ch_01 =  1
         ch_02 =  2
           . . .
         ch_16 = 16
\endverbatim
 */

#include <map>
#include <string>

namespace midipp
{

/**
 *    This class is meant to extend the map of values with additional data
 *    that can be written out to summarize some information about the MIDI
 *    remapping that was done.  Instead of just the integer value to use,
 *    this class holds the names of the items on both ends of the mapping,
 *    plus a usage count.  We also added the "GM equivalent" name to this
 *    class as well.
 */

class annotation
{

private:

   /**
    *    The integer value to which the incoming (key) value is to be
    *    mapped.
    */

   const int m_value;

   /**
    *    The name of the drum note or patch represented by the key value.
    */

   const std::string m_key_name;

   /**
    *    The name of the drum note or patch represented by the integer
    *    value.
    */

   const std::string m_value_name;

   /**
    *    The name of the GM drum note or patch that is replacing the
    *    device's drum note of patch.  Sometimes there is no exact
    *    replacement, so it is good to know what GM sound is replacing the
    *    device's sound.
    */

   const std::string m_gm_name;

   /**
    *    The number of times this particular mapping was performed in the
    *    MIDI remapping operation.
    */

   int m_remap_count;

private:

   annotation ();                      /* hide the default constructor */

public:

   annotation
   (
      int value,
      const std::string & keyname,
      const std::string & valuename,
      const std::string & gmname
   );

   /*
    * The default copy constructor and principal assignment operators
    * should suffice.
    */

   /**
    * \getter m_value
    */

   int value () const
   {
      return m_value;
   }

   /**
    * \getter m_key_name
    */

   const std::string & key_name () const
   {
      return m_key_name;
   }

   /**
    * \getter m_value_name
    */

   const std::string & value_name () const
   {
      return m_value_name;
   }

   /**
    * \getter m_gm_name
    */

   const std::string & gm_name () const
   {
      return m_gm_name;
   }

   /**
    * \setter m_remap_count
    */

   void increment_count ()
   {
      ++m_remap_count;
   }

   /**
    * \getter m_remap_count
    */

   int count () const
   {
      return m_remap_count;
   }

};       // class annotation

/**
 *    This class provides for some basic remappings to be done to MIDI
 *    files, using the old and new facilities of libmidifilex.
 *
 *    It works by holding all sorts of standard C++ map objects that are
 *    used to translate from one numeric value to another.
 *
 *    For use in the midicvtpp application, a single global instance of
 *    this object is created, and is used in static C-style callback
 *    functions that can be used in the C library libmidifilex.
 */

class midimapper
{

   friend void show_maps
   (
      const std::string & tag,
      const midipp::midimapper & container,
      bool full_output
   );

public:

   /**
    *    Provides a constant to indicate an inactive or invalid integer
    *    value.
    */

   static const int NOT_ACTIVE = -1;

private:

   /**
    *    Provides the type of the map between one set of values and
    *    another set of values.
    */

   typedef std::map<int, annotation> midimap;
   typedef std::map<int, annotation>::iterator iterator;
   typedef std::map<int, annotation>::const_iterator const_iterator;
   typedef std::pair<int, annotation> midimap_pair;
   typedef std::pair<iterator, bool> midimap_result;
   typedef std::pair<std::map<int, int>::iterator, bool> intmap_result;

private:

   /**
    *    Provides the style of the INI file.  This is one of the following
    *    strings:
    *
    *       -  "simple".  All of the mapping information is contained in a
    *          single comma-separated value.  We probably won't end up
    *          supporting this option, as  it is a bit harder for humans
    *          to read.  See <code>doc/GM_PSS-790_Drums-save.ini</code>.
    *       -  "sectioned".  The mapping information for each note is
    *          contained in its own "Drum" section.
    *          See <code>doc/GM_PSS-790_Drums.ini</code>.
    *
    *    The name of this attribute in the INI file is "file-style".  Case
    *    is significant.
    */

   std::string m_file_style;

   /**
    *    Provides a nice tag name for the setup, nothing more.  It can be
    *    anything.  A common value is the name of the program that the INI
    *    file is meant for.
    *
    *    The name of this attribute in the INI file is "setup-name".  Case
    *    is significant.
    */

   std::string m_setup_name;

   /**
    *    Saves the name of the INI file, which can be useful in the
    *    show_maps() function.
    */

   std::string m_ini_filespec;

   /**
    *    Saves the name of the input MIDI file, for information only.
    */

   std::string m_in_filename;

   /**
    *    Saves the name of the output MIDI file, for information only.
    */

   std::string m_out_filename;

   /**
    *    Indicates what kind of mapping is allegedly provided by the file.
    *    This can be one of the following values:
    *
    *       -  "drum".  The file describes mapping one pitch/channel to
    *          another pitch/channel, used mostly for coercing old drum
    *          machines to something akin to a General MIDI kit.
    *       -  "patch".  The file describes program (patch) mappings, used
    *          to map old devices patch change values to General MIDI.
    *       -  "multi".  The file describes both "drum" and "patch"
    *          mappings.
    *
    *    The name of this attribute in the INI file is "map-type".  Case
    *    is significant.
    */

   std::string m_map_type;

   /**
    *    Provides the number of records (lines) or sections in the INI
    *    file.  Indicates the number of items being remapped.
    *
    *    This attribute ("record-count") does not appear in the INI file,
    *    as it is calculated as the file is read.
    *
    * \warning
    *    Only applies to "drum" mappings at present.  MUST FIX!
    */

   int m_record_count;

   /**
    *    Provides the channel to use for General MIDI drums.  This value
    *    is usually 9, meaning MIDI channel 10.  However, be careful, as
    *    externally, this value is always on a 1-16 scale, while
    *    internally it is reduced by 1 (a 0-15 scale) to save endless
    *    decrements.
    *
    *    The name of this attribute in the INI file is "gm-channel".  Case
    *    is significant.
    */

   int m_gm_channel;

   /**
    *    Provides the channel that is used by the native device.  Older
    *    MIDI equipment sometimes used channel 16 for percussion.
    *
    *    The name of this attribute in the INI file is "dev-channel".
    *    Case is significant.
    */

   int m_device_channel;

   /**
    *    Provides an optional channel number to filter in the MIDI file.
    *
    *    If this value is specified, then one of two kinds of channel
    *    filtering is turned on:
    *
    *       -# If m_extraction_on is true, then only channel-based events on
    *          the filtered channel will be written to the output MIDI file.
    *       -# If m_rejection_on is true, then only channel-based events
    *          <i> not </i> on the filtered channel will be written to
    *          the output MIDI file.
    *
    *    This attribute is called "extract-channel" or "reject-channel".
    *    This is a command-line option.  By default this value is -1,
    *    which means to not filter by channel.
    */

   int m_filter_channel;

   /**
    *    A faster way to check if m_filter_channel is enabled.
    */

   bool m_extraction_on;

   /**
    *    Changes the extraction-channel to a rejection channel, where only
    *    the channel that matches is dropped from the output MIDI file.
    */

   bool m_rejection_on;

   /**
    *    Indicates that the mapping should occur in the reverse direction.
    *    That is, instead of mapping the notes from the device pitches and
    *    channel to General MIDI, the notes and channel should be mapped
    *    from General MIDI back to the device.  This option is useful for
    *    playing back General MIDI files on old equipment.
    *
    *    Note that this option is an INI option ("reverse"), as well as a
    *    command-line option.  It is specified by alternate means, such as
    *    a command-line parameter like "--reverse".
    */

   bool m_map_reversed;

   /**
    *    Provides the mapping between pitches.  If m_map_reversed is
    *    false, then the key is the GM pitch/note, and the value is the
    *    device pitch/note (which is the GM note needed to produce the
    *    same sound in GM as the device would have produced).  If
    *    m_map_reversed is true, then the key is the GM pitch/note, and
    *    the value is the device pitch/note, so that the MIDI file will be
    *    converted from GM mapping to device mapping.
    */

   midimap m_drum_map;

   /**
    *    Provides the mapping between patches (programs).  If
    *    m_map_reversed is false, then the key is the GM patch/program,
    *    and the value is the device patch/program (which is the GM note
    *    needed to produce the same sound in GM as the device would have
    *    produced).  If m_map_reversed is true, then the key is the GM
    *    patch/program, and the value is the device patch/program so that
    *    the MIDI file will be converted from GM mapping to device
    *    mapping.
    */

   midimap m_patch_map;

   /**
    *    Provides the mapping between channels (optional).  If
    *    m_map_reversed is true, then the mapping of channels is reversed.
    *    There's no need for channel names with this one.
    */

   std::map<int, int> m_channel_map;

   /**
    *    Indicates if the setup is valid.
    */

   bool m_is_valid;

public:

   midimapper ();                      // an unnamed,  no-change mapping
   midimapper                          // arguments are passed to initree
   (
      const std::string & name,
      const std::string & filespec  = "",    // for behavior like C version
      bool reverse_it               = false,
      int filter_channel            = NOT_ACTIVE,
      bool reject_it                = false,
      const std::string & infile    = "",
      const std::string & outfile   = ""
   );

   int repitch (int channel, int input);
   int rechannel (int channel);
   int repatch (int program);

   /**
    *    Determines if the value parameter is usable, or "active".
    *
    * \param value
    *    The integer value to be checked.
    *
    * \return
    *    Returns true if the value is not NOT_ACTIVE.
    */

   static bool active (int value)
   {
      return value != midipp::midimapper::NOT_ACTIVE;
   }

   /**
    *    Determines if both value parameters are usable, or "active".
    *
    * \param v1
    *    The first integer value to be checked.
    *
    * \param v2
    *    The second integer value to be checked.
    *
    * \return
    *    Returns true if both of the values are not NOT_ACTIVE.
    */

   static bool active (int v1, int v2)
   {
      return
      (
          v1 != midipp::midimapper::NOT_ACTIVE &&
          v2 != midipp::midimapper::NOT_ACTIVE
      );
   }

   /**
    * \getter m_file_style
    */

   const std::string & file_style () const
   {
      return m_file_style;
   }

   /**
    * \getter m_setup_name
    */

   const std::string & setup_name () const
   {
      return m_setup_name;
   }

   /**
    * \getter m_ini_filespec
    */

   const std::string & ini_filename () const
   {
      return m_ini_filespec;
   }

   /**
    * \getter m_in_filename
    */

   const std::string & in_filename () const
   {
      return m_in_filename;
   }

   /**
    * \getter m_out_filename
    */

   const std::string & out_filename () const
   {
      return m_out_filename;
   }

   /**
    * \getter m_map_type
    */

   const std::string & map_type () const
   {
      return m_map_type;
   }

   /**
    * \getter m_record_count
    */

   int record_count () const
   {
      return m_record_count;
   }

   /**
    * \getter m_gm_channel
    */

   int gm_channel () const
   {
      return m_gm_channel + 1;
   }

   /**
    * \getter m_device_channel
    */

   int device_channel () const
   {
      return m_device_channel + 1;
   }

   /**
    * \getter m_filter_channel
    */ 

   int filter_channel () const
   {
      int result = m_filter_channel;
      if (m_filter_channel >= 0 && m_filter_channel < 16)
         result = m_filter_channel + 1;

      return result;
   }

   /**
    * \getter m_extraction_on
    */

   bool extract () const
   {
      return m_extraction_on;
   }

   /**
    * \getter m_rejection_on
    */

   bool reject () const
   {
      return m_rejection_on;
   }

   /**
    * \getter m_is_valid;
    */

   bool valid () const
   {
      return m_is_valid;
   }

   /**
    * \getter m_drum_map
    *    Returns a reference to the note map.  The weird thing was that,
    *    when I had left the reference operator out, show_note_map() would
    *    show a lot of notes missing from the map, as if copying the map
    *    wasn't working properly!!!
    */

   const midimap & drum_map () const
   {
      return m_drum_map;
   }

   /**
    * \getter m_patch_map
    *    Returns a reference to the patch map.
    */

   const midimap & patch_map () const
   {
      return m_patch_map;
   }

   /**
    * \getter m_channel_map
    *    Returns a reference to the channel map.
    */

   const std::map<int, int> & channel_map () const
   {
      return m_channel_map;
   }

   /**
    * \getter m_map_reversed
    */

   bool map_reversed () const
   {
      return m_map_reversed;
   }

private:

   bool read_maps (const std::string & filename);
   bool read_unnamed_section (const initree & it);
   bool read_channel_section (const initree & it);

};

/*
 * Free functions in the midipp namespace.
 */

extern void show_maps
(
   const std::string & tag,
   const midipp::midimapper & container,
   bool full_output = true
);

}                 // namespace midipp

/*
 * Global functions.
 */

extern void midimap_init (midipp::midimapper & mm);

#endif            // MIDIPP_MIDIMAPPER_HPP

/*
 * midimapper.hpp
 *
 * vim: sw=3 ts=3 wm=8 et ft=cpp
 */

