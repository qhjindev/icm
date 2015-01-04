/*
 * example usage of MessageBlock
 *
 * This example demonstrates:
 *
 * 1) How to create new MessageBlock, use new allocated memory owned by itself,
 * or use outer memory block passed in,which it doesn't own, that is , not in charge of releasing that memory 
 *      
 * 2) How to clone, duplicate, the MessageBlock
 *
 * 3) How to process data in the MessageBlock
 * 
 * 4) How to destroy the MessageBlock properly
 */

#include "icc/MessageBlock.h"
#include "os/OS_NS_string.h"
#include <string>
#include <iostream>
#include <algorithm>

using namespace std;

int main() {
  /////////////////////////////////////////
  //////// example of creation
  MessageBlock* mb1 = new MessageBlock; // empty data block

  // use buf as data block's bottom memory, but mb2 doesn't own buf, that is,
  // when call delete mb2, or mb2->release(), buf's memory isn't deallocated
  char buf[256];
  MessageBlock* mb2 = new MessageBlock(buf, sizeof(buf)); 

  // same as mb2, use buf1 as mb3's data block's memory, mb3 doesn't own buf1
  // meanwhile mb3 is the MessageBlock chain's head, and the m_cont point to mb2
  char buf1[512];
  MessageBlock* mb3 = new MessageBlock(sizeof(buf1), mb2, buf1);

  // this will allcate 512 bytes heap memory, and mb4 own this memory,
  // wen mb4 is released, this memory will be deallocated
  MessageBlock* mb4 = new MessageBlock(0, 512);

  /////////////////////////////////////////
  //////// example of clone, duplicate, copy construction, assignment...
  // clone() will recreate the whole MessageBlock chain, and the bottom datablock of each MessageBlock is
  // new allocated, each datablock's reference count inited to 1
  MessageBlock* mb5 = mb3->clone();

  // clone() will recreate the whole MessageBlock chain too, the difference from clone() is,
  // the bottom datablok of each MessageBlock is not new allocated, just increase the reference count
  MessageBlock* mb6 = mb3->duplicate();
  
  // assignment and copy construction is forbidden(private member function), so code below are illegal:
  // MessageBlock mb7(*mb6);
  // *mb6 = *mb5;


  ///////////////////////////////////////////
  //////// example of data process 
  string str("testString");
  // mb1 use str's buffer as bottom datablock's memory, but not responsible to free this memory
  mb1->base((char*)str.c_str(), str.size());
  char* cstr = mb1->base();
  cout << "cstr: " << cstr << endl;
  // base() member process data with type of char*, so if it is utilized to process concrete type, 
  // the pointer must be properly converted to the real type first
  string* str1 = new string("testString1");
  // here the second arg of base(char*, size_t) is arbitrary, for we just use the pointer, not care the data
  mb1->base((char*)str1, 1); 
  string* pstr1 = (string*)mb1->base(); // convert to string* first before use
  cout << "str1: " << *pstr1 << endl;
  delete pstr1;

  // copy something into MessageBlock, if there is enough space, copy succeed, else return -1
  mb2->copy(str.c_str(), 5);
  
  // like above, but use strlen(str.c_str()) as the second size argument, that means, suppose copying C string  
  mb2->copy(str.c_str());

  // size() returns the totoal space of the bottom datablock, length() returns the useful data length, that is,
  // the wrPtr() - rdPtr(), such as what we copyed into mb2 in above codes 
  // but this not include other MessageBlocks in the chain, to obtain the total sum, use the totalLength() and totalSize()
  cout << "mb2->size(): " << mb2->size() << endl;
  cout << "mb2->length(): " << mb2->length() << endl;

  // get the sum of length/size of the whole chain
  mb3->copy(str.c_str(), 3);
  cout << "mb3->totalLength(): " << mb3->totalLength() << endl;
  cout << "mb3->totalSize(): " << mb3->totalSize() << endl;
  // the length info is keeped in MessageBlock class, because clone and duplicate all create new MessageBlock, 
  // so the change the lengh(for instance, copy data into MessageBlock) will not interfere each other 
  cout << "mb5->totalLength(): " << mb5->totalLength() << endl;
  cout << "mb5->totalSize(): " << mb5->totalSize() << endl;
  cout << "mb6->totalLength(): " << mb6->totalLength() << endl;
  cout << "mb6->totalSize(): " << mb6->totalSize() << endl;

  // read data from Message block
  char testBuf[512];
  cout << "mb2 length before first read: " << mb2->length() << endl;
  std::copy(mb2->rdPtr(), mb2->rdPtr() + 5, testBuf);
  testBuf[5] = '\0';
  mb2->rdPtr(5); // after read data, advance the readPtr, so next time read begin from the remain data
  cout << "mb2 first read: " << testBuf << endl;
  cout << "mb2 length before second read: " << mb2->length() << endl;
  std::copy(mb2->rdPtr(), mb2->rdPtr() + mb2->length(), testBuf);
  testBuf[mb2->length()] = '\0';
  mb2->rdPtr(mb2->length());
  cout << "mb2 second read: " << testBuf << endl;

  // chain/unchain MessageBlocks
  mb2->cont(mb1); // chain: mb2->mb1
  mb2->cont(0); // unchain

  ///////////////////////////////////////////
  //////// example of destroy
  // release all MessageBlocks of the chain, that is, mb3 and mb2  all deleted
  // after this, delete mb3 or delete mb2 can not be called any more, otherwise the program will crash
  mb3->release();

  // release mb2 and mb3 individually 
  // delete mb2;
  // delete mb3;

  mb1->release(); // same result as delete mb1, because the MessageBlock chain has only one MessageBlock
  
  delete mb4; // equals to mb4->release(); the newed 512 bytes will be deleted

  mb5->release(); // can be called after mb3->release(), because everything is new allocated
  mb6->release(); // can be called after mb3->release(), because the MessageBlocks are new allocated, although the datablocks are refrence count shared

  return 0;
}
