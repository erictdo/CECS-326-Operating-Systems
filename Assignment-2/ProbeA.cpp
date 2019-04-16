#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <cstring>
#include <iostream>
#include <unistd.h>
#include <sys/wait.h>
#include <cstdlib>
#include <time.h>
#include "MsgBuffer.h"
#include "MagicSeeds.h"

using namespace std;

int main()
{
  //Initialize Randomizer
  srand(time(NULL));

  //Generate qid for the queue
  int qid = msgget(ftok(".", 'u'), 0);
  cout << "qid is " << qid << endl;

  //Message buffer
  buffer msg;
  int size = sizeof(msg) - sizeof(long);

  //Condition to terminate Probe A
  bool terminateProbeA = false;

  while (!terminateProbeA)
  {
    //Creates a random event divisible by the seed(997)
    int seedCheck = rand();
    if (seedCheck % alphaSeed == 0) //If divisible by seed, then it is valid
    {
      int terminateValue = rand();
      cout << "Terminate Value: " << terminateValue << endl;
      //Terminates the program if the terminateValue is less than 100
      if (terminateValue < 100)
      {
        cout << "Number generated is < 100. Stopping A" << endl;
        terminateProbeA = true;
      }
      else
      {
        //Send to DataHub
        msg.mtype = alphaSeed;
        strncpy(msg.message, "Hello DataHub, I'm Probe A", size);
        msgsnd(qid, (struct msgbuf *)&msg, size, 0);
        cout << "(" << getpid() << ") "
             << "Message Sent: " << msg.message << " Type: " << msg.mtype << " " << endl;
        //Waiting for acknowledgement from DataHub
        msgrcv(qid, (struct msgbuf *)&msg, size, dh_mtype, 0);
        cout << "(" << getpid() << ") "
             << "Message Received: " << msg.message << " Type: " << msg.mtype << " " << endl;
      }
      cout << endl;
    }
  }

  msg.mtype = 555;
  strncpy(msg.message, "Probe A Terminated", size);
  msgsnd(qid, (struct msgbuf *)&msg, size, 0);
  cout << "(" << getpid() << ") "
       << "Message Sent: " << msg.message << " Type: " << msg.mtype << " " << endl;

  return 0;
}