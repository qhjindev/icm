
// ============================================================================
// = FILENAME
//   IcmStream.h
//
// = DESCRIPTION
//   Common Data Representation (CDR) marshaling and demarshaling
//   classes.
//
//   This implementation was inspired in the CDR class in SunSoft's
//   IIOP engine, but has a completely different implementation and a
//   different interface too.
//
//   The current implementation assumes that the host has 1-byte,
//   2-byte and 4-byte integral types, and that it has single
//   precision and double precision IEEE floats.
//   Those assumptions are pretty good these days, with Crays beign
//   the only known exception.
//
// = AUTHORS
//   Aniruddha Gokhale <gokhale@cs.wustl.edu> and Carlos O'Ryan
//   <coryan@cs.wustl.edu> for the original implementation in TAO.
//   ACE version by Jeff Parsons <parsons@cs.wustl.edu>
//   and Istvan Buki <istvan.buki@euronet.be>.
//   Codeset translation by Jim Rogers (jrogers@viasoft.com) and
//   Carlos O'Ryan <coryan@cs.wustl.edu>
//
// ============================================================================

#ifndef ICM_STREAM_H
#define ICM_STREAM_H

#include <string>
#include <vector>
#include "os/OS.h"
#include "icc/MessageBlock.h"
//#include "icm/Proxy.h"
//#include "ace/SString.h"

namespace IcmProxy
{
	class Object;
}

#if !defined (ACE_LACKS_PRAGMA_ONCE)
# pragma once
#endif /* ACE_LACKS_PRAGMA_ONCE */

// Stuff used by the ACE CDR classes. Watch these values... they're also used
// in the ACE_CDR Byte_Order enum below.
#if defined ACE_LITTLE_ENDIAN
#  define ACE_CDR_BYTE_ORDER 1
// little endian encapsulation byte order has value = 1
#else  /* ! ACE_LITTLE_ENDIAN */
#  define ACE_CDR_BYTE_ORDER 0
// big endian encapsulation byte order has value = 0
#endif /* ! ACE_LITTLE_ENDIAN */

class ACE_Char_Codeset_Translator;
class ACE_WChar_Codeset_Translator;

class ACE_CDR
{
  // = TITLE
  //   Keep constants and some routines common to both Output and
  //   Input CDR streams.
  //
public:
  // = Constants defined by the CDR protocol.
  // By defining as many of these constants as possible as enums we
  // ensure they get inlined and avoid pointless static memory
  // allocations.

  enum
  {
    // Note that some of these get reused as part of the standard
    // binary format: unsigned is the same size as its signed cousin,
    // float is LONG_SIZE, and double is LONGLONG_SIZE.

    OCTET_SIZE = 1,
    SHORT_SIZE = 2,
    //add INT_SIZE and change LONG_SIZE to 8 by qinghua at 09/10/13
    INT_SIZE = 4,
    LONG_SIZE = 8,
    LONGLONG_SIZE = 8,
    LONGDOUBLE_SIZE = 16,

    OCTET_ALIGN = 1,
    SHORT_ALIGN = 2,
    //add INT_SIZE ,change LONG_SIZE to 8 by qinghua at 09/10/13
    INT_ALIGN = 4,
    LONG_ALIGN = 8,
    LONGLONG_ALIGN = 8,
    LONGDOUBLE_ALIGN = 8,
    // Note how the CORBA LongDouble alignment requirements do not
    // match its size...

    MAX_ALIGNMENT = 8,
    // Maximal CDR 1.1 alignment: "quad precision" FP (i.e. "CDR::Long
    // double", size as above).

    DEFAULT_BUFSIZE = ACE_DEFAULT_CDR_BUFSIZE,
    // The default buffer size.
    // @@ TODO We want to add options to control this
    // default value, so this constant should be read as the default
    // default value ;-)

    EXP_GROWTH_MAX = ACE_DEFAULT_CDR_EXP_GROWTH_MAX,
    // The buffer size grows exponentially until it reaches this size;
    // afterwards it grows linearly using the next constant

    LINEAR_GROWTH_CHUNK = ACE_DEFAULT_CDR_LINEAR_GROWTH_CHUNK
    // Once exponential growth is ruled out the buffer size increases
    // in chunks of this size, note that this constants have the same
    // value right now, but it does not need to be so.
  };

  static void swap_2 (const char *orig, char *target);
  static void swap_4 (const char *orig, char *target);
  static void swap_8 (const char *orig, char *target);
  static void swap_16 (const char *orig, char *target);
  // Do byte swapping for each basic IDL type size.  There exist only
  // routines to put byte, halfword (2 bytes), word (4 bytes),
  // doubleword (8 bytes) and quadword (16 byte); because those are
  // the IDL basic type sizes.

  static void mb_align (MessageBlock *mb);
  // Align the message block to ACE_CDR::MAX_ALIGNMENT,
  // set by the CORBA spec at 8 bytes.

  static size_t first_size (size_t minsize);
  // Compute the size of the smallest buffer that can contain at least
  // <minsize> bytes.
  // To understand how a "best fit" is computed look at the
  // algorithm in the code.
  // Basically the buffers grow exponentially, up to a certain point,
  // then the buffer size grows linearly.
  // The advantage of this algorithm is that is rapidly grows to a
  // large value, but does not explode at the end.

  static size_t next_size (size_t minsize);
  // Compute not the smallest, but the second smallest buffer that
  // will fir <minsize> bytes.

  static int grow (MessageBlock *mb, size_t minsize);
  // Increase the capacity of mb to contain at least <minsize> bytes.
  // If <minsize> is zero the size is increased by an amount at least
  // large enough to contain any of the basic IDL types.  Return -1 on
  // failure, 0 on success.

  static void consolidate (MessageBlock *dst,
                          const MessageBlock *src);
  // Copy a message block chain into a single message block,
  // preserving the alignment of the original stream.

  static size_t total_length (const MessageBlock *begin,
                              const MessageBlock *end);

  // Definitions of the IDL basic types, for use in the CDR
  // classes. The cleanest way to avoid complaints from all compilers
  // is to define them all.
  #if defined (CHORUS) && defined (ghs) && !defined (__STANDARD_CXX)
    // This is non-compliant, but a nasty bout with
    // Green Hills C++68000 1.8.8 forces us into it.
    typedef unsigned long Boolean;
  #else  /* ! (CHORUS && ghs 1.8.8) */
    typedef u_char Boolean;
  #endif /* ! (CHORUS && ghs 1.8.8) */

  typedef u_char Octet;
  typedef char Char;
  typedef OS::WChar WChar;
  typedef ACE_INT16 Short;
  typedef ACE_UINT16 UShort;
  typedef ACE_INT32 Int;
  typedef ACE_UINT32 UInt;
  //change Long size to 64 bit by qinghua at 09/10/13
  typedef ACE_INT64 Long;
  typedef ACE_UINT64 ULong;
  typedef ACE_UINT64 ULongLong;

  # if    (defined (_MSC_VER) && (_MSC_VER >= 900)) \
          || (defined (__BORLANDC__) && (__BORLANDC__ >= 0x530))
      typedef __int64 LongLong;
  # elif ACE_SIZEOF_LONG == 8 && !defined(_CRAYMPP)
      typedef long LongLong;
  # elif ACE_SIZEOF_LONG_LONG == 8 && !defined (ACE_LACKS_LONGLONG_T)
  #   if defined (sun) && !defined (ACE_LACKS_U_LONGLONG_T) && !defined (__KCC)
        // sun #defines u_longlong_t, maybe other platforms do also.
        // Use it, at least with g++, so that its -pedantic doesn't
        // complain about no ANSI C++ long long.
        typedef longlong_t LongLong;
  #   else
        // LynxOS 2.5.0 and Linux don't have u_longlong_t.
        typedef long long LongLong;
  #   endif /* sun */
  # else  /* no native 64 bit integer type */

      // If "long long" isn't native, programs can't use these data
      // types in normal arithmetic expressions.  If any particular
      // application can cope with the loss of range, it can define
      // conversion operators itself.
  #   if defined (ACE_BIG_ENDIAN)
        struct LongLong { ACE_CDR::Long h, l; };
  #   else
        struct LongLong { ACE_CDR::Long l, h; };
  #   endif /* ! ACE_BIG_ENDIAN */
  # endif /* no native 64 bit integer type */

  # if ACE_SIZEOF_FLOAT == 4
      typedef float Float;
  # else  /* ACE_SIZEOF_FLOAT != 4 */
      struct Float
      {
  #     if ACE_SIZEOF_INT == 4
          // Use u_int to get word alignment.
          u_int f;
  #     else  /* ACE_SIZEOF_INT != 4 */
          // Applications will probably have trouble with this.
          char f[4];
  #       if defined(_UNICOS) && !defined(_CRAYMPP)
            Float (void);
            Float (const float &init);
            Float & operator= (const float &rhs);
            int operator!= (const Float &rhs) const;
  #       endif /* _UNICOS */
  #     endif /* ACE_SIZEOF_INT != 4 */
      };
  # endif /* ACE_SIZEOF_FLOAT != 4 */

  # if ACE_SIZEOF_DOUBLE == 8
      typedef double Double;
  # else  /* ACE_SIZEOF_DOUBLE != 8 */
      struct Double
      {
  #     if ACE_SIZEOF_LONG == 8
          // Use u_long to get word alignment.
          u_long f;
  #     else  /* ACE_SIZEOF_INT != 8 */
          // Applications will probably have trouble with this.
          char f[8];
  #     endif /* ACE_SIZEOF_INT != 8 */
      };
  # endif /* ACE_SIZEOF_DOUBLE != 8 */

    // 94-9-32 Appendix A defines a 128 bit floating point "long
    // double" data type, with greatly extended precision and four
    // more bits of exponent (compared to "double").  This is an IDL
    // extension, not yet standard.

  #  if   ACE_SIZEOF_LONG_DOUBLE == 16
    typedef long double LongDouble;
  #  else
  #    define NONNATIVE_LONGDOUBLE
    struct ACE_Export LongDouble
    {
      char ld[16];
      int operator== (const LongDouble &rhs) const;
      int operator!= (const LongDouble &rhs) const;
      // @@ also need other comparison operators.
    };
  #  endif /* ACE_SIZEOF_LONG_DOUBLE != 16 */

};

class InputStream;

class OutputStream
{
  // = TITLE
  //   A CDR stream for writing, i.e. for marshalling.
  //
  // = DESCRIPTION
  //   This class is based on the the CORBA spec for Java (98-02-29),
  //   java class omg.org.CORBA.portable.OutputStream.  It diverts in
  //   a few ways:
  //     + Operations taking arrays don't have offsets, because in C++
  //       it is easier to describe an array starting from x+offset.
  //     + Operations return an error status, because exceptions are
  //       not widely available in C++ (yet).
  //
public:
  friend class ACE_Char_Codeset_Translator;
  friend class ACE_WChar_Codeset_Translator;
  // The Codeset translators need access to some private members to
  // efficiently marshal arrays
  friend class InputStream;
  // For reading from an output CDR stream.

  OutputStream (size_t size = 0,
                 int byte_order = ACE_CDR_BYTE_ORDER,
                 size_t memcpy_tradeoff =
                   ACE_DEFAULT_CDR_MEMCPY_TRADEOFF);
  // Default constructor, allocates <size> bytes in the internal
  // buffer, if <size> == 0 it allocates the default size.

  OutputStream (char *data,
                 size_t size,
                 int byte_order = ACE_CDR_BYTE_ORDER,
                 size_t memcpy_tradeoff=
                   ACE_DEFAULT_CDR_MEMCPY_TRADEOFF);
  // Build a CDR stream with an initial buffer, it will *not* remove
  // <data>, since it did not allocated it.

  OutputStream (MessageBlock *data,
                 int byte_order = ACE_CDR_BYTE_ORDER,
                 size_t memcpy_tradeoff=
                   ACE_DEFAULT_CDR_MEMCPY_TRADEOFF);
  // Build a CDR stream with an initial Message_Block chain, it will
  // *not* remove <data>, since it did not allocate it.

  OutputStream (const InputStream& rhs);

  ~OutputStream (void);
  // destructor

  // = Special types.
  // These are needed for insertion and extraction of booleans,
  // octets, chars, and bounded strings.

  struct ACE_Export from_boolean
  {
    from_boolean (ACE_CDR::Boolean b);
    ACE_CDR::Boolean val_;
  };

  struct ACE_Export from_octet
  {
    from_octet (ACE_CDR::Octet o);
    ACE_CDR::Octet val_;
  };

  struct ACE_Export from_char
  {
    from_char (ACE_CDR::Char c);
    ACE_CDR::Char val_;
  };

  struct ACE_Export from_wchar
  {
    from_wchar (ACE_CDR::WChar wc);
    ACE_CDR::WChar val_;
  };

  struct ACE_Export from_string
  {
    from_string (ACE_CDR::Char* s,
                 ACE_CDR::ULong b,
                 ACE_CDR::Boolean nocopy = 0);
    ACE_CDR::Char *val_;
    ACE_CDR::ULong bound_;
    ACE_CDR::Boolean nocopy_;
  };

  struct ACE_Export from_wstring
  {
    from_wstring (ACE_CDR::WChar* ws,
                  ACE_CDR::ULong b,
                  ACE_CDR::Boolean nocopy = 0);
    ACE_CDR::WChar *val_;
    ACE_CDR::ULong bound_;
    ACE_CDR::Boolean nocopy_;
  };

  // = We have one method per basic IDL type....
  // They return 0 on failure and 1 on success.
  ACE_CDR::Boolean write_boolean (ACE_CDR::Boolean x);
  ACE_CDR::Boolean write_char (ACE_CDR::Char x);
  ACE_CDR::Boolean write_wchar (ACE_CDR::WChar x);
  ACE_CDR::Boolean write_octet (ACE_CDR::Octet x);
  ACE_CDR::Boolean write_short (ACE_CDR::Short x);
  ACE_CDR::Boolean write_ushort (ACE_CDR::UShort x);
  ACE_CDR::Boolean write_int (ACE_CDR::Int x);
  ACE_CDR::Boolean write_uint (ACE_CDR::UInt x);
  //ACE_CDR::Boolean write_enum (enum Enum x);
  ACE_CDR::Boolean write_long (ACE_CDR::Long x);
  ACE_CDR::Boolean write_ulong (ACE_CDR::ULong x);
  ACE_CDR::Boolean write_longlong (const ACE_CDR::LongLong &x);
  ACE_CDR::Boolean write_ulonglong (const ACE_CDR::ULongLong &x);
  ACE_CDR::Boolean write_float (ACE_CDR::Float x);
  ACE_CDR::Boolean write_double (const ACE_CDR::Double &x);
  ACE_CDR::Boolean write_longdouble (const ACE_CDR::LongDouble &x);

  // = For string we offer methods that accept a precomputed length.
  ACE_CDR::Boolean write_string (const ACE_CDR::Char *x);
  ACE_CDR::Boolean write_string (ACE_CDR::UInt len,
                                 const ACE_CDR::Char *x);
  ACE_CDR::Boolean write_string (const std::string &x);
  ACE_CDR::Boolean write_wstring (const ACE_CDR::WChar *x);
  ACE_CDR::Boolean write_wstring (ACE_CDR::UInt length,
                                  const ACE_CDR::WChar *x);

  ACE_CDR::Boolean write_obj (IcmProxy::Object *o);

  // = We add one method to write arrays of basic IDL types.
  // Note: the portion written starts at <x> and ends at <x + length>.
  // The length is *NOT* stored into the CDR stream.
  ACE_CDR::Boolean write_boolean_array (const ACE_CDR::Boolean *x,
                                        ACE_CDR::UInt length);
  ACE_CDR::Boolean write_char_array (const ACE_CDR::Char *x,
                                     ACE_CDR::UInt length);
  ACE_CDR::Boolean write_wchar_array (const ACE_CDR::WChar* x,
                                      ACE_CDR::UInt length);
  ACE_CDR::Boolean write_octet_array (const ACE_CDR::Octet* x,
                                      ACE_CDR::UInt length);
  ACE_CDR::Boolean write_short_array (const ACE_CDR::Short *x,
                                      ACE_CDR::UInt length);
  ACE_CDR::Boolean write_ushort_array (const ACE_CDR::UShort *x,
                                       ACE_CDR::UInt length);
  ACE_CDR::Boolean write_int_array (const ACE_CDR::Int *x,
                                     ACE_CDR::UInt length);
  ACE_CDR::Boolean write_long_array (const ACE_CDR::Long *x,
                                     ACE_CDR::UInt length);
  ACE_CDR::Boolean write_ulong_array (const ACE_CDR::ULong *x,
                                      ACE_CDR::UInt length);
  ACE_CDR::Boolean write_longlong_array (const ACE_CDR::LongLong* x,
                                         ACE_CDR::UInt length);
  ACE_CDR::Boolean write_ulonglong_array (const ACE_CDR::ULongLong *x,
                                          ACE_CDR::UInt length);
  ACE_CDR::Boolean write_float_array (const ACE_CDR::Float *x,
                                      ACE_CDR::UInt length);
  ACE_CDR::Boolean write_double_array (const ACE_CDR::Double *x,
                                       ACE_CDR::UInt length);
  ACE_CDR::Boolean write_longdouble_array (const ACE_CDR::LongDouble* x,
                                           ACE_CDR::UInt length);

  ACE_CDR::Boolean write_octet_array_mb (const MessageBlock* mb);
  // Write an octet array contained inside a MB, this can be optimized
  // to minimize copies.


  ACE_CDR::Boolean write_byte_seq(const std::vector<Byte>& v);

  ACE_CDR::Boolean write_bool_seq(const std::vector<bool>& v);

  ACE_CDR::Boolean write_short_seq(const std::vector<Short>& v);

  ACE_CDR::Boolean write_int_seq(const std::vector<Int>& v);

  ACE_CDR::Boolean write_long_seq(const std::vector<Long>& v);

  ACE_CDR::Boolean write_float_seq(const std::vector<Float>& v);

  ACE_CDR::Boolean write_double_seq(const std::vector<Double>& v);

  ACE_CDR::Boolean write_string_seq(const std::vector<std::string>& v);


  // = We have one method per basic IDL type....
  // They return 0 on failure and 1 on success.
  ACE_CDR::Boolean append_boolean (InputStream &);
  ACE_CDR::Boolean append_char (InputStream &);
  ACE_CDR::Boolean append_wchar (InputStream &);
  ACE_CDR::Boolean append_octet (InputStream &);
  ACE_CDR::Boolean append_short (InputStream &);
  ACE_CDR::Boolean append_ushort (InputStream &);
  ACE_CDR::Boolean append_long (InputStream &);
  ACE_CDR::Boolean append_ulong (InputStream &);
  ACE_CDR::Boolean append_longlong (InputStream &);
  ACE_CDR::Boolean append_ulonglong (InputStream &);
  ACE_CDR::Boolean append_float (InputStream &);
  ACE_CDR::Boolean append_double (InputStream &);
  ACE_CDR::Boolean append_longdouble (InputStream &);

  ACE_CDR::Boolean append_wstring (InputStream &);
  ACE_CDR::Boolean append_string (InputStream &);

  int good_bit (void) const;
  // Returns 0 if an error has ocurred, the only expected error is to
  // run out of memory.

  void reset (void);
  // Reuse the CDR stream to write on the old buffer.

  size_t total_length (void) const;
  // Add the length of each message block in the chain.

  const MessageBlock *begin (void) const;
  // Return the start of the message block chain for this CDR stream.
  // NOTE: The complete CDR stream is represented by a chain of
  // message blocks.

  const MessageBlock *end (void) const;
  // Return the last message in the chain that is is use.

  const MessageBlock *current (void) const;
  // Return the <current_> message block in chain.

  const char *buffer (void) const;
  // Access the underlying buffer (read only).

  size_t length (void) const;
  // Return the start and size of the internal buffer.  NOTE: This
  // methods only return information about the first block in the
  // chain.

  int align_write_ptr (size_t alignment);
  // Utility function to allow the user more flexibility.
  // Pads the stream up to the nearest <alignment>-byte boundary.
  // Argument MUST be a power of 2.
  // Returns 0 on success and -1 on failure.

  ACE_Char_Codeset_Translator *char_translator (void) const;
  ACE_WChar_Codeset_Translator *wchar_translator (void) const;
  // Access the codeset translators. They can be nil!

  size_t current_alignment (void) const;
  // Return alignment of the wrPtr(), with respect to the start of
  // the CDR stream.  This is not the same as the alignment of
  // current->wrPtr()!

  int do_byte_swap (void) const;
  // If non-zero then this stream is writing in non-native byte order,
  // this is only meaningful if ACE_ENABLE_SWAP_ON_WRITE is defined.
private:
  OutputStream (const OutputStream& rhs);
  OutputStream& operator= (const OutputStream& rhs);
  // disallow copying...

  ACE_CDR::Boolean write_1 (const ACE_CDR::Octet *x);
  ACE_CDR::Boolean write_2 (const ACE_CDR::UShort *x);
  ACE_CDR::Boolean write_4 (const ACE_CDR::UInt *x);
  ACE_CDR::Boolean write_8 (const ACE_CDR::ULong *x);
  ACE_CDR::Boolean write_16 (const ACE_CDR::LongDouble *x);

  ACE_CDR::Boolean write_array (const void *x,
                                size_t size,
                                size_t align,
                                ACE_CDR::UInt length);
  // write an array of <length> elements, each of <size> bytes and the
  // start aligned at a multiple of <align>. The elements are assumed
  // to be packed with the right alignment restrictions.  It is mostly
  // designed for buffers of the basic types.
  //
  // This operation uses <memcpy>; as explained above it is expected
  // that using assignment is faster that <memcpy> for one element,
  // but for several elements <memcpy> should be more efficient, it
  // could be interesting to find the break even point and optimize
  // for that case, but that would be too platform dependent.

  int adjust (size_t size,
              char *&buf);
  // Returns (in <buf>) the next position in the buffer aligned to
  // <size>, it advances the Message_Block wrPtr past the data
  // (i.e. <buf> + <size>). If necessary it grows the Message_Block
  // buffer.  Sets the good_bit to 0 and returns a -1 on failure.

  int adjust (size_t size,
              size_t align,
              char *&buf);
  // As above, but now the size and alignment requirements may be
  // different.

  int grow_and_adjust (size_t size,
                       size_t align,
                       char *&buf);
  // Grow the CDR stream. When it returns <buf> contains a pointer to
  // memory in the CDR stream, with at least <size> bytes ahead of it
  // and aligned to an <align> boundary. It moved the <wrPtr> to <buf
  // + size>.

private:
  MessageBlock start_;
  // The start of the chain of message blocks.

  MessageBlock *current_;
  // The current block in the chain were we are writing.

  int current_is_writable_;
  // Is the current block writable.  When we steal a buffer from the
  // user and just chain it into the message block we are not supposed
  // to write on it, even if it is past the start and end of the
  // buffer.

#if !defined (ICC_LACKS_CDR_ALIGNMENT)
  size_t current_alignment_;
#endif //ICC_LACKS_CDR_ALIGNMENT
  // The current alignment as measured from the start of the buffer.
  // Usually this coincides with the alignment of the buffer in
  // memory, but, when we chain another buffer this "quasi invariant"
  // is broken.
  // The current_alignment is used to readjust the buffer following
  // the stolen message block.

  int do_byte_swap_;
  // If not zero swap bytes at writing so the created CDR stream byte
  // order does *not* match the machine byte order.  The motivation
  // for such a beast is that in some setting a few (fast) machines
  // can be serving hundreds of slow machines with the opposite byte
  // order, so it makes sense (as a load balancing device) to put the
  // responsability in the writers.  THIS IS NOT A STANDARD IN CORBA,
  // USE AT YOUR OWN RISK

  int good_bit_;
  // Set to 0 when an error ocurrs.

  size_t memcpy_tradeoff_;
  // Break-even point for copying.

protected:
  ACE_Char_Codeset_Translator *char_translator_;
  ACE_WChar_Codeset_Translator *wchar_translator_;
  // If not nil, invoke for translation of character and string data.
};

// ****************************************************************

class InputStream
{
  // = TITLE
  //   A CDR stream for reading, i.e. for demarshalling.
  //
  // = DESCRIPTION
  //   This class is based on the the CORBA spec for Java (98-02-29),
  //   java class omg.org.CORBA.portable.InputStream.  It diverts in a
  //   few ways:
  //     + Operations to retrieve basic types take parameters by
  //       reference.
  //     + Operations taking arrays don't have offsets, because in C++
  //       it is easier to describe an array starting from x+offset.
  //     + Operations return an error status, because exceptions are
  //       not widely available in C++ (yet).
  //
public:
  friend class OutputStream;
  friend class ACE_Char_Codeset_Translator;
  friend class ACE_WChar_Codeset_Translator;
  // The translator need privileged access to efficiently demarshal
  // arrays and the such

  InputStream (const char *buf,
                size_t bufsiz,
                int byte_order = ACE_CDR_BYTE_ORDER);
  // Create an input stream from an arbitrary buffer, care must be
  // exercised wrt alignment, because this contructor will *not* work
  // if the buffer is unproperly aligned.

  InputStream (size_t bufsiz = ACE_CDR::DEFAULT_BUFSIZE,
                int byte_order = ACE_CDR_BYTE_ORDER);
  // Create an empty input stream. The caller is responsible for
  // putting the right data and providing the right alignment.

  InputStream (const MessageBlock *data,
                int byte_order = ACE_CDR_BYTE_ORDER);
  // Create an input stream from an MessageBlock

  InputStream (DataBlock *data,
                int byte_order = ACE_CDR_BYTE_ORDER);
  // Create an input stream from an DataBlock

  InputStream (const InputStream& rhs);
  InputStream& operator= (const InputStream& rhs);
  // These make a copy of the current stream state, but do not copy
  // the internal buffer, so the same stream can be read multiple
  // times efficiently.

  InputStream (const InputStream& rhs,
                size_t size,
                ACE_CDR::Long offset);
  // When interpreting indirected TypeCodes it is useful to make a
  // "copy" of the stream starting in the new position.

  InputStream (const InputStream& rhs,
                size_t size);
  // This creates an encapsulated stream, the first byte must be (per
  // the spec) the byte order of the encapsulation.

  InputStream (const OutputStream& rhs);
  // Create an input CDR from an output CDR.

  struct ACE_Export Transfer_Contents
  {
    // Helper class to transfer the contents from one input CDR to
    // another without requiring any extra memory allocations, data
    // copies or too many temporaries.
    Transfer_Contents (InputStream &rhs);

    InputStream &rhs_;
  };
  InputStream (Transfer_Contents rhs);
  // Transfer the contents from <rhs> to a new CDR

  ~InputStream (void);
  // Destructor

  // = Special types.
  // These extract octets, chars, booleans, and bounded strings

  struct ACE_Export to_boolean
  {
    to_boolean (ACE_CDR::Boolean &b);
    ACE_CDR::Boolean &ref_;
  };

  struct ACE_Export to_char
  {
    to_char (ACE_CDR::Char &c);
    ACE_CDR::Char &ref_;
  };

  struct ACE_Export to_wchar
  {
    to_wchar (ACE_CDR::WChar &wc);
    ACE_CDR::WChar &ref_;
  };

  struct ACE_Export to_octet
  {
    to_octet (ACE_CDR::Octet &o);
    ACE_CDR::Octet &ref_;
  };

  struct ACE_Export to_string
  {
    to_string (ACE_CDR::Char *&s,
               ACE_CDR::ULong b);
    ACE_CDR::Char *&val_;
    ACE_CDR::ULong bound_;
  };

  struct ACE_Export to_wstring
  {
    to_wstring (ACE_CDR::WChar *&ws,
                ACE_CDR::ULong b);
    ACE_CDR::WChar *&val_;
    ACE_CDR::ULong bound_;
  };

  // = We have one method per basic IDL type....
  // They return 0 on failure and 1 on success.
  ACE_CDR::Boolean read_boolean (ACE_CDR::Boolean& x);
  ACE_CDR::Boolean read_char (ACE_CDR::Char &x);
  ACE_CDR::Boolean read_wchar (ACE_CDR::WChar& x);
  ACE_CDR::Boolean read_octet (ACE_CDR::Octet& x);
  ACE_CDR::Boolean read_short (ACE_CDR::Short &x);
  ACE_CDR::Boolean read_ushort (ACE_CDR::UShort &x);
  ACE_CDR::Boolean read_int (ACE_CDR::Int &x);
  ACE_CDR::Boolean read_uint (ACE_CDR::UInt &x);
  //ACE_CDR::Boolean read_enum (enum Enum &x);
  ACE_CDR::Boolean read_long (ACE_CDR::Long &x);
  ACE_CDR::Boolean read_ulong (ACE_CDR::ULong &x);
  ACE_CDR::Boolean read_longlong (ACE_CDR::LongLong& x);
  ACE_CDR::Boolean read_ulonglong (ACE_CDR::ULongLong& x);
  ACE_CDR::Boolean read_float (ACE_CDR::Float &x);
  ACE_CDR::Boolean read_double (ACE_CDR::Double &x);
  ACE_CDR::Boolean read_longdouble (ACE_CDR::LongDouble &x);

  ACE_CDR::Boolean read_string (ACE_CDR::Char *&x);
  ACE_CDR::Boolean read_string (std::string &x);
  ACE_CDR::Boolean read_wstring (ACE_CDR::WChar*& x);

  ACE_CDR::Boolean read_obj (IcmProxy::Object* &o);

  // = One method for each basic type...
  // The buffer <x> must be large enough to contain <length>
  // elements.
  // They return -1 on failure and 0 on success.
  ACE_CDR::Boolean read_boolean_array (ACE_CDR::Boolean* x,
                                       ACE_CDR::UInt length);
  ACE_CDR::Boolean read_boolean_array (std::vector<bool>& v,
                                       ACE_CDR::UInt length);
  ACE_CDR::Boolean read_char_array (ACE_CDR::Char *x,
                                    ACE_CDR::UInt length);
  ACE_CDR::Boolean read_wchar_array (ACE_CDR::WChar* x,
                                     ACE_CDR::UInt length);
  ACE_CDR::Boolean read_octet_array (ACE_CDR::Octet* x,
                                     ACE_CDR::UInt length);
  ACE_CDR::Boolean read_short_array (ACE_CDR::Short *x,
                                     ACE_CDR::UInt length);
  ACE_CDR::Boolean read_ushort_array (ACE_CDR::UShort *x,
                                      ACE_CDR::UInt length);

  ACE_CDR::Boolean read_int_array (ACE_CDR::Int *x,
                                    ACE_CDR::UInt length);

  ACE_CDR::Boolean read_long_array (ACE_CDR::Long *x,
                                    ACE_CDR::UInt length);
  ACE_CDR::Boolean read_ulong_array (ACE_CDR::ULong *x,
                                     ACE_CDR::UInt length);
  ACE_CDR::Boolean read_longlong_array (ACE_CDR::LongLong* x,
                                        ACE_CDR::UInt length);
  ACE_CDR::Boolean read_ulonglong_array (ACE_CDR::ULongLong* x,
                                         ACE_CDR::UInt length);
  ACE_CDR::Boolean read_float_array (ACE_CDR::Float *x,
                                     ACE_CDR::UInt length);
  ACE_CDR::Boolean read_double_array (ACE_CDR::Double *x,
                                      ACE_CDR::UInt length);
  ACE_CDR::Boolean read_longdouble_array (ACE_CDR::LongDouble* x,
                                          ACE_CDR::UInt length);

  ACE_CDR::Boolean read_byte_seq(std::vector<Byte>& v);

  ACE_CDR::Boolean read_bool_seq(std::vector<bool>& v);

  ACE_CDR::Boolean read_short_seq(std::vector<Short>& v);

  ACE_CDR::Boolean read_int_seq(std::vector<Int>& v);

  ACE_CDR::Boolean read_long_seq(std::vector<Long>& v);

  ACE_CDR::Boolean read_float_seq(std::vector<Float>& v);

  ACE_CDR::Boolean read_double_seq(std::vector<Double>& v);

  ACE_CDR::Boolean read_string_seq(std::vector<std::string>& v);


  // = We have one method per basic IDL type....
  // They return 0 on failure and 1 on success.
  ACE_CDR::Boolean skip_boolean (void);
  ACE_CDR::Boolean skip_char (void);
  ACE_CDR::Boolean skip_wchar (void);
  ACE_CDR::Boolean skip_octet (void);
  ACE_CDR::Boolean skip_short (void);
  ACE_CDR::Boolean skip_ushort (void);
  ACE_CDR::Boolean skip_long (void);
  ACE_CDR::Boolean skip_ulong (void);
  ACE_CDR::Boolean skip_longlong (void);
  ACE_CDR::Boolean skip_ulonglong (void);
  ACE_CDR::Boolean skip_float (void);
  ACE_CDR::Boolean skip_double (void);
  ACE_CDR::Boolean skip_longdouble (void);

  ACE_CDR::Boolean skip_wstring (void);
  ACE_CDR::Boolean skip_string (void);
  // The next field must be a string, this method skips it. It is
  // useful in parsing a TypeCode.
  // Return 0 on failure and 1 on success.

  ACE_CDR::Boolean skip_bytes (size_t n);
  // Skip <n> bytes in the CDR stream.
  // Return 0 on failure and 1 on success.

  int good_bit (void) const;
  // returns zero if a problem has been detected.

  const MessageBlock* start (void) const;
  // Return the start of the message block chain for this CDR stream.
  // NOTE: In the current implementation the chain has length 1, but
  // we are planning to change that.

  // = The following functions are useful to read the contents of the
  //   CDR stream from a socket or file.

  int grow (size_t newsize);
  // Grow the internal buffer, reset <rdPtr> to the first byte in the
  // new buffer that is properly aligned, and set <wrPtr> to <rdPtr>
  // + newsize

  void reset_byte_order (int byte_order);
  // After reading and partially parsing the contents the user can
  // detect a change in the byte order, this method will let him
  // change it.

  void reset (const MessageBlock *data,
              int byte_order);
  // Re-initialize the CDR stream, copying the contents of the chain
  // of message_blocks starting from <data>.

  MessageBlock *steal_contents (void);
  // Steal the contents from the currect CDR.

  void steal_from (InputStream &cdr);
  // Steal the contents of <cdr> and make a shallow copy into this
  // stream.

  void reset_contents (void);
  // Re-initialize the CDR stream, forgetting about the old contents
  // of the stream and allocating a new buffer (from the allocators).

  char* rdPtr (void);
  // Returns the current position for the rdPtr....

  size_t length (void) const;
  // Return how many bytes are left in the stream.

  int align_read_ptr (size_t alignment);
  // Utility function to allow the user more flexibility.
  // Skips up to the nearest <alignment>-byte boundary.
  // Argument MUST be a power of 2.
  // Returns 0 on success and -1 on failure.

  int do_byte_swap (void) const;
  // If non-zero then this stream is writing in non-native byte order,
  // this is only meaningful if ACE_ENABLE_SWAP_ON_WRITE is defined.

  int byte_order (void) const;
  // If <do_byte_swap> returns 1, this returns ACE_CDR_BYTE_ORDER else
  // it returns ~ACE_CDR_BYTE_ORDER.

  ACE_Char_Codeset_Translator *char_translator (void) const;
  ACE_WChar_Codeset_Translator *wchar_translator (void) const;
  // Access the codeset translators. They can be nil!

protected:
  MessageBlock start_;
  // The start of the chain of message blocks, even though in the
  // current version the chain always has length 1.

  int do_byte_swap_;
  // The CDR stream byte order does not match the one on the machine,
  // swapping is needed while reading.

  int good_bit_;
  // set to 0 when an error occurs.

  ACE_Char_Codeset_Translator *char_translator_;
  ACE_WChar_Codeset_Translator *wchar_translator_;
  // If not nil, invoke for translation of character and string data.

private:
  ACE_CDR::Boolean read_1 (ACE_CDR::Octet *x);
  ACE_CDR::Boolean read_2 (ACE_CDR::UShort *x);
  ACE_CDR::Boolean read_4 (ACE_CDR::UInt *x);
  ACE_CDR::Boolean read_8 (ACE_CDR::ULong *x);
  ACE_CDR::Boolean read_16 (ACE_CDR::LongDouble *x);

  // Several types can be read using the same routines, since TAO
  // tries to use native types with known size for each CORBA type.
  // We could use void* or char* to make the interface more
  // consistent, but using native types let us exploit the strict
  // alignment requirements of CDR streams and implement the
  // operations using asignment.

  ACE_CDR::Boolean read_array (void* x,
                               size_t size,
                               size_t align,
                               ACE_CDR::UInt length);
  // Read an array of <length> elements, each of <size> bytes and the
  // start aligned at a multiple of <align>. The elements are assumed
  // to be packed with the right alignment restrictions.  It is mostly
  // designed for buffers of the basic types.
  //
  // This operation uses <memcpy>; as explained above it is expected
  // that using assignment is faster that <memcpy> for one element,
  // but for several elements <memcpy> should be more efficient, it
  // could be interesting to find the break even point and optimize
  // for that case, but that would be too platform dependent.

  void rdPtr (size_t offset);
  // Move the rdPtr ahead by <offset> bytes.

  char* end (void);
  // Points to the continuation field of the current message block.

  int adjust (size_t size,
              char *&buf);
  // Returns (in <buf>) the next position in the buffer aligned to
  // <size>, it advances the Message_Block rdPtr past the data
  // (i.e. <buf> + <size>).  Sets the good_bit to 0 and returns a -1
  // on failure.

  int adjust (size_t size,
              size_t align,
              char *&buf);
  // As above, but now the size and alignment requirements may be
  // different.
};

// ****************************************************************

class ACE_Char_Codeset_Translator
{
  // = TITLE
  //   Codeset translation routines common to both Output and Input
  //   CDR streams.
  //
  // = DESCRIPTION
  //   This class is a base class for defining codeset translation
  //   routines to handle the character set translations required by
  //   both CDR Input streams and CDR Output streams.
  //
public:
  virtual ACE_CDR::Boolean read_char (InputStream&,
                                      ACE_CDR::Char&) = 0;
  // Read a single character from the stream, converting from the
  // stream codeset to the native codeset

  virtual ACE_CDR::Boolean read_string (InputStream&,
                                        ACE_CDR::Char *&) = 0;
  // Read a string from the stream, including the length, converting
  // the characters from the stream codeset to the native codeset

  virtual ACE_CDR::Boolean read_char_array (InputStream&,
                                            ACE_CDR::Char*,
                                            ACE_CDR::ULong) = 0;
  // Read an array of characters from the stream, converting the
  // characters from the stream codeset to the native codeset.

  virtual ACE_CDR::Boolean write_char (OutputStream&,
                                       ACE_CDR::Char) = 0;
  // Write a single character to the stream, converting from the
  // native codeset to the stream codeset

  virtual ACE_CDR::Boolean write_string (OutputStream&,
                                         ACE_CDR::ULong,
                                         const ACE_CDR::Char*) = 0;
  // Write a string to the stream, including the length, converting
  // from the native codeset to the stream codeset

  virtual ACE_CDR::Boolean write_char_array (OutputStream&,
                                             const ACE_CDR::Char*,
                                             ACE_CDR::ULong) = 0;
  // Write an array of characters to the stream, converting from the
  // native codeset to the stream codeset

protected:
  ACE_CDR::Boolean read_1 (InputStream& input,
                           ACE_CDR::Octet *x);
  ACE_CDR::Boolean write_1 (OutputStream& output,
                            const ACE_CDR::Octet *x);
  // Children have access to low-level routines because they cannot
  // use read_char or something similar (it would recurse).

  ACE_CDR::Boolean read_array (InputStream& input,
                               void* x,
                               size_t size,
                               size_t align,
                               ACE_CDR::UInt length);
  // Efficiently read <length> elements of size <size> each from
  // <input> into <x>; the data must be aligned to <align>.

  ACE_CDR::Boolean write_array (OutputStream& output,
                                const void *x,
                                size_t size,
                                size_t align,
                                ACE_CDR::UInt length);
  // Efficiently write <length> elements of size <size> from <x> into
  // <output>. Before inserting the elements enough padding is added
  // to ensure that the elements will be aligned to <align> in the
  // stream.

  int adjust (OutputStream& out,
              size_t size,
              size_t align,
              char *&buf);
  // Exposes the stream implementation of <adjust>, this is useful in
  // many cases to minimize memory allocations during marshaling.
  // On success <buf> will contain a contiguous area in the CDR stream
  // that can hold <size> bytes aligned to <align>.
  // Results

  void good_bit (OutputStream& out, int bit);
  // Used by derived classes to set errors in the CDR stream.
};

// ****************************************************************

class ACE_WChar_Codeset_Translator
{
  // = TITLE
  //   Codeset translation routines common to both Output and Input
  //   CDR streams.
  //
  // = DESCRIPTION
  //   This class is a base class for defining codeset translation
  //   routines to handle the character set translations required by
  //   both CDR Input streams and CDR Output streams.
  //
public:
  virtual ACE_CDR::Boolean read_wchar (InputStream&,
                                       ACE_CDR::WChar&) = 0;
  virtual ACE_CDR::Boolean read_wstring (InputStream&,
                                         ACE_CDR::WChar *&) = 0;
  virtual ACE_CDR::Boolean read_wchar_array (InputStream&,
                                             ACE_CDR::WChar*,
                                             ACE_CDR::ULong) = 0;
  virtual ACE_CDR::Boolean write_wchar (OutputStream&,
                                        ACE_CDR::WChar) = 0;
  virtual ACE_CDR::Boolean write_wstring (OutputStream&,
                                          ACE_CDR::ULong,
                                          const ACE_CDR::WChar*) = 0;
  virtual ACE_CDR::Boolean write_wchar_array (OutputStream&,
                                              const ACE_CDR::WChar*,
                                              ACE_CDR::ULong) = 0;

protected:
  ACE_CDR::Boolean read_1 (InputStream& input,
                           ACE_CDR::Octet *x);
  ACE_CDR::Boolean read_2 (InputStream& input,
                           ACE_CDR::UShort *x);
  ACE_CDR::Boolean read_4 (InputStream& input,
                           ACE_CDR::UInt *x);
  ACE_CDR::Boolean write_1 (OutputStream& output,
                            const ACE_CDR::Octet *x);
  ACE_CDR::Boolean write_2 (OutputStream& output,
                            const ACE_CDR::UShort *x);
  ACE_CDR::Boolean write_4 (OutputStream& output,
                            const ACE_CDR::UInt *x);
  // Children have access to low-level routines because they cannot
  // use read_char or something similar (it would recurse).

  ACE_CDR::Boolean read_array (InputStream& input,
                               void* x,
                               size_t size,
                               size_t align,
                               ACE_CDR::UInt length);
  // Efficiently read <length> elements of size <size> each from
  // <input> into <x>; the data must be aligned to <align>.

  ACE_CDR::Boolean write_array (OutputStream& output,
                                const void *x,
                                size_t size,
                                size_t align,
                                ACE_CDR::UInt length);
  // Efficiently write <length> elements of size <size> from <x> into
  // <output>. Before inserting the elements enough padding is added
  // to ensure that the elements will be aligned to <align> in the
  // stream.

  int adjust (OutputStream& out,
              size_t size,
              size_t align,
              char *&buf);
  // Exposes the stream implementation of <adjust>, this is useful in
  // many cases to minimize memory allocations during marshaling.
  // On success <buf> will contain a contiguous area in the CDR stream
  // that can hold <size> bytes aligned to <align>.
  // Results

  void good_bit (OutputStream& out, int bit);
  // Used by derived classes to set errors in the CDR stream.
};

#if defined (__ACE_INLINE__)
//# include "ace/CDR_Stream.i"
#else /* __ACE_INLINE__ */

// Not used by CORBA or TAO
extern ACE_Export ACE_CDR::Boolean operator<< (OutputStream &os,
                                               ACE_CDR::Char x);
// CDR output operators for primitive types

extern ACE_Export ACE_CDR::Boolean operator<< (OutputStream &os,
                                               ACE_CDR::Short x);
extern ACE_Export ACE_CDR::Boolean operator<< (OutputStream &os,
                                               ACE_CDR::UShort x);
extern ACE_Export ACE_CDR::Boolean operator<< (OutputStream &os,
                                               ACE_CDR::Int x);
extern ACE_Export ACE_CDR::Boolean operator<< (OutputStream &os,
                                               ACE_CDR::UInt x);
extern ACE_Export ACE_CDR::Boolean operator<< (OutputStream &os,
                                               ACE_CDR::Long x);
extern ACE_Export ACE_CDR::Boolean operator<< (OutputStream &os,
                                               ACE_CDR::ULong x);
extern ACE_Export ACE_CDR::Boolean operator<< (OutputStream &os,
                                               ACE_CDR::LongLong x);
extern ACE_Export ACE_CDR::Boolean operator<< (OutputStream &os,
                                               ACE_CDR::ULongLong x);
extern ACE_Export ACE_CDR::Boolean operator<< (OutputStream& os,
                                               ACE_CDR::LongDouble x);
extern ACE_Export ACE_CDR::Boolean operator<< (OutputStream &os,
                                               ACE_CDR::Float x);
extern ACE_Export ACE_CDR::Boolean operator<< (OutputStream &os,
                                               ACE_CDR::Double x);
extern ACE_Export ACE_CDR::Boolean operator<< (OutputStream &os,
                                               const std::string &x);

// CDR output operator from helper classes

extern ACE_Export ACE_CDR::Boolean operator<< (OutputStream &os,
                                               OutputStream::from_boolean x);
extern ACE_Export ACE_CDR::Boolean operator<< (OutputStream &os,
                                               OutputStream::from_char x);
extern ACE_Export ACE_CDR::Boolean operator<< (OutputStream &os,
                                               OutputStream::from_wchar x);
extern ACE_Export ACE_CDR::Boolean operator<< (OutputStream &os,
                                               OutputStream::from_octet x);
extern ACE_Export ACE_CDR::Boolean operator<< (OutputStream &os,
                                               OutputStream::from_string x);
extern ACE_Export ACE_CDR::Boolean operator<< (OutputStream &os,
                                               OutputStream::from_wstring x);
extern ACE_Export ACE_CDR::Boolean operator<< (OutputStream &os,
                                               const ACE_CDR::Char* x);
extern ACE_Export ACE_CDR::Boolean operator<< (OutputStream &os,
                                               const ACE_CDR::WChar* x);

// Not used by CORBA or TAO
extern ACE_Export ACE_CDR::Boolean operator>> (InputStream &is,
                                               ACE_CDR::Char &x);
// CDR input operators for primitive types

extern ACE_Export ACE_CDR::Boolean operator>> (InputStream &is,
                                               ACE_CDR::Short &x);
extern ACE_Export ACE_CDR::Boolean operator>> (InputStream &is,
                                               ACE_CDR::UShort &x);
extern ACE_Export ACE_CDR::Boolean operator>> (InputStream &is,
                                               ACE_CDR::Int &x);
extern ACE_Export ACE_CDR::Boolean operator>> (InputStream &is,
                                               ACE_CDR::UInt &x);
extern ACE_Export ACE_CDR::Boolean operator>> (InputStream &is,
                                               ACE_CDR::Long &x);
extern ACE_Export ACE_CDR::Boolean operator>> (InputStream &is,
                                               ACE_CDR::ULong &x);
extern ACE_Export ACE_CDR::Boolean operator>> (InputStream &is,
                                               ACE_CDR::LongLong &x);
extern ACE_Export ACE_CDR::Boolean operator>> (InputStream &is,
                                               ACE_CDR::ULongLong &x);
extern ACE_Export ACE_CDR::Boolean operator>> (InputStream &is,
                                               ACE_CDR::LongDouble &x);
extern ACE_Export ACE_CDR::Boolean operator>> (InputStream &is,
                                               ACE_CDR::Float &x);
extern ACE_Export ACE_CDR::Boolean operator>> (InputStream &is,
                                               ACE_CDR::Double &x);
extern ACE_Export ACE_CDR::Boolean operator>> (InputStream &is,
                                               std::string &x);

// CDR input operator from helper classes

extern ACE_Export ACE_CDR::Boolean operator>> (InputStream &is,
                                               InputStream::to_boolean x);
extern ACE_Export ACE_CDR::Boolean operator>> (InputStream &is,
                                               InputStream::to_char x);
extern ACE_Export ACE_CDR::Boolean operator>> (InputStream &is,
                                               InputStream::to_wchar x);
extern ACE_Export ACE_CDR::Boolean operator>> (InputStream &is,
                                               InputStream::to_octet x);
extern ACE_Export ACE_CDR::Boolean operator>> (InputStream &is,
                                               InputStream::to_string x);
extern ACE_Export ACE_CDR::Boolean operator>> (InputStream &is,
                                               InputStream::to_wstring x);
extern ACE_Export ACE_CDR::Boolean operator>> (InputStream &is,
                                               ACE_CDR::Char*& x);
extern ACE_Export ACE_CDR::Boolean operator>> (InputStream &is,
                                               ACE_CDR::WChar*& x);

#endif /* __ACE_INLINE */

#endif /* ICC_CDR_STREAM_H */
