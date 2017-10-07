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

int main (int argc, char *argv[])
{
  string data = " muahaha "; //+ std::to_string(id);
  int id = strtol(argv[1], NULL, 0);

  zmq::context_t context(1);
  zmq::socket_t  requester(context, ZMQ_REQ);

  zmq::socket_t responder(context, ZMQ_REP);
  responder.bind(serwers[id]);

  for(int i = 0; i < client_number; i++){
    if (id != i)  requester.connect(clients[i]);
  }

  zmq::pollitem_t items [] = {
    { requester, 0, ZMQ_POLLIN, 0 },
    { responder, 0, ZMQ_POLLIN, 0 }
  };

  int a = getchar();
  putchar(a);

  bool requesting = true;
  int to_release = 0;
  int logical_cs_clock = 0;
  std::set<std::string> needed(begin(priorities), end(priorities));
  needed.erase(priorities[id]);
  std::set<std::string> blocked;

  string code = "r ";
  const char *temp1 = (code.append(priorities[id]).append(data).append(to_string(logical_cs_clock))).c_str();
  zmq::message_t request_start (strlen(temp1));
  memcpy (request_start.data (), temp1, strlen(temp1));
  printf("Sending: %s\n",temp1);
  requester.send (request_start);

  while (1) {
    zmq::poll (&items [0],2, -1);
    //Własne zapytania o SK
    if (items [0].revents & ZMQ_POLLIN) {
      zmq::message_t m1;
      requester.recv(&m1);
      istringstream iss(string(static_cast<char*>(m1.data()), m1.size()));
      vector<string> tokens;
      copy(istream_iterator<string>(iss),
      istream_iterator<string>(),
      back_inserter(tokens));
      printf("Received: %s  %s  %s  %s\n",tokens[0].c_str(),tokens[1].c_str(),
      tokens[2].c_str(),tokens[3].c_str());
      if(strtol(tokens[3].c_str(), NULL, 0) > logical_cs_clock){
        logical_cs_clock = (strtol(tokens[3].c_str(), NULL, 0));
        data = " "+tokens[2]+" ";
      }

      if(tokens[0] == "y"){
        printf("Needed pre delete: %d\n",needed.size());
        needed.erase(tokens[1]);
        printf("Needed post delete: %d\n",needed.size());
        if(needed.empty() == false){
          string code = "r ";
          const char *temp2 = (code.append(priorities[id]).append(data).append(to_string(logical_cs_clock))).c_str();
          zmq::message_t m2 (strlen(temp2));
          memcpy (m2.data (), temp2, strlen(temp2));
          printf("Sending: %s\n",temp2);
          requester.send (m2);
        }
      } else if (tokens[0] == "n"){
          string code = "r ";
          const char *temp3 = (code.append(priorities[id]).append(data).append(to_string(logical_cs_clock))).c_str();
          zmq::message_t m3 (strlen(temp3));
          memcpy (m3.data (), temp3, strlen(temp3));
          printf("Sending: %s\n",temp3);
          requester.send (m3);
      }
    }
    //Odpowiedzi na czyjeś zapytania o SK
    if (items [1].revents & ZMQ_POLLIN) {
      zmq::message_t m4;
      responder.recv (&m4);
      istringstream iss(string(static_cast<char*>(m4.data()), m4.size()));
      vector<string> tokens;
      copy(istream_iterator<string>(iss),
      istream_iterator<string>(),
      back_inserter(tokens));
      printf("Received: %s  %s  %s  %s\n",tokens[0].c_str(),tokens[1].c_str(),
      tokens[2].c_str(),tokens[3].c_str());
      if(strtol(tokens[3].c_str(), NULL, 0) > logical_cs_clock){
        logical_cs_clock = (strtol(tokens[3].c_str(), NULL, 0));
        data = " "+tokens[2]+" ";
      }
      if(tokens[0]=="r"){
        if(requesting == true && stoi(priorities[id]) > stoi(tokens[1])){
          string code = "n ";
          const char *temp4 = (code.append(priorities[id]).append(data).append(to_string(logical_cs_clock))).c_str();
          zmq::message_t m5 (strlen(temp4));
          memcpy (m5.data (), temp4, strlen(temp4));
          printf("Sending: %s\n",temp4);
          responder.send (m5);
          blocked.insert(tokens[1]);
        }else {
          string code = "y ";
          const char *temp5 = (code.append(priorities[id]).append(data).append(to_string(logical_cs_clock))).c_str();
          zmq::message_t m6 (strlen(temp5));
          memcpy (m6.data (), temp5, strlen(temp5));
          printf("Sending: %s\n",temp5);
          responder.send (m6);
          blocked.erase(tokens[1]);
        }
      }
    }

    printf("Blocked size: %d\n", blocked.size());

    if(requesting && needed.empty() == true){
      printf("Entering CS\n");
      sleep(5); // robienie pracy
      printf("Exited CS\n");
      data = " muahaha" + std::to_string(id+logical_cs_clock) + " ";
      requesting = false;
      logical_cs_clock += 1;
      to_release = client_number-1;
    } else if (!requesting && blocked.empty() == true){
      sleep(3);
      requesting = true;
      printf("Restarting - again requesting for CS!\n");
      copy(begin(priorities), end(priorities), inserter(needed, needed.end()));
      needed.erase(priorities[id]);
      string code = "r ";
      const char *tempt6 = (code.append(priorities[id]).append(data).append(to_string(logical_cs_clock))).c_str();
      zmq::message_t request_restart (strlen(tempt6));
      memcpy (request_restart.data (), tempt6, strlen(tempt6));
      printf("Sending: %s\n",tempt6);
      requester.send (request_restart);
    }

    sleep(1);
  }
  return 0;
}
