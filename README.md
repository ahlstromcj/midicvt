# midicvt 0.4.2.0
(C) Chris Ahlstrom 2014-2023

This project supports MIDI-to-text-to-MIDI conversions like midicomp and
midi2text (it is derived from those projects), and adds support for direct
MIDI-to-MIDI conversions driven by an INI file, using a C++ add-on.
The C++ project can also transform the patch numbers of files, and the drum
notes and channel as well, converting device-specific values (e.g. Yamaha
PSS-790) to General MIDI (GM) format, for example.

The newest version, `0.4.2`, adds some more important bug fixes, additional
documentation, and a `--summarize` option to show a summary of what conversions
were made by `midicvtpp`.

## IMPORTANT:
We have fixed a lot of bugs; if we have broken your use-cases, please file a
bug report.

## TODO:

   1. Fill in a more areas of documentation.
   2. Add support for creating a Debian package.
   3. Add still more tests and examples.

Currently Linux-only, and built by GNU Autotools.  See the INSTALL file.

## Author
### Chris Ahlstrom

I am a professional programmer using C and C++, and lately, Java.
My hobbies are writing Free software, bicycling, and soccer.
