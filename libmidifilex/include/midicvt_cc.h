/*
 * midicvt - A text-MIDI translater
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
 * \file          midicvt_cc.h
 *
 *    This module provides a place to collect the names of MIDI Control
 *    Change (Continuous Controller, CC) for the upcoming --human option.
 *
 * \library       midicvt application
 * \author        Chris Ahlstrom
 * \date          2016-05-19
 * \updates       2016-05-19
 * \version       $Revision$
 * \license       GNU GPL
 *
 *    This module provides a function to safely access the static/private
 *    list of MIDI CC names.
 */

extern const char * midi_controller_name (int cc);

/*
 * midicvt_cc.h
 *
 * vim: sw=3 ts=3 wm=8 et ft=c
 */

