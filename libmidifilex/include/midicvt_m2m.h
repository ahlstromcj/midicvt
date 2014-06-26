#ifndef LIBMIDIFILEX_MIDICVT_M2M_H
#define LIBMIDIFILEX_MIDICVT_M2M_H

/*
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2 of the License, or (at your
 * option) any later version. This program is distributed in the hope that
 * it will be useful, but WITHOUT ANY WARRANTY; without even the implied
 * warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details. You should have received a
 * copy of the GNU General Public License along with this program; if not,
 * write to the...
 *
 * Free Software Foundation, Inc.,
 * 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

/**
 * \file          midicvt_m2m.h
 *
 *    This module provides functions for a basic MIDI-to-MIDI conversion
 *    application.
 *
 * \library       midicvt application portion of libmidifilex
 * \author        Chris Ahlstrom and many others; see documentation
 * \date          2014-04-29
 * \updates       2014-05-13
 * \version       $Revision$
 * \license       GNU GPL
 */

#include <stdio.h>                     /* FILE *                              */
#include <midicvt_macros.h>            /* cbool_t and more                    */
#include <midifilex.h>                 /* Mf_nomerge and much more            */

EXTERN_C_DEC

extern void midicvt_initfuncs_m2m (void);

/*
 * The following functions are exposed for use in the midicvtpp C++
 * application.
 */

extern int m2m_non (int chan, int pitch, int vol);
extern int m2m_noff (int chan, int pitch, int vol);
extern int m2m_pressure (int chan, int pitch, int pressure);
extern int m2m_program (int chan, int program);
extern int m2m_parameter (int chan, int control, int value);
extern int m2m_pitchbend (int chan, int lsb, int msb);
extern int m2m_chanpressure (int chan, int pressure);

EXTERN_C_END

#endif         /*  LIBMIDIFILEX_MIDICVT_M2M_H */

/*
 * midicvt_m2m.h
 *
 * vim: sw=3 ts=3 wm=8 et ft=c
 */

