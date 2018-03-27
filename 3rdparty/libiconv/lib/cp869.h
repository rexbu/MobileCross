/*
 * Copyright (C) 1999-2001, 2016 Free Software Foundation, Inc.
 * This file is part of the GNU LIBICONV Library.
 *
 * The GNU LIBICONV Library is free software; you can redistribute it
 * and/or modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * The GNU LIBICONV Library is distributed in the hope that it will be
 * useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with the GNU LIBICONV Library; see the file COPYING.LIB.
 * If not, see <http://www.gnu.org/licenses/>.
 */

/*
 * CP869
 */

static const unsigned short cp869_2uni[128] = {
  /* 0x80 */
  0xfffd, 0xfffd, 0xfffd, 0xfffd, 0xfffd, 0xfffd, 0x0386, 0xfffd,
  0x00b7, 0x00ac, 0x00a6, 0x2018, 0x2019, 0x0388, 0x2015, 0x0389,
  /* 0x90 */
  0x038a, 0x03aa, 0x038c, 0xfffd, 0xfffd, 0x038e, 0x03ab, 0x00a9,
  0x038f, 0x00b2, 0x00b3, 0x03ac, 0x00a3, 0x03ad, 0x03ae, 0x03af,
  /* 0xa0 */
  0x03ca, 0x0390, 0x03cc, 0x03cd, 0x0391, 0x0392, 0x0393, 0x0394,
  0x0395, 0x0396, 0x0397, 0x00bd, 0x0398, 0x0399, 0x00ab, 0x00bb,
  /* 0xb0 */
  0x2591, 0x2592, 0x2593, 0x2502, 0x2524, 0x039a, 0x039b, 0x039c,
  0x039d, 0x2563, 0x2551, 0x2557, 0x255d, 0x039e, 0x039f, 0x2510,
  /* 0xc0 */
  0x2514, 0x2534, 0x252c, 0x251c, 0x2500, 0x253c, 0x03a0, 0x03a1,
  0x255a, 0x2554, 0x2569, 0x2566, 0x2560, 0x2550, 0x256c, 0x03a3,
  /* 0xd0 */
  0x03a4, 0x03a5, 0x03a6, 0x03a7, 0x03a8, 0x03a9, 0x03b1, 0x03b2,
  0x03b3, 0x2518, 0x250c, 0x2588, 0x2584, 0x03b4, 0x03b5, 0x2580,
  /* 0xe0 */
  0x03b6, 0x03b7, 0x03b8, 0x03b9, 0x03ba, 0x03bb, 0x03bc, 0x03bd,
  0x03be, 0x03bf, 0x03c0, 0x03c1, 0x03c3, 0x03c2, 0x03c4, 0x0384,
  /* 0xf0 */
  0x00ad, 0x00b1, 0x03c5, 0x03c6, 0x03c7, 0x00a7, 0x03c8, 0x0385,
  0x00b0, 0x00a8, 0x03c9, 0x03cb, 0x03b0, 0x03ce, 0x25a0, 0x00a0,
};

static int
cp869_mbtowc (conv_t conv, ucs4_t *pwc, const unsigned char *s, size_t n)
{
  unsigned char c = *s;
  if (c < 0x80) {
    *pwc = (ucs4_t) c;
    return 1;
  }
  else {
    unsigned short wc = cp869_2uni[c-0x80];
    if (wc != 0xfffd) {
      *pwc = (ucs4_t) wc;
      return 1;
    }
  }
  return RET_ILSEQ;
}

static const unsigned char cp869_page00[32] = {
  0xff, 0x00, 0x00, 0x9c, 0x00, 0x00, 0x8a, 0xf5, /* 0xa0-0xa7 */
  0xf9, 0x97, 0x00, 0xae, 0x89, 0xf0, 0x00, 0x00, /* 0xa8-0xaf */
  0xf8, 0xf1, 0x99, 0x9a, 0x00, 0x00, 0x00, 0x88, /* 0xb0-0xb7 */
  0x00, 0x00, 0x00, 0xaf, 0x00, 0xab, 0x00, 0x00, /* 0xb8-0xbf */
};
static const unsigned char cp869_page03[80] = {
  0x00, 0x00, 0x00, 0x00, 0xef, 0xf7, 0x86, 0x00, /* 0x80-0x87 */
  0x8d, 0x8f, 0x90, 0x00, 0x92, 0x00, 0x95, 0x98, /* 0x88-0x8f */
  0xa1, 0xa4, 0xa5, 0xa6, 0xa7, 0xa8, 0xa9, 0xaa, /* 0x90-0x97 */
  0xac, 0xad, 0xb5, 0xb6, 0xb7, 0xb8, 0xbd, 0xbe, /* 0x98-0x9f */
  0xc6, 0xc7, 0x00, 0xcf, 0xd0, 0xd1, 0xd2, 0xd3, /* 0xa0-0xa7 */
  0xd4, 0xd5, 0x91, 0x96, 0x9b, 0x9d, 0x9e, 0x9f, /* 0xa8-0xaf */
  0xfc, 0xd6, 0xd7, 0xd8, 0xdd, 0xde, 0xe0, 0xe1, /* 0xb0-0xb7 */
  0xe2, 0xe3, 0xe4, 0xe5, 0xe6, 0xe7, 0xe8, 0xe9, /* 0xb8-0xbf */
  0xea, 0xeb, 0xed, 0xec, 0xee, 0xf2, 0xf3, 0xf4, /* 0xc0-0xc7 */
  0xf6, 0xfa, 0xa0, 0xfb, 0xa2, 0xa3, 0xfd, 0x00, /* 0xc8-0xcf */
};
static const unsigned char cp869_page20[16] = {
  0x00, 0x00, 0x00, 0x00, 0x00, 0x8e, 0x00, 0x00, /* 0x10-0x17 */
  0x8b, 0x8c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* 0x18-0x1f */
};
static const unsigned char cp869_page25[168] = {
  0xc4, 0x00, 0xb3, 0x00, 0x00, 0x00, 0x00, 0x00, /* 0x00-0x07 */
  0x00, 0x00, 0x00, 0x00, 0xda, 0x00, 0x00, 0x00, /* 0x08-0x0f */
  0xbf, 0x00, 0x00, 0x00, 0xc0, 0x00, 0x00, 0x00, /* 0x10-0x17 */
  0xd9, 0x00, 0x00, 0x00, 0xc3, 0x00, 0x00, 0x00, /* 0x18-0x1f */
  0x00, 0x00, 0x00, 0x00, 0xb4, 0x00, 0x00, 0x00, /* 0x20-0x27 */
  0x00, 0x00, 0x00, 0x00, 0xc2, 0x00, 0x00, 0x00, /* 0x28-0x2f */
  0x00, 0x00, 0x00, 0x00, 0xc1, 0x00, 0x00, 0x00, /* 0x30-0x37 */
  0x00, 0x00, 0x00, 0x00, 0xc5, 0x00, 0x00, 0x00, /* 0x38-0x3f */
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* 0x40-0x47 */
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* 0x48-0x4f */
  0xcd, 0xba, 0x00, 0x00, 0xc9, 0x00, 0x00, 0xbb, /* 0x50-0x57 */
  0x00, 0x00, 0xc8, 0x00, 0x00, 0xbc, 0x00, 0x00, /* 0x58-0x5f */
  0xcc, 0x00, 0x00, 0xb9, 0x00, 0x00, 0xcb, 0x00, /* 0x60-0x67 */
  0x00, 0xca, 0x00, 0x00, 0xce, 0x00, 0x00, 0x00, /* 0x68-0x6f */
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* 0x70-0x77 */
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* 0x78-0x7f */
  0xdf, 0x00, 0x00, 0x00, 0xdc, 0x00, 0x00, 0x00, /* 0x80-0x87 */
  0xdb, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* 0x88-0x8f */
  0x00, 0xb0, 0xb1, 0xb2, 0x00, 0x00, 0x00, 0x00, /* 0x90-0x97 */
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* 0x98-0x9f */
  0xfe, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* 0xa0-0xa7 */
};

static int
cp869_wctomb (conv_t conv, unsigned char *r, ucs4_t wc, size_t n)
{
  unsigned char c = 0;
  if (wc < 0x0080) {
    *r = wc;
    return 1;
  }
  else if (wc >= 0x00a0 && wc < 0x00c0)
    c = cp869_page00[wc-0x00a0];
  else if (wc >= 0x0380 && wc < 0x03d0)
    c = cp869_page03[wc-0x0380];
  else if (wc >= 0x2010 && wc < 0x2020)
    c = cp869_page20[wc-0x2010];
  else if (wc >= 0x2500 && wc < 0x25a8)
    c = cp869_page25[wc-0x2500];
  if (c != 0) {
    *r = c;
    return 1;
  }
  return RET_ILUNI;
}
