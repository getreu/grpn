/*

  Copyright (C) 2002  Paul Wilkins

  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License
  as published by the Free Software Foundation; either version 2
  of the License, or (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

*/
/* constant.c  by Paul Wilkins 3/27/97 */

#include <math.h>

#include "real.h"
#include "complex.h"

Real *realE;
Real *realPi;
Real *realPi2;
Real *realZero;
Real *realHalf;
Real *realTen;
Real *realOne;
Real *realTwo;
Real *realMOne;
Real *real180Pi;
Cmplx *cmplxI;

void setup_constant(){
    Real *tmp;

    realE  = setRealDouble(newReal(), M_E);
    realPi = setRealDouble(newReal(), M_PI);
    realPi2 = setRealDouble(newReal(), M_PI_2);
    realZero = setRealDouble(newReal(), 0.0);
    realHalf = setRealDouble(newReal(), 0.5);
    realTen = setRealDouble(newReal(), 10.0);
    realOne = setRealDouble(newReal(), 1.0);
    realTwo = setRealDouble(newReal(), 2.0);
    realMOne = setRealDouble(newReal(), -1.0);

    tmp = setRealDouble(newReal(), 180.0);
    real180Pi = divReal(tmp, realPi);
    freeReal(tmp);

    tmp = setRealDouble(newReal(), 1.0);
    cmplxI = setCmplxReal(newCmplx(), realZero, tmp);
    freeReal(tmp);
}
