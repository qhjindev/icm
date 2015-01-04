#ifndef SEQ_ICE
#define SEQ_ICE

module demo
{

struct package
{
  string str1;
  long length;
  short op;
};
 
sequence<package> MsgSeq;

};



#endif