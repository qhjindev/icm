
#include "MyCallbackI.h"
#include <iostream>
#include <sstream>
using namespace std;


void My::MyCallbackI::reportProgress(const ::My::OpProgress& progress) {
  //ostringstream oss;
  cout << "receive:" << " seq:" << progress.seq << " status:" << progress.status << endl;
  //cout << oss.str() << endl;
}
