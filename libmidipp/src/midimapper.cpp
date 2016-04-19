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
 * \file          midimapper.cpp
 *
 *    This module provides functions for advanced MIDI/text conversions.
 *
 * \library       libmidipp
 * \author        Chris Ahlstrom
 * \date          2014-04-24
 * \updates       2016-04-19
 * \version       $Revision$
 * \license       GNU GPL
 *
 *    This module uses some of the libmidifilex callbacks of the C modules
 *    in libmidifilex/src/midicvt_base.c, but for the purposes of writing
 *    out another MIDI file, rather than a text file.  The new MIDI file
 *    has certain keys or patches remapped between General MIDI (GM)
 *    settings and the settings an old MIDI device such as the Yamaha
 *    PSS-790.
 *
 *    Conversion from MIDI to text starts with the (new) C function
 *    mftransform().
 */

#include <stdlib.h>                    /* atoi()                              */

#include <ininames.hpp>                /* label/value names for the INI files */
#include <initree.hpp>                 /* midimpp::initree                    */
#include <midicvt_globals.h>           /* nullptr, true, false, etc.          */
#include <midicvt_macros.h>            /* nullptr, true, false, etc.          */
#include <midicvt_m2m.h>               /* the basic MIDI-to-MIDI C callbacks  */
#include <midifilex.h>                 /* Mf_on and other callback pointers   */
#include <midimapper.hpp>              /* this module's functions and stuff   */

/*
 *    The following functions use the midimapper functions to modify
 *    the channel, note, or patch numbers.  These functions are registered
 *    with the libmidifilex library at run-time.
 */

EXTERN_C_DEC

static int midimap_non (int chan, int pitch, int vol);
static int midimap_noff (int chan, int pitch, int vol);
static int midimap_pressure (int chan, int pitch, int press);
static int midimap_patch (int chan, int program);
static int midimap_parameter (int chan, int control, int value);
static int midimap_pitchbend (int chan, int lsb, int msb);
static int midimap_chanpressure (int chan, int pressure);

EXTERN_C_END

namespace midipp
{

/**
 *    Principal constructor for the annotation class.
 *
 * \param value
 *    The integer value to which the incoming (key) value is to be
 *    mapped.
 *
 * \param keyname
 *    The name of the drum note or patch represented by the key value.
 *
 * \param valuename
 *    The name of the drum note or patch represented by the integer
 *    value.
 */

annotation::annotation
(
   int value,
   const std::string & keyname,
   const std::string & valuename,
   const std::string & gmname
) :
   m_value        (value),
   m_key_name     (keyname),
   m_value_name   (valuename),
   m_gm_name      (gmname),
   m_remap_count  (0)
{
   // no other code
}

/**
 *    This constructor creates an unnamed, no-change mapping object.
 *
 *    This object can be used for testing.
 */

midimapper::midimapper ()
 :
   m_file_style      (),
   m_setup_name      (),
   m_map_type        (),
   m_record_count    (0),
   m_gm_channel      (10 - 1),         // MIDI channel 10 on 0-15 scale
   m_device_channel  (10 - 1),         // ditto
   m_filter_channel  (NOT_ACTIVE),
   m_extraction_on   (false),
   m_rejection_on    (false),
   m_map_reversed    (false),
   m_drum_map        (),
   m_patch_map       (),
   m_channel_map     (),
   m_is_valid        (false)
{
   // no code
}

/**
 *    This constructor creates an named mapping object, reading the
 *    mapping from an INI file.
 *
 *    This object can be used for testing.
 *
 * \param name
 *    Provides a handy name to refer to when reading the output of this
 *    object.
 *
 * \param filespec
 *    Provides the full file-path specification of an INI-style file to be
 *    read.  See the example files in the <code>tests/inifiles</code>
 *    directory.
 *
 * \param reverse_it
 *    Indicates if the numeric values are to be swapped.  Thus, applying
 *    the MIDI mapper operation for a given INI file, and then reversing
 *    it, should yield something like the original MIDI file.
 *
 * \param filter_channel
 *    Provides a single channel to be treated specially, being either
 *    saved alone, or dropped from the MIDI file.  This value should range
 *    from 1 to 16, and is converted internally to the 0 to 15 range.  The
 *    default value is NOT_ACTIVE (-1), which means no channel filtering
 *    is to be done.
 *
 * \param reject_it
 *    If the \a filter_channel is valid, this boolean parameter indicates
 *    that the channel is to be dropped from the output MIDI file.  If
 *    false (the default value), the channel is the only one kept in the
 *    output MIDI file.
 */

midimapper::midimapper
(
   const std::string & name,
   const std::string & filespec,
   bool reverse_it,
   int filter_channel,
   bool reject_it
) :
   m_file_style      (),
   m_setup_name      (name),
   m_map_type        (),
   m_record_count    (0),
   m_gm_channel      (NOT_ACTIVE),
   m_device_channel  (NOT_ACTIVE),
   m_filter_channel
   (
      (filter_channel > 0 && filter_channel <= 16) ?
         filter_channel - 1 : NOT_ACTIVE
   ),
   m_extraction_on
   (
      filter_channel > 0 && filter_channel <= 16
   ),
   m_rejection_on    (m_extraction_on && reject_it),
   m_map_reversed    (reverse_it),
   m_drum_map        (),
   m_patch_map       (),
   m_channel_map     (),
   m_is_valid        (true)
{
   if (! filespec.empty())
      m_is_valid = read_maps(filespec);

   if (m_rejection_on)
      m_extraction_on = false;
}

/**
 *    Creates an integer mapping map from the given INI file.
 *
 *    An initree object is temporarily created in order to read the file
 *    and provide the settings we need.  We don't need to use
 *    shared-pointers, since this function is written in a safe manner.
 *
 *    This function iterates through the sections.  Note than an unnamed
 *    section is treated differently:  The "gm-channel" and
 *    "dev-channel" values are looked up and set.
 *
 *    Inserts the pair of (gm_note, gm_device_note), depending on the
 *    m_map_reversed flag.  If false (the normal case), then we want the
 *    key to be the gm_note, and the value to be the gm_device_note value
 *    that best matches the device's original sound.  That is, given a
 *    MIDI file scored for a non-GM device, we want the notes to be
 *    remapped to the notes that GM needs to make a the sound intended by
 *    the device-note.
 *
 *    If the m_map_reversed flag is true, then we want to take a GM MIDI
 *    file and re-map it for the corresponding device note.
 *
 *    std::map::insert<> returns a pair of values:  an iterator into the
 *    map, and a boolean value for success/failure.  If the insertion
 *    fails, the pair is already in the map.  We tell the user about this,
 *    but do not treat it as a fatal error.
 *
 * \param filename
 *    Provides the full path specification of the file to be read.
 *
 * \return
 *    Returns true if the operation succeeded.
 */

bool
midimapper::read_maps (const std::string & filename)
{
   initree * itree = new (std::nothrow) initree(m_setup_name, filename);
   bool result = not_nullptr(itree);
   if (result)
   {
      const initree & it = *itree;
      result = read_unnamed_section(it);
      if (result)
         result = read_channel_section(it);

      if (result)
      {
         m_drum_map.clear();
         m_patch_map.clear();
         for (initree::const_iterator ici = it.begin(); ici != it.end(); ++ici)
         {
            const initree::Section & section = ici->second;
            if (! section.name().empty())       // if not the unnamed section
            {
               std::string gmvaluename;
               std::string devvaluename;
               std::string gmname;
               int gmvalue = NOT_ACTIVE;
               int devvalue = NOT_ACTIVE;
               gm_ini_section_t sect = INI_SECTION_UNKNOWN;
               int c = section.name().compare
               (
                  0, DRUM_SECTION.size(), DRUM_SECTION
               );
               if (c == 0)
               {
                  sect = INI_SECTION_DRUM;
               }
               else
               {
                  c = section.name().compare
                  (
                     0, PATCH_SECTION.size(), PATCH_SECTION
                  );
                  if (c == 0)
                     sect = INI_SECTION_PATCH;
                  else
                  {
                     if (section.name() != CHANNEL_SECTION)
                     {
                        warnprintf
                        (
                           "! Unknown section name '%s'\n",
                           section.name().c_str()
                        );
                     }
                     continue;
                  }
               }
               initree::Section::const_iterator sci;
               if (sect == INI_SECTION_DRUM)
               {
                  sci = section.find(DRUM_LABEL_GM_NOTE);
                  if (sci != section.end())
                     gmvalue = atoi(sci->second.c_str());

                  sci = section.find(DRUM_LABEL_DEV_NOTE);
                  if (sci != section.end())
                     devvalue = atoi(sci->second.c_str());

                  sci = section.find(DRUM_LABEL_GM_NAME);
                  if (sci != section.end())
                     gmvaluename = sci->second;

                  sci = section.find(DRUM_LABEL_DEV_NAME);
                  if (sci != section.end())
                     devvaluename = sci->second;

                  sci = section.find(DRUM_LABEL_GM_EQUIV);
                  if (sci != section.end())
                     gmname = sci->second;
               }
               else if (sect == INI_SECTION_PATCH)
               {
                  sci = section.find(PATCH_LABEL_GM_PATCH);
                  if (sci != section.end())
                     gmvalue = atoi(sci->second.c_str());

                  sci = section.find(PATCH_LABEL_DEV_PATCH);
                  if (sci != section.end())
                     devvalue = atoi(sci->second.c_str());

                  sci = section.find(PATCH_LABEL_GM_NAME);
                  if (sci != section.end())
                     gmvaluename = sci->second;

                  sci = section.find(PATCH_LABEL_DEV_NAME);
                  if (sci != section.end())
                     devvaluename = sci->second;

                  sci = section.find(PATCH_LABEL_GM_EQUIV);
                  if (sci != section.end())
                     gmname = sci->second;
               }
               result = active(gmvalue, devvalue);
               if (result)
               {
                  if (m_map_reversed)
                  {
                     int temp = gmvalue;
                     gmvalue = devvalue;
                     devvalue = temp;
                     std::string stemp = gmvaluename;
                     gmvaluename = devvaluename;
                     devvaluename = stemp;
                     gmname = devvaluename;
                  }

                  annotation an(devvalue, gmvaluename, devvaluename, gmname);
                  midimap_pair p = std::make_pair(gmvalue, an);
                  midimap_result resultpair;
                  if (sect == INI_SECTION_DRUM)
                     resultpair = m_drum_map.insert(p);
                  else if (sect == INI_SECTION_PATCH)
                     resultpair = m_patch_map.insert(p);

                  result = resultpair.second;
                  if (result)
                  {
                     ++m_record_count;
                  }
                  else
                  {
                     char temp[80];
                     snprintf
                     (
                        temp, sizeof temp, "value pair (%d, %d) not inserted",
                        gmvalue, devvalue
                     );
                     errprint(temp);
                     result = true;    /* failure to insert is not fatal      */
                  }
               }
               else
               {
                  errprint("missing value value");
                  break;
               }
            }
         }
         if (result && m_map_reversed)
         {
            int temp = m_gm_channel;
            m_gm_channel = m_device_channel;
            m_device_channel = temp;
         }
      }
      delete itree;
      if (midicvt_option_debug())
         show_maps("read_maps()", *this);
   }
   if (m_setup_name == GM_INI_TESTING)
   {
      infoprint("testing, so ending before file processing");
      result = false;
   }
   return result;
}

/**
 *    This function searches through the unnamed section at the top of the
 *    file.  Note than an unnamed section is treated differently:  The
 *    "gm_channel" and "device_channel" values are looked up and set.
 *
 * \change ca 2016-04-17
 *    We no longer error and break if an unnamed-section value tag is not
 *    present.  It should merely disable the functionality of that tag,
 *    not the whole remapping file.  Note that this functionality was
 *    never part of the unit test, and we ought to fix that lack at some
 *    point.
 *
 * \param it
 *    Provides the tree of INI name/value pairs to analyze.
 *
 * \return
 *    Returns true if the whole tree has any elements.  If it has an
 *    unnamed section, then 'true' also indicates that no errors occurred
 *    in processing those values.
 */

bool
midimapper::read_unnamed_section (const initree & it)
{
   bool result = it.size() > 0;
   if (result)
   {
      /*
       * Can we speed this up by calling find("")?
       */

      for (initree::const_iterator ici = it.begin(); ici != it.end(); ++ici)
      {
         const initree::Section & section = ici->second;
         if (section.name().empty())               // if the unnamed section
         {
            initree::Section::const_iterator sci;
            sci = section.find(GM_INI_GM_CHANNEL);
            if (sci != section.end())
            {
               m_gm_channel = atoi(sci->second.c_str()) - 1;
            }
            else
            {
               /*
                * \change ca 2016-04-17
                * result = false;
                * break;
                */
            }
            sci = section.find(GM_INI_DEV_CHANNEL);
            if (sci != section.end())
            {
               m_device_channel = atoi(sci->second.c_str()) - 1;
            }
            else
            {
               /*
                * \change ca 2016-04-17
                * result = false;
                * break;
                */
            }
            sci = section.find(GM_INI_FILE_STYLE);
            if (sci != section.end())
               m_file_style = sci->second;

            if (m_setup_name != GM_INI_TESTING)    // a special value
            {
               sci = section.find(GM_INI_SETUP_NAME);
               if (sci != section.end())
                  m_setup_name = sci->second;
            }
            sci = section.find(GM_INI_MAP_TYPE);
            if (sci != section.end())
               m_map_type = sci->second;

            sci = section.find(GM_INI_EXTRACT_CHANNEL);
            if (sci != section.end())
            {
               if (sci->second != GM_INI_NO_VALUE)
               {
                  m_filter_channel = atoi(sci->second.c_str());
                  if (m_filter_channel >= 0 && m_filter_channel < 16)
                  {
                     m_extraction_on = true;
                     m_rejection_on  = false;
                  }
               }
            }
            sci = section.find(GM_INI_REJECT_CHANNEL);
            if (sci != section.end())
            {
               if (sci->second != GM_INI_NO_VALUE)
               {
                  m_filter_channel = atoi(sci->second.c_str());
                  if (m_filter_channel >= 0 && m_filter_channel < 16)
                  {
                     m_extraction_on = false;
                     m_rejection_on  = true;
                  }
               }
            }
            sci = section.find(GM_INI_REVERSE);
            if (sci != section.end())
               m_map_reversed = sci->second == "true";

            /*
             * LATER:
             *
             * sci = section.find(GM_INI_TESTING);
             * if (sci != section.end())
             *    m_map_testing = sci->second == "true";
             */

            break;
         }
      }
   }
   return result;
}

/**
 *    This function searches through the Channel section, if it exists.
 *
 *    Each entry must be of the following format, or an error will occur:
 *
\verbatim
         ch_01 = 10
\endverbatim
 *
 * \param it
 *    Provides the tree of INI name/value pairs to analyze.
 *
 * \return
 *    Returns true if there was no channel section to bother with, or it
 *    there was, and the processing of it succeeded.  This function
 *    assumes the read_unnamed_section() function was already called, and
 *    succeeded.
 */

bool
midimapper::read_channel_section (const initree & it)
{
   bool result = true;
   m_channel_map.clear();
   initree::const_iterator ici = it.find(CHANNEL_SECTION);
   if (ici != it.end())
   {
      const initree::Section & section = ici->second;
      result = section.name() == CHANNEL_SECTION;  // double-check
      if (result)
      {
         size_t sz = CHANNEL_TOKEN.size();
         initree::Section::const_iterator sci;
         for (sci = section.begin(); sci != section.end(); sci++)
         {
            const std::string & name = sci->first;
            result = name.compare(0, sz, CHANNEL_TOKEN) == 0;
            if (result)
            {
               std::string::size_type p = name.find_first_of("0123456789");
               result = p != std::string::npos;
               if (result)
               {
                  const char * number = name.c_str() + p;
                  int inchannel = atoi(number);
                  result = inchannel > 0 && inchannel <= 16;   // need constant
                  if (result)
                  {
                     int outchannel = atoi(sci->second.c_str());
                     result = outchannel > 0 && outchannel <= 16;
                     if (result)
                     {
                        --inchannel;
                        --outchannel;
                     }
                     if (result)
                     {
                        if (m_map_reversed)
                        {
                           int temp = inchannel;
                           inchannel = outchannel;
                           outchannel = temp;
                        }

                        std::pair<int, int> p = std::make_pair
                        (
                           inchannel, outchannel
                        );
                        intmap_result respair = m_channel_map.insert(p);
                        result = respair.second;
                     }
                  }
               }
               if (! result)
               {
                  errprint("Invalid specification in channel section");
                  break;
               }
            }
         }
      }
   }
   return result;
}

/**
 *    Changes a note value based on the note-mapping that was provided.
 *
 *    Since the reversal option for note values is set up during the
 *    construction of the note map, the m_map_reversed flag does not
 *    need to be checked in this function.
 *
 * \param channel
 *    Provides the channel for the input note.  Only if the channel
 *    matches the one channel to be remapped, will repitching occur.
 *
 * \param input
 *    Provides the value of the input note, as obtained from the input
 *    MIDI file.
 *
 * \return
 *    Returns the output note value corresponding to the input note value.
 *    If the input note is not found in the map, it is returned unaltered.
 *    The map is empty if an empty INI file-name was passed to the
 *    constructor.
 */

int
midimapper::repitch (int channel, int input)
{
   if (channel == m_device_channel)
   {
      iterator ni = m_drum_map.find(input);
      if (ni != m_drum_map.end())
      {
         input = ni->second.value();
         ni->second.increment_count();
      }
   }
   return input;
}

/**
 *    Changes a channel value based on the channel-mapping that was
 *    provided.
 *
 *    Note that there are actually two possible channel-mappings. The
 *    first one is based on the values of gm-channel and dev-channel, and
 *    is used to map drum tracks (e.g. from channel 16 to channel 10).
 *    The second mapping allows more general mapping, and is applied only
 *    if the channel isn't affected by the drum mapping.  The second form
 *    is specified by the "[Channels]" section.
 *
 *    The reversal option is accounted for at setup time for both of these
 *    types of mappings, as was done for the drum (note) value mappings.
 *
 *    If the m_filter_channel value is valid (between 0 and 15), and the
 *    input channel does <i> not </i> match this value, then -1
 *    (midimapper::NOT_ACTIVE) is returned to indicate to reject the
 *    event.
 *
 * \param channel
 *    Provides the value of the input channel, as obtained from the input
 *    MIDI file.
 *
 * \return
 *    Returns one of the following values:
 *       -  The output channel value corresponding to the input channel
 *          value if the input channel matches m_gm_channel, and
 *          m_gm_channel is set.
 *       -  The original input channel if the input channel is not one
 *          that matches m_gm_channel in the channel mapping, or if
 *          m_gm_channel is not activated (i.e. set to -1 [NOT_ACTIVE]).
 *       -  NOT_ACTIVE (-1) is returned if the input channel doesn't match
 *          m_filter_channel and m_extraction_on is true.  In other
 *          words, only one channel is allowed to be output
 *       -  NOT_ACTIVE (-1) is returned if the input channel does match
 *          m_filter_channel and m_rejection_on is true. In other words,
 *          one channel is systematically rejected from the output.
 */

int
midimapper::rechannel (int channel)
{
   if (m_extraction_on && channel != m_filter_channel)
   {
      return NOT_ACTIVE;
   }
   else if (m_rejection_on && channel == m_filter_channel)
   {
      return NOT_ACTIVE;
   }
   else
   {
      /**
       * Only if m_gm_channel and m_device_channel have been set are they
       * used to detect and alter the channel number of the event.  If
       * they aren't active, or if the channel isn't the drum channel to
       * be remapped, then the "[Channels]" map is checked.
       */

      if (active(m_gm_channel, m_device_channel) && channel == m_device_channel)
      {
         channel = m_gm_channel;
      }
      else
      {
         std::map<int, int>::const_iterator ci = m_channel_map.find(channel);
         if (ci != m_channel_map.end())
            channel = ci->second;
      }
      return channel;
   }
}

/**
 *    Changes a program/patch value based on the patch-mapping that was
 *    provided.
 *
 *    Since the reversal option for note values is set up during the
 *    construction of the note map, the m_map_reversed flag does not
 *    need to be checked in this function.
 *
 * \param program
 *    Provides the value of the input patch, as obtained from the input
 *    MIDI file.
 *
 * \return
 *    Returns the output patch value corresponding to the input patch value.
 *    If the input patch is not found in the map, it is returned unaltered.
 *    The map is empty if an empty INI file-name was passed to the
 *    constructor.
 */

int
midimapper::repatch (int program)
{
   iterator ni = m_patch_map.find(program);
   if (ni != m_patch_map.end())
   {
      program = ni->second.value();
      ni->second.increment_count();
   }
   return program;
}

/**
 *    Writes out the contents of the pitch-map container out to stderr.
 *    We can't write to stdout because that is often redirected to a file.
 *    This implementation is a for_each style of looping through each
 *    container.
 *
 * \param tag
 *    Identifies the object in the human-readable output.
 *
 * \param container
 *    The midimapper through which iteration is done for showing.
 *
 * \param full_output
 *    Defaults to true, which means to show everything.  If false,
 *    map entries not used in the conversion are not shown.
 */

void
show_maps
(
   const std::string & tag,
   const midipp::midimapper & container,
   bool full_output
)
{
   fprintf
   (
      stderr,
      "midipp::midimap '%s':\n"
      "   INI style:               '%s'\n"
      "   Setup name:              '%s'\n"
      "   Map type:                '%s'\n"
      "   Record count:             %d\n"
      "   GM drum channel:          %d\n"
      "   Device drum channel:      %d\n"
      "   Filter channel:           %d\n"
      "   Extract channel:          %s\n"
      "   Reject channel:           %s\n"
      "   Reverse map:              %s\n"
      "Drum/note map:\n"
      "   Size:                     %d\n"
      ,
      tag.c_str(),
      container.file_style().c_str(),
      container.setup_name().c_str(),
      container.map_type().c_str(),
      container.record_count(),
      container.gm_channel(),             // accessor adds 1 automatically
      container.device_channel(),         // ditto
      container.filter_channel(),         // tritto, unless NOT_ACTIVE
      bool_to_cstr(container.extract()),
      bool_to_cstr(container.reject()),
      bool_to_cstr(container.map_reversed()),
      int(container.drum_map().size())
   );
   if (! container.drum_map().empty())
   {
      const char * fpformat = 
         "%4d: Note  #%2d  %-24s ---> #%2d  %-24s (%s)\n";

      int testcounter = 0;
      midipp::midimapper::const_iterator mi = container.drum_map().begin();
      for ( ; mi != container.drum_map().end(); ++mi)
      {
         std::string gmname;
         std::string devname;
         std::string equivname;
         int gm;
         int dev;
         bool show_it = full_output;
         if (! full_output)
            show_it = mi->second.count() > 0;

         if (show_it)
         {
            /*
             * Not sure why we're reversing these here.  They were already
             * reversed when the containers were created.
             */

#if 0
            if (container.map_reversed())
            {
               dev = mi->first;
               gm = mi->second.value();
               gmname = mi->second.value_name();
            }
            else
            {
#endif
            gm = mi->first;
            dev = mi->second.value();
            gmname = mi->second.key_name();
            devname = mi->second.value_name();
            equivname = mi->second.gm_name();
#if 0
            }
#endif
            fprintf
            (
               stderr, fpformat,
               mi->second.count(), gm, gmname.c_str(),
               dev, devname.c_str(),
               equivname.c_str()
            );
            ++testcounter;
         }
      }
      fprintf(stderr, "   %d drum records dumped\n", testcounter);
   }
   fprintf
   (
      stderr,
      "Patch/program map:\n"
      "   Size:                     %d\n"
      ,
      int(container.patch_map().size())
   );
   if (! container.patch_map().empty())
   {
      const char * fpformat = 
         "%4d: Patch #%3d %-24s ---> #%3d %-24s (%s)\n";

      int testcounter = 0;
      midipp::midimapper::const_iterator mi = container.patch_map().begin();
      for ( ; mi != container.patch_map().end(); ++mi)
      {
         std::string gmname;
         std::string devname;
         std::string equivname;
         int gm;
         int dev;
         bool show_it = full_output;
         if (! full_output)
            show_it = mi->second.count() > 0;

         if (show_it)
         {
            /*
             * Not sure why we're reversing these here.  They were already
             * reversed when the containers were created.
             */
#if 0
            if (container.map_reversed())
            {
               gm = mi->second;
               dev = mi->first;
            }
            else
            {
#endif
               gm = mi->first;
               dev = mi->second.value();
               gmname = mi->second.key_name();
               devname = mi->second.value_name();
               equivname = mi->second.gm_name();
#if 0
            }
#endif
            fprintf
            (
               stderr, fpformat,
               mi->second.count(), gm, gmname.c_str(),
               dev, devname.c_str(),
               equivname.c_str()
            );
            ++testcounter;
         }
      }
      fprintf(stderr, "   %d patch records dumped\n", testcounter);
   }
   if (full_output)
   {
      fprintf
      (
         stderr,
         "- Channel map:\n"
         "-    Size:                     %d\n"
         ,
         int(container.channel_map().size())
      );
      if (! container.channel_map().empty())
      {
         const char * fpformat = "   Channel #%2d ---> #%2d\n" ;

         /*
          * Currently the channel map is never reversed.
          *
          * "-    Out channel #%d <--- In  #%d\n" :
          */

         int testcounter = 0;
         std::map<int, int>::const_iterator mi = container.channel_map().begin();
         for ( ; mi != container.channel_map().end(); ++mi)
         {
            int in;
            int out;
#if 0
            if (container.map_reversed())
            {
               in = mi->second;
               out = mi->first;
            }
            else
            {
#endif
               in = mi->first;
               out = mi->second;
#if 0
            }
#endif
            fprintf(stderr, fpformat, in + 1, out + 1);
            ++testcounter;
         }
         fprintf(stderr, "   %d channel records dumped\n", testcounter);
      }
   }
}

}                 // namespace midipp

/**
 *    Holds the single pointer to the midipp::midimapper object the caller
 *    created.  Since we're integrating with C code, we can't really use
 *    more than one object at a time anyway.
 */

static midipp::midimapper * gs_singleton_midimapper = nullptr;

/**
 *    Hooks the midipp::midimapper object provided to the global pointer
 *    for such objects, to allow the C routines to be able to use the
 *    features of the MIDI mapper.
 *
 *    This function calls midicvt_initfuncs_m2m() to set up the
 *    MIDI-to-MIDI callbacks, but then overrides some of them with the
 *    C call backs in the rest of this module.
 *
 * \param mm
 *    Provides the MIDI mapper object to be used.
 */

void
midimap_init (midipp::midimapper & mm)
{
   gs_singleton_midimapper = &mm;
   midicvt_initfuncs_m2m();
   Mf_on             = midimap_non;
   Mf_off            = midimap_noff;
   Mf_pressure       = midimap_pressure;
   Mf_program        = midimap_patch;
   Mf_parameter      = midimap_parameter;
   Mf_pitchbend      = midimap_pitchbend;
   Mf_chanpressure   = midimap_chanpressure;
}

/**
 *    This static C function uses the singleton midipp::midimapper object
 *    to remap the channel and pitch values, and passes them to the
 *    corresponding callback function defined in the midicvt_m2m.c module.
 *
 *    If the midipp::midimapper object is not hooked in, then the values
 *    are passed unchanged.  This then yields the same functionality as
 *    running the following C program from this library:
 *
\verbatim
      $ ./midicvt --m2m in.mid out.mid
\endverbatim
 *
 * \param chan
 *    Provides the channel number to write.
 *
 * \param pitch
 *    Provides the MIDI note number to write.
 *
 * \param vol
 *    Provides the MIDI note velocity to write.
 *
 * \return
 *    Returns the return-value of the corresponding midicvt_m2m callback
 *    function.
 */

static int
midimap_non (int chan, int pitch, int vol)
{
   int result = not_nullptr(gs_singleton_midimapper);
   int chan_2;
   int pitch_2;
   if (result)
   {
      chan_2 = gs_singleton_midimapper->rechannel(chan);
      pitch_2 = gs_singleton_midimapper->repitch(chan, pitch);
   }
   else
   {
      chan_2 = chan;
      pitch_2 = pitch;
   }
   if (midipp::midimapper::active(chan_2, pitch_2))
      result = m2m_non(chan_2, pitch_2, vol);
   else
      result = 0;

   return result;
}

/**
 *    This static C function uses the singleton midipp::midimapper object
 *    to remap the channel and pitch values, and passes them to the
 *    corresponding callback function defined in the midicvt_m2m.c module.
 *
 *    If the midipp::midimapper object is not hooked in, then the values
 *    are passed unchanged.
 *
 * \param chan
 *    Provides the channel number to write.
 *
 * \param pitch
 *    Provides the MIDI note number to write.
 *
 * \param vol
 *    Provides the MIDI note velocity to write.
 *
 * \return
 *    Returns the return-value of the corresponding midicvt_m2m callback
 *    function.
 */

static int
midimap_noff (int chan, int pitch, int vol)
{
   int result = not_nullptr(gs_singleton_midimapper);
   int chan_2;
   int pitch_2;
   if (result)
   {
      chan_2 = gs_singleton_midimapper->rechannel(chan);
      pitch_2 = gs_singleton_midimapper->repitch(chan, pitch);
   }
   else
   {
      chan_2 = chan;
      pitch_2 = pitch;
   }
   if (midipp::midimapper::active(chan_2, pitch_2))
      result = m2m_noff(chan_2, pitch_2, vol);
   else
      result = 0;

   return result;
}

/**
 *    This static C function uses the singleton midipp::midimapper object
 *    to remap the channel and pitch values, and passes them to the
 *    corresponding callback function defined in the midicvt_m2m.c module.
 *
 *    If the midipp::midimapper object is not hooked in, then the values
 *    are passed unchanged.
 *
 * \param chan
 *    Provides the channel number to write.
 *
 * \param pitch
 *    Provides the MIDI note number to write.
 *
 * \param pressure
 *    Provides the MIDI polyphonic key pressure value to write.
 *
 * \return
 *    Returns the return-value of the corresponding midicvt_m2m callback
 *    function.
 */

static int
midimap_pressure (int chan, int pitch, int pressure)
{
   int result = not_nullptr(gs_singleton_midimapper);
   int chan_2;
   int pitch_2;
   if (result)
   {
      chan_2 = gs_singleton_midimapper->rechannel(chan);
      pitch_2 = gs_singleton_midimapper->repitch(chan, pitch);
   }
   else
   {
      chan_2 = chan;
      pitch_2 = pitch;
   }
   if (midipp::midimapper::active(chan_2, pitch_2))
      result = m2m_pressure(chan_2, pitch_2, pressure);
   else
      result = 0;

   return result;
}

/**
 *    Remaps the channel and the program (patch) number, and passes them
 *    to the corresponding callback function defined in the midicvt_m2m.c
 *    module.
 *
 *    If the midipp::midimapper object is not hooked in, then the values
 *    are passed unchanged.
 *
 * \param chan
 *    Provides the channel number to write.
 *
 * \param patch
 *    Provides the patch/program number to write.
 *
 * \return
 *    Returns the return-value of the corresponding midicvt_m2m callback
 *    function.
 */

static int
midimap_patch (int chan, int patch)
{
   int result = not_nullptr(gs_singleton_midimapper);
   int chan_2;
   int patch_2;
   if (result)
   {
      chan_2 = gs_singleton_midimapper->rechannel(chan);
      patch_2 = gs_singleton_midimapper->repatch(patch);
   }
   else
   {
      chan_2 = chan;
      patch_2 = patch;
   }
   if (midipp::midimapper::active(chan_2, patch_2))
      result = m2m_program(chan_2, patch_2);
   else
      result = 0;

   return result;
}

/**
 *    For filtering parameter/control messages.
 *
 * \param chan
 *    Provides the channel number to write.
 *
 * \param control
 *    Provides the MIDI controller number to write.
 *
 * \param value
 *    Provides the MIDI controller parameter value to write.
 *
 * \return
 *    Returns the result of the m2m_parameter() call.  If the channel is
 *    not active, then 0 (false) is returned.
 */

static int
midimap_parameter (int chan, int control, int value)
{
   int result = not_nullptr(gs_singleton_midimapper);
   int chan_2;
   if (result)
      chan_2 = gs_singleton_midimapper->rechannel(chan);
   else
      chan_2 = chan;

   if (midipp::midimapper::active(chan_2))
      result = m2m_parameter(chan_2, control, value);
   else
      result = 0;

   return result;
}

/**
 *    For filtering pitchbend messages.
 *
 * \param chan
 *    Provides the channel number to write.
 *
 * \param lsb
 *    Provides the least-significant bits of the MIDI pitch-wheel value to
 *    write.
 *
 * \param msb
 *    Provides the most-significant bits of the MIDI pitch-wheel value to
 *    write.
 *
 * \return
 *    Returns the result of the m2m_pitchbend() call.  If the channel is
 *    not active, then 0 (false) is returned.
 */

static int
midimap_pitchbend (int chan, int lsb, int msb)
{
   int result = not_nullptr(gs_singleton_midimapper);
   int chan_2;
   if (result)
      chan_2 = gs_singleton_midimapper->rechannel(chan);
   else
      chan_2 = chan;

   if (midipp::midimapper::active(chan_2))
      result = m2m_pitchbend(chan_2, lsb, msb);
   else
      result = 0;

   return result;
}

/**
 *    For filtering channel-pressure messages.
 *
 * \param chan
 *    Provides the channel number to write.
 *
 * \param pressure
 *    Provides the patch/program number to write.
 *
 * \return
 *    Returns the result of the m2m_chanpressure() call.  If the channel is
 *    not active, then 0 (false) is returned.
 */

static int
midimap_chanpressure (int chan, int pressure)
{
   int result = not_nullptr(gs_singleton_midimapper);
   int chan_2;
   if (result)
      chan_2 = gs_singleton_midimapper->rechannel(chan);
   else
      chan_2 = chan;

   if (midipp::midimapper::active(chan_2))
      result = m2m_chanpressure(chan_2, pressure);
   else
      result = 0;

   return result;
}

/*
 * midimapper.cpp
 *
 * vim: sw=3 ts=3 wm=8 et ft=cpp
 */
