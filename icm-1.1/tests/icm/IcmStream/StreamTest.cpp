#include <string>
#include "icm/IcmStream.h"

int main(int argc, char* argv[]) {
  char buf[512];
  memset(buf,1,512);
  InputStream is(buf,512);
  is.grow(20);

  int i = 123;
  is.read_int(i);

  //std::string str = "abc";
  //is.read_string(str);

  //OutputStream* os = new OutputStream((MessageBlock*)is.start());
  MessageBlock* mb = (MessageBlock*)is.start();
  mb->rdPtr(mb->base());

  OutputStream* os = new OutputStream(mb);

  return 0;
}
