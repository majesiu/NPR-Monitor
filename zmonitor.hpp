#include "zhelpers.hpp"
#include <unistd.h>

using namespace std;

string acquireCS(int id){
  zmq::context_t context_ipr(1);
  zmq::socket_t ipr(context_ipr, ZMQ_PAIR);
  // Assign the in-process name "#1"
  ipr.connect("tcp://localhost:1234"+to_string(id));
  // ipr.connect("inproc://my-endpoint"+to_string(id));
  string code = "s ";
  const char *temp1 = code.c_str();
  zmq::message_t request_start (strlen(temp1));
  memcpy (request_start.data (), temp1, strlen(temp1));
  printf("Sending: %s\n",temp1);
  ipr.send (request_start);
  zmq::message_t m1;
  ipr.recv(&m1);
  istringstream iss(string(static_cast<char*>(m1.data()), m1.size()));
  vector<string> tokens;
  copy(istream_iterator<string>(iss),
  istream_iterator<string>(),
  back_inserter(tokens));
  printf("Received: %s\n",tokens[0].c_str());
  return tokens[0].c_str();
}

void release_CS(int id, string data){
  zmq::context_t context_ipr(1);
  zmq::socket_t ipr(context_ipr, ZMQ_PAIR);
  ipr.connect("tcp://localhost:1234"+to_string(id));
  //ipr.connect("inproc://my-endpoint"+to_string(id));
  string code = "e ";
  const char *temp1 = (code.append(data+" ")).c_str();
  zmq::message_t request_end (strlen(temp1));
  memcpy (request_end.data (), temp1, strlen(temp1));
  printf("Sending: %s\n",temp1);
  ipr.send (request_end);
}
