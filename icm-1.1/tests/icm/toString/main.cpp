#include <iostream>
using namespace std;

#include "test.h"

int main(int argc, char* argv[]) {

  demo::Req1 req1;
  req1.push_back("hello1");
  req1.push_back("hello11");
  req1.push_back("hello111");

  demo::Req1I req1I;
  req1I.push_back(1);
  req1I.push_back(2);
  req1I.push_back(3);

//  ICC_DEBUG("req1I: %s", demo::__toString(req1I).c_str());
//  return 0;

  demo::Req3 req3;
  req3.syn = 100;
  req3.request1 = req1;

  demo::Req1 req11;
  req11.push_back("hello2");
  req11.push_back("hello22");
  req11.push_back("hello222");

  demo::Req3 req31;
  req31.syn = 101;
  req31.request1 = req11;

  demo::Req1R req1R;
  req1R.push_back(req3);
  req1R.push_back(req31);

  demo::Req2 req2;
  req2.insert(make_pair(1, "bye1"));
  req2.insert(make_pair(2, "bye11"));
  req2.insert(make_pair(3, "bye111"));
  req2.insert(make_pair(4, "bye1111"));

  demo::Req4 req41;
  req41.syn = 100;
  req41.request1 = req1;
  req41.syn2 = 202;
  demo::Req4 req42;
  req42.syn = 101;
  req42.request1 = req11;
  req42.syn2 = 303;

  cout << "req41: " << req41.toString() << endl;
  cout << "req42: " << req42.toString() << endl;

  return 0;
}
