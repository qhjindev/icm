#include "os/OS_NS_string.h"
#include "icc/Log.h"
#include "icc/MessageBlock.h"
#include <assert.h>
#include <string.h>
#include <algorithm>

MessageBlock::MessageBlock ()
: m_dataBlock (0)
{
  this->initI (0,   //size
               0,   //cont
               0,   //data
               false, //delete data
               0);  //data block
}

MessageBlock::~MessageBlock ()
{
  if (this->dataBlock ())
    this->m_dataBlock->release ();

  this->m_prev = 0;
  this->m_next = 0;
}

int
MessageBlock::initI (size_t size,
                     MessageBlock* msgCont,
                     const char* data,
                     bool deleteData,
                     DataBlock* db)
{
  this->m_rdPtr = 0;
  this->m_wrPtr = 0;
  this->m_cont = msgCont;
  this->m_next = 0;
  this->m_prev = 0;

  if (this->m_dataBlock != 0) {
    this->m_dataBlock->release ();
    this->m_dataBlock = 0;
  }

  if (db == 0) {
    db = new DataBlock (size, data, deleteData);
  }

  this->dataBlock (db);
  return 0;
}

MessageBlock::MessageBlock(const char *data, size_t size)
: m_dataBlock (0)
{
  if (this->initI (size, 0, data, false, 0) == -1) {

    ACE_ERROR ((LM_ERROR,"MessageBlock"));
  }
}

MessageBlock::MessageBlock(size_t size, MessageBlock *cont, const char *data, bool clone)
: m_dataBlock (0)
{
  if (this->initI (size,
                   cont,
                   data,
//                   data ? false : true,
                   clone,
                   0) == -1) {
    ACE_ERROR ((LM_ERROR,"MessageBlock"));
  }
}

MessageBlock::MessageBlock (size_t size,
                            MessageBlock* cont,
                            const char* data,
                            bool deleteData,
                            DataBlock* db)
{
  if (this->initI (size,
                   cont,
                   data,
                   deleteData,
                   db) == -1) {
    ACE_ERROR ((LM_ERROR,"MessageBlock"));
  }
}

MessageBlock::MessageBlock (DataBlock* dataBlock)
: m_dataBlock (0)
{
  if (this->initI (0, 0, 0, false, dataBlock) == -1) {
    ACE_ERROR ((LM_ERROR,"MessageBlock"));
  }
}

int
MessageBlock::init (size_t size, MessageBlock* cont, const char* data)
{
  return this->initI (size,
                      cont,
                      data,
                      data ? true : false,
                      0);
}

int
MessageBlock::init (const char* data, size_t size)
{
  return this->initI (size, 0, data, false, 0);
}

MessageBlock*
MessageBlock::duplicate(void) const
{
  MessageBlock* nb = new MessageBlock (0, 0, 0, false, this->dataBlock ()->duplicate ());
  nb->rdPtr (this->m_rdPtr);
  nb->wrPtr (this->m_wrPtr);

  if (this->m_cont) {
    nb->m_cont = this->m_cont->duplicate ();

    if (nb->m_cont == 0) {
      nb->release ();
      nb = 0;
    }
  }

  return nb;
}

MessageBlock*
MessageBlock::duplicate(const MessageBlock *mb)
{
  if (mb == 0)
    return 0;
  else
    return mb->duplicate ();
}

MessageBlock*
MessageBlock::clone () const
{
  DataBlock* db = this->m_dataBlock->clone ();
  if (db == 0)
    return 0;

  MessageBlock* nb = new MessageBlock (0, 0, 0, false, db);
  if (nb == 0) {
    db->release ();
    return 0;
  }

  nb->rdPtr (this->m_rdPtr);
  nb->wrPtr (this->m_wrPtr);

  if (this->cont () != 0 &&
     (nb->m_cont = this->cont ()->clone()) == 0) {
       nb->release ();
       return 0;
  }

  return nb;
}

MessageBlock &
MessageBlock::operator = (const MessageBlock &)
{
  return *this;
}

void
MessageBlock::dataBlock (DataBlock* db)
{
  if (this->m_dataBlock != 0)
    this->m_dataBlock->release ();

  this->m_dataBlock = db;

  this->rdPtr (this->dataBlock()->base());
  this->wrPtr (this->dataBlock()->base());
}

int
MessageBlock::copy (const char* buf, size_t n)
{
  size_t len = this->end() - this->wrPtr();

  if (len < n)
    return -1;
  else {
    (void) OS::memcpy (this->wrPtr (), buf, n);
    this->wrPtr (n);
    return 0;
  }
}

int
MessageBlock::copy (const char* buf)
{
  size_t len = this->end() - this->wrPtr();
  size_t bufLen = strlen (buf) + 1;
  if (len < bufLen)
    return -1;
  else {
    (void) OS::memcpy (this->wrPtr (), buf, bufLen);
    this->wrPtr (bufLen);
    return 0;
  }
}

void
MessageBlock::crunch (void)
{
  if (this->rdPtr () > this->base()) {
    size_t len = this->length ();
    (void) OS::memmove (this->base (), this->rdPtr (), len);
    this->rdPtr (this->base ());
    this->wrPtr (this->base () + len);
  }
}

DataBlock*
MessageBlock::dataBlock (void) const
{
  return this->m_dataBlock;
}

int
MessageBlock::size (size_t length)
{
  if (this->dataBlock ()->size (length) == -1)
    return -1;

  return 0;
}

size_t
MessageBlock::totalSize (void) const
{
  size_t size = 0;

  for (const MessageBlock* i = this; i != 0; i = i->cont ()) {
    size += i->size ();
  }

  return size;
}

size_t
MessageBlock::totalLength(void) const
{
  size_t length = 0;
  for (const MessageBlock* i = this; i != 0; i = i->cont ()) {
    length += i->length ();
  }

  return length;
}

size_t
MessageBlock::totalCapacity(void) const
{
  size_t size = 0;

  for (const MessageBlock* i = this; i != 0; i = i->cont ()) {
    size += i->capacity ();
  }

  return size;
}

MessageBlock*
MessageBlock::release (void)
{
  if (this->m_cont) {
    MessageBlock* mb = this->m_cont;
    MessageBlock* tmp;

    do {
      tmp = mb;
      mb = mb->m_cont;
      tmp->m_cont = 0;

      DataBlock* db = tmp->dataBlock ();
      if(db != 0) {
        db->release ();
      }
    } while (mb);

    this->m_cont = 0;
  }

  if (this->dataBlock ()) {
    this->dataBlock()->release ();
    this->m_dataBlock = 0;
  }

  delete this;

  return 0;
}

MessageBlock*
MessageBlock::release (MessageBlock* mb)
{
  if (mb != 0)
    return mb->release ();
  else
    return 0;
}

int
MessageBlock::referenceCount(void) const
{
  return dataBlock () ? dataBlock ()->referenceCount () : 0;
}

size_t
MessageBlock::length(void) const
{
  return this->m_wrPtr - this->m_rdPtr;
}

void
MessageBlock::length(size_t len)
{
  this->m_wrPtr = this->m_rdPtr + len;
}

size_t
MessageBlock::size (void) const
{
  return this->m_dataBlock->size ();
}

size_t
MessageBlock::capacity(void) const
{
  return this->m_dataBlock->capacity ();
}

char*
MessageBlock::base (void) const
{
  return this->m_dataBlock->base ();
}

void
MessageBlock::base (char* msgData, size_t msgLength)
{
  this->m_rdPtr = 0;
  this->m_wrPtr = 0;
  this->m_dataBlock->base (msgData, msgLength);
}

char*
MessageBlock::rdPtr (void) const
{
  return this->base () + this->m_rdPtr;
}

void
MessageBlock::wrPtr (char* newPtr)
{
  this->m_wrPtr = newPtr - this->base ();
}

char*
MessageBlock::mark (void) const
{
  return this->dataBlock ()->mark ();
}

char*
MessageBlock::end (void) const
{
  return this->dataBlock()->end();
}

void
MessageBlock::rdPtr (char* newPtr)
{
  this->m_rdPtr = newPtr - this->base ();
}

void
MessageBlock::rdPtr (size_t n)
{
  this->m_rdPtr += n;
}

char*
MessageBlock::wrPtr(void) const
{
  return this->base () + this->m_wrPtr;
}

void
MessageBlock::wrPtr(size_t n)
{
  this->m_wrPtr += n;
}

void
MessageBlock::reset()
{
  this->m_rdPtr = 0;
  this->m_wrPtr = 0;
}

size_t
MessageBlock::space (void) const
{
  return this->mark () - this->wrPtr ();
}

DataBlock*
MessageBlock::replaceDataBlock(DataBlock *db)
{
  DataBlock* old = this->m_dataBlock;
  this->m_dataBlock = db;

  this->rdPtr (this->m_dataBlock->base ());
  this->wrPtr (this->m_dataBlock->base ());

  return old;
}

MessageBlock*
MessageBlock::cont (void) const
{
  return this->m_cont;
}

void
MessageBlock::cont (MessageBlock *mb)
{
  this->m_cont = mb;
}

MessageBlock *
MessageBlock::next (void) const
{
  return this->m_next;
}

void
MessageBlock::next (MessageBlock *mb)
{
  this->m_next = mb;
}

MessageBlock *
MessageBlock::prev (void) const
{
  return this->m_prev;
}

void
MessageBlock::prev (MessageBlock *mb)
{
  this->m_prev = mb;
}

/// ******************************************************** ////

DataBlock::DataBlock (void)
  : m_curSize(0),
    m_maxSize(0),
    m_base(0),
    m_referenceCount(1),
    m_deleteData (false)
{
}

DataBlock::DataBlock (size_t size, const char* msgData, bool deleteData)
  : m_curSize(size),
    m_maxSize(size),
    m_base((char*)msgData),
    m_referenceCount(1),
    m_deleteData (deleteData)
{
  if(m_deleteData == false) {
    if(msgData == 0 && size != 0) {
      this->m_base = new char[size];
      m_deleteData = true;
    } else {
      this->m_base = (char*)msgData;
    }
  } else {
    if(msgData == 0 && size != 0) {
      this->m_base = new char[size];
    } else if(msgData != 0 && size != 0){
      this->m_base = new char[size];
      std::copy(msgData, msgData + size, m_base);
    }
  }
}

DataBlock::~DataBlock (void)
{
  assert(this->m_referenceCount == 0);

  if (this->m_deleteData)
    delete[] this->m_base;
}

char*
DataBlock::base (void) const
{
  return this->m_base;
}

void
DataBlock::base (char* msgData, size_t msgLength)
{
  if(this->m_deleteData) {
    delete[] this->m_base;
  }
  this->m_maxSize = msgLength;
  this->m_curSize = msgLength;
  this->m_base = msgData;
  this->m_deleteData = false;
}

char*
DataBlock::end (void) const
{
  return this->m_base + this->m_maxSize;
}

size_t
DataBlock::capacity (void) const
{
  return this->m_maxSize;
}

char*
DataBlock::mark (void) const
{
  return this->m_base + this->m_curSize;
}

size_t
DataBlock::size (void) const
{
  return this->m_curSize;
}

int
DataBlock::size (size_t length)
{
  if (length <= this->m_maxSize) {
    this->m_curSize = length;
  } else {
    char* buf = new char[length];
    OS::memcpy (buf, this->m_base, this->m_curSize);
    if(this->m_deleteData) {
      delete[] this->m_base;
    }
    this->m_maxSize = length;
    this->m_curSize = length;
    this->m_base = buf;
  }

  return 0;
}

DataBlock*
DataBlock::clone (void) const
{
  DataBlock* nb = new DataBlock (this->m_maxSize, 0, true);
  OS::memcpy (nb->m_base, this->m_base, this->m_maxSize);

  return nb;
}

DataBlock*
DataBlock::cloneNoCopy (void) const
{
  DataBlock* nb = new DataBlock (this->m_maxSize, 0, true);

  return nb;
}

DataBlock*
DataBlock::duplicate (void)
{
  this->m_referenceCount++;

  return this;
}

int
DataBlock::referenceCount (void) const
{
  return this->m_referenceCount;
}

DataBlock*
DataBlock::release (void)
{
  this->m_referenceCount--;

  DataBlock* result = 0;
  if (this->m_referenceCount == 0)
    delete this;
  else
    result = this;

  return result;
}

