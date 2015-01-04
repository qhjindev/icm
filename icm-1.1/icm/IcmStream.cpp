// CDR_Stream.cpp,v 1.32 2000/03/06 16:27:23 coryan Exp

//#define ACE_BUILD_DLL
#include "os/OS.h"
#include "os/Basic_Types.h"
//#include "os/OS_NS_String.h"
#include "icm/IcmStream.h"
#include "icm/Proxy.h"
#include "icm/Communicator.h"
#include <algorithm>

// ****************************************************************
ACE_INLINE void
ACE_CDR::swap_2 (const char *orig, char* target)
{
  target[1] = *orig++;
  target[0] = *orig++;
}

ACE_INLINE void
ACE_CDR::swap_4 (const char *orig, char* target)
{
  target [3] = *orig++;
  target [2] = *orig++;
  target [1] = *orig++;
  target [0] = *orig++;
}

ACE_INLINE void
ACE_CDR::swap_8 (const char *orig, char* target)
{
  target [7] = *orig++;
  target [6] = *orig++;
  target [5] = *orig++;
  target [4] = *orig++;
  target [3] = *orig++;
  target [2] = *orig++;
  target [1] = *orig++;
  target [0] = *orig++;
}

ACE_INLINE void
ACE_CDR::swap_16 (const char *orig, char* target)
{
  target [15] = *orig++;
  target [14] = *orig++;
  target [13] = *orig++;
  target [12] = *orig++;
  target [11] = *orig++;
  target [10] = *orig++;
  target [9] = *orig++;
  target [8] = *orig++;
  target [7] = *orig++;
  target [6] = *orig++;
  target [5] = *orig++;
  target [4] = *orig++;
  target [3] = *orig++;
  target [2] = *orig++;
  target [1] = *orig++;
  target [0] = *orig++;
}

ACE_INLINE void
ACE_CDR::mb_align (MessageBlock *mb)
{
  char *start = ACE_ptr_align_binary (mb->base (),
                                      ACE_CDR::MAX_ALIGNMENT);
  mb->rdPtr (start);
  mb->wrPtr (start);

#if defined (ACE_HAS_PURIFY)
  // This is a good place to zero the buffer; this is not needed
  // by IIOP, but makes Purify happy.
  if (mb->base () != 0)
    {
      (void) OS::memset (mb->base (), 0, mb->size ());
    }
#endif /* ACE_HAS_PURIFY */
}

ACE_INLINE size_t
ACE_CDR::first_size (size_t minsize)
{
  if (minsize == 0)
    return ACE_CDR::DEFAULT_BUFSIZE;

  size_t newsize = ACE_CDR::DEFAULT_BUFSIZE;
  while (newsize < minsize)
    {
      if (newsize < ACE_CDR::EXP_GROWTH_MAX)
        {
          // We grow exponentially at the beginning, this is fast and
          // reduces the number of allocations.
          newsize *= 2;
        }
      else
        {
          // but continuing with exponential growth can result in over
          // allocations and easily yield an allocation failure.
          // So we grow linearly when the buffer is too big.
          newsize += ACE_CDR::LINEAR_GROWTH_CHUNK;
        }
    }
  return newsize;
}

ACE_INLINE size_t
ACE_CDR::next_size (size_t minsize)
{
  size_t newsize =
    ACE_CDR::first_size (minsize);

  if (newsize == minsize)
    {
      // If necessary increment the size
      if (newsize < ACE_CDR::EXP_GROWTH_MAX)
        newsize *= 2;
      else
        newsize += ACE_CDR::LINEAR_GROWTH_CHUNK;
    }

  return newsize;
}

// ****************************************************************

// implementing the special types
ACE_INLINE
OutputStream::from_boolean::from_boolean (ACE_CDR::Boolean b)
  : val_ (b)
{
}

ACE_INLINE
InputStream::to_boolean::to_boolean (ACE_CDR::Boolean &b)
  : ref_ (b)
{
}

ACE_INLINE
OutputStream::from_octet::from_octet (ACE_CDR::Octet o)
  : val_ (o)
{
}

ACE_INLINE
InputStream::to_octet::to_octet (ACE_CDR::Octet &o)
  : ref_ (o)
{
}

ACE_INLINE
OutputStream::from_char::from_char (ACE_CDR::Char c)
  : val_ (c)
{
}

ACE_INLINE
InputStream::to_char::to_char (ACE_CDR::Char &c)
  : ref_ (c)
{
}

ACE_INLINE
OutputStream::from_wchar::from_wchar (ACE_CDR::WChar wc)
  : val_ (wc)
{
}

ACE_INLINE
InputStream::to_wchar::to_wchar (ACE_CDR::WChar &wc)
  : ref_ (wc)
{
}

ACE_INLINE
OutputStream::from_string::from_string (ACE_CDR::Char *s,
                                         ACE_CDR::ULong b,
                                         ACE_CDR::Boolean nocopy)
  : val_ (s),
    bound_ (b),
    nocopy_ (nocopy)
{
}

ACE_INLINE
InputStream::to_string::to_string (ACE_CDR::Char *&s,
                                    ACE_CDR::ULong b)
  : val_ (s),
    bound_ (b)
{
}

ACE_INLINE
OutputStream::from_wstring::from_wstring (ACE_CDR::WChar *ws,
                                           ACE_CDR::ULong b,
					   ACE_CDR::Boolean nocopy)
  : val_ (ws),
    bound_ (b),
    nocopy_ (nocopy)
{
}

ACE_INLINE
InputStream::to_wstring::to_wstring (ACE_CDR::WChar *&ws,
                                      ACE_CDR::ULong b)
  : val_ (ws),
    bound_ (b)
{
}

ACE_INLINE
InputStream::Transfer_Contents::Transfer_Contents (InputStream &rhs)
  :  rhs_ (rhs)
{
}

// ****************************************************************

ACE_INLINE ACE_CDR::Boolean
OutputStream::write_obj (IcmProxy::Object *o)
{
	if(!o)
		return false;
	Communicator::instance()->proxyFactory()->proxyToStream(o, this);

	return true;
}

ACE_INLINE
OutputStream::~OutputStream (void)
{
  if (this->start_.cont () != 0)
    {
      MessageBlock::release (this->start_.cont ());
      this->start_.cont (0);
    }
  this->current_ = 0;
}

ACE_INLINE void
OutputStream::reset (void)
{
  this->current_ = &this->start_;
  this->current_is_writable_ = 1;
  ACE_CDR::mb_align (&this->start_);
#if !defined(ICC_LACKS_CDR_ALIGNMENT)
  this->current_alignment_ = 0;
#endif

  // It is tempting not to remove the memory, but we need to do so to
  // release any potential user buffers chained in the continuation
  // field.
  MessageBlock *cont = this->start_.cont ();
  if (cont != 0)
    {
      MessageBlock::release (cont);
      this->start_.cont (0);
    }

}

// Decode the CDR stream.

ACE_INLINE ACE_CDR::Boolean
OutputStream::write_octet (ACE_CDR::Octet x)
{
  return this->write_1 (ACE_reinterpret_cast (const ACE_CDR::Octet*,&x));
}

ACE_INLINE ACE_CDR::Boolean
OutputStream::write_boolean (ACE_CDR::Boolean x)
{
  return (ACE_CDR::Boolean) this->write_octet (x ? 1 : 0);
}

ACE_INLINE ACE_CDR::Boolean
OutputStream::write_char (ACE_CDR::Char x)
{
  if (this->char_translator_ == 0)
    return this->write_1 (ACE_reinterpret_cast (const ACE_CDR::Octet*, &x));
  return this->char_translator_->write_char (*this, x);
}

ACE_INLINE ACE_CDR::Boolean
OutputStream::write_wchar (ACE_CDR::WChar x)
{
  if (this->wchar_translator_ == 0)
    return this->write_2 (ACE_reinterpret_cast (const ACE_CDR::UShort*, &x));
  return this->wchar_translator_->write_wchar (*this, x);
}

ACE_INLINE ACE_CDR::Boolean
OutputStream::write_short (ACE_CDR::Short x)
{
  return this->write_2 (ACE_reinterpret_cast (const ACE_CDR::UShort*, &x));
}

ACE_INLINE ACE_CDR::Boolean
OutputStream::write_ushort (ACE_CDR::UShort x)
{
  return this->write_2 (ACE_reinterpret_cast (const ACE_CDR::UShort*, &x));
}

ACE_INLINE ACE_CDR::Boolean
OutputStream::write_int (ACE_CDR::Int x)
{
  return this->write_4 (ACE_reinterpret_cast (const ACE_CDR::UInt*, &x));
}

ACE_INLINE ACE_CDR::Boolean
OutputStream::write_uint (ACE_CDR::UInt x)
{
  return this->write_4 (&x);
}
/*
ACE_INLINE ACE_CDR::Boolean
OutputStream::write_enum (enum Enum x)
{
  return this->write_4 (ACE_reinterpret_cast (const ACE_CDR::UInt*, &x));
}*/

ACE_INLINE ACE_CDR::Boolean
OutputStream::write_long (ACE_CDR::Long x)
{
  return this->write_8 (ACE_reinterpret_cast (const ACE_CDR::ULong*, &x));
}

ACE_INLINE ACE_CDR::Boolean
OutputStream::write_ulong (ACE_CDR::ULong x)
{
  return this->write_8 (ACE_reinterpret_cast (const ACE_CDR::ULong*, &x));
}

ACE_INLINE ACE_CDR::Boolean
OutputStream::write_longlong (const ACE_CDR::LongLong &x)
{
  return this->write_8 (ACE_reinterpret_cast (const ACE_CDR::ULongLong*, &x));
}

ACE_INLINE ACE_CDR::Boolean
OutputStream::write_ulonglong (const ACE_CDR::ULongLong &x)
{
  return this->write_8 (ACE_reinterpret_cast (const ACE_CDR::ULongLong*,&x));
}

ACE_INLINE ACE_CDR::Boolean
OutputStream::write_float (ACE_CDR::Float x)
{
  int swap = this->do_byte_swap_;
  this->do_byte_swap_ = 0;
  ACE_INLINE ACE_CDR::Boolean result = this->write_4 (ACE_reinterpret_cast (const ACE_CDR::UInt*, &x));
  this->do_byte_swap_ = swap;
  return result;
}

ACE_INLINE ACE_CDR::Boolean
OutputStream::write_double (const ACE_CDR::Double &x)
{
  int swap = this->do_byte_swap_;
  this->do_byte_swap_ = 0;
  ACE_INLINE ACE_CDR::Boolean result = this->write_8 (ACE_reinterpret_cast (const ACE_CDR::ULongLong*, &x));
  this->do_byte_swap_ = swap;
  return result;
}

ACE_INLINE ACE_CDR::Boolean
OutputStream::write_longdouble (const ACE_CDR::LongDouble &x)
{
  int swap = this->do_byte_swap_;
  this->do_byte_swap_ = 0;
  ACE_INLINE ACE_CDR::Boolean result = this->write_16 (ACE_reinterpret_cast (const ACE_CDR::LongDouble*,&x));
  this->do_byte_swap_ = swap;
  return result;
}

ACE_INLINE ACE_CDR::Boolean
OutputStream::write_string (const ACE_CDR::Char *x)
{
  if (x != 0)
    return this->write_string (OS::strlen(x), x);
  return this->write_string (0, 0);
}

ACE_INLINE ACE_CDR::Boolean
OutputStream::write_string (const std::string &x)
{
  return this->write_string (x.length(), x.c_str());
}

ACE_CDR::Boolean
OutputStream::write_string_seq(const std::vector<std::string>& v)
{
  int result = 1;
  this->write_uint(v.size());
  if(v.size() > 0) {
    for (size_t i = 0; i < v.size(); i++) {
      result = this->write_string (v[i]);
    }
  }

  return result;
}

ACE_INLINE ACE_CDR::Boolean
OutputStream::write_wstring (const ACE_CDR::WChar *x)
{
  if (x != 0)
    return this->write_wstring (OS::wslen (x), x);
  return this->write_wstring (0, 0);
}

ACE_INLINE ACE_CDR::Boolean
OutputStream::write_char_array (const ACE_CDR::Char *x,
                                 ACE_CDR::UInt length)
{
  if (this->char_translator_ == 0)
    return this->write_array (x,
                              ACE_CDR::OCTET_SIZE,
                              ACE_CDR::OCTET_ALIGN,
                              length);
  return this->char_translator_->write_char_array (*this, x, length);
}

ACE_INLINE ACE_CDR::Boolean
OutputStream::write_wchar_array (const ACE_CDR::WChar* x,
                                  ACE_CDR::UInt length)
{
  if (this->wchar_translator_ == 0)
    return this->write_array (x,
                              ACE_CDR::SHORT_SIZE,
                              ACE_CDR::SHORT_ALIGN,
                              length);
  return this->wchar_translator_->write_wchar_array (*this, x, length);
}

ACE_INLINE ACE_CDR::Boolean
OutputStream::write_octet_array (const ACE_CDR::Octet* x,
                                  ACE_CDR::UInt length)
{
  return this->write_array (x,
                            ACE_CDR::OCTET_SIZE,
                            ACE_CDR::OCTET_ALIGN,
                            length);
}

ACE_CDR::Boolean
OutputStream::write_byte_seq(const std::vector<Byte>& v)
{
  this->write_uint (v.size());
  if (v.size() > 0) {
    this->write_octet_array(&v[0], v.size());
  }

  return 0;
}

ACE_CDR::Boolean
OutputStream::write_bool_seq(const std::vector<bool>& v) {
  this->write_uint (v.size());
  if (v.size() > 0) {
    std::vector<ACE_CDR::Boolean> bv;
    for(std::vector<bool>::const_iterator it = v.begin();
        it != v.end();
        ++it)
    {
      bv.push_back(*it);
    }
    this->write_boolean_array(&bv[0], bv.size());
  }

  return 0;
}

ACE_INLINE ACE_CDR::Boolean
OutputStream::write_short_array (const ACE_CDR::Short *x,
                                  ACE_CDR::UInt length)
{
  return this->write_array (x,
                            ACE_CDR::SHORT_SIZE,
                            ACE_CDR::SHORT_ALIGN,
                            length);
}

ACE_CDR::Boolean
OutputStream::write_short_seq(const std::vector<Short>& v)
{
  this->write_uint (v.size());
  if (v.size() > 0) {
    this->write_short_array(&v[0], v.size());
  }

  return 0;
}


ACE_INLINE ACE_CDR::Boolean
OutputStream::write_ushort_array (const ACE_CDR::UShort *x,
                                   ACE_CDR::UInt length)
{
  return this->write_array (x,
                            ACE_CDR::SHORT_SIZE,
                            ACE_CDR::SHORT_ALIGN,
                            length);
}

ACE_INLINE ACE_CDR::Boolean
OutputStream::write_int_array (const ACE_CDR::Int *x,
                                 ACE_CDR::UInt length)
{
  return this->write_array (x,
                            ACE_CDR::INT_SIZE,
                            ACE_CDR::INT_ALIGN,
                            length);
}

ACE_CDR::Boolean
OutputStream::write_int_seq(const std::vector<Int>& v)
{
  this->write_uint (v.size());
  if (v.size() > 0) {
    this->write_int_array(&v[0], v.size());
  }

  return 0;
}

ACE_INLINE ACE_CDR::Boolean
OutputStream::write_long_array (const ACE_CDR::Long *x,
                                 ACE_CDR::UInt length)
{
  return this->write_array (x,
                            ACE_CDR::LONG_SIZE,
                            ACE_CDR::LONG_ALIGN,
                            length);
}

ACE_CDR::Boolean
OutputStream::write_long_seq(const std::vector<Long>& v)
{
  this->write_uint (v.size());
  if (v.size() > 0) {
    this->write_long_array(&v[0], v.size());
  }

  return 0;
}

ACE_INLINE ACE_CDR::Boolean
OutputStream::write_ulong_array (const ACE_CDR::ULong *x,
                                  ACE_CDR::UInt length)
{
  return this->write_array (x,
                            ACE_CDR::LONG_SIZE,
                            ACE_CDR::LONG_ALIGN,
                            length);
}

ACE_INLINE ACE_CDR::Boolean
OutputStream::write_longlong_array (const ACE_CDR::LongLong *x,
                                     ACE_CDR::UInt length)
{
  return this->write_array (x,
                            ACE_CDR::LONGLONG_SIZE,
                            ACE_CDR::LONGLONG_ALIGN,
                            length);
}

ACE_INLINE ACE_CDR::Boolean
OutputStream::write_ulonglong_array (const ACE_CDR::ULongLong *x,
                                      ACE_CDR::UInt length)
{
  return this->write_array (x,
                            ACE_CDR::LONGLONG_SIZE,
                            ACE_CDR::LONGLONG_ALIGN,
                            length);
}

ACE_INLINE ACE_CDR::Boolean
OutputStream::write_float_array (const ACE_CDR::Float *x,
                                  ACE_CDR::UInt length)
{
  return this->write_array (x,
                            ACE_CDR::INT_SIZE,
                            ACE_CDR::INT_ALIGN,
                            length);
}


ACE_CDR::Boolean
OutputStream::write_float_seq(const std::vector<Float>& v)
{
  this->write_uint (v.size());
  if (v.size() > 0) {
    this->write_float_array(&v[0], v.size());
  }

  return 0;
}


ACE_INLINE ACE_CDR::Boolean
OutputStream::write_double_array (const ACE_CDR::Double *x,
                                   ACE_CDR::UInt length)
{
  return this->write_array (x,
                            ACE_CDR::LONGLONG_SIZE,
                            ACE_CDR::LONGLONG_ALIGN,
                            length);
}

ACE_CDR::Boolean
OutputStream::write_double_seq(const std::vector<Double>& v)
{
  this->write_uint (v.size());
  if (v.size() > 0) {
    this->write_double_array(&v[0], v.size());
  }

  return 0;
}

ACE_INLINE ACE_CDR::Boolean
OutputStream::write_longdouble_array (const ACE_CDR::LongDouble* x,
                                       ACE_CDR::UInt length)
{
  return this->write_array (x,
                            ACE_CDR::LONGDOUBLE_SIZE,
                            ACE_CDR::LONGDOUBLE_ALIGN,
                            length);
}

ACE_INLINE int
OutputStream::good_bit (void) const
{
  return this->good_bit_;
}

ACE_INLINE int
OutputStream::adjust (size_t size,
                       size_t align,
                       char*& buf)
{
#if !defined (ICC_LACKS_CDR_ALIGNMENT)
  size_t offset =
    ACE_align_binary (this->current_alignment_, align)
    - this->current_alignment_;

  buf = this->current_->wrPtr () + offset;
#else
  buf = this->current_->wrPtr ();
#endif

  char *end = buf + size;

  if (this->current_is_writable_
      && end <= this->current_->end ())
    {
#if !defined (ICC_LACKS_CDR_ALIGNMENT)
      this->current_alignment_ += offset + size;
#endif //ICC_LACKS_CDR_ALIGNMENT
      this->current_->wrPtr (end);
      return 0;
    }

  return this->grow_and_adjust (size, align, buf);
}

ACE_INLINE int
OutputStream::adjust (size_t size, char*& buf)
{
  return this->adjust (size, size, buf);
}

ACE_INLINE const MessageBlock*
OutputStream::begin (void) const
{
  return &this->start_;
}

ACE_INLINE const MessageBlock*
OutputStream::end (void) const
{
  return this->current_->cont ();
}

ACE_INLINE const MessageBlock*
OutputStream::current (void) const
{
  return this->current_;
}

ACE_INLINE size_t
OutputStream::total_length (void) const
{
  return ACE_CDR::total_length (this->begin (), this->end ());
}

ACE_INLINE const char*
OutputStream::buffer (void) const
{
  return this->start_.rdPtr ();
}

ACE_INLINE size_t
OutputStream::length (void) const
{
  return this->start_.length ();
}

ACE_INLINE int
OutputStream::do_byte_swap (void) const
{
  return this->do_byte_swap_;
}

ACE_INLINE size_t
OutputStream::current_alignment (void) const
{
#if !defined(ICC_LACKS_CDR_ALIGNMENT)
  return this->current_alignment_;
#endif
  return 0;
}

ACE_INLINE int
OutputStream::align_write_ptr (size_t alignment)
{
  char *buf = ACE_ptr_align_binary (this->current_->wrPtr (),
                                    alignment);

  if (buf <= this->current_->end ())
    {
      this->current_->wrPtr (buf);
      return 0;
    }

  return this->grow_and_adjust (0, alignment, buf);
}

ACE_INLINE ACE_Char_Codeset_Translator *
OutputStream::char_translator (void) const
{
  return this->char_translator_;
}

ACE_INLINE ACE_WChar_Codeset_Translator *
OutputStream::wchar_translator (void) const
{
  return this->wchar_translator_;
}

// ****************************************************************

ACE_CDR::Boolean
InputStream::read_obj(IcmProxy::Object* &o)
{
	o = Communicator::instance()->proxyFactory()->streamToProxy(this);
	if(o)
		return true;
	else
		return false;
}

ACE_INLINE
InputStream::~InputStream (void)
{
}

ACE_INLINE ACE_CDR::Boolean
InputStream::read_octet (ACE_CDR::Octet& x)
{
  return this->read_1 (&x);
}

ACE_INLINE ACE_CDR::Boolean
InputStream::read_boolean (ACE_CDR::Boolean& x)
{
  ACE_CDR::Octet tmp;
  this->read_octet (tmp);
  x = tmp ? 1 : 0;
  return (ACE_CDR::Boolean) this->good_bit_;
}

ACE_INLINE ACE_CDR::Boolean
InputStream::read_char (ACE_CDR::Char &x)
{
  if (this->char_translator_ == 0)
    return this->read_1 (ACE_reinterpret_cast (ACE_CDR::Octet*, &x));
  return this->char_translator_->read_char (*this, x);
}


ACE_INLINE ACE_CDR::Boolean
InputStream::read_wchar (ACE_CDR::WChar& x)
{
  if (this->wchar_translator_ == 0)
    return this->read_2 (ACE_reinterpret_cast (ACE_CDR::UShort*,&x));
  return this->wchar_translator_->read_wchar (*this, x);
}

ACE_INLINE ACE_CDR::Boolean
InputStream::read_short (ACE_CDR::Short &x)
{
  return this->read_2 (ACE_reinterpret_cast (ACE_CDR::UShort*, &x));
}

ACE_INLINE ACE_CDR::Boolean
InputStream::read_ushort (ACE_CDR::UShort &x)
{
  return this->read_2 (ACE_reinterpret_cast (ACE_CDR::UShort*, &x));
}

ACE_INLINE ACE_CDR::Boolean
InputStream::read_int (ACE_CDR::Int &x)
{
  return this->read_4 (ACE_reinterpret_cast (ACE_CDR::UInt*, &x));
}

ACE_INLINE ACE_CDR::Boolean
InputStream::read_uint (ACE_CDR::UInt &x)
{
  return this->read_4 (&x);
}
/*
ACE_INLINE ACE_CDR::Boolean
InputStream::read_enum (enum Enum &x)
{
  return this->read_4 (ACE_reinterpret_cast (ACE_CDR::UInt*, &x));
}
*/
ACE_INLINE ACE_CDR::Boolean
InputStream::read_long (ACE_CDR::Long &x)
{
  return this->read_8 (ACE_reinterpret_cast (ACE_CDR::ULong*, &x));
}


ACE_INLINE ACE_CDR::Boolean
InputStream::read_ulong (ACE_CDR::ULong &x)
{
  return this->read_8 (ACE_reinterpret_cast (ACE_CDR::ULong*, &x));
}


ACE_INLINE ACE_CDR::Boolean
InputStream::read_longlong (ACE_CDR::LongLong &x)
{
  return this->read_8 (ACE_reinterpret_cast (ACE_CDR::ULongLong*, &x));
}

ACE_INLINE ACE_CDR::Boolean
InputStream::read_ulonglong (ACE_CDR::ULongLong &x)
{
  return this->read_8 (ACE_reinterpret_cast (ACE_CDR::ULongLong*,&x));
}

ACE_INLINE ACE_CDR::Boolean
InputStream::read_float (ACE_CDR::Float &x)
{
  int swap = this->do_byte_swap_;
  this->do_byte_swap_ = 0;
  ACE_INLINE ACE_CDR::Boolean result = this->read_4 (ACE_reinterpret_cast (ACE_CDR::UInt*, &x));
  this->do_byte_swap_ = swap;
  return result;
}


ACE_INLINE ACE_CDR::Boolean
InputStream::read_double (ACE_CDR::Double &x)
{
  int swap = this->do_byte_swap_;
  this->do_byte_swap_ = 0;
  ACE_INLINE ACE_CDR::Boolean result = this->read_8 (ACE_reinterpret_cast (ACE_CDR::ULongLong*, &x));
  this->do_byte_swap_ = swap;
  return result;
}

ACE_INLINE ACE_CDR::Boolean
InputStream::read_longdouble (ACE_CDR::LongDouble &x)
{
  int swap = this->do_byte_swap_;
  this->do_byte_swap_ = 0;
  ACE_INLINE ACE_CDR::Boolean result = this->read_16 (ACE_reinterpret_cast (ACE_CDR::LongDouble*,&x));
  this->do_byte_swap_ = swap;
  return result;
}

ACE_INLINE ACE_CDR::Boolean
InputStream::read_char_array (ACE_CDR::Char* x,
                               ACE_CDR::UInt length)
{
  if (this->char_translator_ == 0)
    return this->read_array (x,
                             ACE_CDR::OCTET_SIZE,
                             ACE_CDR::OCTET_ALIGN,
                             length);
  return this->char_translator_->read_char_array (*this, x, length);
}

ACE_INLINE ACE_CDR::Boolean
InputStream::read_wchar_array (ACE_CDR::WChar* x,
                                ACE_CDR::UInt length)
{
  if (this->wchar_translator_ == 0)
    return this->read_array (x,
                             ACE_CDR::SHORT_SIZE,
                             ACE_CDR::SHORT_ALIGN,
                             length);
  return this->wchar_translator_->read_wchar_array (*this, x, length);
}

ACE_INLINE ACE_CDR::Boolean
InputStream::read_octet_array (ACE_CDR::Octet* x,
                                ACE_CDR::UInt length)
{
  return this->read_array (x,
                           ACE_CDR::OCTET_SIZE,
                           ACE_CDR::OCTET_ALIGN,
                           length);
}

ACE_CDR::Boolean
InputStream::read_byte_seq (std::vector<Byte>& v)
{
  UInt sz;
  this->read_uint(sz);
  if (sz > 0) {
    v.resize(sz);
    this->read_octet_array(&v[0],sz);
  }

  return 0;
}

ACE_CDR::Boolean
InputStream::read_bool_seq(std::vector<bool>& v)
{
  UInt sz;
  this->read_uint(sz);
  if (sz > 0) {
    this->read_boolean_array(v,sz);
  }

  return 0;
}

ACE_CDR::Boolean
InputStream::read_short_array (ACE_CDR::Short *x,
                                ACE_CDR::UInt length)
{
  return this->read_array (x,
                           ACE_CDR::SHORT_SIZE,
                           ACE_CDR::SHORT_ALIGN,
                           length);
}

ACE_CDR::Boolean
InputStream::read_short_seq (std::vector<Short>& v)
{
  UInt sz = 0;
  this->read_uint(sz);

  if (sz > 0) {
    v.resize(sz);
    this->read_short_array(&v[0], sz);
  }

  return 0;
}

ACE_INLINE ACE_CDR::Boolean
InputStream::read_ushort_array (ACE_CDR::UShort *x,
                                 ACE_CDR::UInt length)
{
  return this->read_array (x,
                           ACE_CDR::SHORT_SIZE,
                           ACE_CDR::SHORT_ALIGN,
                           length);
}

ACE_CDR::Boolean
InputStream::read_int_seq (std::vector<Int>& v)
{
  UInt sz = 0;
  this->read_uint(sz);

  if (sz > 0) {
    v.resize(sz);
    this->read_int_array(&v[0], sz);
  }
  return 0;
}

ACE_INLINE ACE_CDR::Boolean
InputStream::read_int_array (ACE_CDR::Int *x,
                               ACE_CDR::UInt length)
{
  return this->read_array (x,
                           ACE_CDR::INT_SIZE,
                           ACE_CDR::INT_ALIGN,
                           length);
}

ACE_INLINE ACE_CDR::Boolean
InputStream::read_long_array (ACE_CDR::Long *x,
                               ACE_CDR::UInt length)
{
  return this->read_array (x,
                           ACE_CDR::LONG_SIZE,
                           ACE_CDR::LONG_ALIGN,
                           length);
}

ACE_CDR::Boolean
InputStream::read_long_seq (std::vector<Long>& v)
{
  UInt sz = 0;
  this->read_uint(sz);

  if (sz > 0) {
    v.resize(sz);
    this->read_long_array(&v[0], sz);
  }
  return 0;
}

ACE_INLINE ACE_CDR::Boolean
InputStream::read_ulong_array (ACE_CDR::ULong *x,
                                ACE_CDR::UInt length)
{
  return this->read_array (x,
                           ACE_CDR::LONG_SIZE,
                           ACE_CDR::LONG_ALIGN,
                           length);
}

ACE_INLINE ACE_CDR::Boolean
InputStream::read_longlong_array (ACE_CDR::LongLong *x,
                                   ACE_CDR::UInt length)
{
  return this->read_array (x,
                           ACE_CDR::LONGLONG_SIZE,
                           ACE_CDR::LONGLONG_ALIGN,
                           length);
}

ACE_INLINE ACE_CDR::Boolean
InputStream::read_ulonglong_array (ACE_CDR::ULongLong *x,
                                    ACE_CDR::UInt length)
{
  return this->read_array (x,
                           ACE_CDR::LONGLONG_SIZE,
                           ACE_CDR::LONGLONG_ALIGN,
                           length);
}

ACE_INLINE ACE_CDR::Boolean
InputStream::read_float_array (ACE_CDR::Float *x,
                                ACE_CDR::UInt length)
{
  return this->read_array (x,
                           ACE_CDR::INT_SIZE,
                           ACE_CDR::INT_ALIGN,
                           length);
}

ACE_CDR::Boolean
InputStream::read_float_seq (std::vector<Float>& v)
{
  UInt sz = 0;
  this->read_uint(sz);

  if (sz > 0) {
    v.resize(sz);
    this->read_float_array(&v[0], sz);
  }
  return 0;
}

ACE_INLINE ACE_CDR::Boolean
InputStream::read_double_array (ACE_CDR::Double *x,
                                 ACE_CDR::UInt length)
{
  return this->read_array (x,
                           ACE_CDR::LONGLONG_SIZE,
                           ACE_CDR::LONGLONG_ALIGN,
                           length);
}

ACE_CDR::Boolean
InputStream::read_double_seq (std::vector<Double>& v)
{
  UInt sz = 0;
  this->read_uint(sz);

  if (sz > 0) {
    v.resize(sz);
    this->read_double_array(&v[0], sz);
  }
  return 0;
}

ACE_INLINE ACE_CDR::Boolean
InputStream::read_longdouble_array (ACE_CDR::LongDouble* x,
                                     ACE_CDR::UInt length)
{
  return this->read_array (x,
                           ACE_CDR::LONGDOUBLE_SIZE,
                           ACE_CDR::LONGDOUBLE_ALIGN,
                           length);
}

ACE_INLINE ACE_CDR::Boolean
InputStream::skip_char (void)
{
  ACE_CDR::Char x;
  return this->read_1 (ACE_reinterpret_cast (ACE_CDR::Octet*,&x));
}

ACE_INLINE ACE_CDR::Boolean
InputStream::skip_wchar (void)
{
  ACE_CDR::WChar x;
  return this->read_2 (ACE_reinterpret_cast (ACE_CDR::UShort*,&x));
}

ACE_INLINE ACE_CDR::Boolean
InputStream::skip_octet (void)
{
  ACE_CDR::Octet x;
  return this->read_1 (&x);
}

ACE_INLINE ACE_CDR::Boolean
InputStream::skip_boolean (void)
{
  ACE_CDR::Octet tmp;
  this->read_octet (tmp);
  return this->good_bit_;
}

ACE_INLINE ACE_CDR::Boolean
InputStream::skip_short (void)
{
  ACE_CDR::Short x;
  return this->read_2 (ACE_reinterpret_cast (ACE_CDR::UShort*,&x));
}

ACE_INLINE ACE_CDR::Boolean
InputStream::skip_ushort (void)
{
  ACE_CDR::UShort x;
  return this->read_2 (ACE_reinterpret_cast (ACE_CDR::UShort*,&x));
}

ACE_INLINE ACE_CDR::Boolean
InputStream::skip_long (void)
{
  ACE_CDR::Long x;
  return this->read_8 (ACE_reinterpret_cast (ACE_CDR::ULong*,&x));
}

ACE_INLINE ACE_CDR::Boolean
InputStream::skip_ulong (void)
{
  ACE_CDR::ULong x;
  return this->read_8 (ACE_reinterpret_cast (ACE_CDR::ULong*,&x));
}

ACE_INLINE ACE_CDR::Boolean
InputStream::skip_longlong (void)
{
  ACE_CDR::LongLong x;
  return this->read_8 (ACE_reinterpret_cast (ACE_CDR::ULongLong*,&x));
}

ACE_INLINE ACE_CDR::Boolean
InputStream::skip_ulonglong (void)
{
  ACE_CDR::ULongLong x;
  return this->read_8 (ACE_reinterpret_cast (ACE_CDR::ULongLong*,&x));
}

ACE_INLINE ACE_CDR::Boolean
InputStream::skip_float (void)
{
  float x;
  return this->read_4 (ACE_reinterpret_cast (ACE_CDR::UInt*,&x));
}

ACE_INLINE ACE_CDR::Boolean
InputStream::skip_double (void)
{
  double x;
  return this->read_8 (ACE_reinterpret_cast (ACE_CDR::ULongLong*,&x));
}

ACE_INLINE ACE_CDR::Boolean
InputStream::skip_longdouble (void)
{
  ACE_CDR::LongDouble x;
  return this->read_16 (ACE_reinterpret_cast (ACE_CDR::LongDouble*,&x));
}

ACE_INLINE char*
InputStream::end (void)
{
  return this->start_.end ();
}

ACE_INLINE void
InputStream::rdPtr (size_t offset)
{
  this->start_.rdPtr (offset);
}

ACE_INLINE char*
InputStream::rdPtr (void)
{
  return this->start_.rdPtr ();
}

ACE_INLINE int
InputStream::adjust (size_t size,
                      size_t align,
                      char*& buf)
{
#if !defined (ICC_LACKS_CDR_ALIGNMENT)
  buf = ACE_ptr_align_binary (this->rdPtr (), align);
#else
  buf = this->rdPtr();
#endif //ICC_LACKS_CDR_ALIGNMENT

  char *end = buf + size;
  if (end <= this->end ())
    {
      this->start_.rdPtr (end);
      return 0;
    }

  this->good_bit_ = 0;
  return -1;
}

ACE_INLINE int
InputStream::adjust (size_t size,
                      char*& buf)
{
  return this->adjust (size, size, buf);
}

ACE_INLINE size_t
InputStream::length (void) const
{
  return this->start_.length ();
}

ACE_INLINE const MessageBlock*
InputStream::start (void) const
{
  return &this->start_;
}

ACE_INLINE int
InputStream::good_bit (void) const
{
  return this->good_bit_;
}

// ****************************************************************

ACE_INLINE ACE_CDR::Boolean
operator<< (OutputStream &os, ACE_CDR::Char x)
{
  os.write_char (x);
  return os.good_bit ();
}

ACE_INLINE ACE_CDR::Boolean
operator<< (OutputStream &os, ACE_CDR::Short x)
{
  os.write_short (x);
  return os.good_bit ();
}

ACE_INLINE ACE_CDR::Boolean
operator<< (OutputStream &os, ACE_CDR::UShort x)
{
  os.write_ushort (x);
  return os.good_bit ();
}

ACE_INLINE ACE_CDR::Boolean
operator<< (OutputStream &os, ACE_CDR::Int x)
{
  os.write_int (x);
  return os.good_bit ();
}

ACE_INLINE ACE_CDR::Boolean
operator<< (OutputStream &os, ACE_CDR::UInt x)
{
  os.write_uint (x);
  return os.good_bit ();
}

ACE_INLINE ACE_CDR::Boolean
operator<< (OutputStream &os, ACE_CDR::Long x)
{
  os.write_long (x);
  return os.good_bit ();
}

ACE_INLINE ACE_CDR::Boolean
operator<< (OutputStream &os, ACE_CDR::ULong x)
{
  os.write_ulong (x);
  return os.good_bit ();
}
/*
ACE_INLINE ACE_CDR::Boolean
operator<< (OutputStream &os, ACE_CDR::LongLong x)
{
  os.write_longlong (x);
  return os.good_bit ();
}

ACE_INLINE ACE_CDR::Boolean
operator<< (OutputStream &os, ACE_CDR::ULongLong x)
{
  os.write_ulonglong (x);
  return os.good_bit ();
}
*/
ACE_INLINE ACE_CDR::Boolean
operator<< (OutputStream &os, ACE_CDR::LongDouble x)
{
  os.write_longdouble (x);
  return os.good_bit ();
}

ACE_INLINE ACE_CDR::Boolean
operator<< (OutputStream &os, ACE_CDR::Float x)
{
  os.write_float (x);
  return os.good_bit ();
}

ACE_INLINE ACE_CDR::Boolean
operator<< (OutputStream &os, ACE_CDR::Double x)
{
  os.write_double (x);
  return os.good_bit ();
}

ACE_INLINE ACE_CDR::Boolean
operator<< (OutputStream &os, const ACE_CDR::Char *x)
{
  os.write_string (x);
  return os.good_bit ();
}

ACE_INLINE ACE_CDR::Boolean
operator<< (OutputStream &os, const ACE_CDR::WChar *x)
{
  os.write_wstring (x);
  return os.good_bit ();
}

ACE_INLINE ACE_CDR::Boolean
operator<< (OutputStream &os, const std::string &x)
{
  os.write_string (x);
  return os.good_bit ();
}

// The following use the helper classes
ACE_INLINE ACE_CDR::Boolean
operator<< (OutputStream &os, OutputStream::from_boolean x)
{
  os.write_boolean (x.val_);
  return os.good_bit ();
}

ACE_INLINE ACE_CDR::Boolean
operator<< (OutputStream &os, OutputStream::from_char x)
{
  os.write_char (x.val_);
  return os.good_bit ();
}

ACE_INLINE ACE_CDR::Boolean
operator<< (OutputStream &os, OutputStream::from_wchar x)
{
  os.write_wchar (x.val_);
  return os.good_bit ();
}

ACE_INLINE ACE_CDR::Boolean
operator<< (OutputStream &os, OutputStream::from_octet x)
{
  os.write_octet (x.val_);
  return os.good_bit ();
}

ACE_INLINE ACE_CDR::Boolean
operator<< (OutputStream &os, OutputStream::from_string x)
{
  ACE_CDR::UInt len = OS::strlen (x.val_);
  os.write_string (len, x.val_);
  return os.good_bit () && (len <= x.bound_);
}

ACE_INLINE ACE_CDR::Boolean
operator<< (OutputStream &os, OutputStream::from_wstring x)
{
  ACE_CDR::UInt len = OS::wslen (x.val_);
  os.write_wstring (len, x.val_);
  return os.good_bit () && (len <= x.bound_);
}

// ****************************************************************

ACE_INLINE ACE_CDR::Boolean
operator>> (InputStream &is, ACE_CDR::Char &x)
{
  is.read_char (x);
  return is.good_bit ();
}

ACE_INLINE ACE_CDR::Boolean
operator>> (InputStream &is, ACE_CDR::Short &x)
{
  is.read_short (x);
  return is.good_bit ();
}

ACE_INLINE ACE_CDR::Boolean
operator>> (InputStream &is, ACE_CDR::UShort &x)
{
  is.read_ushort (x);
  return is.good_bit ();
}

ACE_INLINE ACE_CDR::Boolean
operator>>(InputStream &is, ACE_CDR::Int &x)
{
  is.read_int (x);
  return is.good_bit ();
}

ACE_INLINE ACE_CDR::Boolean
operator>> (InputStream &is, ACE_CDR::UInt &x)
{
  is.read_uint (x);
  return is.good_bit ();
}

ACE_INLINE ACE_CDR::Boolean
operator>>(InputStream &is, ACE_CDR::Long &x)
{
  is.read_long (x);
  return is.good_bit ();
}

ACE_INLINE ACE_CDR::Boolean
operator>> (InputStream &is, ACE_CDR::ULong &x)
{
  is.read_ulong (x);
  return is.good_bit ();
}
/*
ACE_INLINE ACE_CDR::Boolean
operator>> (InputStream& is, ACE_CDR::LongLong &x)
{
  is.read_longlong (x);
  return is.good_bit ();
}

ACE_INLINE ACE_CDR::Boolean
operator>> (InputStream& is, ACE_CDR::ULongLong &x)
{
  is.read_ulonglong (x);
  return is.good_bit ();
}
*/
ACE_INLINE ACE_CDR::Boolean
operator>> (InputStream& is, ACE_CDR::LongDouble &x)
{
  is.read_longdouble (x);
  return is.good_bit ();
}

ACE_INLINE ACE_CDR::Boolean
operator>> (InputStream &is, ACE_CDR::Float &x)
{
  is.read_float (x);
  return is.good_bit ();
}

ACE_INLINE ACE_CDR::Boolean
operator>> (InputStream &is, ACE_CDR::Double &x)
{
  is.read_double (x);
  return is.good_bit ();
}

ACE_INLINE ACE_CDR::Boolean
operator>> (InputStream &is, ACE_CDR::Char *&x)
{
  is.read_string (x);
  return is.good_bit ();
}

ACE_INLINE ACE_CDR::Boolean
operator>> (InputStream &is, ACE_CDR::WChar *&x)
{
  is.read_wstring (x);
  return is.good_bit ();
}

ACE_INLINE ACE_CDR::Boolean
operator>> (InputStream &is, std::string &x)
{
  is.read_string (x);
  return is.good_bit ();
}

// The following use the helper classes
ACE_INLINE ACE_CDR::Boolean
operator>> (InputStream &is, InputStream::to_boolean x)
{
  is.read_boolean (x.ref_);
  return is.good_bit ();
}

ACE_INLINE ACE_CDR::Boolean
operator>> (InputStream &is, InputStream::to_char x)
{
  is.read_char (x.ref_);
  return is.good_bit ();
}

ACE_INLINE ACE_CDR::Boolean
operator>> (InputStream &is, InputStream::to_wchar x)
{
  is.read_wchar (x.ref_);
  return is.good_bit ();
}

ACE_INLINE ACE_CDR::Boolean
operator>> (InputStream &is, InputStream::to_octet x)
{
  is.read_octet (x.ref_);
  return is.good_bit ();
}

ACE_INLINE ACE_CDR::Boolean
operator>> (InputStream &is, InputStream::to_string x)
{
  is.read_string (x.val_);
  // check if the bounds are satisfied
  return (is.good_bit () &&
          (OS::strlen (x.val_) <= x.bound_));
}

ACE_INLINE ACE_CDR::Boolean
operator>> (InputStream &is, InputStream::to_wstring x)
{
  is.read_wstring (x.val_);
  // check if the bounds are satisfied
  return (is.good_bit () &&
          (OS::wslen (x.val_) <= x.bound_));
}

// ***************************************************************************
// We must define these methods here because they use the "read_*" inlined
// methods of the InputStream class
// ***************************************************************************

ACE_INLINE ACE_CDR::Boolean
OutputStream::append_boolean (InputStream &stream)
{
  ACE_CDR::Boolean x;
  return (stream.read_boolean (x) ? this->write_boolean (x) : 0);
}

ACE_INLINE ACE_CDR::Boolean
OutputStream::append_char (InputStream &stream)
{
  ACE_CDR::Char x;
  return (stream.read_char (x) ? this->write_char (x) : 0);
}

ACE_INLINE ACE_CDR::Boolean
OutputStream::append_wchar (InputStream &stream)
{
  ACE_CDR::WChar x;
  return (stream.read_wchar (x) ? this->write_wchar (x) : 0);
}

ACE_INLINE ACE_CDR::Boolean
OutputStream::append_octet (InputStream &stream)
{
  ACE_CDR::Octet x;
  return (stream.read_octet (x) ? this->write_octet (x) : 0);
}

ACE_INLINE ACE_CDR::Boolean
OutputStream::append_short (InputStream &stream)
{
  ACE_CDR::Short x;
  return (stream.read_short (x) ? this->write_short (x) : 0);
}

ACE_INLINE ACE_CDR::Boolean
OutputStream::append_ushort (InputStream &stream)
{
  ACE_CDR::UShort x;
  return (stream.read_ushort (x) ? this->write_ushort (x) : 0);
}

ACE_INLINE ACE_CDR::Boolean
OutputStream::append_long (InputStream &stream)
{
  ACE_CDR::Long x;
  return (stream.read_long (x) ? this->write_long (x) : 0);
}

ACE_INLINE ACE_CDR::Boolean
OutputStream::append_ulong (InputStream &stream)
{
  ACE_CDR::ULong x;
  return (stream.read_ulong (x) ? this->write_ulong (x) : 0);
}

ACE_INLINE ACE_CDR::Boolean
OutputStream::append_longlong (InputStream &stream)
{
  ACE_CDR::LongLong x;
  return (stream.read_longlong (x) ? this->write_longlong (x) : 0);
}

ACE_INLINE ACE_CDR::Boolean
OutputStream::append_ulonglong (InputStream &stream)
{
  ACE_CDR::ULongLong x;
  return (stream.read_ulonglong (x) ? this->write_ulonglong (x) : 0);
}

ACE_INLINE ACE_CDR::Boolean
OutputStream::append_float (InputStream &stream)
{
  ACE_CDR::Float x;
  return (stream.read_float (x) ? this->write_float (x) : 0);
}

ACE_INLINE ACE_CDR::Boolean
OutputStream::append_double (InputStream &stream)
{
  ACE_CDR::Double x;
  return (stream.read_double (x) ? this->write_double (x) : 0);
}

ACE_INLINE ACE_CDR::Boolean
OutputStream::append_longdouble (InputStream &stream)
{
  ACE_CDR::LongDouble x;
  return (stream.read_longdouble (x) ? this->write_longdouble (x) : 0);
}

ACE_INLINE ACE_CDR::Boolean
OutputStream::append_string (InputStream &stream)
{
  ACE_CDR::Char *x;
  ACE_CDR::Boolean flag =
    (stream.read_string (x) ? this->write_string (x) : 0);
  delete [] x;
  return flag;
}

ACE_INLINE ACE_CDR::Boolean
OutputStream::append_wstring (InputStream &stream)
{
  ACE_CDR::WChar *x;
  ACE_CDR::Boolean flag =
    (stream.read_wstring (x) ? this->write_wstring (x) : 0);
  delete [] x;
  return flag;
}

ACE_INLINE void
InputStream::reset_byte_order (int byte_order)
{
  // compulsory network byte oder, cannot change
  this->do_byte_swap_ = ACE_CDR_BYTE_ORDER;
}

ACE_INLINE int
InputStream::do_byte_swap (void) const
{
  return this->do_byte_swap_;
}

ACE_INLINE int
InputStream::byte_order (void) const
{
  if (this->do_byte_swap ())
    return !ACE_CDR_BYTE_ORDER;
  else
    return ACE_CDR_BYTE_ORDER;
}

ACE_INLINE int
InputStream::align_read_ptr (size_t alignment)
{
  char *buf = ACE_ptr_align_binary (this->rdPtr (),
                                    alignment);

  if (buf <= this->end ())
    {
      this->start_.rdPtr (buf);
      return 0;
    }

  this->good_bit_ = 0;
  return -1;
}

ACE_INLINE ACE_Char_Codeset_Translator *
InputStream::char_translator (void) const
{
  return this->char_translator_;
}

ACE_INLINE ACE_WChar_Codeset_Translator *
InputStream::wchar_translator (void) const
{
  return this->wchar_translator_;
}

// ****************************************************************

ACE_INLINE ACE_CDR::Boolean
ACE_Char_Codeset_Translator::read_1 (InputStream& input,
                                     ACE_CDR::Octet *x)
{
  return input.read_1 (x);
}

ACE_INLINE ACE_CDR::Boolean
ACE_Char_Codeset_Translator::write_1 (OutputStream& output,
                                      const ACE_CDR::Octet *x)
{
  return output.write_1 (x);
}

ACE_INLINE ACE_CDR::Boolean
ACE_Char_Codeset_Translator::read_array (InputStream& in,
                                         void* x,
                                         size_t size,
                                         size_t align,
                                         ACE_CDR::UInt length)
{
  return in.read_array (x, size, align, length);
}

ACE_INLINE ACE_CDR::Boolean
ACE_Char_Codeset_Translator::write_array (OutputStream& out,
                                          const void *x,
                                          size_t size,
                                          size_t align,
                                          ACE_CDR::UInt length)
{
  return out.write_array(x, size, align, length);
}

ACE_INLINE int
ACE_Char_Codeset_Translator::adjust (OutputStream& out,
                                     size_t size,
                                     size_t align,
                                     char *&buf)
{
  return out.adjust(size, align, buf);
}

ACE_INLINE void
ACE_Char_Codeset_Translator::good_bit (OutputStream& out, int bit)
{
  out.good_bit_ = bit;
}

// ****************************************************************

ACE_INLINE ACE_CDR::Boolean
ACE_WChar_Codeset_Translator::read_1 (InputStream& input,
                                      ACE_CDR::Octet *x)
{
  return input.read_1 (x);
}

ACE_INLINE ACE_CDR::Boolean
ACE_WChar_Codeset_Translator::read_2 (InputStream& input,
                                      ACE_CDR::UShort *x)
{
  return input.read_2 (x);
}

ACE_INLINE ACE_CDR::Boolean
ACE_WChar_Codeset_Translator::read_4 (InputStream& input,
                                      ACE_CDR::UInt *x)
{
  return input.read_4 (x);
}

ACE_INLINE ACE_CDR::Boolean
ACE_WChar_Codeset_Translator::write_1 (OutputStream& output,
                                       const ACE_CDR::Octet *x)
{
  return output.write_1 (x);
}

ACE_INLINE ACE_CDR::Boolean
ACE_WChar_Codeset_Translator::write_2 (OutputStream& output,
                                       const ACE_CDR::UShort *x)
{
  return output.write_2 (x);
}

ACE_INLINE ACE_CDR::Boolean
ACE_WChar_Codeset_Translator::write_4 (OutputStream& output,
                                       const ACE_CDR::UInt *x)
{
  return output.write_4 (x);
}

ACE_INLINE ACE_CDR::Boolean
ACE_WChar_Codeset_Translator::read_array (InputStream& in,
                                          void* x,
                                          size_t size,
                                          size_t align,
                                          ACE_CDR::UInt length)
{
  return in.read_array (x, size, align, length);
}

ACE_INLINE ACE_CDR::Boolean
ACE_WChar_Codeset_Translator::write_array (OutputStream& out,
                                           const void *x,
                                           size_t size,
                                           size_t align,
                                           ACE_CDR::UInt length)
{
  return out.write_array(x, size, align, length);
}

ACE_INLINE int
ACE_WChar_Codeset_Translator::adjust (OutputStream& out,
                                      size_t size,
                                      size_t align,
                                      char *&buf)
{
  return out.adjust(size, align, buf);
}

ACE_INLINE void
ACE_WChar_Codeset_Translator::good_bit (OutputStream& out, int bit)
{
  out.good_bit_ = bit;
}

int
ACE_CDR::grow (MessageBlock *mb, size_t minsize)
{
  size_t newsize =
    ACE_CDR::first_size (minsize + ACE_CDR::MAX_ALIGNMENT);

  if (newsize <= mb->size ())
    return 0;

  DataBlock *db =
    mb->dataBlock ()->cloneNoCopy ();
  db->size (newsize);

  MessageBlock tmp (db);
  ACE_CDR::mb_align (&tmp);

//  tmp.copy (mb->rdPtr (), mb->length());
  tmp.copy (mb->base(), mb->size());
  mb->dataBlock (tmp.dataBlock ()->duplicate ());
  mb->rdPtr (tmp.rdPtr ());
  mb->wrPtr (tmp.wrPtr ());

  return 0;
}

size_t
ACE_CDR::total_length (const MessageBlock* begin,
                       const MessageBlock* end)
{
  size_t l = 0;
  // Compute the total size.
  for (const MessageBlock *i = begin;
       i != end;
       i = i->cont ())
    l += i->length ();
  return l;
}

typedef u_int ptr_arith_t;

void
ACE_CDR::consolidate (MessageBlock *dst,
                      const MessageBlock *src)
{
  if (src == 0)
    return;

  size_t newsize =
    ACE_CDR::first_size (ACE_CDR::total_length (src, 0));
  dst->size (newsize);

  // We must copy the contents of <src> into the new buffer, but
  // respecting the alignment.
  ptrdiff_t srcalign =
		  ptrdiff_t(src->rdPtr ()) % ACE_CDR::MAX_ALIGNMENT;
  ptrdiff_t dstalign =
		  ptrdiff_t(dst->rdPtr ()) % ACE_CDR::MAX_ALIGNMENT;
  int offset = srcalign - dstalign;
  if (offset < 0)
    offset += ACE_CDR::MAX_ALIGNMENT;
  dst->rdPtr (offset);
  dst->wrPtr (dst->rdPtr ());

  for (const MessageBlock* i = src;
       i != 0;
       i = i->cont ())
    {
      dst->copy (i->rdPtr (), i->length ());
    }
}

#if defined (NONNATIVE_LONGDOUBLE)
int
ACE_CDR::LongDouble::operator== (const ACE_CDR::LongDouble &rhs) const
{
  return OS::memcmp (this->ld, rhs.ld, 16) == 0;
}

int
ACE_CDR::LongDouble::operator!= (const ACE_CDR::LongDouble &rhs) const
{
  return OS::memcmp (this->ld, rhs.ld, 16) != 0;
}
#endif /* NONNATIVE_LONGDOUBLE */

#if defined(_UNICOS) && !defined(_CRAYMPP)
// placeholders to get things compiling
ACE_CDR::Float::Float()
{
}

ACE_CDR::Float::Float(const float & init)
{
}

ACE_CDR::Float &
ACE_CDR::Float::operator= (const float &rhs)
{
    return *this;
}

int
ACE_CDR::Float::operator!= (const ACE_CDR::Float &rhs) const
{
    return 0;
}
#endif /* _UNICOS */

// ****************************************************************

OutputStream::OutputStream (size_t size,
                              int byte_order,
                              size_t memcpy_tradeoff)
  :  start_ (size ? size : ACE_CDR::DEFAULT_BUFSIZE + ACE_CDR::MAX_ALIGNMENT),
     current_is_writable_ (1),
#if !defined(ICC_LACKS_CDR_ALIGNMENT)
     current_alignment_ (0),
#endif
     do_byte_swap_ (ACE_CDR_BYTE_ORDER),
     good_bit_ (1),
     memcpy_tradeoff_ (memcpy_tradeoff),
     char_translator_ (0),
     wchar_translator_ (0)
{
  ACE_CDR::mb_align (&this->start_);
  this->current_ = &this->start_;
}

OutputStream::OutputStream (char *data, size_t size,
                              int byte_order,
                              size_t memcpy_tradeoff)
  :  start_ (size, 0, data, true),
     current_is_writable_ (1),
#if !defined(ICC_LACKS_CDR_ALIGNMENT)
     current_alignment_ (0),
#endif
     do_byte_swap_ (ACE_CDR_BYTE_ORDER),
     good_bit_ (1),
     memcpy_tradeoff_ (memcpy_tradeoff),
     char_translator_ (0),
     wchar_translator_ (0)
{
  // We cannot trust the buffer to be properly aligned
  ACE_CDR::mb_align (&this->start_);
  this->current_ = &this->start_;
}

OutputStream::OutputStream (MessageBlock *data,
                              int byte_order,
                              size_t memcpy_tradeoff)
  :  start_ (data->dataBlock ()->duplicate ()),
     current_is_writable_ (1),
#if !defined(ICC_LACKS_CDR_ALIGNMENT)
     current_alignment_ (0),
#endif
     do_byte_swap_ (ACE_CDR_BYTE_ORDER),
     good_bit_ (1),
     memcpy_tradeoff_ (memcpy_tradeoff),
     char_translator_ (0),
     wchar_translator_ (0)
{
  // We cannot trust the buffer to be properly aligned
  ACE_CDR::mb_align (&this->start_);
  this->current_ = &this->start_;
}

OutputStream::OutputStream (const InputStream& rhs)
  : //start_ (rhs.length () + ACE_CDR::MAX_ALIGNMENT),
//    do_byte_swap_ (rhs.do_byte_swap_),
    do_byte_swap_ (ACE_CDR_BYTE_ORDER),
    good_bit_ (1),
    char_translator_ (0),
    wchar_translator_ (0)
{
  ACE_CDR::mb_align (&this->start_);
  for (const MessageBlock *i = rhs.start ();
       i != 0;
       i = i->cont ())
    this->start_.copy (i->rdPtr (), i->length ());
}

int
OutputStream::grow_and_adjust (size_t size,
                                size_t align,
                                char*& buf)
{
  if (!this->current_is_writable_
      || this->current_->cont () == 0
      || this->current_->cont ()->size () < size + ACE_CDR::MAX_ALIGNMENT)
    {
      // Calculate the new buffer's length; if growing for encode, we
      // don't grow in "small" chunks because of the cost.
      size_t cursize = this->current_->size ();
      if (this->current_->cont () != 0)
        cursize = this->current_->cont ()->size ();

#if !defined(ICC_LACK_CDR_ALIGNMENT)
      size_t minsize = size + ACE_CDR::MAX_ALIGNMENT;
#endif

      // Make sure that there is enough room for <minsize> bytes, but
      // also make it bigger than whatever our current size is.
      if (minsize < cursize)
        {
          minsize = cursize;
        }

      size_t newsize =
        ACE_CDR::next_size (minsize);

      this->good_bit_ = 0;
      if(newsize > 10000) {
    	  ICC_DEBUG("newsize too big:: %d", newsize);
      }
      assert(newsize <= 100000);
      MessageBlock* tmp = new  MessageBlock (newsize);

      this->good_bit_ = 1;

#if !defined(ICC_LACKS_CDR_ALIGNMENT)
      // The new block must start with the same alignment as the
      // previous block finished.
      ptrdiff_t tmpalign =
    		  ptrdiff_t(tmp->rdPtr ()) % ACE_CDR::MAX_ALIGNMENT;
      ptrdiff_t curalign =
    		  ptrdiff_t(this->current_alignment_) % ACE_CDR::MAX_ALIGNMENT;
      int offset = curalign - tmpalign;
      if (offset < 0)
        offset += ACE_CDR::MAX_ALIGNMENT;
      tmp->rdPtr (offset);
      tmp->wrPtr (tmp->rdPtr ());
#endif

      // grow the chain and set the current block.
      tmp->cont (this->current_->cont ());
      this->current_->cont (tmp);
    }
  this->current_ = this->current_->cont ();
  this->current_is_writable_ = 1;

  return this->adjust (size, align, buf);
}

ACE_CDR::Boolean
OutputStream::write_string (ACE_CDR::UInt len,
                             const char *x)
{
  // @@ This is a slight violation of "Optimize for the common case",
  // i.e. normally the translator will be 0, but OTOH the code is
  // smaller and should be better for the cache ;-) ;-)
  if (this->char_translator_ != 0)
    return this->char_translator_->write_string (*this, len, x);

  if (len != 0)
    {
      if (this->write_uint (len + 1))
        {
          return this->write_char_array (x, len + 1);
        }
    }
  else
    {
      // Be nice to programmers: treat nulls as empty strings not
      // errors. (OMG-IDL supports languages that don't use the C/C++
      // notion of null v. empty strings; nulls aren't part of the OMG-IDL
      // string model.)
      if (this->write_uint (1))
        {
          return this->write_char (0);
        }
    }

  return 0;
}

ACE_CDR::Boolean
OutputStream::write_wstring (ACE_CDR::UInt len,
                              const ACE_CDR::WChar *x)
{
  // @@ This is a slight violation of "Optimize for the common case",
  // i.e. normally the translator will be 0, but OTOH the code is
  // smaller and should be better for the cache ;-) ;-)
  if (this->wchar_translator_ != 0)
    return this->wchar_translator_->write_wstring (*this, len, x);

  if (x != 0)
    {
      if (this->write_uint (len + 1))
        {
          return this->write_wchar_array (x, len + 1);
        }
    }
  else
    {
      if (this->write_uint (1))
        {
          return this->write_wchar (0);
        }
    }
  return 0;
}

ACE_CDR::Boolean
OutputStream::write_octet_array_mb (const MessageBlock* mb)
{
  // If the buffer is small and it fits in the current message
  // block it is be cheaper just to copy the buffer.
  for (const MessageBlock* i = mb;
       i != 0;
       i = i->cont ())
    {
      size_t length = i->length ();

      // If the mb does not own its data we are forced to make a copy.
//      if (ACE_BIT_ENABLED (i->flags (),
  //                         MessageBlock::DONT_DELETE))
        {
          if (! this->write_array (i->rdPtr (),
                                   ACE_CDR::OCTET_SIZE,
                                   ACE_CDR::OCTET_ALIGN,
                                   length))
            {
              return 0;
            }
          continue;
        }

      if (length < this->memcpy_tradeoff_
          && this->current_->wrPtr () + length < this->current_->end ())
        {
          if (! this->write_array (i->rdPtr (),
                                   ACE_CDR::OCTET_SIZE,
                                   ACE_CDR::OCTET_ALIGN,
                                   length))
            {
              return 0;
            }
          continue;
        }

      MessageBlock* cont;
      this->good_bit_ = 0;
      ACE_NEW_RETURN (cont,
                      MessageBlock (i->dataBlock ()->duplicate ()),
                      0);
      this->good_bit_ = 1;

      if (cont != 0)
        {
          if (this->current_->cont () != 0)
            MessageBlock::release (this->current_->cont ());
          cont->rdPtr (i->rdPtr ());
          cont->wrPtr (i->wrPtr ());

          this->current_->cont (cont);
          this->current_ = cont;
          this->current_is_writable_ = 0;
#if !defined (ICC_LACKS_CDR_ALIGNMENT)
          this->current_alignment_ =
            (this->current_alignment_ + cont->length ()) % ACE_CDR::MAX_ALIGNMENT;
#endif
        }
      else
        {
          this->good_bit_ = 0;
          return 0;
        }
    }
  return 1;
}


ACE_CDR::Boolean
OutputStream::write_1 (const ACE_CDR::Octet *x)
{
  char *buf;
  if (this->adjust (1, buf) == 0)
    {
      *ACE_reinterpret_cast(ACE_CDR::Octet*, buf) = *x;
      return 1;
    }

  return 0;
}

ACE_CDR::Boolean
OutputStream::write_2 (const ACE_CDR::UShort *x)
{
  char *buf;
  if (this->adjust (ACE_CDR::SHORT_SIZE, buf) == 0)
    {
      if(this->do_byte_swap_) {
        std::reverse_copy((char*)x, (char*)x + 2, buf);
      } else {
        std::copy((char*)x, (char*)x + 2, buf);
      }
      return 1;

//#if !defined (ACE_ENABLE_SWAP_ON_WRITE)
//      *ACE_reinterpret_cast(ACE_CDR::UShort*,buf) = *x;
//      return 1;
//#else
//      if (!this->do_byte_swap_)
//        {
//          *ACE_reinterpret_cast (ACE_CDR::UShort *, buf) = *x;
//          return 1;
//        }
//      else
//        {
//          ACE_CDR::swap_2 (ACE_reinterpret_cast (const char*, x), buf);
//          return 1;
//        }
//#endif /* ACE_ENABLE_SWAP_ON_WRITE */
    }

  return 0;
}

ACE_CDR::Boolean
OutputStream::write_4 (const ACE_CDR::UInt *x)
{
  char *buf;
  if (this->adjust (ACE_CDR::INT_SIZE, buf) == 0)
  {
    if(this->do_byte_swap_) {
      std::reverse_copy((char*)x, (char*)x + 4, buf);
//      ICC_ERROR("OutputStream::write_4 swaped, to write: %u, written: %u", *x, *(ACE_CDR::UInt*)buf);
    } else {
      std::copy((char*)x, (char*)x + 4, buf);
//      ICC_ERROR("OutputStream::write_4 no swap, to write: %u, written: %u", *x, *(ACE_CDR::UInt*)buf);
    }
    return 1;

//#if !defined (ACE_ENABLE_SWAP_ON_WRITE)
//      *ACE_reinterpret_cast(ACE_CDR::UInt*,buf) = *x;
//      return 1;
//#else
//      if (!this->do_byte_swap_)
//        {
//          *ACE_reinterpret_cast (ACE_CDR::ULong *, buf) = *x;
//          return 1;
//        }
//      else
//        {
//          ACE_CDR::swap_4 (ACE_reinterpret_cast (const char*, x), buf);
//          return 1;
//        }
//#endif /* ACE_ENABLE_SWAP_ON_WRITE */
  }

  return 0;
}

ACE_CDR::Boolean
OutputStream::write_8 (const ACE_CDR::ULong *x)
{
  char *buf;
  if (this->adjust (ACE_CDR::LONGLONG_SIZE, buf) == 0)
  {
    if(this->do_byte_swap_) {
      std::reverse_copy((char*)x, (char*)x + 8, buf);
    } else {
      std::copy((char*)x, (char*)x + 8, buf);
    }
    return 1;
//#if !defined (ACE_ENABLE_SWAP_ON_WRITE)
//      *ACE_reinterpret_cast(ACE_CDR::ULongLong*,buf) = *x;
//      return 1;
//#else
//      if (!this->do_byte_swap_)
//        {
//          *ACE_reinterpret_cast (ACE_CDR::ULongLong *, buf) = *x;
//          return 1;
//        }
//      else
//        {
//          ACE_CDR::swap_8 (ACE_reinterpret_cast (const char*, x), buf);
//          return 1;
//        }
//#endif /* ACE_ENABLE_SWAP_ON_WRITE */
  }

  return 0;
}

ACE_CDR::Boolean
OutputStream::write_16 (const ACE_CDR::LongDouble *x)
{
  char* buf;
  if (this->adjust (ACE_CDR::LONGDOUBLE_SIZE,
                    ACE_CDR::LONGDOUBLE_ALIGN,
                    buf) == 0)
  {
    if(this->do_byte_swap_) {
      std::reverse_copy((char*)x, (char*)x + 16, buf);
    } else {
      std::copy((char*)x, (char*)x + 16, buf);
    }
    return 1;

//#if !defined (ACE_ENABLE_SWAP_ON_WRITE)
//      *ACE_reinterpret_cast(ACE_CDR::LongDouble*,buf) = *x;
//      return 1;
//#else
//      if (!this->do_byte_swap_)
//        {
//          *ACE_reinterpret_cast (ACE_CDR::LongDouble *, buf) = *x;
//          return 1;
//        }
//      else
//        {
//          ACE_CDR::swap_16 (ACE_reinterpret_cast (const char*, x), buf);
//          return 1;
//        }
//#endif /* ACE_ENABLE_SWAP_ON_WRITE */
  }

  return 0;
}

ACE_CDR::Boolean
OutputStream::write_array (const void *x,
                            size_t size,
                            size_t align,
                            ACE_CDR::UInt length)
{
  char *buf;
  char* source = (char*)x;
  if (this->adjust (size * length, align, buf) == 0)
  {
    for(ACE_CDR::UInt i = 0; i < length; ++i) {
      if(this->do_byte_swap_) {
        std::reverse_copy(source, source + size, buf);
      } else {
        std::copy(source, source + size, buf);
      }
      source += size;
      buf += size;
    }

    return 1;
  }
  this->good_bit_ = 0;
  return 0;
}

ACE_CDR::Boolean
OutputStream::write_boolean_array (const ACE_CDR::Boolean* x,
                                    ACE_CDR::UInt length)
{
  // It is hard to optimize this, the spec requires that on the wire
  // booleans be represented as a byte with value 0 or 1, but in
  // memoery it is possible (though very unlikely) that a boolean has
  // a non-zero value (different from 1).
  // We resort to a simple loop.
  const ACE_CDR::Boolean* end = x + length;
  for (const ACE_CDR::Boolean* i = x; i != end && this->good_bit (); ++i)
    {
      this->write_boolean (*i);
    }
  return this->good_bit ();
}

// ****************************************************************

InputStream::InputStream (const char *buf,
                            size_t bufsiz,
                            int byte_order)
  : start_ (buf, bufsiz),
    do_byte_swap_ (ACE_CDR_BYTE_ORDER),
    good_bit_ (1),
    char_translator_ (0),
    wchar_translator_ (0)
{
  this->start_.wrPtr (bufsiz);
}

InputStream::InputStream (size_t bufsiz,
                            int byte_order)
  : start_ (bufsiz),
    do_byte_swap_ (ACE_CDR_BYTE_ORDER),
    good_bit_ (1),
    char_translator_ (0),
    wchar_translator_ (0)
{
}

InputStream::InputStream (const MessageBlock *data,
                            int byte_order)
  : start_ (),
    good_bit_ (1),
    char_translator_ (0),
    wchar_translator_ (0)
{
  this->reset (data, byte_order);
}

InputStream::InputStream (DataBlock *data,
                            int byte_order)
  : start_ (data),
    do_byte_swap_ (ACE_CDR_BYTE_ORDER),
    good_bit_ (1),
    char_translator_ (0),
    wchar_translator_ (0)
{
}

InputStream::InputStream (const InputStream& rhs,
                            size_t size,
                            ACE_CDR::Long offset)
  : start_ (rhs.start_.dataBlock ()->duplicate ()),
//    do_byte_swap_ (rhs.do_byte_swap_),
    do_byte_swap_ (ACE_CDR_BYTE_ORDER),
    good_bit_ (1),
    char_translator_ (0),
    wchar_translator_ (0)
{
  char* newpos = rhs.start_.rdPtr() + offset;
  if (this->start_.base () <= newpos
      && newpos <= this->start_.end ()
      && newpos + size <= this->start_.end ())
    {
      this->start_.rdPtr (newpos);
      this->start_.wrPtr (newpos + size);
    }
  else
    {
      this->good_bit_ = 0;
    }
}

InputStream::InputStream (const InputStream& rhs,
                            size_t size)
  : start_ (rhs.start_.dataBlock ()->duplicate ()),
//    do_byte_swap_ (rhs.do_byte_swap_),
    do_byte_swap_ (ACE_CDR_BYTE_ORDER),
    good_bit_ (1),
    char_translator_ (0),
    wchar_translator_ (0)
{
  char* newpos = rhs.start_.rdPtr();
  if (this->start_.base () <= newpos
      && newpos <= this->start_.end ()
      && newpos + size <= this->start_.end ())
    {
      // Notice that MessageBlock::duplicate may leave the
      // wrPtr() with a higher value that what we actually want.
      this->start_.rdPtr (newpos);
      this->start_.wrPtr (newpos + size);

      ACE_CDR::Octet byte_order;
      this->read_octet (byte_order);
//      this->do_byte_swap_ = (byte_order != ACE_CDR_BYTE_ORDER);
      this->do_byte_swap_ = (ACE_CDR_BYTE_ORDER);
    }
  else
    {
      this->good_bit_ = 0;
    }
}

InputStream::InputStream (const InputStream& rhs)
  : start_ (rhs.start_.dataBlock ()->duplicate ()),
//    do_byte_swap_ (rhs.do_byte_swap_),
    do_byte_swap_ (ACE_CDR_BYTE_ORDER),
    good_bit_ (1),
    char_translator_ (rhs.char_translator_),
    wchar_translator_ (rhs.wchar_translator_)
{
  this->start_.rdPtr (rhs.start_.rdPtr ());
  this->start_.wrPtr (rhs.start_.wrPtr ());
}

InputStream::InputStream (InputStream::Transfer_Contents x)
  : start_ (x.rhs_.start_.dataBlock ()),
//    do_byte_swap_ (x.rhs_.do_byte_swap_),
    do_byte_swap_ (ACE_CDR_BYTE_ORDER),
    good_bit_ (1),
    char_translator_ (x.rhs_.char_translator_),
    wchar_translator_ (x.rhs_.wchar_translator_)
{
  this->start_.rdPtr (x.rhs_.start_.rdPtr ());
  this->start_.wrPtr (x.rhs_.start_.wrPtr ());

  DataBlock* db = this->start_.dataBlock ()->cloneNoCopy ();
  (void) x.rhs_.start_.replaceDataBlock (db);
}

InputStream&
InputStream::operator= (const InputStream& rhs)
{
  if (this != &rhs)
    {
      this->start_.dataBlock (rhs.start_.dataBlock ()->duplicate ());
      this->start_.rdPtr (rhs.start_.rdPtr ());
      this->start_.wrPtr (rhs.start_.wrPtr ());
      this->do_byte_swap_ = rhs.do_byte_swap_;
      this->good_bit_ = 1;
    }
  return *this;
}

InputStream::InputStream (const OutputStream& rhs)
  : start_ (rhs.total_length () + ACE_CDR::MAX_ALIGNMENT),
//    do_byte_swap_ (rhs.do_byte_swap_),
    do_byte_swap_ (ACE_CDR_BYTE_ORDER),
    good_bit_ (1),
    char_translator_ (0),
    wchar_translator_ (0)
{
  ACE_CDR::mb_align (&this->start_);
  for (const MessageBlock *i = rhs.begin ();
       i != rhs.end ();
       i = i->cont ())
    this->start_.copy (i->rdPtr (), i->length ());
}

ACE_CDR::Boolean
InputStream::read_string (char *&x)
{
  // @@ This is a slight violation of "Optimize for the common case",
  // i.e. normally the translator will be 0, but OTOH the code is
  // smaller and should be better for the cache ;-) ;-)
  if (this->char_translator_ != 0)
    return this->char_translator_->read_string (*this, x);

  ACE_CDR::UInt len;

  this->read_uint (len);
  if (len > 0)
    {
      ACE_NEW_RETURN (x,
                      ACE_CDR::Char[len],
                      0);
      if (this->read_char_array (x, len))
        return 1;
      delete [] x;
    }

  x = 0;
  return 0;
}

ACE_CDR::Boolean
InputStream::read_string (std::string &x)
{
  ACE_CDR::Char *data;
  if (this->read_string (data))
    {
      x = data;
      delete [] data;
      return 1;
    }

  x = "";
  return 0;
}

ACE_CDR::Boolean
InputStream::read_string_seq (std::vector<std::string>& v)
{
  UInt sz = 0;
  this->read_uint(sz);

  if (sz > 0) {
    v.resize(sz);
    for (size_t i = 0; i < sz; i++) {
      //std::string x;
      this->read_string(v[i]);
    }
  }
  return 0;
}

ACE_CDR::Boolean
InputStream::read_wstring (ACE_CDR::WChar*& x)
{
  // @@ This is a slight violation of "Optimize for the common case",
  // i.e. normally the translator will be 0, but OTOH the code is
  // smaller and should be better for the cache ;-) ;-)
  if (this->wchar_translator_ != 0)
    return this->wchar_translator_->read_wstring (*this, x);

  ACE_CDR::UInt len;
  this->read_uint (len);
  if (this->good_bit())
    {
      ACE_NEW_RETURN (x,
                      ACE_CDR::WChar[len],
                      0);
      if (this->read_wchar_array (x, len))
        return 1;

      delete [] x;
    }
  x = 0;
  return 0;
}

ACE_CDR::Boolean
InputStream::read_array (void* x,
                          size_t size,
                          size_t align,
                          ACE_CDR::UInt length)
{
  char* buf;
  char* dest = (char*)x;
  if (this->adjust (size * length, align, buf) == 0)
    {
    for(ACE_CDR::UInt i = 0; i < length; ++i) {
      if(this->do_byte_swap_) {
        std::reverse_copy(buf, buf + size, dest);
//        ICC_ERROR("OutputStream::read_4 swaped, to read: %u, read: %u", *(ACE_CDR::UInt*)buf, *x);
      } else {
        std::copy(buf, buf + size, dest);
//        ICC_ERROR("OutputStream::read_4 no swap, to read: %u, read: %u", *(ACE_CDR::UInt*)buf, *x);
      }
      dest += size;
      buf += size;
    }

      return this->good_bit_;
    }
  return 0;
}

ACE_CDR::Boolean
InputStream::read_boolean_array (ACE_CDR::Boolean *x,
                                  ACE_CDR::UInt length)
{
  // It is hard to optimize this, the spec requires that on the wire
  // booleans be represented as a byte with value 0 or 1, but in
  // memoery it is possible (though very unlikely) that a boolean has
  // a non-zero value (different from 1).
  // We resort to a simple loop.
  for (ACE_CDR::ULong i = 0; i != length && this->good_bit_; ++i)
    {
      this->read_boolean (x[i]);
    }
  return this->good_bit_;
}

ACE_CDR::Boolean
InputStream::read_boolean_array (std::vector<bool>& v,
                                  ACE_CDR::UInt length)
{
  // It is hard to optimize this, the spec requires that on the wire
  // booleans be represented as a byte with value 0 or 1, but in
  // memoery it is possible (though very unlikely) that a boolean has
  // a non-zero value (different from 1).
  // We resort to a simple loop.
  for (ACE_CDR::ULong i = 0; i != length && this->good_bit_; ++i)
    {
      ACE_CDR::Boolean x;
      this->read_boolean (x);
      v.push_back(x);
    }
  return this->good_bit_;
}


ACE_CDR::Boolean
InputStream::read_1 (ACE_CDR::Octet *x)
{
  if (this->rdPtr () < this->end ())
    {
      *x = *ACE_reinterpret_cast (ACE_CDR::Octet*,this->rdPtr());
      this->start_.rdPtr (1);
      return 1;
    }

  this->good_bit_ = 0;
  return 0;
}

ACE_CDR::Boolean
InputStream::read_2 (ACE_CDR::UShort *x)
{
  char *buf;
  if (this->adjust (ACE_CDR::SHORT_SIZE, buf) == 0)
  {
    if(this->do_byte_swap_) {
      std::reverse_copy(buf, buf + 2, (char*)x);
    } else {
      std::copy(buf, buf + 2, (char*)x);
    }
    return 1;
//#if !defined (ACE_DISABLE_SWAP_ON_READ)
//      if (!this->do_byte_swap_)
//        {
//          *x = *ACE_reinterpret_cast (ACE_CDR::UShort*, buf);
//        }
//      else
//        {
//          ACE_CDR::swap_2 (buf, ACE_reinterpret_cast (char*, x));
//        }
//#else
//      *x = *ACE_reinterpret_cast(ACE_CDR::UShort*,buf);
//#endif /* ACE_DISABLE_SWAP_ON_READ */
//      return 1;
  }
  return 0;
}

ACE_CDR::Boolean
InputStream::read_4 (ACE_CDR::UInt *x)
{
  char *buf;
  if (this->adjust (ACE_CDR::INT_SIZE, buf) == 0)
  {
    if(this->do_byte_swap_) {
      std::reverse_copy(buf, buf + 4, (char*)x);
//      ICC_ERROR("OutputStream::read_4 swaped, to read: %u, read: %u", *(ACE_CDR::UInt*)buf, *x);
    } else {
      std::copy(buf, buf + 4, (char*)x);
//      ICC_ERROR("OutputStream::read_4 no swap, to read: %u, read: %u", *(ACE_CDR::UInt*)buf, *x);
    }
    return 1;
  }
  return 0;
}

ACE_CDR::Boolean
InputStream::read_8 (ACE_CDR::ULong *x)
{
  char *buf;
  if (this->adjust (ACE_CDR::LONG_SIZE, buf) == 0)
    {
    if(this->do_byte_swap_) {
      std::reverse_copy(buf, buf + 8, (char*)x);
    } else {
      std::copy(buf, buf + 8, (char*)x);
    }
    return 1;
//#if !defined (ACE_DISABLE_SWAP_ON_READ)
//      if (!this->do_byte_swap_)
//        {
//          *x = *ACE_reinterpret_cast (ACE_CDR::ULong *, buf);
//        }
//      else
//        {
//          ACE_CDR::swap_8 (buf, ACE_reinterpret_cast (char*, x));
//        }
//#else
//      *x = *ACE_reinterpret_cast(ACE_CDR::ULongLong*,buf);
//#endif /* ACE_DISABLE_SWAP_ON_READ */
//      return 1;
    }
  return 0;
}

ACE_CDR::Boolean
InputStream::read_16 (ACE_CDR::LongDouble *x)
{
  char *buf;
  if (this->adjust (ACE_CDR::LONGLONG_SIZE,
                    ACE_CDR::LONGLONG_ALIGN,
                    buf) == 0)
    {
    if(this->do_byte_swap_) {
      std::reverse_copy(buf, buf + 16, (char*)x);
    } else {
      std::copy(buf, buf + 16, (char*)x);
    }
    return 1;
//#if !defined (ACE_DISABLE_SWAP_ON_READ)
//      if (!this->do_byte_swap_)
//        {
//          *x = *ACE_reinterpret_cast (ACE_CDR::LongDouble *, buf);
//        }
//      else
//        {
//          ACE_CDR::swap_16 (buf, ACE_reinterpret_cast (char*, x));
//        }
//#else
//      *x = *ACE_reinterpret_cast(ACE_CDR::LongDouble*,buf);
//#endif /* ACE_DISABLE_SWAP_ON_READ */
//      return 1;
    }
  return 0;
}

ACE_CDR::Boolean
InputStream::skip_string (void)
{
  ACE_CDR::UInt len;
  if (this->read_uint (len))
    {
      if (this->rdPtr () + len <= this->end ())
        {
          this->rdPtr (len);
          return 1;
        }
      this->good_bit_ = 0;
    }

  return 0;
}

ACE_CDR::Boolean
InputStream::skip_bytes (size_t len)
{
  if (this->rdPtr () + len <= this->end ())
    {
      this->rdPtr (len);
      return 1;
    }
  this->good_bit_ = 0;
  return 0;
}

int
InputStream::grow (size_t newsize)
{
  if (ACE_CDR::grow (&this->start_, newsize) == -1)
    return -1;

  ACE_CDR::mb_align (&this->start_);
  this->start_.wrPtr (newsize);
  return 0;
}

void
InputStream::reset (const MessageBlock* data,
                     int byte_order)
{
  this->reset_byte_order (byte_order);
  ACE_CDR::consolidate (&this->start_, data);
}

void
InputStream::steal_from (InputStream &cdr)
{
//  this->do_byte_swap_ = cdr.do_byte_swap_;
  this->do_byte_swap_ = ACE_CDR_BYTE_ORDER;
  this->start_.dataBlock (cdr.start_.dataBlock ()->duplicate ());
  this->start_.rdPtr (cdr.start_.rdPtr ());
  this->start_.wrPtr (cdr.start_.wrPtr ());

  cdr.reset_contents ();
}

MessageBlock*
InputStream::steal_contents (void)
{
  MessageBlock* block =
    this->start_.clone ();
  this->start_.dataBlock (block->dataBlock ()->clone ());
  ACE_CDR::mb_align (&this->start_);

  return block;
}

void
InputStream::reset_contents (void)
{
  this->start_.dataBlock (this->start_.dataBlock ()->cloneNoCopy ());
}
