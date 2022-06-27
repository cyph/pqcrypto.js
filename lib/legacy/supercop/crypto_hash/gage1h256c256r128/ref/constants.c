/*
 * Gage / Ingage Reference C Implementation
 *
 * Copyright 2018:
 *     Danilo Gligoroski <danilog@ntnu.no>
 *     Daniel Otte <bg@nerilex.org>
 *     Hristina Mihajloska <hristina.mihajloska@finki.ukim.mk>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301, USA.
 *
 */

#include <inttypes.h>

#include "parameters.h"
/*
 * These  constants will be used as leading 2-bit values in different rounds performing
 * d-transformations on the inner state. They are obtained with genconstants.c
 *
 */
const uint8_t leader[255] = {
    0, 3, 0, 3, 0, 1, 2, 3, 2, 1, 2, 3, 2, 1, 2, 3,
    0, 1, 2, 3, 2, 3, 0, 3, 2, 3, 2, 3, 2, 1, 2, 1,
    0, 3, 2, 1, 0, 3, 2, 1, 2, 1, 2, 1, 0, 1, 2, 3,
    0, 1, 0, 1, 0, 3, 2, 3, 0, 3, 0, 3, 0, 3, 0, 3,
    2, 3, 2, 3, 0, 1, 2, 1, 2, 1, 0, 1, 0, 3, 0, 1,
    2, 3, 2, 3, 2, 3, 2, 3, 0, 1, 0, 1, 2, 1, 2, 3,
    2, 3, 0, 1, 0, 3, 2, 1, 2, 1, 0, 1, 0, 1, 0, 1,
    2, 1, 0, 1, 2, 3, 2, 1, 0, 1, 2, 1, 0, 3, 0, 3,
    2, 3, 0, 1, 0, 1, 0, 1, 2, 3, 0, 1, 2, 1, 0, 3,
    0, 1, 2, 3, 0, 3, 2, 3, 0, 1, 2, 1, 0, 1, 0, 1,
    2, 1, 2, 1, 2, 3, 0, 3, 2, 1, 2, 1, 2, 3, 0, 1,
    2, 1, 2, 3, 0, 1, 0, 1, 2, 3, 2, 3, 2, 1, 2, 3,
    0, 3, 2, 3, 2, 1, 2, 1, 2, 3, 2, 1, 2, 1, 0, 1,
    2, 1, 0, 1, 0, 3, 2, 1, 2, 3, 0, 1, 0, 3, 2, 3,
    2, 1, 0, 3, 2, 3, 0, 1, 2, 3, 0, 1, 0, 3, 0, 3,
    2, 1, 2, 1, 0, 3, 0, 1, 0, 3, 0, 3, 0, 1, 2  
};

struct state {
	uint8_t a[INTERNAL_STATE_SIZE_BYTES];
};

/* Quasigroups Nr. 173 (in lexicigraphical order of all 576 4x4 quasigroups) */
const uint8_t Quas[4][4] = {
	{1,0,3,2},
	{0,2,1,3},
	{2,3,0,1},
	{3,1,2,0}
};
