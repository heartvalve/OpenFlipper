/*
-------------------------------------------------------------------------------

  Philips 3D Solutions - Eindhoven

  CRC32 implementation adapted from public domain code written by Eric Durbin.

-------------------------------------------------------------------------------
*/

#ifndef _CRC32_HH
#define _CRC32_HH

#ifdef __cplusplus
extern "C" {
#endif

/* crc32.hh
  header file for crc32 checksum
*/

/* function prototypes */
unsigned long CalcCRC32(unsigned char *p, unsigned long len);

#ifdef __cplusplus
}
#endif

#endif /* _CRC32_HH */
