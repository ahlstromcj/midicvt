midicvt 0.4.2.1
Chris Ahlstrom
2015-08-11 to 2016-05-27

This project supports MIDI-to-text-to-MIDI conversions like midicomp and
midi2text (it is derived from those projects), and adds support for direct
MIDI-to-MIDI conversions driven by an INI file, using a C++ add-on.
The C++ project can also transform the patch numbers of files, and the drum
notes and channel as well, converting device-specific values (e.g. Yamaha
PSS-790) to General MIDI (GM) format, for example.

Version 0.4.1 adds some more important bug fixes, additional documentation,
and a --summarize option to shows a summary of what conversions were made
by midicvtpp.

Version 0.4.2 will add a --human option for much more readable output for
humans only.

IMPORTANT:  We have fixed a lot of bugs; if we have broken your use-cases,
            please file a bug report.

TODO:

   1. Fill in a more areas of documentation.
   2. Add support for creating a Debian package.
   3. Add still more tests and examples.

Currently Linux-only, and built by GNU Autotools.

Chris Ahlstrom, 2014-06-26

   I am a professional programmer using C and C++, and lately, Java.
   My hobbies are writing Free software, bicycling, and soccer.

# vim: ts=3 sw=3 et ft=sh
