/* _Decimal32 quantumd32 implementation.

   Copyright (C) 2014 Free Software Foundation, Inc.

   This file is part of the Decimal Floating Point C Library.

   The Decimal Floating Point C Library is free software; you can
   redistribute it and/or modify it under the terms of the GNU Lesser
   General Public License version 2.1.

   The Decimal Floating Point C Library is distributed in the hope that
   it will be useful, but WITHOUT ANY WARRANTY; without even the implied
   warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See
   the GNU Lesser General Public License version 2.1 for more details.

   You should have received a copy of the GNU Lesser General Public
   License version 2.1 along with the Decimal Floating Point C Library;
   if not, write to the Free Software Foundation, Inc., 59 Temple Place,
   Suite 330, Boston, MA 02111-1307 USA.

   Please see libdfp/COPYING.txt for more information.  */

#include <math.h>

_Decimal32 __quantumd32 (_Decimal32 x)
{
  _Decimal64 ret;
  asm ("dxex    %0,%0\n"
       "dcffix  %1,%0\n"
    : "=f"(ret) :
      "f"((_Decimal64) x));
  if (ret == -1.0DD)
    return __builtin_infd32 ();
  else if ((ret == -2.0DD) || (ret == -3.0DD))
    return __builtin_nand32 ("");
  return ret - 398.0DD;
}
weak_alias (__quantumd32, quantumd32)
