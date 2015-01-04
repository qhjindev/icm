
#include "MyHelloI.h"
#include <iostream>
#include <sstream>
using namespace std;

std::string demo::MyHelloI::sayHello(const ::std::string& msg, Short u, Long& v)
{
  ostringstream oss;
  oss << "receive:" << " msg:" << msg << " u:" << u << " v:" << v ;
  std::string tmp = oss.str();
  cout << tmp;

  v = 0x1234 + v;

  return tmp;
}
