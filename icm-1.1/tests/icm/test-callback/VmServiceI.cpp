
#include "icc/Thread.h"
#include "VmServiceI.h"
#include <iostream>
#include <sstream>
using namespace std;

class MyThread : public Thread {
public:
  int svc(void) {
    //sleep(5);

    My::OpProgress progress;
    progress.seq = 100;
    progress.status = "ok";

    cout << "start call reportProgress" << endl;
    cb->reportProgress(progress);

    return 0;
  }

  IcmProxy::My::MyCallback* cb;
};

int My::VmServiceI::setCallback(IcmProxy::My::MyCallback * cb) {

  cout << "enter setCallback" << endl;

  MyThread* t = new MyThread;
  t->cb = cb;
  t->activate();

  cout << "leave setCallback" << endl;
  return 0;
}
