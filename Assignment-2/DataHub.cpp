#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <cstring>
#include <iostream>
#include <unistd.h>
#include <sys/wait.h>
#include <cstdlib>
#include "MsgBuffer.h"
#include "MagicSeeds.h"
#include "force_patch.h"

using namespace std;

int main()
{
  //Create Message Queue (ID)
  int qid = msgget(ftok(".", 'u'), IPC_EXCL | IPC_CREAT | 0600);

  //Create Message Buffer (from MsgBuffer.h)
  buffer msg;
  int size = sizeof(msg) - sizeof(long);

  //Keep track of termination of probes
  bool statusA = true;
  bool statusB = true;
  bool statusC = true;

  //Message Counter
  int msgCounter = 0;

  //Get pid of B
  pid_t pidB;
  msgrcv(qid, (struct msgbuf *)&msg, size, 100, 0);
  pidB = atoi(msg.message);

  //Program loop for receiving message from Probes
  //  True = Receiving Data from at least 1 probe
  //  False = All probes terminated
  bool messageLoop = true;

  //Program Runs until all Probes terminate
  while (messageLoop)
  {
    if (statusA)
    {
      //Receive message
      msgrcv(qid, (struct msgbuf *)&msg, size, alphaSeed, 0);
      cout << getpid() << ": Message received from Probe A" << endl;
      cout << msg.message << endl;
      cout << endl;

      //If Probe A is terminated, then set statusA to false
      if (msg.message == "Probe A Terminated")
      {
        statusA = false;
      }
      else //Probe A is still running
      {
        msg.mtype = 10;
        strncpy(msg.message, "Hello Probe A", size);
        msgsnd(qid, (struct msgbuf *)&msg, size, 0);
      }
    }
    if (statusB)
    {
      //Receive message
      msgrcv(qid, (struct msgbuf *)&msg, size, betaSeed, 0);

      //If msgCouner is greater than or equal to 10000,
      //then terminate Probe B and set statusB to false
      if (msgCounter >= 10000)
      {
        cout << "Message counter has reached 10,000" << endl;
        msgctl(qid, IPC_RMID, NULL);
        force_patch(pidB);
        statusB = false;
      }
    }
    if (statusC)
    {
      //Receive message
      msgrcv(qid, (struct msgbuf *)&msg, size, rhoSeed, 0);

      //If Probe C is terminated, then set statusC to false
      if (msg.message == "Probe C Terminated") //Terminates when
      {
        statusC = false;
      }
    }
    if (!statusA && !statusB && !statusC)
    {
      messageLoop = false;
    }
    msgCounter++;
  }

  //Delete Message Queue
  msgctl(qid, IPC_RMID, NULL);
  cout << "Message Queue has been deleted" << endl;

  return 0;
}