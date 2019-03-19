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

  //Message buffer
  buffer msg;
  int size = sizeof(msg) - sizeof(long);

  //Initialize mType
  msg.mtype = alphaSeed;

  //Condition to terminate Probe A
  bool terminateProbeA = false;

  while (!terminateProbeA)
  {
    //Creates a random event divisible by the seed(997)
    int seedCheck = rand();
    if (seedCheck % alphaSeed == 0) //If divisible by seed, then it is valid
    {
      int terminateValue = rand();
      //Terminates the program if the event is less than 100
      if (terminateValue < 100)
      {
        cout << "Number generated is < 100. Stopping A" << endl;
        terminateProbeA = true;
      }
      else
      {
        strncpy(msg.message, "ProbeA", size);
        msgsnd(qid, (struct msgbuf *)&msg, size, 0);
        //Waiting to receive message back from DataHub
        msgrcv(qid, (struct msgbuf *)&msg, size, 10, 0);
      }
    }
  }

  strncpy(msg.message, "Probe A Terminated", size);
  msgsnd(qid, (struct msgbuf *)&msg, size, 0);
  cout << "Probe A is terminating" << endl;

  return 0;
}