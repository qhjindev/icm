
#include <algorithm>

#include "BasicStream.h"

bool BasicStream::readSize(ULong& v)
{
  Byte byte;
  readByte(byte);
  unsigned char val = static_cast<unsigned char> (byte);
  if(val == 255) {
    readLong((Long&)v);
    if(v < 0)
      return false;
  } else {
    v = static_cast<int> (static_cast<unsigned char> (byte));
  }

  return true;
}

bool BasicStream::writeSize(ULong v)
{
  assert(v >= 0);
  if(v > 254) {
    writeByte(Byte(255));
    writeLong(v);
  } else {
    writeByte(static_cast<Byte> (v));
  }
  return true;
}

void BasicStream::resize(Container::size_type sz)
{
  b.resize(sz);
}

bool BasicStream::writeBool(bool v)
{
  b.push_back(static_cast<Byte>(v));
  return true;
}

bool BasicStream::writeBoolSeq(const std::vector<bool>& v)
{
  ULong sz = static_cast<ULong> (v.size());
  writeSize(sz);
  if(sz > 0) {
    Container::size_type pos = b.size();
    resize(pos + sz);
    copy(v.begin(), v.end(), b.begin() + pos);
  }
  return true;
}

bool BasicStream::readBool(bool & v)
{
  if (i > b.end())
    return false;
  v = *i++;
  return true;
}

bool BasicStream::readBoolSeq(std::vector<bool> &v)
{
  ULong sz;
  readSize(sz);
  if(sz > 0) {
    v.resize(sz);
    copy(i, i + sz, v.begin());
    i += sz;
  } else {
    v.clear();
  }

  return true;
}

bool BasicStream::writeByte(Byte v)
{
  b.push_back(v);
  return true;
}

bool BasicStream::writeByteSeq(const std::vector<Byte>& v)
{
  ULong sz = static_cast<ULong> (v.size());
  writeSize(sz);
  if(sz > 0) {
    Container::size_type pos = b.size();
    resize(pos + sz);
    memcpy(&b[pos], &v[0], sz);
  }
  return true;
}

bool BasicStream::readByte(Byte &v)
{
  if(i >= b.end())
    return false;
  v = *i++;
  return true;
}

bool BasicStream::readByteSeq(std::vector<Byte>& v)
{
  ULong sz;
  readSize(sz);
  if(sz > 0) {
    //vector<Byte> (i, i + sz).swap(v);
    i += sz;
  } else {
    v.clear();
  }

  return true;
}

bool BasicStream::writeShort(Short v)
{
  Container::size_type pos = b.size();
  resize(pos + sizeof(Short));
  Byte* dest = &b[pos];
#ifdef ACE_BIG_ENDIAN
  const Byte* src = reinterpret_cast<const Byte*>(&v) + sizeof(Short) - 1;
  *dest++ = *src--;
  *dest = *src;
#else
  const Byte* src = reinterpret_cast<const Byte*>(&v);
  *dest++ = *src++;
  *dest = *src;
#endif
  return true;
}

bool BasicStream::writeShortSeq(const std::vector<Short>& v)
{
  ULong sz = static_cast<ULong> (v.size());
  writeSize(sz);
  if(sz > 0) {
    Container::size_type pos = b.size();
    resize(pos + sz * sizeof(Short));
#ifdef ACE_BIG_ENDIAN
    const Byte* src = reinterpret_cast<const Byte*>(&v[0]) + sizeof(Short) - 1;
    Byte* dest = &(*(b.begin() + pos));
    for(int j=0; j < sz; ++j) {
      *dest++ = *src--;
      *dest++ = *src--;
      src += 2 * sizeof(Short);
    }
#else
    memcpy(&b[pos], reinterpret_cast<const Byte*> (&v[0]), sz * sizeof(Short));
#endif
  }
  return true;
}

bool BasicStream::writeLong(Long v)
{
  Container::size_type pos = b.size();
  resize(pos + sizeof(Long));
  Byte* dest = &b[pos];
#ifdef ACE_BIG_ENDIAN
  const Byte* src = reinterpret_cast<const Byte*>(&v) + sizeof(Long) -1;
  *dest++ = *src--;
  *dest++ = *src--;
  *dest++ = *src--;
  *dest++ = *src--;
  *dest++ = *src--;
  *dest++ = *src--;
  *dest++ = *src--;
  *dest = *src;
#else
  const Byte* src = reinterpret_cast<const Byte*> (&v);
//  *dest++ = *src++;
//  *dest++ = *src++;
//  *dest++ = *src++;
//  *dest++ = *src++;
  *dest++ = *src++;
  *dest++ = *src++;
  *dest++ = *src++;
  *dest = *src;
#endif

  return true;
}

bool BasicStream::writeLongSeq(const std::vector<Long>& v)
{
  ULong sz = static_cast<ULong> (v.size());
  writeSize(sz);
  if(sz > 0) {
    Container::size_type pos = b.size();
    resize(pos + sz * sizeof(Long));
#ifdef ACE_BIG_ENDIAN
    const Byte* src = reinterpret_cast<const Byte*>(&v[0]) + sizeof(Long) - 1;
    Byte* dest = &(*(b.begin() + pos));
    for(int j = 0; j < sz; ++j)
    {
      *dest++ = *src--;
      *dest++ = *src--;
      *dest++ = *src--;
      *dest++ = *src--;
      *dest++ = *src--;
      *dest++ = *src--;
      *dest++ = *src--;
      *dest++ = *src--;
      src += 2 * sizeof(Long);
    }
#else
  memcpy(&b[pos], reinterpret_cast<const Byte*> (&v[0]), sz * sizeof(Long));
#endif
  }
  return true;
}

bool BasicStream::writeFloat(float v)
{
  Container::size_type pos = b.size();
  resize(pos + sizeof(float));
  Byte* dest = &b[pos];
#ifdef ACE_BIG_ENDIAN
  const Byte* src = reinterpret_cast<const Byte*>(&v) + sizeof(float) - 1;
  *dest++ = *src--;
  *dest++ = *src--;
  *dest++ = *src--;
  *dest = *src;
#else
  const Byte* src = reinterpret_cast<const Byte*> (&v);
  *dest++ = *src++;
  *dest++ = *src++;
  *dest++ = *src++;
  *dest = *src;
#endif

  return true;
}

bool BasicStream::writeFloatSeq(const std::vector<float>& v)
{
  ULong sz = static_cast<ULong> (v.size());
  writeSize(sz);
  if (sz > 0) {
    Container::size_type pos = b.size();
    resize(pos + sz * sizeof(float));
#ifdef ACE_BIG_ENDIAN
    const Byte* src = reinterpret_cast<const Byte*>(&v[0]) + sizeof(float) - 1;
    Byte* dest = &(*(b.begin() + pos));
    for(int j = 0; j < sz; ++j)
    {
      *dest++ = *src--;
      *dest++ = *src--;
      *dest++ = *src--;
      *dest++ = *src--;
      src += 2 * sizeof(float);
    }
#else
    memcpy(&b[pos], reinterpret_cast<const Byte*> (&v[0]), sz * sizeof(float));
#endif
  }

  return true;
}

bool BasicStream::writeDouble(double v)
{
  Container::size_type pos = b.size();
  resize(pos + sizeof(double));
  Byte* dest = &b[pos];
#ifdef ACE_BIG_ENDIAN
  const Byte* src = reinterpret_cast<const Byte*>(&v) + sizeof(double) - 1;
  *dest++ = *src--;
  *dest++ = *src--;
  *dest++ = *src--;
  *dest++ = *src--;
  *dest++ = *src--;
  *dest++ = *src--;
  *dest++ = *src--;
  *dest = *src;
#else
  const Byte* src = reinterpret_cast<const Byte*> (&v);
  *dest++ = *src++;
  *dest++ = *src++;
  *dest++ = *src++;
  *dest++ = *src++;
  *dest++ = *src++;
  *dest++ = *src++;
  *dest++ = *src++;
  *dest = *src;
#endif
  return true;
}

bool BasicStream::writeDoubleSeq(const std::vector<double>& v)
{
  ULong sz = static_cast<ULong> (v.size());
  writeSize(sz);
  if (sz > 0) {
    Container::size_type pos = b.size();
    resize(pos + sz * sizeof(double));
#ifdef ACE_BIG_ENDIAN
    const Byte* src = reinterpret_cast<const Byte*>(&v[0]) + sizeof(double) - 1;
    Byte* dest = &(*(b.begin() + pos));
    for(int j = 0; j < sz; ++j)
    {
      *dest++ = *src--;
      *dest++ = *src--;
      *dest++ = *src--;
      *dest++ = *src--;
      *dest++ = *src--;
      *dest++ = *src--;
      *dest++ = *src--;
      *dest++ = *src--;
      src += 2 * sizeof(double);
    }
#else
    memcpy(&b[pos], reinterpret_cast<const Byte*> (&v[0]), sz * sizeof(double));
#endif
  }
  return true;
}

bool BasicStream::writeString(const std::string& v)
{
  ULong sz = static_cast<ULong> (v.size());
  writeSize(sz);
  if (sz > 0) {
    Container::size_type pos = b.size();
    resize(pos + sz);
    memcpy(&b[pos], v.c_str(), sz);
  }
  return true;
}

bool BasicStream::writeStringSeq(const std::vector<std::string>& v)
{
  ULong sz = static_cast<ULong> (v.size());
  writeSize(sz);
  if (sz > 0) {
    std::vector<std::string>::const_iterator p;
    for (p = v.begin(); p != v.end(); ++p) {
      writeString(*p);
    }
  }
  return true;
}

bool BasicStream::readShort(Short& v)
{
  if(b.end() - i < static_cast<int> (sizeof(Short))) {
    return false;
  }
  const Byte* src = &(*i);
  i += sizeof(Short);
#ifdef ACE_BIG_ENDIAN
  Byte* dest = reinterpret_cast<Byte*>(&v) + sizeof(Short) - 1;
  *dest-- = *src++;
  *dest = *src;
#else
  Byte* dest = reinterpret_cast<Byte*> (&v);
  *dest++ = *src++;
  *dest = *src;
#endif
  return true;
}

bool BasicStream::readShortSeq(std::vector<Short>& v)
{
  ULong sz;
  readSize(sz);
  if (sz > 0) {
    //checkFixedSeq(sz, static_cast<int> (sizeof(Short)));
    Container::iterator begin = i;
    i += sz * static_cast<int> (sizeof(Short));
    v.resize(sz);
#ifdef ACE_BIG_ENDIAN
    const Byte* src = &(*begin);
    Byte* dest = reinterpret_cast<Byte*>(&v[0]) + sizeof(Short) - 1;
    for(int j = 0; j < sz; ++j)
    {
      *dest-- = *src++;
      *dest-- = *src++;
      dest += 2 * sizeof(Short);
    }
#else
    std::copy(begin, i, reinterpret_cast<Byte*> (&v[0]));
#endif
  } else {
    v.clear();
  }
  return true;
}


  //bool readInt(Int);
  //bool readIntSeq(const std::vector<Int>&);

bool BasicStream::readLong(Long& v)
{
  if (b.end() - i < static_cast<int> (sizeof(Long))) {
    //throw UnmarshalOutOfBoundsException(__FILE__, __LINE__);
    return false;
  }
  const Byte* src = &(*i);
  i += sizeof(Long);
#ifdef ACE_BIG_ENDIAN
  Byte* dest = reinterpret_cast<Byte*>(&v) + sizeof(Long) - 1;
  *dest-- = *src++;
  *dest-- = *src++;
  *dest-- = *src++;
  *dest-- = *src++;
  *dest-- = *src++;
  *dest-- = *src++;
  *dest-- = *src++;
  *dest = *src;
#else
  Byte* dest = reinterpret_cast<Byte*> (&v);
//  *dest++ = *src++;
//  *dest++ = *src++;
//  *dest++ = *src++;
//  *dest++ = *src++;
  *dest++ = *src++;
  *dest++ = *src++;
  *dest++ = *src++;
  *dest = *src;
#endif
  return true;
}

bool BasicStream::readLongSeq(std::vector<Long>& v)
{
  ULong sz;
  readSize(sz);
  if (sz > 0) {
    //checkFixedSeq(sz, static_cast<int> (sizeof(Long)));
    Container::iterator begin = i;
    i += sz * static_cast<int> (sizeof(Long));
    v.resize(sz);
#ifdef ACE_BIG_ENDIAN
    const Byte* src = &(*begin);
    Byte* dest = reinterpret_cast<Byte*>(&v[0]) + sizeof(Long) - 1;
    for(int j = 0; j < sz; ++j)
    {
      *dest-- = *src++;
      *dest-- = *src++;
      *dest-- = *src++;
      *dest-- = *src++;
      *dest-- = *src++;
      *dest-- = *src++;
      *dest-- = *src++;
      *dest-- = *src++;
      dest += 2 * sizeof(Long);
    }
#else
    std::copy(begin, i, reinterpret_cast<Byte*> (&v[0]));
#endif
  } else {
    v.clear();
  }
  return true;
}

bool BasicStream::readFloat(float& v)
{
  if (b.end() - i < static_cast<int> (sizeof(float))) {
    //throw UnmarshalOutOfBoundsException(__FILE__, __LINE__);
    return false;
  }
  const Byte* src = &(*i);
  i += sizeof(float);
#ifdef ACE_BIG_ENDIAN
  Byte* dest = reinterpret_cast<Byte*>(&v) + sizeof(float) - 1;
  *dest-- = *src++;
  *dest-- = *src++;
  *dest-- = *src++;
  *dest = *src;
#else
  Byte* dest = reinterpret_cast<Byte*> (&v);
  *dest++ = *src++;
  *dest++ = *src++;
  *dest++ = *src++;
  *dest = *src;
#endif
  return true;
}

bool BasicStream::readFloatSeq(std::vector<float>& v)
{
  ULong sz;
  readSize(sz);
  if (sz > 0) {
    //checkFixedSeq(sz, static_cast<int> (sizeof(float)));
    Container::iterator begin = i;
    i += sz * static_cast<int> (sizeof(float));
    v.resize(sz);
#ifdef ACE_BIG_ENDIAN
    const Byte* src = &(*begin);
    Byte* dest = reinterpret_cast<Byte*>(&v[0]) + sizeof(float) - 1;
    for(int j = 0; j < sz; ++j)
    {
      *dest-- = *src++;
      *dest-- = *src++;
      *dest-- = *src++;
      *dest-- = *src++;
      dest += 2 * sizeof(float);
    }
#else
    std::copy(begin, i, reinterpret_cast<Byte*> (&v[0]));
#endif
  } else {
    v.clear();
  }
  return true;
}


bool BasicStream::readDouble(double& v)
{
  if (b.end() - i < static_cast<int> (sizeof(double))) {
    //throw UnmarshalOutOfBoundsException(__FILE__, __LINE__);
    return false;
  }
  const Byte* src = &(*i);
  i += sizeof(double);
#ifdef ACE_BIG_ENDIAN
  Byte* dest = reinterpret_cast<Byte*>(&v) + sizeof(double) - 1;
  *dest-- = *src++;
  *dest-- = *src++;
  *dest-- = *src++;
  *dest-- = *src++;
  *dest-- = *src++;
  *dest-- = *src++;
  *dest-- = *src++;
  *dest = *src;
#else
  Byte* dest = reinterpret_cast<Byte*> (&v);
  *dest++ = *src++;
  *dest++ = *src++;
  *dest++ = *src++;
  *dest++ = *src++;
  *dest++ = *src++;
  *dest++ = *src++;
  *dest++ = *src++;
  *dest = *src;
#endif
  return true;
}

bool BasicStream::readDoubleSeq(std::vector<double>& v)
{
  ULong sz;
  readSize(sz);
  if (sz > 0) {
    //checkFixedSeq(sz, static_cast<int> (sizeof(double)));
    Container::iterator begin = i;
    i += sz * static_cast<int> (sizeof(double));
    v.resize(sz);
#ifdef ACE_BIG_ENDIAN
    const Byte* src = &(*begin);
    Byte* dest = reinterpret_cast<Byte*>(&v[0]) + sizeof(double) - 1;
    for(int j = 0; j < sz; ++j)
    {
      *dest-- = *src++;
      *dest-- = *src++;
      *dest-- = *src++;
      *dest-- = *src++;
      *dest-- = *src++;
      *dest-- = *src++;
      *dest-- = *src++;
      *dest-- = *src++;
      dest += 2 * sizeof(double);
    }
#else
    std::copy(begin, i, reinterpret_cast<Byte*> (&v[0]));
#endif
  } else {
    v.clear();
  }
  return true;
}

bool BasicStream::readString(std::string& v)
{
  ULong sz;
  readSize(sz);
  if (sz > 0) {
    if (b.end() - i < (int)sz) {
      //throw UnmarshalOutOfBoundsException(__FILE__, __LINE__);
      return false;
    }
    //string(reinterpret_cast<const char*> (&*i), reinterpret_cast<const char*> (&*i) + sz).swap(v);
    //	v.assign(reinterpret_cast<const char*>(&(*i)), sz);
    i += sz;
  } else {
    v.clear();
  }
  return true;
}

bool BasicStream::readStringSeq(std::vector<std::string>& v)
{
  ULong sz;
  readSize(sz);
  if (sz > 0) {
    //startSeq(sz, 1);
    v.resize(sz);
    for (ULong i = 0; i < sz; ++i) {
      readString(v[i]);
      //checkSeq();
      //endElement();
    }
    //endSeq(sz);
  } else {
    v.clear();
  }
  return true;
}

