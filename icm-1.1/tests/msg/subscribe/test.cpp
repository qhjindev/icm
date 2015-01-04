#include <iostream>

using namespace std;

int main() {

  int* pi = new int[3];
  char* pc = (char*)pi;
  ++pc;
  ++pi;
  *pi = 1;
//  *(int*)pc = 2;
return 0;
/*short i = 1;
char* pc = (char*)&i;
if(pc[1] == 1)
  cout << "big endian!\n";
else if(pc[1] == 0)
  cout << "little endian!\n";
else
  cout << "unknown endian!\n";
return 0;
*/
}

