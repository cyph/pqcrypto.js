///////////////////////////////////////////////////////////////////////////////
// sparkleARM.c: Assembler implementation for ARM of the SPARKLE permutation.//
// This file is part of the SPARKLE submission to NIST's LW Crypto Project.  //
// Version 0.2.0 (2019-03-28), see <http://www.cryptolux.org/> for updates.  //
// Authors: The SPARKLE Group (C. Beierle, A. Biryukov, L. Cardoso dos       //
// Santos, J. Groszschaedl, L. Perrin, A. Udovenko, V. Velichkov, Q. Wang).  //
// License: GPLv3 (see LICENSE file), other licenses available upon request. //
// Copyright (C) 2019 University of Luxembourg <http://www.uni.lu/>.         //
// ------------------------------------------------------------------------- //
// This program is free software: you can redistribute it and/or modify it   //
// under the terms of the GNU General Public License as published by the     //
// Free Software Foundation, either version 3 of the License, or (at your    //
// option) any later version. This program is distributed in the hope that   //
// it will be useful, but WITHOUT ANY WARRANTY; without even the implied     //
// warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the  //
// GNU General Public License for more details. You should have received a   //
// copy of the GNU General Public License along with this program. If not,   //
// see <http://www.gnu.org/licenses/>.                                       //
///////////////////////////////////////////////////////////////////////////////

#ifndef _SPARKLE_ARM_H
#define _SPARKLE_ARM_H

    void ARXBOXasm(uint32_t *x, uint32_t *y, const uint32_t c);
    void sparkle_optARM(uint32_t *state, int nb, int ns);
    void ARXBoxfullasm(uint32_t *state, int nb, int ns);

#endif //_SPARKLE_ARM_H
