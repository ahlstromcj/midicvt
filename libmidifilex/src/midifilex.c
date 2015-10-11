/* $Id: midifile.c,v 1.4 1991/11/17 21:57:26 piet Rel piet $ */

/*
 * midifile 1.11
 *
 * Read and write a MIDI file.  Externally-assigned function pointers are
 * called upon recognizing things in the file.
 *
 * Original release by Tim Thompson, tjt@twitch.att.com
 *
 * June 1989 - Added writing capability, M. Czeiszperger.
 *
 * Oct 1991 - Modifications by Piet van Oostrum <piet@cs.ruu.nl>:
 *    Changed identifiers to be 7 char unique.
 *    Added sysex write capability (mf_w_sysex_event)
 *    Corrected a bug in writing of tempo track
 *    Added code to implement running status on write
 *    Added check for meta end of track insertion
 *    Added a couple of include files to get proper int=short compilation
 *
 * Nov 1991 - Piet van Oostrum <piet@cs.ruu.nl>
 *    mf_w_tempo needs a delta time parameter otherwise the tempo cannot
 *      be changed during the piece.
 *
 * Apr 1993 - Piet van Oostrum <piet@cs.ruu.nl>
 *    decl of malloc replaced by #include <malloc.h>
 *    readheader() declared void.
 *
 * Aug 1993 - Piet van Oostrum <piet@cs.ruu.nl>
 *    sequencer_specific in midifile.h was wrong
 *
 *          The file format implemented here is called
 *          Standard MIDI Files, and is part of the Musical
 *          instrument Digital Interface specification.
 *          The spec is avaiable from:
 *
 *               International MIDI Association
 *               5316 West 57th Street
 *               Los Angeles, CA 90056
 *
 *          An in-depth description of the spec can also be found
 *          in the article "Introducing Standard MIDI Files", published
 *          in Electronic Musician magazine, April, 1989.
 *
 * Apr 2014 - Chris Ahlstrom <>
 *    Convert to modern C conventions, reorganize the code, improve the
 *    documentation, fix bugs, add minor enhancements.  Also added a new
 *    version of mfread(), called mftransform(), to better handle direct
 *    MIDI-to-MIDI conversions.
 */

/**
 * \file          midifilex.c
 *
 *    This module provides functions for handling the reading and writing
 *    of MIDI files.
 *
 * \library       midicvt application
 * \author        Other authors (see below), with modifications by Chris
 *                Ahlstrom,
 * \date          2014-04-08
 * \updates       2015-10-11
 * \version       $Revision$
 * \license       GNU GPL
 *
 *    This file is based on midifile.c of the midifile library written by
 *    Tim Thompson (tjt@blink.att.com) and updated by Michael Czeiszperger
 *    (mike@pan.com).  However, there were some bugs in the write code and
 *    Piet (see below) added some features he needed. He also changed some
 *    of the names to cope with the 7-character limit for external
 *    identifiers in the Sozobon compiler. He made an updated version of
 *    the library available, and split the read and write portions.
 *    However, he ended up with two midifile.c modules, each having
 *    differences, including differences that could cause bugs.
 *
 *    Chris (see below) updated the code to more modern C conventions
 *    (there's still more to do, such as adding more "const" items) and
 *    tried to improved the readability of the code).  Also got a start on
 *    adding Doxygen markup to generate a more readable reference manual.
 *    Also added a Mf_report function pointer, to be enabled for easier
 *    diagnosis of MIDI conversion.
 *
\verbatim
      Piet van Oostrum, Dept of Computer Science, Utrecht University,
      P.O. Box 80.089, 2508 TB Utrecht, The Netherlands
      email: piet@cs.ruu.nl
\endverbatim
 *
\verbatim
      Chris Ahlstrom, Charleston SC, USA
      email: ahlstromcj@gmail.com
\endverbatim
 *
 * Discussion:
 *
 *    For converting a MIDI file to text, the code calls mfread(), which
 *    reads the MIDI file and then calls the set of callbacks (in the
 *    midicvt_base.c module) that convert the output to human-readable
 *    form.
 *
 *    For converting the human-readable text to MIDI, the code calls
 *    midicvt_compile(), which reads the header text.  Then it calls
 *    mfwrite(), which calls essentially only the my_writetrack()
 *    callback.  That callback knows how to parse the human-readable data
 *    and generate MIDI output for it.
 *
 *    When converting directly from MIDI to MIDI, we have two choices:
 *
 *       -  Call mfread(), replacing the text-output callbacks in
 *          midicvt_base.c with the MIDI-output callbacks in
 *          midicvt_m2m.c.  This has some minor issues.
 *       -  Call readheader() to get the initial information. Then call
 *          mfwrite() with a new write-track routine.  However, this
 *          routine must also read MIDI [instead of flex data].
 *
 *    So it seems easier to use the first method, and hack it so it works
 *    correctly whether the output is human-readable or MIDI.
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "midicvt_globals.h"           /* midi_file_options...()              */
#include "midicvt_helpers.h"           /* midi_file_offset() for errors       */
#include "midifilex.h"

/**
 *    Functions to be called while processing and writing the MIDI file.
 */

int (* Mf_getc) (void)                          = nullptr;
int (* Mf_error) (const char *)                 = nullptr;
int (* Mf_report) (const char *)                = nullptr;
int (* Mf_header) (int, int, int)               = nullptr;
int (* Mf_starttrack) (void)                    = nullptr;
int (* Mf_endtrack)
(
   long header_offset,
   unsigned long tracksize
) = nullptr;
int (* Mf_on) (int, int, int)                   = nullptr;
int (* Mf_off) (int, int, int)                  = nullptr;
int (* Mf_pressure) (int, int, int)             = nullptr;
int (* Mf_parameter) (int, int, int)            = nullptr;
int (* Mf_pitchbend) (int, int, int)            = nullptr;
int (* Mf_program) (int, int)                   = nullptr;
int (* Mf_chanpressure) (int, int)              = nullptr;
int (* Mf_sysex) (int, char *)                  = nullptr;
int (* Mf_arbitrary) (int, char *)              = nullptr;
int (* Mf_metamisc) (int, int, char *)          = nullptr;
int (* Mf_seqnum) (short int)                   = nullptr;
int (* Mf_eot) (void)                           = nullptr;
int (* Mf_smpte) (int, int, int, int, int)      = nullptr;
int (* Mf_tempo) (long)                         = nullptr;
int (* Mf_timesig) (int, int, int, int)         = nullptr;
int (* Mf_keysig) (int, int)                    = nullptr;
int (* Mf_sqspecific) (int, char *)             = nullptr;
int (* Mf_text) (int, int, char *)              = nullptr;
int (* Mf_putc) (unsigned char)                 = nullptr;
int (* Mf_wtrack) (void)                        = nullptr;
int (* Mf_wtempotrack) (void)                   = nullptr;

/**
 *    1 => continue'ed system exclusives are not collapsed.
 */

int Mf_nomerge = 0;

/**
 *    Current time in delta-time units.
 */

long Mf_currtime = 0L;

/*
 * private stuff
 */

static long s_Mf_toberead = 0L;
static long s_Mf_numbyteswritten = 0L;

/**
 *    Reports an error, then calls Mf_error if the Mf_error callback has
 *    been assigned, then exits with an error-code of 1.
 *
 * \param s
 *    Provides the error message.
 */

static void
mferror (char * s)
{
   fprintf
   (
      stderr, "? Error at offset %ld (0x%lx)\n",
      midi_file_offset(), midi_file_offset()
   );
   if (Mf_error)
       (void) (*Mf_error)(s);

   exit(1);
}

/**
 *    Reports an information message.  Useful in debugging.  To enable it,
 *    simply set Mf_report equal to your reporting function.  In its
 *    usage inside this module, it acts very similar to the user's
 *    callback functions.  This makes it easy to compare the user's output
 *    to what was actually encountered in the MIDI file.
 *
 * \param s
 *    Provides the error message.
 */

static void
mfreport (char * s)
{
   if (Mf_report)
      (void) (*Mf_report)(s);
}

/**
 * \return
 *    Returns true if the Mf_report function is enabled.
 */

static cbool_t
mfreportable (void)
{
   return not_nullptr(Mf_report) ? true : false ;
}

/**
 *    Provides an error report for a bad byte.
 *
 *    Since mferror() is called, the application will exit.
 *
 * \param c
 *    Provides the bad byte, in integer format.
 */

static void
badbyte (int c)
{
    char buff[32];
    snprintf(buff, sizeof(buff), "unexpected byte reading track: 0x%02x", c);
    mferror(buff);
}

/**
 *    Writes a single character.
 *
 *    If an error occurs, then this functon calls mferror() and aborts.
 *    [But mferror() will call exit().]
 *
 * \param c
 *    Provides the character to output with the Mf_putc() callback function.
 *
 * \return
 *    Returns the return value of Mf_putc().  If this value is EOF, then
 *    mferror() is called.
 */

static int
eputc (unsigned char c)
{
    int return_val;
    if (is_nullptr(Mf_putc))
    {
        mferror("Mf_putc undefined");
        return (-1);
    }
    return_val = (*Mf_putc)(c);
    if (return_val == EOF)
        mferror("error writing");

    ++s_Mf_numbyteswritten;
    return return_val;
}

/*
 *    Reads a single character using the Mf_getc() callback.
 *
 *    This function will call mferror() to abort on EOF.
 *
 * \return
 *    Returns the character read by the Mf_getc() callback.
 */

static int
egetc (void)
{
    int c = (*Mf_getc)();
    if (c == EOF)
        mferror("Premature EOF");

    s_Mf_toberead--;
    return c;
}

/**
 *    The code below allows collection of a system exclusive message of
 *    arbitrary length.  The message buffer is expanded as necessary.
 *    The only visible data/routines are msginit(), msgadd(), msg(),
 *    msgleng().
 */

static char * s_message_buffer = nullptr;

/**
 *    Holds the size of the currently allocated message buffer.
 */

static int s_message_size = 0;

/**
 *    Holds the index of next available location in s_message_buffer.
 */

static int s_message_index = 0;

/**
 *    Re-allocates the message buffer by the standard increment of 128
 *    bytes.
 *
 *    If it cannot allocate the new buffer, then mferror() is called.
 */

static void
biggermsg (void)
{
   static const int s_message_increment = 128;
   char * newmess;
   char * oldmess = s_message_buffer;
   int oldleng = s_message_size;
   s_message_size += s_message_increment;
   newmess = (char *) malloc((unsigned) (sizeof(char) * s_message_size));
   if (is_nullptr(newmess))
       mferror("biggermsg(): malloc error!");

   if (not_nullptr(oldmess))           /* copy old message to larger new one */
   {
       register char * p = newmess;
       register char * q = oldmess;
       register char * endq = &oldmess[oldleng];
       for ( ; q != endq; p++, q++ )
           *p = *q;

       free(oldmess);
   }
   s_message_buffer = newmess;         /* this is left active at exit!       */
}

/**
 *    Sets s_message_index to 0.
 */

static void
msginit (void)
{
   s_message_index = 0;
}

/**
 *    Provides a pointer to the message buffer.
 *
 * \return
 *    Returns the s_message_buffer static variable.
 */

static char *
msg (void)
{
   return s_message_buffer;
}

/**
 *    Provides the current index into the message buffer.
 *
 * \return
 *    Returns the value of s_message_index.
 */

static int
msgleng (void)
{
   return s_message_index;
}

/**
 *    Adds a character to the message buffer.
 *
 *    If necessary, it re-allocates a larger message buffer by calling
 *    biggermsg().
 *
 * \param c
 *    The character to add to the message buffer.
 */

static void
msgadd (int c)
{
   if (s_message_index >= s_message_size)
       biggermsg();

   s_message_buffer[s_message_index++] = c;
   if (mfreportable())
   {
      char temp[64];
      char k = (c >= ' ') ? (char) c : ' ' ;
      (void) snprintf
      (
         temp, sizeof(temp),
         "s_message_buffer[%d] == %c [0x%x]", s_message_index-1, k, c
      );
      mfreport(temp);
   }
}

/**
 *    Handles a channel message.
 *
 *    As noted in the description of the \a status parameter, there are
 *    various supported messages.  Depending on the type of message, one
 *    of the following callback calls will be made:
 *
\verbatim
           (void) (*Mf_off)(chan, c1, c2);
           (void) (*Mf_on)(chan, c1, c2);
           (void) (*Mf_pressure)(chan, c1, c2);
           (void) (*Mf_parameter)(chan, c1, c2);
           (void) (*Mf_program)(chan, c1);
           (void) (*Mf_chanpressure)(chan, c1);
           (void) (*Mf_pitchbend)(chan, c1, c2);
\endverbatim
 *
 * \param status
 *    Provides the type+channel byte.  The type nybble ranges from 0x80 to
 *    0xe0, and there are thus 7 values, corresponding to note-off,
 *    note-on, pressure, parameter (control), program (patch), channel
 *    pressure, and pitch-bend.  The channel nybble ranges from 0 to 0xf
 *    (i.e. MIDI channels 1 to 16).
 *
 * \param c1
 *    Provides the first byte of the messsage.  Depending on the message
 *    type, this value can be a MIDI note number, a control number (e.g.
 *    the number for a sustain message), a program/patch number, a
 *    pitch-wheel change's first value byte), or a system-common message.
 *
 * \param c2
 *    Provides the second byte of the messsage.  Depending on the message
 *    type, this value can be a velocity value or control level value, the
 *    second bye of a pitch-wheel change, or this parameter can be left
 *    unused.
 */

static void
chanmessage (int status, int c1, int c2)
{
   int chan = status & 0x0f;
   if (mfreportable())
   {
      char temp[80];
      const char * msgtype = "unknown";
      switch (status & 0xf0)
      {
      case 0x80: msgtype = "Note-off";         break;
      case 0x90: msgtype = "Note-on";          break;
      case 0xa0: msgtype = "Pressure";         break;
      case 0xb0: msgtype = "Parameter";        break;
      case 0xc0: msgtype = "Program";          break;
      case 0xd0: msgtype = "Channel-pressure"; break;
      case 0xe0: msgtype = "Pitchbend";        break;
      }
      (void) snprintf
      (
         temp, sizeof(temp), "%s ch. %d (%d [0x%x], %d [0x%x])",
         msgtype, chan, c1, c1, c2, c2
      );
      mfreport(temp);
   }
   switch (status & 0xf0)
   {
   case 0x80:

       if (Mf_off)
           (void) (*Mf_off)(chan, c1, c2);
       break;

   case 0x90:

       if (Mf_on)
           (void) (*Mf_on)(chan, c1, c2);
       break;

   case 0xa0:

       if (Mf_pressure)
           (void) (*Mf_pressure)(chan, c1, c2);
       break;

   case 0xb0:

       if (Mf_parameter)
           (void) (*Mf_parameter)(chan, c1, c2);
       break;

   case 0xc0:

       if (Mf_program)
           (void) (*Mf_program)(chan, c1);
       break;

   case 0xd0:

       if (Mf_chanpressure)
           (void) (*Mf_chanpressure)(chan, c1);
       break;

   case 0xe0:

       if (Mf_pitchbend)
           (void) (*Mf_pitchbend)(chan, c1, c2);
       break;
   }
}

/**
 *    Handle a system-exclusive message.
 *
 *    The msgleng() and msg() values are passed to the Mf_sysex() callback
 *    function.
 */

static void
sysex (void)
{
   if (mfreportable())
   {
      char temp[80];
      (void) snprintf
      (
         temp, sizeof(temp), "Sysex message of length %d [0x%x]",
         msgleng(), msgleng()
      );
      mfreport(temp);
   }
   if (Mf_sysex)
      (void) (*Mf_sysex)(msgleng(), msg());
}

/**
 *    Read a varying-length number.
 *
 *    A variable-length quantity is a MIDI number that is represented by a
 *    string of bytes where all bytes but the last have bit 7 set.  In
 *    each byte, only the 7 least-significant bits provide the numeric
 *    value.
 *
 *       -  Numbers between 0 and 127 (0x7F) are represented by a single
 *          byte.
 *       -  0x80 is represented as "0x81 0x00".
 *       -  0x0FFFFFFF (the largest number) is represented as "0xFF 0xFF
 *          0xFF 0x7F".
 *
 *    This function doesn't return the number of characters it took, it
 *    returns the value of the varying-length number.
 */

static long
readvarinum (void)
{
   int c = egetc();
   long value = c;
   if (c & 0x80)                       /* i.e. bit 7 is set                   */
   {
      value &= 0x7f;                   /* mask off bit 7                      */
      do
      {
          c = egetc();
          value = (value << 7) + (c & 0x7f);    /* mask off & add next one    */

      } while (c & 0x80);              /* while bit 7 is set                  */
   }
   return value;
}

/**
 *    Convert a quad of integers into a 32-bit number.
 *
 * \param c1
 *    Provides the most-significant portion of the number.  This portion
 *    gets shifted leftward by 8 bits three times.
 *
 * \param c2
 *    Provides the second portion of the number.
 *
 * \param c3
 *    Provides the third portion of the number.
 *
 * \param c4
 *    Provides the least-significant portion of the number.  This portion
 *    gets shifted leftward not at all.
 *
 * \return
 *    The total value represented by the four parameters is returned.
 */

static long
to32bit (int c1, int c2, int c3, int c4)
{
   long value = (c1 & 0xff);
   value = (value << 8) + (c2 & 0xff);
   value = (value << 8) + (c3 & 0xff);
   value = (value << 8) + (c4 & 0xff);
   return value;
}

/**
 *    Convert a duo of integers into a 16-bit number.
 *
 * \param c1
 *    Provides the most-significant portion of the number.  This portion
 *    gets shifted leftward by 8 bits once.
 *
 * \param c2
 *    Provides the least-significant portion of the number.  This portion
 *    gets shifted leftward not at all.
 *
 * \return
 *    The total value represented by the two parameters is returned.
 */

static short int
to16bit (int c1, int c2)
{
   return ((c1 & 0xff) << 8) + (c2 & 0xff);
}

/**
 *    Reads four bytes and converts them to a 32-bit number.
 *
 * \todo
 *    -  We need to use an actual 32-bit return type for 64-bit systems.
 *    -  Do we need to make the temp variables volatile; can the compiler
 *       reorder them?
 *
 * \return
 *    Returns the total value represented by the four characters.
 */

static long int
read32bit (void)
{
   int c1 = egetc();
   int c2 = egetc();
   int c3 = egetc();
   int c4 = egetc();
   return to32bit(c1, c2, c3, c4);
}

/**
 *    Reads two bytes and converts them to a 16-bit number.
 *
 * \return
 *    Returns the total value represented by the two characters.
 */

static short int
read16bit (void)
{
   int c1 = egetc();
   int c2 = egetc();
   return to16bit(c1, c2);
}

/**
 *    write32bit() and write16bit() are used to make sure that the byte
 *    order of the various data types remains constant between machines.
 *    This helps make sure that the code will be portable from one system
 *    to the next.  It is slightly dangerous that it assumes that longs
 *    have at least 32 bits and ints have at least 16 bits, but this has
 *    been true at least on PCs, UNIX machines, and Macintosh's.
 *
 * \todo
 *    Provide the proper 32-bit data types needed to do this more
 *    portably.
 *
 * \param data
 *    Provides the 32 bits of data to be written, one byte at a time.
 */

void
write32bit (unsigned long data)
{
   eputc((unsigned) ((data >> 24) & 0xff));
   eputc((unsigned) ((data >> 16) & 0xff));
   eputc((unsigned) ((data >> 8 ) & 0xff));
   eputc((unsigned) (data & 0xff));
}

/**
 *    Writes two bytes of data, one byte at a time.
 *
 * \param data
 *    Provides the 16 bits of data to be written, one byte at a time.
 */

static void
write16bit (int data)
{
   eputc((unsigned) ((data & 0xff00) >> 8));
   eputc((unsigned) (data & 0xff));
}

/**
 *    Write multi-length bytes to MIDI format files.
 *    We changed the name of this function from "writevarinum()" to
 *    "writevarinum()" to match "readvarinum()" and cut down on some
 *    confusion.
 *
 * \param value
 *    Provides the value to be written.
 */

static void
writevarinum (unsigned long value)
{
   unsigned long buffer = value & 0x7f;
   while ((value >>= 7) > 0)
   {
       buffer <<= 8;
       buffer |= 0x80;
       buffer += (value & 0x7f);
   }
   for (;;)
   {
      eputc((unsigned char) (buffer & 0xff));
      if (buffer & 0x80)
         buffer >>= 8;
      else
         break;
   }
}

/**
 *    Handles a meta-event.
 *
 *    The type of event is passed in as a parameter.   The message itself
 *    is found in msg(), while the length of the message is provided by
 *    msgleng().
 *
 * \change ca 2015-10-11
 *    The current version of the test file b4uacuse-GM-format.midi has
 *    a missing value near the beginning.  Actually, the value isn't
 *    missing.  It's just that it is a sequence number of 0, which is
 *    written in the allowed alternate format, "FF 00 00", instead
 *    of the normal format "FF 00 02 ss ss", where "ss ss" would be
 *    "00 00".  Anyway, this cause a null msg() return, which we must
 *    ignore, to avoid a crash.
 *
 * \param type
 *    Provides the type of meta event.  The following value sets are
 *    handled:
 *    -  0x00.  Sequence number. The Mf_seqnum() callback is called.
 *    -  0x01 to 0x0f.  Text event.  0x01 to 0x07 are standard text
 *       events, while the rest are "unrecognized".  The Mf_text()
 *       callback is called.
 *    -  0x2f.  End of track. The Mf_eot() callback is called.
 *    -  0x51.  Set tempo. The Mf_tempo() callback is called.
 *    -  0x54.  SMPTE. The Mf_smpte() callback is called.
 *    -  0x58.  Time signature. The Mf_timesig() callback is called.
 *    -  0x59.  Key signature. The Mf_keysig() callback is called.
 *    -  0x7f.  Sequencer specific. The Mf_sqspecific() callback is called.
 *    -  Default.  Miscellaneous. The Mf_metamisc() callback is called.
 *
 */

static void
metaevent (int type)
{
   char * m = msg();
   if (not_nullptr(m))                    /* \change ca 2015-10-11   */
   {
      int leng = msgleng();
      short int seqnum;                   /* used in case 0x00       */
      long lv;                            /* used in case 0x51       */
      switch (type)
      {
      case 0x00:

         seqnum = to16bit(m[0], m[1]);
         if (mfreportable())
         {
            char temp[64];
            (void) snprintf
            (
               temp, sizeof(temp), "Meta seqnum (type %d [0x%x])=%d [0x%x]",
               type, type, (int) seqnum, (int) seqnum
            );
            mfreport(temp);
         }
         if (Mf_seqnum)
             (void) (*Mf_seqnum)(seqnum);
         break;

      case 0x01:                         /* Text event           */
      case 0x02:                         /* Copyright notice     */
      case 0x03:                         /* Sequence/Track name  */
      case 0x04:                         /* Instrument name      */
      case 0x05:                         /* Lyric                */
      case 0x06:                         /* Marker               */
      case 0x07:                         /* Cue point            */
      case 0x08:
      case 0x09:
      case 0x0a:
      case 0x0b:
      case 0x0c:
      case 0x0d:
      case 0x0e:
      case 0x0f:

         if (mfreportable())
         {
            char temp[64];
            (void) snprintf
            (
               temp, sizeof(temp), "Meta text (type=%d [0x%x]), length=%d [0x%x]",
               type, type, leng, leng
            );
            mfreport(temp);
         }
         if (Mf_text)                   /* These are all text events             */
            (void) (*Mf_text)(type, leng, m);
         break;

      case 0x2f:                         /* End of Track                         */

         if (mfreportable())
         {
            char temp[64];
            (void) snprintf
            (
               temp, sizeof(temp), "Meta end-of-track (type=%d [0x%x])",
               type, type
            );
            mfreport(temp);
         }
         if (Mf_eot)
            (void) (*Mf_eot)();
         break;

      case 0x51:                         /* Set tempo */

         lv = to32bit(0, m[0], m[1], m[2]);
         if (mfreportable())
         {
            char temp[64];
            (void) snprintf
            (
               temp, sizeof(temp),
               "Meta tempo (type=%d [0x%x]), value=%ld [0x%lx]",
               type, type, lv, lv
            );
            mfreport(temp);
         }
         if (Mf_tempo)
            (void) (*Mf_tempo)(lv);
         break;

      case 0x54:

         if (mfreportable())
         {
            char temp[64];
            (void) snprintf
            (
               temp, sizeof(temp), "Meta SMPTE (type=%d [0x%x])", type, type
            );
            mfreport(temp);
         }
         if (Mf_smpte)
            (void) (*Mf_smpte)(m[0], m[1], m[2], m[3], m[4]);
         break;

      case 0x58:

         if (mfreportable())
         {
            char temp[64];
            (void) snprintf
            (
               temp, sizeof(temp), "Meta timesig (type=%d [0x%x])", type, type
            );
            mfreport(temp);
         }
         if (Mf_timesig)
            (void) (*Mf_timesig)(m[0], m[1], m[2], m[3]);
         break;

      case 0x59:

         if (mfreportable())
         {
            char temp[64];
            (void) snprintf
            (
               temp, sizeof(temp), "Meta keysig (type=%d [0x%x])", type, type
            );
            mfreport(temp);
         }
         if (Mf_keysig)
            (void) (*Mf_keysig)(m[0], m[1]);
         break;

      case 0x7f:

         if (mfreportable())
         {
            char temp[64];
            (void) snprintf
            (
               temp, sizeof(temp),
               "Meta sqspecific (type=%d [0x%x]), length=%d [0x%x]",
               type, type, leng, leng
            );
            mfreport(temp);
         }
         if (Mf_sqspecific)
            (void) (*Mf_sqspecific)(leng, m);
         break;

      default:

         if (mfreportable())
         {
            char temp[64];
            (void) snprintf
            (
               temp, sizeof(temp), "Meta misc (type=%d [0x%x]), length=%d [0x%x]",
               type, type, leng, leng
            );
            mfreport(temp);
         }
         if (Mf_metamisc)
             (void) (*Mf_metamisc)(type, leng, m);
      }
   }
}

/**
 *    Reads through the "MThd" or "MTrk" header string.
 *
 *    Characters are read via the Mf_getc() callback function.  If the
 *    characters read do not match the expected string, then a fatal error
 *    occurs.
 *
 * \param s
 *    Provides the string that is expected to be read from the file.
 *
 * \return
 *    Returns the last character obtained, or READMT_EOF if not characters
 *    could be read (and the --strict option is in force).  Also returns
 *    READMT_EOF if the match was unable to be detected.  Returns
 *    READMT_IGNORE_NON_MTRK if there is no match, but the --ignore option
 *    is active.
 */

static int
readmt (const char * s)
{
   int result = READMT_EOF;
   int n = 0;
   int c;
   const char * p = s;
   cbool_t result_is_set = false;
   while (n++ < 4 && (c = (*Mf_getc)()) != EOF)
   {
      if (c != *p++)
      {
         char buff[64];
         (void) snprintf
         (
            buff, sizeof(buff), "Expecting '%s', but input[%d] == '%c' [0x%x]",
            s, n-1, (char) c, c
         );
         if (midicvt_option_strict())
         {
            result = READMT_EOF;
            mferror(buff);                   /* exit()'s the application */
         }
         else if (midicvt_option_ignore())
         {
            if (! result_is_set)
            {
               result_is_set = true;
               result = READMT_IGNORE_NON_MTRK;
            }
         }
      }
      if (! result_is_set)
         result = c;
   }
   return result;
}

/**
 *    Reads a header chunk.
 *
 *    First, readmt() is called to verify that "MThd" was retrieved from
 *    the file.  If this succeeds, then the following items are read:
 *
 *       -# Length of the header (32 bits).  This value is saved in the
 *          global variable s_Mf_toberead.
 *       -# Format of the header (16 bits).
 *       -# Number of tracks (16 bits).
 *       -# The division value (16 bits).
 *
 *    The last three values are passed to the Mf_header() callback
 *    function as parameters.  This function should reduce the value of
 *    s_Mf_toberead as bytes are processed.
 *
 *    If s_Mf_toberead is still greater than 0, then the extra characters
 *    are flushed by calling egetc() s_Mf_toberead times.
 */

static int
readheader (void)
{
   int result = readmt("MThd");
   if (result != READMT_EOF)
   {
      cbool_t ignore = result == READMT_IGNORE_NON_MTRK;
      int format, ntrks, division;
      s_Mf_toberead = read32bit();
      format = read16bit();
      ntrks = read16bit();
      division = read16bit();
      if (! ignore)
      {
         if (Mf_header)
            (void) (*Mf_header)(format, ntrks, division);
      }
      if (mfreportable())
      {
         char temp[80];
         (void) snprintf
         (
            temp, sizeof(temp),
            "MThd chunk-size=%ld, format=%d [0x%x], "
            "tracks=%d [0x%x], division=%d [0x%x]",
            s_Mf_toberead, format, format,
            ntrks, ntrks, division, division
         );
         mfreport(temp);
      }

      /* flush any extra stuff, in case the length of header is not 6 */

      while (s_Mf_toberead > 0)
         (void) egetc();
   }
   return result;
}

/**
 *    Provides a helper array for the readtrack() and readtrack_m2m()
 *    functions.
 *
 *    This static array is indexed by the high half of a status byte.  Its
 *    value is either the number of bytes needed (1 or 2) for a channel
 *    message, or 0 (meaning it's not a channel message).
 */

static int s_chantype [] =
{
    0, 0, 0, 0, 0, 0, 0, 0,       /* 0x00 through 0x70 */
    2, 2, 2, 2, 1, 1, 2, 0        /* 0x80 through 0xf0 */
};

/**
 *    Reads a track chunk.
 *
 *    First, readmt() is called to verify that "MTrk" was retrieved from
 *    the file.  If this succeeds, then this function reads the length of
 *    the header (32 bits).  This value is saved in the global variable
 *    s_Mf_toberead.  Then Mf_currtime is set to 0.  The Mf_starttrack()
 *    callback is called.
 *
 *    While s_Mf_toberead is non-zero, a byte is read and the following
 *    events are checked:
 *
 *       -  0xff.  Meta event.
 *       -  0xf0.  System exclusive message.
 *       -  0xf7.  Sysex continuation or arbitrary data.
 *
 *    A lot of other processing is done (see the code), and then the
 *    Mf_endtrack() calllback is called.
 *
 * \note
 *    Be sure to compare this function to the new variant,
 *    readtrack_m2m().
 *
 * \return
 *    Returns true if the "MTrk" marker was found.
 */

static cbool_t
readtrack (void)
{
   int readcode = readmt("MTrk");
   cbool_t result = readcode != READMT_EOF;
   if (result)
   {
      cbool_t sysexcontinue = false;   /* last message unfinished sysex?   */
      cbool_t running = false;         /* true when running status used    */
      int status = 0;                  /* status (e.g. 0x90==note-on)      */
      cbool_t ignore = readcode == READMT_IGNORE_NON_MTRK;
      s_Mf_toberead = read32bit();
      if (mfreportable())
      {
         char temp[80];
         snprintf(temp, sizeof(temp), "MTrk chunk-size=%ld", s_Mf_toberead);
         mfreport(temp);
      }
      Mf_currtime = 0;
      if (! ignore)
      {
         if (Mf_starttrack)
             (void) (*Mf_starttrack)();
      }
      while (s_Mf_toberead > 0)
      {
         int c;
         int c1 = 0;
         long lookfor;
         int needed;
         int type;
         Mf_currtime += readvarinum();    /* delta time                       */
         if (mfreportable())
         {
            char temp[64];
            snprintf(temp, sizeof(temp), "Delta time = %ld", Mf_currtime);
            mfreport(temp);
         }
         c = egetc();
         if (sysexcontinue && c != 0xf7)
             mferror("didn't find expected continuation of a SysEx");

         if ((c & 0x80) == 0)             /* bit 7 is not set                 */
         {
            if (status == 0)              /* running status?                  */
                mferror("readtrack(): unexpected running status");

            /**
             * \note
             *    The test file ex1.mid has the Note On byte missing from
             *    the notes after the first two!  However, the note value,
             *    which is in c, lets us reach here, and the code then
             *    copies status (which currently holds the Note On byte)
             *    into c, effectively restoring the Note On byte.
             */

            running = true;
            c1 = c;
            c = status;
         }
         else if (c < 0xf0)
         {
            status = c;
            running = false;
         }
         needed = s_chantype[(c >> 4) & 0xf];
         if (needed)                      /* ie. is it a channel message?     */
         {
             if (! running)
                 c1 = egetc();

             if (! ignore)
                chanmessage(status, c1, (needed > 1) ? egetc() : 0);
             continue;
         }
         switch (c)
         {
         case 0xff:                       /* meta event                       */

             type = egetc();
             lookfor = s_Mf_toberead - readvarinum();   /* makes no sense */
             msginit();
             while (s_Mf_toberead >= lookfor)           /* not ">" !      */
                 msgadd(egetc());

             if (! ignore)
                metaevent(type);
             break;

         case 0xf0:                       /* start of system exclusive msg    */

             lookfor = s_Mf_toberead - readvarinum();   /* makes no sense */
             msginit();
             msgadd(0xf0);
             while (s_Mf_toberead >= lookfor)           /* not ">" !      */
                 msgadd(c = egetc());

             if (c == 0xf7 || Mf_nomerge == 0)
             {
                if (! ignore)
                    sysex();
             }
             else
                 sysexcontinue = true;    /* merge into next message          */
             break;

         case 0xf7:                       /* sysex contin. or arbitrary stuff */

             lookfor = s_Mf_toberead - readvarinum();
             if (! sysexcontinue)
                 msginit();

             while (s_Mf_toberead > lookfor)
                 msgadd(c = egetc());

             if (! sysexcontinue)
             {
                 if (Mf_arbitrary)
                     (void) (*Mf_arbitrary)(msgleng(), msg());
             }
             else if (c == 0xf7)
             {
                 if (! ignore)
                    sysex();

                 sysexcontinue = false;
             }
             break;

         default:

             badbyte(c);
             break;
         }
      }
      if (! ignore)
      {
         if (Mf_endtrack)
            (void) (*Mf_endtrack)(0, 0);
      }
   }
   return result;
}

/**
 *    Calls readheader(), then calls readtrack() while there is data to be
 *    read.
 *
 *    Once done, we delete the message buffer to avoid a valgrind leakage
 *    indication at exit.
 *
 * \note
 *    This function and mfwrite() are the only non-static functions in this
 *    file?  Not any more!
 */

void
mfread (void)
{
   if (Mf_getc == nullptr)
       mferror("mfread() called without setting Mf_getc");

   if (readheader() != READMT_EOF)
   {
      while (readtrack())
          ;
   }
   if (not_nullptr(s_message_buffer))
   {
      free(s_message_buffer);
      s_message_buffer = nullptr;
   }
}

/**
 *    Provided for backward compatibility with the original library.  This
 *    function simply calls mfread().
 */

void
midifile (void)
{
   mfread();
}

/**
 *    Holds the last status code.
 */

static int s_laststat = 0;

/**
 *    Holds the last meta event type.
 */

static int s_lastmeta = 0;

/**
 *    Writes a track chunk.  This involves the following steps:
 *
 *       -# Write "MTrk" (as a tricky #define in midifilex.h)
 *       -# Write 0 as the track length.
 *       -# Call the wtrack callback.
 *       -# Write 0, meta-event, end-of-track, and 0.
 *       -# Go back and rewrite the 32-bit track header.
 *       -# Rewrite the 32-bit track length.
 *
 * \note
 *    Why not use the global Mf_wtrack() function instead of passing it as
 *    a parameter here?
 *
 * \param which_track
 *    Indicates the track number of the track to be written.  If -1, then
 *    the track is a tempo-track.
 *
 * \param fp
 *    The output file descriptor.
 *
 * \param wtrack
 *    The function to call to do that actual writing.  Usually, this
 *    function is either Mf_wtempotrack or Mf_wtrack.
 */

void
mf_w_track_chunk
(
   int which_track,
   FILE * fp,
   int (* wtrack)(void)
)
{
   long place_marker;
   unsigned long trkhdr = MTrk;
   unsigned long trklength = 0;

   /*
    * Remember where the length was written, because we don't know how
    * long it will be until we've finished writing.
    */

   long offset = ftell(fp);
   write32bit(trkhdr);                 /* Write the track chunk header        */
   write32bit(trklength);
   s_Mf_numbyteswritten = 0L;          /* the header's length doesn't count   */
   s_laststat = 0;                     /* global variable!!!                  */
   if (mfreportable())
   {
      char temp[64];
      snprintf(temp, sizeof(temp), "Writing track chunk %d", which_track);
      mfreport(temp);
   }

   /*
    * Not sure if it is an error not have a tempo-track function wired
    * into libmidifilex for real usage in compiling a text file into MIDI.
    */

   if (not_nullptr(wtrack))
      (*wtrack)();                     /* global side-effects occur           */

   if (s_laststat != meta_event || s_lastmeta != end_of_track)
   {
       eputc(0);                       /* write end of track meta event       */
       eputc(meta_event);
       eputc(end_of_track);
       eputc(0);
   }
   s_laststat = 0;

   /*
    * It's impossible to know how long the track chunk will be beforehand,
    * so the position of the track length data is kept so that it can be
    * written after the chunk has been generated.
    */

   place_marker = ftell(fp);

   /*
    * This method turned out not to be portable because the parameter
    * returned from ftell is not guaranteed to be in bytes on every machine.
    *
    * track.length = place_marker - offset - (long) sizeof(track);
    */

   if (offset > 0)                     /* do only if valid, avoid exit() */
   {
      if (fseek(fp, offset, 0) < 0)
         mferror("error seeking during final stage of write");
   }
   trklength = s_Mf_numbyteswritten;
   write32bit(trkhdr);                 /* rewrite track header w/right length */
   write32bit(trklength);
   if (place_marker > 0)               /* do only if valid, avoid exit() */
   {
      if (fseek(fp, place_marker, 0))
         mferror("error seeking during final stage of write");
   }
}

/**
 *    Saves the offset of the header portion of the current track.
 *    This value is use only for m2m (MIDI-to-MIDI) processing.
 */

static long s_track_header_offset = 0;

/**
 *    Reads and writes track information.
 *
 *
 * \param which_track
 *    Indicates the track number of the track to be written.  If -1, then
 *    the track is a tempo-track.
 *
 * \param fp
 *    The output file descriptor.
 *
 * \param wtrack
 *    The function to call to do that actual writing.  Usually, this
 *    function is either Mf_wtempotrack or Mf_wtrack.
 */

void
mf_w_track_start (int which_track, FILE * fp)
{
   unsigned long trkhdr = MTrk;
   unsigned long trklength = 0;

   /*
    * Remember where the length was written, because we don't know how
    * long it will be until we've finished writing.
    */

   s_track_header_offset = ftell(fp);
   write32bit(trkhdr);                 /* Write the track chunk header        */
   write32bit(trklength);
   s_Mf_numbyteswritten = 0L;          /* the header's length doesn't count   */
   s_laststat = 0;                     /* global variable!!!                  */
   if (mfreportable())
   {
      char temp[64];
      snprintf(temp, sizeof(temp), "Writing track chunk %d", which_track);
      mfreport(temp);
   }
}

/**
 *    Writes a header chunk.  This involves writing the following values:
 *
 *       -# Header identifier "MThd" (using a tricky long integer), 32
 *          bits.
 *       -# Chunk length, set to 6.  32 bits.
 *       -# Format.  16 bits.
 *       -# Number of tracks.  16 bits.
 *       -# Divisions.  16 bits.
 *
 * \param format
 *    Provides the format byte to describe the SMF type of the file.
 *
 * \param ntracks
 *    Provides the number of tracks in the file.
 *
 * \param division
 *    Provides the division parameter of the file.
 */

void
mf_w_header_chunk (int format, int ntracks, int division)
{
   unsigned long ident = MThd;        /* Head chunk identifier              */
   unsigned long length = 6;          /* Chunk length                       */

   /*
    * Individual bytes of the header must be written separately to preserve
    * byte order across cpu types :-(
    */

   write32bit(ident);
   write32bit(length);
   write16bit(format);
   write16bit(ntracks);
   write16bit(division);
}

/**
 *    mfwrite() is the only function you'll need to call to write out a MIDI
 *    file.
 *
 *    First, the Mf_putc() and Mf_wtrack() callbacks are checked to make
 *    sure that they have been assigned to callback functions.
 *
 *    Then mf_w_header_chunk(format, ntracks, division) is called.
 *
 *    If the format is SMF 1, then a track chunk is written by passing the
 *    Mf_wtempotrack() callback to the mf_w_track_chunk() function.
 *
 *    Finally, the rest of the tracks are written by passing the
 *    Mf_wtrack() callback to the mf_w_track_chunk() function.
 *
 * \param format
 *    Indicates the level of SMF (standard MIDI file) support.
 *    -  0: Single multi-channel track.
 *    -  1: Multiple simultaneous tracks.
 *    -  2: One or more sequentially independent single track patterns.
 *
 * \param ntracks
 *    Provides the number of tracks in the file.
 *
 * \param division
 *    This parameter is kind of tricky, it can represent two things,
 *    depending on whether it is positive or negative (bit 15 set or not).
 *    If bit 15 of division is zero, bits 14 through 0 represent the
 *    number of delta-time "ticks" which make up a quarter note.  If bit
 *    15 of division is a one, delta-times in a file correspond to
 *    subdivisions of a second similiar to SMPTE and MIDI time code.
 *    In this format bits 14 through 8 contain one of four values - 24,
 *    -25, -29, or -30, corresponding  to the four standard SMPTE and
 *    MIDI time code frame per second formats, where -29 represents 30
 *    drop frame.  The second byte consisting of bits 7 through 0
 *    corresponds the the resolution within a frame.  Refer the Standard
 *    MIDI Files 1.0 spec for more details.
 *
 * \param fp
 *    This should be the open file pointer to the file you want to write.
 *    It will have be a global in order to work with Mf_putc.
 */

void
mfwrite (int format, int ntracks, int division, FILE * fp)
{
   int i;
   if (is_nullptr(Mf_putc))
       mferror("mfmf_write() called without setting Mf_putc");

   if (is_nullptr(Mf_wtrack))
       mferror("mfmf_write() called without setting Mf_wtrack");

   /*
    * Every MIDI file starts with a header.
   */

   mf_w_header_chunk(format, ntracks, division);

   /*
    * In format 1 files, the first track is a tempo map.
    */

   if (format == 1)
   {
      if (not_nullptr(Mf_wtempotrack))
      {
          mf_w_track_chunk(-1, fp, Mf_wtempotrack);
          --ntracks;
      }
      else
      {
         /*
          * Sounds like this is not an error, so don't bitch about it.
          *
          * mferror("mfmf_write() called without setting Mf_tempotrack");
          */
      }
   }
   for (i = 0; i < ntracks; i++)       /* rest of file is a series of tracks  */
       mf_w_track_chunk(i, fp, Mf_wtrack);
}

/**
 *    Library routine to mf_write a single MIDI track event in the
 *    standard MIDI file format. The format is:
 *
\verbatim
         <delta-time> <event>
\endverbatim
 *
 *    In this case, event can be any multi-byte midi message, such as
 *    "note on", "note off", etc.
 *
 * \note
 *    This routine uses an array to pass in variable numbers of
 *    parameters.  Here's an alternate function signature to consider for
 *    the future, where the bytes are passed in a variable-parameter list:
 *
\verbatim
      int mf_w_midi_event
      (
         unsigned long delta_time,
         unsigned int type,
         unsigned int chan,
         unsigned long size,
         ...
      )
\endverbatim
 *
 * \param delta_time
 *    Provides the time in ticks since the last event.
 *
 * \param type
 *    Provides the type of event.
 *
 * \param chan
 *    Provides The midi channel.
 *
 * \param data
 *    Provides A pointer to a block of chars containing the META EVENT,
 *    data.
 *
 * \param size
 *    Provides The length of the midi-event data.
 *
 * \return
 *    Returns the \a size parameter.
 */

int
mf_w_midi_event
(
   unsigned long delta_time,
   unsigned int type,
   unsigned int chan,
   unsigned char * data,
   unsigned long size
)
{
#if 0
   static int s_runstat = 0;           /* if non-zero, use running status     */
#endif
   int i;
   unsigned char c;
   writevarinum(delta_time);

   /*
    * All MIDI events start with the type in the first four bits, and the
    * channel in the lower four bits.
    */

   c = type | chan;
   if (chan > 15)
      perror("error: MIDI channel greater than 16\n");

#if 0
   if (! s_runstat || s_laststat != c)
      eputc(c);
#endif

   eputc(c);                           /* s_runstat was always 0! */

   s_laststat = c;
   for (i = 0; i < (int) size; i++)    /* write out the data bytes */
      eputc(data[i]);

   return size;
}

/**
 *    Library routine to mf_write a single meta event in the standard MIDI
 *    file format. The format of a meta event is:
 *
\verbatim
         <delta-time><FF><type><length><bytes>
\endverbatim
 *
 * \param delta_time
 *    Provides the time in ticks since the last event.
 *
 * \param type
 *    Provides the type of meta event.
 *
 * \param data
 *    Provides A pointer to a block of chars containing the META EVENT,
 *    data.
 *
 * \param size
 *    Provides the length of the meta-event data.
 *
 * \return
 *    Returns the number of bytes written for this meta-event.  This
 *    functio used to return the \a size parameter, but no one ever used
 *    the return value, and we need it in midicvt_m2m.c to adjust the
 *    track-size correctly.
 */

int
mf_w_meta_event
(
   unsigned long delta_time,
   unsigned char type,                 /* not int! */
   unsigned char * data,
   unsigned long size
)
{
   int i;
   unsigned long byteswritten = s_Mf_numbyteswritten;
   writevarinum(delta_time);
   eputc(meta_event);                  /* mark that we're writing meta-event  */
   s_laststat = meta_event;
   eputc(type);                        /* The type of meta event              */
   s_lastmeta = type;
   writevarinum(size);                  /* length of the data bytes to follow  */
   for (i = 0; i < (int) size; i++)
   {
      if (eputc(data[i]) != data[i])
         return (-1);
   }
   size = s_Mf_numbyteswritten - byteswritten;
   return (int) size;
}

/*
 *    Library routine to mf_write a single sysex (or arbitrary)
 *    event in the standard MIDI file format. The format of the event is:
 *
\verbatim
         <delta-time><type><length><bytes>
\endverbatim
 *
 * \param delta_time
 *    Provides the time in ticks since the last event.
 *
 * \param data
 *    Provides A pointer to a block of chars containing the EVENT data.
 *    The first byte is the type (0xf0 for sysex, 0xf7 otherwise)
 *
 * \param size
 *    Provides The length of the sysex-event data.
 *
 * \return
 *    Returns the \a size parameter.
 */

int
mf_w_sysex_event
(
   unsigned long delta_time,
   unsigned char * data,
   unsigned long size
)
{
   int i;
   writevarinum(delta_time);
   eputc(*data);                       /* The type of sysex event             */
   s_laststat = 0;
   writevarinum(size - 1);              /* length of the data bytes to follow  */
   for (i = 1; i < (int) size; i++)
   {
      if (eputc(data[i]) != data[i])
         return (-1);
   }
   return size;
}

/**
 *    Writes the tempo data.
 *
 *    All tempos are written as 120 beats/minute, expressed in
 *    microseconds/quarter note.
 *
 * \param delta_time
 *    Provides the time in ticks since the last event.
 *
 * \param tempo
 *    Provides the temp value to write.
 */

void
mf_w_tempo (unsigned long delta_time, unsigned long tempo)
{
    writevarinum(delta_time);
    eputc(meta_event);
    s_laststat = meta_event;
    eputc(set_tempo);
    eputc(3);
    eputc((unsigned) (0xff & (tempo >> 16)));
    eputc((unsigned) (0xff & (tempo >> 8)));
    eputc((unsigned) (0xff & tempo));
}

/**
 *    Converts seconds to ticks.
 *
 *    Calculates the value of
 *
\verbatim
      1000 * secs      1
      ------------ * -----
      4 * division   tempo
\endverbatim
 *
 * \param secs
 *    Provides the seconds value to be converted.
 *
 * \param division
 *    Provides the division units.
 *
 * \param tempo
 *    Provides the tempo value
 *
 * \return
 *    Returns the value of the seconds in ticks, as per the formula shown.
 */

unsigned long
mf_sec2ticks (float secs, int division, unsigned int tempo)
{
   return (long)(((secs * 1000.0) / 4.0 * division) / tempo);
}

/**
 *    Provides the inverse of mf_sec2ticks.
 *
 *    This routine converts delta times in ticks into seconds. The else
 *    statement is needed because the formula is different for tracks based
 *    on notes and tracks based on SMPTE times.
 *
 * \param ticks
 *    Provides the ticks value to be converted.
 *
 * \param division
 *    Provides the division units.
 *
 * \param tempo
 *    Provides the tempo value
 *
 * \return
 *    Returns the value of the ticks in seconds. as per the formula shown.
 */

float
mf_ticks2sec (unsigned long ticks, int division, unsigned int tempo)
{
   if (division > 0)
   {
      return
      (
         (float)
         (
            ((float)(ticks) * (float)(tempo)) / ((float)(division) * 1000000.0)
         )
      );
   }
   else
   {
      /**
       * \warning
       *    If 0, then this should throw an exception!!!
       */

      float smpte_format = upperbyte(division);
      float smpte_resolution = lowerbyte(division);
      return (float) ticks / (smpte_format * smpte_resolution * 1000000.0);
   }
}

/**
 *    Reads a track chunk for MIDI-to-MIDI conversion.
 *
 *    This function is a modified version of readtrack() that works better
 *    for direct MIDI-to-MIDI conversion using the callbacks defined in
 *    the midicvt_m2m.c module.
 *
 *    A new function was made from readtrack() in order to better avoid
 *    breaking that function for existing functionality.  The functionality
 *    is virtual identical to readtrack().  However, there is one big
 *    puzzle to figure out... Why does this function have to <i> set </i>
 *    the current time, rather than just add to it the way readtrack()
 *    does?
 *
 \verbatim
         Mf_currtime += readvarinum();
         Mf_currtime  = readvarinum();
 \endverbatim
 *
 * \return
 *    Returns true if the "MTrk" marker was found.  Actually, if any marker
 *    is found, and there is no EOF returned.
 */

static cbool_t
readtrack_m2m (void)
{
   int readcode = readmt("MTrk");
   cbool_t result = readcode != READMT_EOF;
   if (result)
   {
      cbool_t sysexcontinue = false;   /* last msg was unfinished sysex?   */
      cbool_t running = false;         /* true when running status used    */
      int status = 0;                  /* status (e.g. 0x90 == note-on)    */
      cbool_t ignore = readcode == READMT_IGNORE_NON_MTRK;
      s_Mf_toberead = read32bit();
      if (mfreportable())
      {
         char tmp[80];
         snprintf(tmp, sizeof(tmp), "MTrk chunk-size=%ld", s_Mf_toberead);
         mfreport(tmp);
      }
      Mf_currtime = 0;
      if (! ignore)
      {
         if (Mf_starttrack)
             (void) (*Mf_starttrack)();
      }
      while (s_Mf_toberead > 0)
      {
         int c;
         int c1 = 0;
         long lookfor;
         int needed;
         int type;

         /*
          * \question
          *    In readtrack(), this statement uses a '+='.  Here, we have
          *    to use '=' to get the same result!  Why?
          */

         Mf_currtime  = readvarinum();    /* delta time;  '=' or '+=' ???     */
         if (mfreportable())
         {
            char temp[80];
            snprintf(temp, sizeof(temp), "Delta time = %ld", Mf_currtime);
            mfreport(temp);
         }
         c = egetc();
         if (sysexcontinue && c != 0xf7)
             mferror("didn't find expected continuation of a sysex");

         if ((c & 0x80) == 0)             /* bit 7 is not set                 */
         {
            if (status == 0)              /* running status?                  */
                mferror("readtrack_m2m(): unexpected running status");

            /**
             * \note
             *    The test file ex1.mid has the Note On byte missing from
             *    the notes after the first two!  However, the note value,
             *    which is in c, lets us reach here, and the code then
             *    copies status (which currently holds the Note On byte)
             *    into c, effectively restoring the Note On byte.
             */

            running = true;
            c1 = c;
            c = status;
         }
         else if (c < 0xf0)
         {
            status = c;
            running = false;
         }
         needed = s_chantype[(c >> 4) & 0xf];
         if (needed)                      /* ie. is it a channel message?     */
         {
             if (! running)
                 c1 = egetc();

             if (! ignore)
                chanmessage(status, c1, (needed > 1) ? egetc() : 0);
             continue;
         }
         switch (c)
         {
         case 0xff:                       /* meta event                       */

             type = egetc();
             lookfor = s_Mf_toberead - readvarinum();   /* makes no sense */
             msginit();
             while (s_Mf_toberead >= lookfor)           /* not ">" !      */
                 msgadd(egetc());

             if (! ignore)
                metaevent(type);
             break;

         case 0xf0:                       /* start of system exclusive msg    */

             lookfor = s_Mf_toberead - readvarinum();   /* makes no sense */
             msginit();
             msgadd(0xf0);
             while (s_Mf_toberead >= lookfor)           /* not ">" !      */
                 msgadd(c = egetc());

             if (c == 0xf7 || Mf_nomerge == 0)
             {
                if (! ignore)
                 sysex();
             }
             else
                 sysexcontinue = true;    /* merge into next message          */
             break;

         case 0xf7:                       /* sysex contin. or arbitrary stuff */

             lookfor = s_Mf_toberead - readvarinum();
             if (! sysexcontinue)
                 msginit();

             while (s_Mf_toberead > lookfor)
                 msgadd(c = egetc());

             if (! sysexcontinue)
             {
                 if (Mf_arbitrary)
                     (void) (*Mf_arbitrary)(msgleng(), msg());
             }
             else if (c == 0xf7)
             {
                 if (! ignore)
                    sysex();

                 sysexcontinue = false;
             }
             break;

         default:

             badbyte(c);
             break;
         }
      }
      if (! ignore)
      {
         if (Mf_endtrack)
            (*Mf_endtrack)(s_track_header_offset, s_Mf_numbyteswritten);
      }
   }
   return result;
}

/**
 *    This function provides an alternative to mfread() to better handle
 *    direct MIDI-to-MIDI conversions.
 *
 *    The mfread() function, with suitable callbacks (see the
 *    midicvt_m2m.c module) almost works for MIDI-to-MIDI conversions.
 *    However, the existing "write-track" callbacks are a bit difficult to
 *    use because they actually require some ability to read input, and
 *    don't help keep track of file pointers.
 *
 *    Calls readheader(), which works fine with the m2m_header() callback.
 *    then calls readtrack_m2m() while there is data to be read.
 *
 *    Once done, we delete the message buffer to avoid a valgrind leakage
 *    indication at exit.
 */

void
mftransform (void)
{
   if (Mf_getc == nullptr)
       mferror("mfread() called without setting Mf_getc");

   if (readheader() != READMT_EOF)
   {
      while (readtrack_m2m())
          ;
   }
   if (not_nullptr(s_message_buffer))
   {
      free(s_message_buffer);
      s_message_buffer = nullptr;
   }
}

/*
 * midifilex.c
 *
 * vim: sw=3 ts=3 wm=8 et ft=c
 */
