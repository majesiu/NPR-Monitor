#include "zmonitor.hpp"

int main(int argc, char *argv[]){

  int id = strtol(argv[1], NULL, 0);

  sleep(1);
  string data = acquireCS(id);
  printf("acuired data: %s\n", data.c_str());
  sleep(3);
  release_CS(id, data+argv[1]);

  return 0;
}
