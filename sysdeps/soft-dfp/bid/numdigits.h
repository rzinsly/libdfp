/* Number of digits functions.

   Copyright (C) 2014, Free Software Foundation, Inc.

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

   Please see dfp/COPYING.txt for more information.  */

#define NUMDIGITS_SUPPORT 1

/* NOTE: Little-Endian support only so far.  */

#ifndef _BID_NUMDIGITS_H
# define _BID_NUMDIGITS_H

#include <string.h>
#include <stdlib.h>

#include <bid-private.h>
#include <get_digits.h>

#define BID_SIG_MASK            0x80000000u
#define BID_EXPONENT_ENC_MASK   0x60000000u
#define BID_EXP_SHIFT_LARGE32   21
#define BID_EXP_SHIFT_SMALL32   23
#define BID_EXP_MASK32          0xffu
#define BID_EXP_SHIFT_LARGE64   19
#define BID_EXP_SHIFT_SMALL64   21
#define BID_EXP_MASK64          0x3ffu
#define BID_EXP_SHIFT_LARGE128  15
#define BID_EXP_SHIFT_SMALL128  17
#define BID_EXP_MASK128         0x3fffu

static inline int
getexpd32 (_Decimal32 x)
{
  unsigned int hi;
  unsigned int shift;
  union ieee754r_Decimal32 d;

  d.sd = x;
  hi = d.si;

  if ((hi & BID_EXPONENT_ENC_MASK) == BID_EXPONENT_ENC_MASK)
    shift = BID_EXP_SHIFT_LARGE32;
  else
    shift = BID_EXP_SHIFT_SMALL32;

  return ((hi >> shift) & BID_EXP_MASK32) - DECIMAL32_Bias;
}

static inline int
getexpd64 (_Decimal64 x)
{
  unsigned int hi;
  unsigned int shift;
  union ieee754r_Decimal64 d;

  d.dd = x;
  hi = d.di[1];

  if ((hi & BID_EXPONENT_ENC_MASK) == BID_EXPONENT_ENC_MASK)
    shift = BID_EXP_SHIFT_LARGE64;
  else
    shift = BID_EXP_SHIFT_SMALL64;

  return ((hi >> shift) & BID_EXP_MASK64) - DECIMAL64_Bias;
}

static inline int
getexpd128 (_Decimal128 x)
{
  unsigned int hi;
  unsigned int shift;
  union ieee754r_Decimal128 d;

  d.td = x;
  hi = d.ti[3];

  if ((hi & BID_EXPONENT_ENC_MASK) == BID_EXPONENT_ENC_MASK)
    shift = BID_EXP_SHIFT_LARGE128;
  else
    shift = BID_EXP_SHIFT_SMALL128;

  return (int)((hi >> shift) & BID_EXP_MASK128) - DECIMAL128_Bias;
}


static inline _Decimal32
setexpd32 (_Decimal32 x, int exp)
{
  unsigned int shift;
  union ieee754r_Decimal32 d;

  d.sd = x;

  if ((d.si & BID_EXPONENT_ENC_MASK) == BID_EXPONENT_ENC_MASK)
    shift = BID_EXP_SHIFT_LARGE32;
  else
    shift = BID_EXP_SHIFT_SMALL32;

  d.si &= ~((BID_EXP_MASK32) << shift);
  d.si |= ((exp + DECIMAL32_Bias) & BID_EXP_MASK32) << shift;
  return d.sd;
}

static inline _Decimal64
setexpd64 (_Decimal64 x, int exp)
{
  unsigned int shift;
  union ieee754r_Decimal64 d;

  d.dd = x;

  if ((d.di[1] & BID_EXPONENT_ENC_MASK) == BID_EXPONENT_ENC_MASK)
    shift = BID_EXP_SHIFT_LARGE64;
  else
    shift = BID_EXP_SHIFT_SMALL64;

  d.di[1] &= ~((BID_EXP_MASK64) << shift);
  d.di[1] |= ((exp + DECIMAL64_Bias) & BID_EXP_MASK64) << shift;
  return d.dd;
}

static inline _Decimal128
setexpd128 (_Decimal128 x, int exp)
{
  unsigned int shift;
  union ieee754r_Decimal128 d;

  d.td = x;

  if ((d.ti[3] & BID_EXPONENT_ENC_MASK) == BID_EXPONENT_ENC_MASK)
    shift = BID_EXP_SHIFT_LARGE128;
  else
    shift = BID_EXP_SHIFT_SMALL128;

  d.ti[3] &= ~((BID_EXP_MASK128) << shift);
  d.ti[3] |= ((exp + DECIMAL128_Bias) & BID_EXP_MASK128) << shift;
  return d.td;
}


static inline int
numdigitsd32 (_Decimal32 x)
{
  int firstdigit = 0;
  int len = 0;
  char digits[8] = { 0 };

  __get_digits_d32 (x, digits, NULL, NULL, NULL, NULL);

  while (digits[firstdigit] == '0')
    firstdigit++;

  len = strlen (digits + firstdigit);
  /* Hardware DFP always returns 1 digit if the mantissa is zero.  We should
   * do the same.  */
  return (len == 0 ? 1 : len);
}

static inline int
numdigitsd64 (_Decimal64 x)
{
  int firstdigit = 0;
  int len = 0;
  char digits[17] = { 0 };

  __get_digits_d64 (x, digits, NULL, NULL, NULL, NULL);

  while (digits[firstdigit] == '0')
    firstdigit++;

  len = strlen (digits + firstdigit);
  /* Hardware DFP always returns 1 digit if the mantissa is zero.  We should
   * do the same.  */
  return (len == 0 ? 1 : len);
}

static inline int
numdigitsd128 (_Decimal128 x)
{
  int firstdigit = 0;
  int len = 0;
  char digits[34] = { 0 };

  __get_digits_d128 (x, digits, NULL, NULL, NULL, NULL);

  while (digits[firstdigit] == '0')
    firstdigit++;

  len = strlen (digits + firstdigit);
  /* Hardware DFP always returns 1 digit if the mantissa is zero.  We should
   * do the same.  */
  return (len == 0 ? 1 : len);
}

static inline
int __bid_required_bits_32 (unsigned long int value)
{
  return value == 0 ? 0 : (32 - __builtin_clz (value));
}

static inline _Decimal32
setdigitsd32 (_Decimal32 x, char *str)
{
  unsigned long int number;
  union ieee754r_Decimal32 d;
  unsigned int sign;
  int exp;

  d.sd = x;
  number = strtol (str, NULL, 10);

  exp = getexpd32 (x);
  sign = (d.si & BID_SIG_MASK);
  if (__bid_required_bits_32 (number) <= 23)
    d.si = sign | (number & 0x807FFFFFU);
  else
    d.si = sign | (0x60800000U | (number & 0x001FFFFFU));
  d.sd = setexpd32 (d.sd, exp);

  return d.sd;
}


static inline
int __bid_required_bits_64 (unsigned long long int value)
{
  return (value == 0ULL ? 0 : 64 - __builtin_clzll (value));
}

static inline _Decimal64
setdigitsd64 (_Decimal64 x, char *str)
{
  unsigned long long int number;
  union ieee754r_Decimal64 d;
  unsigned int sign;
  int exp;

  d.dd = x;
  number = strtoll (str, NULL, 10);

  exp = getexpd64 (x);
  sign = (d.di[1] & BID_SIG_MASK);
  if (__bid_required_bits_64 (number) <= 53)
    d.di[1] = sign | ((number >> 32L) & 0x001FFFFFU);
  else
    d.di[1] = (sign | (0x60040000U) | ((number >> 32L) & 0x0007FFFFU));
  d.di[0] = number & 0xFFFFFFFFU;
  d.dd = setexpd64 (d.dd, exp);

  return d.dd;
}

static inline __uint128_t
get_number128 (char* sz)
{
  // make some reasonable assumptions
  int radix = 10, size, i;
  char ch;
  __uint128_t a;
  unsigned int n;
  int minus = 0;

  size = strlen (sz);

  a = 0;
  i = 0;

  // Check for minus sign.
  if (sz[i] == '-')
    {
      ++i;
      minus = 1;
    }

  while (i < size)
    {
      ch = sz[i];

      if (ch >= 'A' && ch <= 'Z')
	{
	  if (((ch - 'A') + 10) < radix)
	    n = (ch - 'A') + 10;
	  else
	    break;
	}
      else if (ch >= 'a' && ch <= 'z')
        {
	  if (((ch - 'a') + 10) < radix)
	    n = (ch - 'a') + 10;
	  else
	    break;
	}
      else if (ch >= '0' && ch <= '9')
	{
	  if ((ch - '0') < radix)
	    n = (ch - '0');
	  else
	    break;
	}
      else
	/* Completely invalid character  */
	break;

      a *= radix;
      a += n;
      ++i;
    }

  /* If this was a negative number, do that two's compliment  */
  if (minus)
    a = -a;

  return a;
}

static inline
int __bid_required_bits_128 (__uint128_t value)
{
  /* Use 64-bits operations to keep it simple, another solution would
   * be to construct a lookup table for int128.  */
  int ret;
  unsigned long long int hi = (value >> 64);
  if ((ret = __bid_required_bits_64 (hi)) == 0)
    {
      unsigned long long int lo = (value & 0xFFFFFFFFFFFFFFFFULL);
      return __bid_required_bits_64 (lo);
    }
  return ret + 64;
}

static inline _Decimal128
setdigitsd128 (_Decimal128 x, char *str)
{
  __uint128_t number;
  union ieee754r_Decimal128 d;
  unsigned int sign;
  int exp;

  d.td = x;
  number = get_number128 (str);

  exp = getexpd128 (x);
  sign = (d.ti[3] & BID_SIG_MASK);
  if (__bid_required_bits_128 (number) <= 113)
    d.ti[3] = sign | ((number >> 96) & 0x0001FFFF);
  else
    d.ti[3] = (sign | (0x60004000U) | ((number >> 96) & 0x00007FFFU));
  d.ti[2] = (number >> 64) & 0xFFFFFFFFU;
  d.ti[1] = (number >> 32) & 0xFFFFFFFFU;
  d.ti[0] = number & 0xFFFFFFFFU;
  d.td = setexpd128 (d.td, exp);

  return d.td;
}

static inline _Decimal32
left_justifyd32 (_Decimal32 x)
{
  int firstdigit = 0, len;
  char digits[8+7] = { 0 };

  __get_digits_d32 (x, digits, NULL, NULL, NULL, NULL);
  while (digits[firstdigit] == '0')
    firstdigit++;
  len = strlen (digits + firstdigit);
  if (len)
    {
      int exp = getexpd32 (x);
      /* pad the significant digits with enough trailing zeroes */
      if ((exp - firstdigit) < -DECIMAL32_Bias)
	firstdigit = DECIMAL32_Bias + exp;
      memset (digits + firstdigit + len, '0', firstdigit);
      x = setdigitsd32 (x, digits + firstdigit);
      x = setexpd32 (x, exp - firstdigit);
    }

  return x;
}

static inline _Decimal64
left_justifyd64 (_Decimal64 x)
{
  int firstdigit = 0, len;
  char digits[16+17] = { 0 };

  __get_digits_d64 (x, digits, NULL, NULL, NULL, NULL);
  while (digits[firstdigit] == '0')
    firstdigit++;
  len = strlen (digits + firstdigit);
  if (len)
    {
      int exp = getexpd64 (x);
      /* pad the significant digits with enough trailing zeroes */
      if ((exp - firstdigit) < -DECIMAL64_Bias)
	firstdigit = DECIMAL64_Bias + exp;
      memset (digits + firstdigit + len, '0', firstdigit);
      x = setdigitsd64 (x, digits + firstdigit);
      x = setexpd64 (x, exp - firstdigit);
    }

  return x;
}

static inline _Decimal128
left_justifyd128 (_Decimal128 x)
{
  int firstdigit = 0, len;
  char digits[35+34] = { 0 };

  __get_digits_d128 (x, digits, NULL, NULL, NULL, NULL);
  while (digits[firstdigit] == '0')
    firstdigit++;
  len = strlen (digits + firstdigit);
  if (len)
    {
      int exp = getexpd128 (x);
      /* pad the significant digits with enough trailing zeroes */
      if ((exp - firstdigit) < -DECIMAL128_Bias)
	firstdigit = DECIMAL128_Bias + exp;
      memset(digits + firstdigit + len, '0', firstdigit);
      x = setdigitsd128 (x, digits + firstdigit);
      x = setexpd128 (x, exp - firstdigit);
    }

  return x;
}

#endif
