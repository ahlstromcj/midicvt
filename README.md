midicvt 0.4.0.1
Chris Ahlstrom
2015-08-11 to 2016-04-23

This project supports MIDI-to-text-to-MIDI conversions like midicomp and
midi2text (it is derived from those projects), and adds support for direct
MIDI-to-MIDI conversions driven by an INI file, using a C++ add-on.
The C++ project can also transform the patch numbers of files, and the drum
notes and channel as well, converting device-specific (e.g. Yamaha PSS-790)
to General MIDI (GM) format, for example.

The newest version, 0.4.0, adds some important bug fixes, additional
documentation, and a --summarize option to shows a summary of what conversions
were made by midicvtpp.

Currently Linux-only, and built by GNU Autotools.

TODO:

   1. Fill in a more areas of documentation.
   2. Add support for creating a Debian package.
   3. Add still more tests and examples.

Chris Ahlstrom, 2014-06-26

   I am a professional programmer using C and C++, and lately, Java.
   My hobbies are writing Free software, bicycling, and soccer.

# vim: ts=3 sw=3 et ft=sh
