#include <iostream>
#include <sys/ipc.h>
#include <unistd.h>
int main() {
  std::cout << "Olá Mundo" << std::endl;
  sleep(3);
  exit(0);
}
