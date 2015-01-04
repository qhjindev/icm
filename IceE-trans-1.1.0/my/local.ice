#ifndef LOCAL_ICE
#define LOCAL_ICE

module demo
{

struct MyStruct
{
  long abc;
  string str;
};

class XYZ
{
  string get();
};

/*
class A
{
  string getV ();
};
*/

//sequence<MyStruct> MyStructSeq;

local class LA;

sequence<LA> ASeq;

class MyHello
{

  //long testSeq (MyStruct cd, out long lvalue);
  
  long setA (LA theA);
  int getA (out LA theA);
};


};

#endif
