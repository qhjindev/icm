#ifndef _ICC_MESSAGE_BLOCK_H
#define _ICC_MESSAGE_BLOCK_H

#include <cstddef>

class DataBlock;

class MessageBlock
{
public:
  MessageBlock ();

  MessageBlock (DataBlock*);

  MessageBlock (const char *data, size_t size = 0);

  MessageBlock (size_t size, MessageBlock* cont = 0, const char* data = 0, bool clone = false);

  int init (const char* data, size_t size = 0);

  int init (size_t size, MessageBlock* cont = 0, const char* data = 0);

  virtual ~MessageBlock (void);

  virtual MessageBlock* clone (void) const;

  MessageBlock* duplicate (void) const;

  static MessageBlock *duplicate (const MessageBlock* mb);

  MessageBlock* release (void);

  static MessageBlock* release (MessageBlock* mb);

  int copy (const char* buf, size_t n);

  int copy (const char* buf);

  void crunch (void);

  void reset (void);

  char* base (void) const;

  void base (char* data, size_t size);

  char* end (void) const;

  char* mark (void) const;

  // Get the read pointer.
  char* rdPtr (void) const;

  // Set the read pointer to <ptr>.
  void rdPtr (char* ptr);

  // Set the read pointer ahead <n> bytes.
  void rdPtr (size_t n);

  // Get the write pointer.
  char *wrPtr (void) const;

  // Set the write pointer to <ptr>.
  void wrPtr (char *ptr);

  // Set the write pointer ahead <n> bytes.  This is used to compute
  // the <length> of a message.
  void wrPtr (size_t n);

  // = Message length is <wr_ptr> - <rd_ptr>.
  // Get the length of the message
  size_t length (void) const;

  // Set the length of the message
  void length (size_t n);

  // Get the length of the <Message_Block>s, including chained
  // <Message_Block>s.
  size_t totalLength (void) const;

  // = Set/get <Message_Block> size info.
  // Get the total number of bytes in all <MessageBlock>s, including
  // chained <MessageBlock>s.
  size_t totalSize (void) const;

  // Get the number of bytes in the top-level <MessageBlock> (i.e.,
  // does not consider the bytes in chained <MessageBlock>s).
  size_t size (void) const;

  // Set the number of bytes in the top-level <MessageBlock>,
  // reallocating space if necessary.  However, the <rd_ptr_> and
  // <wr_ptr_> remain at the original offsets into the buffer, even if
  // it is reallocated.  Returns 0 if successful, else -1.
  int size (size_t length);

  // Get the number of allocated bytes in all <MessageBlock>, including
  // chained <MessageBlock>s.
  size_t totalCapacity (void) const;

  // Get the number of allocated bytes in the top-level <Message_Block>.
  size_t capacity (void) const;

  // Get the number of bytes available after the <wr_ptr_> in the
  // top-level <MessageBlock>.
  size_t space (void) const;

  // = <Data_Block> methods.
  // Get a pointer to the data block. Note that the <MessageBlock>
  // still references the block; this call does not change the reference
  // count.
  DataBlock *dataBlock (void) const;

  // Set a new data block pointer. The original <DataBlock> is released
  // as a result of this call. If you need to keep the original block, call
  // <replace_data_block> instead. Upon return, this <MessageBlock>
  // holds a pointer to the new <DataBlock>, taking over the reference
  // you held on it prior to the call.
  void dataBlock (DataBlock *);

  // Set a new data block pointer. A pointer to the original <ACE_Data_Block>
  // is returned, and not released (as it is with <data_block>).
  DataBlock *replaceDataBlock (DataBlock*);

  MessageBlock* cont (void) const;

  void cont (MessageBlock *mb);

  MessageBlock *next (void) const;

  void next (MessageBlock *mb);

  MessageBlock *prev (void) const;

  void prev (MessageBlock *mb);

  int referenceCount (void) const;

protected:

  MessageBlock (size_t size, 
                MessageBlock* cont, 
                const char* data, 
                bool deleteData,
                DataBlock* db);

  int initI (size_t size,
             MessageBlock* cont,
             const char* data,
             bool deleteData,
             DataBlock* db);

  size_t m_rdPtr;

  size_t m_wrPtr;

  MessageBlock *m_cont;

  MessageBlock *m_next;

  MessageBlock *m_prev;

  DataBlock *m_dataBlock;

private:
  MessageBlock &operator= (const MessageBlock &);
  MessageBlock (const MessageBlock &);

};

class DataBlock
{
public:
  DataBlock(void);

  DataBlock(size_t size, const char* msgData, bool deleteData);

  virtual ~DataBlock(void);

  char* base(void) const;

  void base(char* data, size_t size);

  char* end(void) const;

  char* mark(void) const;

  size_t size(void) const;

  int size(size_t length);

  size_t capacity (void) const;

  virtual DataBlock* clone () const;

  virtual DataBlock* cloneNoCopy () const;

  DataBlock* duplicate (void);

  DataBlock* release (void);

  int referenceCount (void) const;

protected:

  size_t m_curSize;

  size_t m_maxSize;

  char* m_base;

  int m_referenceCount;

  //Whether we should delete base on destructor
  bool m_deleteData;

private:
  DataBlock &operator= (const DataBlock &);
  DataBlock (const DataBlock &);
};


#endif //_ICC_MESSAGE_BLOCK_H
