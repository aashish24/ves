/*========================================================================
  VES --- VTK OpenGL ES Rendering Toolkit

      http://www.kitware.com/ves

  Copyright 2011 Kitware, Inc.

  Licensed under the Apache License, Version 2.0 (the "License");
  you may not use this file except in compliance with the License.
  You may obtain a copy of the License at

      http://www.apache.org/licenses/LICENSE-2.0

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.
 ========================================================================*/
#include "vesByteSwap.h"
#include <memory.h>
#include <stdio.h>
#include <iostream>

using std::ostream;

//----------------------------------------------------------------------------
vesByteSwap::vesByteSwap()
{
}

//----------------------------------------------------------------------------
vesByteSwap::~vesByteSwap()
{
}

//----------------------------------------------------------------------------
// Define swap functions for each type size.
template <size_t s> struct vesByteSwapper;
template<> struct vesByteSwapper<1>
{
  static inline void Swap(char*) {}
};
template<> struct vesByteSwapper<2>
{
  static inline void Swap(char* data)
  {
    char one_byte;
    one_byte = data[0]; data[0] = data[1]; data[1] = one_byte;
  }
};
template<> struct vesByteSwapper<4>
{
  static inline void Swap(char* data)
  {
    char one_byte;
    one_byte = data[0]; data[0] = data[3]; data[3] = one_byte;
    one_byte = data[1]; data[1] = data[2]; data[2] = one_byte;
  }
};
template<> struct vesByteSwapper<8>
{
  static inline void Swap(char* data)
  {
    char one_byte;
    one_byte = data[0]; data[0] = data[7]; data[7] = one_byte;
    one_byte = data[1]; data[1] = data[6]; data[6] = one_byte;
    one_byte = data[2]; data[2] = data[5]; data[5] = one_byte;
    one_byte = data[3]; data[3] = data[4]; data[4] = one_byte;
  }
};

//----------------------------------------------------------------------------
// Define range swap functions.
template <class T> inline void vesByteSwapRange(T* first, int num)
{
  // Swap one value at a time.
  T* last = first + num;
  for(T* p=first; p != last; ++p)
  {
    vesByteSwapper<sizeof(T)>::Swap(reinterpret_cast<char*>(p));
  }
}
inline bool vesByteSwapRangeWrite(const char* first, int num,
                                  FILE* f, int)
{
  // No need to swap segments of 1 byte.
  size_t status=fwrite(first, sizeof(char), static_cast<size_t>(num), f);
  return status==static_cast<size_t>(num);
}
inline bool vesByteSwapRangeWrite(const signed char* first, int num,
                                  FILE* f, int)
{
  // No need to swap segments of 1 byte.
  size_t status=fwrite(first, sizeof(signed char),static_cast<size_t>(num),f);
  return status==static_cast<size_t>(num);
}
inline bool vesByteSwapRangeWrite(const unsigned char* first, int num,
                                  FILE* f, int)
{
  // No need to swap segments of 1 byte.
  size_t status=fwrite(first,sizeof(unsigned char),static_cast<size_t>(num),f);
  return status==static_cast<size_t>(num);
}
template <class T>
inline bool vesByteSwapRangeWrite(const T* first, int num, FILE* f, long)
{
  // Swap and write one value at a time.  We do not need to do this in
  // blocks because the file stream is already buffered.
  const T* last = first + num;
  bool result=true;
  for(const T* p=first; p != last && result; ++p)
  {
    // Use a union to avoid breaking C++ aliasing rules.
    union { T value; char data[sizeof(T)]; } temp = {*p};
    vesByteSwapper<sizeof(T)>::Swap(temp.data);
    size_t status=fwrite(temp.data, sizeof(T), 1, f);
    result=status==1;
  }
  return result;
}
inline void vesByteSwapRangeWrite(const char* first, int num,
                                  ostream* os, int)
{
  // No need to swap segments of 1 byte.
  os->write(first, num*static_cast<int>(sizeof(char)));
}
inline void vesByteSwapRangeWrite(const signed char* first, int num,
                                  ostream* os, int)
{
  // No need to swap segments of 1 byte.
  os->write(reinterpret_cast<const char*>(first),
            num*static_cast<int>(sizeof(signed char)));
}
inline void vesByteSwapRangeWrite(const unsigned char* first, int num,
                                  ostream* os, int)
{
  // No need to swap segments of 1 byte.
  os->write(reinterpret_cast<const char*>(first),
            num*static_cast<int>(sizeof(unsigned char)));
}
template <class T>
inline void vesByteSwapRangeWrite(const T* first, int num,
                                  ostream* os, long)
{
  // Swap and write one value at a time.  We do not need to do this in
  // blocks because the file stream is already buffered.
  const T* last = first + num;
  for(const T* p=first; p != last; ++p)
  {
    // Use a union to avoid breaking C++ aliasing rules.
    union { T value; char data[sizeof(T)]; } temp = {*p};
    vesByteSwapper<sizeof(T)>::Swap(temp.data);
    os->write(temp.data, sizeof(T));
  }
}

//----------------------------------------------------------------------------
// Define swap functions for each endian-ness.
#if defined(VTK_WORDS_BIGENDIAN)
template <class T> inline void vesByteSwapBE(T*) {}
template <class T> inline void vesByteSwapBERange(T*, int) {}
template <class T>
inline bool vesByteSwapBERangeWrite(const T* p, int num, FILE* f)
{
  size_t status=fwrite(p, sizeof(T), static_cast<size_t>(num), f);
  return status==static_cast<size_t>(num);
}
template <class T>
inline void vesByteSwapBERangeWrite(const T* p, int num, ostream* os)
{
  os->write((char*)p, sizeof(T)*num);
}
template <class T> inline void vesByteSwapLE(T* p)
{
  vesByteSwapper<sizeof(T)>::Swap(reinterpret_cast<char*>(p));
}
template <class T> inline void vesByteSwapLERange(T* p, int num)
{
  vesByteSwapRange(p, num);
}
template <class T>
inline bool vesByteSwapLERangeWrite(const T* p, int num, FILE* f)
{
  return vesByteSwapRangeWrite(p, num, f, 1);
}
template <class T>
inline void vesByteSwapLERangeWrite(const T* p, int num, ostream* os)
{
  vesByteSwapRangeWrite(p, num, os, 1);
}
#else
template <class T> inline void vesByteSwapBE(T* p)
{
  vesByteSwapper<sizeof(T)>::Swap(reinterpret_cast<char*>(p));
}
template <class T> inline void vesByteSwapBERange(T* p, int num)
{
  vesByteSwapRange(p, num);
}
template <class T>
inline bool vesByteSwapBERangeWrite(const T* p, int num, FILE* f)
{
  return vesByteSwapRangeWrite(p, num, f, 1);
}
template <class T>
inline void vesByteSwapBERangeWrite(const T* p, int num, ostream* os)
{
  vesByteSwapRangeWrite(p, num, os, 1);
}
template <class T> inline void vesByteSwapLE(T*) {}
template <class T> inline void vesByteSwapLERange(T*, int) {}
template <class T>
inline bool vesByteSwapLERangeWrite(const T* p, int num, FILE* f)
{
  size_t status=fwrite(p, sizeof(T), static_cast<size_t>(num), f);
  return status==static_cast<size_t>(num);
}
template <class T>
inline void vesByteSwapLERangeWrite(const T* p, int num, ostream* os)
{
  os->write(reinterpret_cast<const char*>(p),
            static_cast<int>(sizeof(T))*num);
}
#endif

//----------------------------------------------------------------------------
#define VTK_BYTE_SWAP_IMPL(T)                                                   \
  void vesByteSwap::SwapLE(T* p) { vesByteSwapLE(p); }                          \
  void vesByteSwap::SwapBE(T* p) { vesByteSwapBE(p); }                          \
  void vesByteSwap::SwapLERange(T* p, int num)                            \
{ vesByteSwapLERange(p, num); }                                             \
  void vesByteSwap::SwapBERange(T* p, int num)                            \
{ vesByteSwapBERange(p, num); }                                             \
  bool vesByteSwap::SwapLERangeWrite(const T* p, int num, FILE* file)     \
{ return vesByteSwapLERangeWrite(p, num, file); }                                  \
  bool vesByteSwap::SwapBERangeWrite(const T* p, int num, FILE* file)     \
{ return vesByteSwapBERangeWrite(p, num, file); }                                  \
  void vesByteSwap::SwapLERangeWrite(const T* p, int num, ostream* os)    \
{ vesByteSwapLERangeWrite(p, num, os); }                                    \
  void vesByteSwap::SwapBERangeWrite(const T* p, int num, ostream* os)    \
{ vesByteSwapBERangeWrite(p, num, os); }
VTK_BYTE_SWAP_IMPL(float)
VTK_BYTE_SWAP_IMPL(double)
VTK_BYTE_SWAP_IMPL(char)
VTK_BYTE_SWAP_IMPL(short)
VTK_BYTE_SWAP_IMPL(int)
VTK_BYTE_SWAP_IMPL(long)
VTK_BYTE_SWAP_IMPL(signed char)
VTK_BYTE_SWAP_IMPL(unsigned char)
VTK_BYTE_SWAP_IMPL(unsigned short)
VTK_BYTE_SWAP_IMPL(unsigned int)
VTK_BYTE_SWAP_IMPL(unsigned long)
#if defined(VTK_IMPL_USE_LONG_LONG)
VTK_BYTE_SWAP_IMPL(long long)
VTK_BYTE_SWAP_IMPL(unsigned long long)
#endif
#if defined(VTK_IMPL_USE___INT64)
VTK_BYTE_SWAP_IMPL(__int64)
VTK_BYTE_SWAP_IMPL(unsigned __int64)
#endif
#undef VTK_BYTE_SWAP_IMPL

typedef short vesByteSwapType2;
typedef int vesByteSwapType4;
typedef double vesByteSwapType8;

//----------------------------------------------------------------------------
#define VTK_BYTE_SWAP_SIZE(S)                                                   \
  void vesByteSwap::Swap##S##LE(void* p)                                        \
{ vesByteSwap::SwapLE(static_cast<vesByteSwapType##S*>(p)); }               \
  void vesByteSwap::Swap##S##BE(void* p)                                        \
{ vesByteSwap::SwapBE(static_cast<vesByteSwapType##S*>(p)); }               \
  void vesByteSwap::Swap##S##LERange(void* p, int n)                            \
{ vesByteSwap::SwapLERange(static_cast<vesByteSwapType##S*>(p), n); }       \
  void vesByteSwap::Swap##S##BERange(void* p, int n)                            \
{ vesByteSwap::SwapBERange(static_cast<vesByteSwapType##S*>(p), n); }       \
  bool vesByteSwap::SwapWrite##S##LERange(const void* p, int n, FILE* f)        \
{ return vesByteSwap::SwapLERangeWrite(static_cast<const vesByteSwapType##S*>(p),  \
  n, f); }                                    \
  bool vesByteSwap::SwapWrite##S##BERange(const void* p, int n, FILE* f)        \
{ return vesByteSwap::SwapBERangeWrite(static_cast<const vesByteSwapType##S*>(p),  \
  n, f); }                                    \
  void vesByteSwap::SwapWrite##S##LERange(const void* p, int n, ostream* os)    \
{ vesByteSwap::SwapLERangeWrite(static_cast<const vesByteSwapType##S*>(p),  \
  n, os); }                                   \
  void vesByteSwap::SwapWrite##S##BERange(const void* p, int n, ostream* os)    \
{ vesByteSwap::SwapBERangeWrite(static_cast<const vesByteSwapType##S*>(p),  \
  n, os); }
VTK_BYTE_SWAP_SIZE(2)
VTK_BYTE_SWAP_SIZE(4)
VTK_BYTE_SWAP_SIZE(8)
#undef VTK_BYTE_SWAP_SIZE

//----------------------------------------------------------------------------
// Swaps the bytes of a buffer.  Uses an arbitrary word size, but
// assumes the word size is divisible by two.
void vesByteSwap::SwapVoidRange(void *buffer, int numWords, int wordSize)
{
  unsigned char temp, *out, *buf;
  int idx1, idx2, inc, half;

  half = wordSize / 2;
  inc = wordSize - 1;
  buf = static_cast<unsigned char *>(buffer);

  for (idx1 = 0; idx1 < numWords; ++idx1)
  {
    out = buf + inc;
    for (idx2 = 0; idx2 < half; ++idx2)
    {
      temp = *out;
      *out = *buf;
      *buf = temp;
      ++buf;
      --out;
    }
    buf += half;
  }
}
