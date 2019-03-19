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

  msg.mtype = 100;                                         // initiailize msg.mtype to 100
  strncpy(msg.message, to_string(getpid()).c_str(), size); // copy the pid to the char array in the message
  msgsnd(qid, (struct msgbuf *)&msg, size, 0);             // sends the message to the queue id

  //Condition to terminate Probe B
  bool terminateProbeB = true;

  while (!terminateProbeB)
  {
    int seedCheck = rand();        // generating a random value
    msg.mtype = betaSeed;          // Probe B magic seed given 257
    if (seedCheck % betaSeed == 0) // If divisible by seed, then it is valid
    {
      strncpy(msg.message, "ProbeB", size);        // copy the ProbeB's pid to the char array in the message
      msgsnd(qid, (struct msgbuf *)&msg, size, 0); // sends the message to the queue
    }
  }
  return 0;
}
