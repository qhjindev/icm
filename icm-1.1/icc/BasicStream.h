#ifndef _ICC_BASIC_STREAM_H_
#define _ICC_BASIC_STREAM_H_

#include <vector>
#include <string>
#include <assert.h>
#include "os/Basic_Types.h"

class BasicStream
{
public:
  BasicStream()
  {
    i = b.begin();
  }

  BasicStream (const BasicStream& orig) :
    b (orig.b), i(orig.i)
  {
  }

  bool writeBool(bool);
  bool writeBoolSeq(const std::vector<bool>&);

  bool writeByte(Byte);
  bool writeByteSeq(const std::vector<Byte>&);

  bool writeShort(Short);
  bool writeShortSeq(const std::vector<Short>&);

  //bool writeInt(Int);
  //bool writeIntSeq(const std::vector<Int>&);

  bool writeLong(Long);
  bool writeLongSeq(const std::vector<Long>&);

  bool writeFloat(float);
  bool writeFloatSeq(const std::vector<float>&);

  bool writeDouble(double);
  bool writeDoubleSeq(const std::vector<double>&);

  bool writeString(const std::string&);
  bool writeStringSeq(const std::vector<std::string>&);

  bool readBool(bool&);
  bool readBoolSeq(std::vector<bool>&);

  bool readByte(Byte& v);
  bool readByteSeq(std::vector<Byte>&);

  bool readShort(Short&);
  bool readShortSeq(std::vector<Short>&);

  //bool readInt(Int);
  //bool readIntSeq(const std::vector<Int>&);

  bool readLong(Long&);
  bool readLongSeq(std::vector<Long>&);

  bool readFloat(float&);
  bool readFloatSeq(std::vector<float>&);

  bool readDouble(double&);
  bool readDoubleSeq(std::vector<double>&);

  bool readString(std::string&);
  bool readStringSeq(std::vector<std::string>&);

  bool readSize(ULong& v);
  bool writeSize(ULong v);

  class Container
  {
  public:
    typedef Byte value_type;
    typedef Byte* iterator;
    typedef const Byte* const_iterator;
    typedef Byte& reference;
    typedef const Byte& const_reference;
    typedef Byte* pointer;
    typedef size_t size_type;

    Container() : mBuf(0),mSize(0),mCapacity(0) 
    {
    }

    Container(const Container& c)
    {
      mBuf = 0;
      mSize = 0;
      mCapacity = 0;
      this->resize (c.size ());
      memcpy (this->mBuf, c.begin(), c.size());
    }

    ~Container()
    {
      free(mBuf);
    }

    iterator begin()
    {
      return mBuf;
    }

    const_iterator begin() const
    {
      return mBuf;
    }

    iterator end()
    {
      return mBuf + mSize;
    }

    const_iterator end() const
    {
      return mBuf + mSize;
    }

    size_type size() const
    {
      return mSize;
    }

    bool empty() const
    {
      return !mSize;
    }

    void clear()
    {
      free(mBuf);
      mBuf = 0;
      mSize = 0;
      mCapacity = 0;
    }

    bool resize(size_type n)
    {
      if(n == 0) {
        clear();
      } else {
        if(n > mCapacity) {
          mCapacity = std::max<size_type>(n, 2 * mCapacity);
          mCapacity = std::max<size_type>(static_cast<size_type> (240), mCapacity);

          if(mBuf) {
	    pointer old_mBuf = mBuf; 
            mBuf = reinterpret_cast<pointer> (realloc(mBuf, mCapacity));
	    if(mBuf == NULL) {
		free(old_mBuf);
	    }
          } else {
            mBuf = reinterpret_cast<pointer> (malloc(mCapacity));
          }

          if(!mBuf) 
            return false;
        }

        mSize = n;
      }

      return true;
    }

    void push_back(value_type v)
    {
      resize(mSize + 1);
      mBuf[mSize - 1] = v;
    }

    reference operator[](size_type n)
    {
      assert(n < mSize);
      return mBuf[n];
    }

    const_reference operator[](size_type n) const
    {
      assert(n < mSize);
      return mBuf[n];
    }

  private:
    //Container(const Container&);
    //void operator=(const Container&);

    pointer mBuf;
    size_type mSize;
    size_type mCapacity;
  };

  void resize(Container::size_type sz);

  Container b;
  Container::iterator i;
};




#endif //_ICC_BASIC_STREAM_H_
