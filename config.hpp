#include "zhelpers.hpp"
#include <unistd.h>
#include <set>

using namespace std;

const int client_number = 3;

std::string clients[client_number] =
{"tcp://localhost:5557", "tcp://localhost:5558", "tcp://localhost:5559"};

std::string serwers[client_number] =
{"tcp://*:5557", "tcp://*:5558", "tcp://*:5559"};

std::string priorities[client_number] = {"1000","2000","3000"};
