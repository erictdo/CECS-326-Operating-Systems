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

  //Send PID to DataHub
  msg.mtype = 666;
  strncpy(msg.message, to_string(getpid()).c_str(), size);
  msgsnd(qid, (struct msgbuf *)&msg, size, 0);
  cout << "(" << getpid() << ") "
       << "Message Sent: " << msg.message << " Type: " << msg.mtype << " " << endl;

  //Condition to terminate Probe B
  bool terminateProbeB = false;

  while (!terminateProbeB)
  {
    int seedCheck = rand();        // generating a random value
    msg.mtype = betaSeed;          // Probe B magic seed given 257
    if (seedCheck % betaSeed == 0) // If divisible by seed, then it is valid
    {
      msg.mtype = betaSeed;
      strncpy(msg.message, "Hello DataHub, I'm Probe B", size);
      msgsnd(qid, (struct msgbuf *)&msg, size, 0);
      cout << "(" << getpid() << ") "
           << "Message Sent: " << msg.message << " Type: " << msg.mtype << " " << endl;
    }
  }
  return 0;
}
