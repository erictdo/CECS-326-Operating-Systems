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
  cout << "qid is " << qid << endl;

  //Create Message Buffer (from MsgBuffer.h)
  buffer msg;
  int size = sizeof(msg) - sizeof(long);

  //Keep track of termination of probes
  bool statusA = true;
  bool statusB = true;
  bool statusC = true;

  //Message Counter
  int msgCounter = 0;

  //Initialize pid from ProbeB
  pid_t pidB;

  //Program loop for receiving message from Probes
  //  True = Receiving Data from at least 1 probe
  //  False = All probes terminated
  bool messageLoop = true;

  //Program Runs until all Probes terminate
  while (messageLoop)
  {
    //If there are any messages in the queue
    if (msgrcv(qid, (struct msgbuf *)&msg, size, 0, 0) != -1)
    {
      cout << "TEST 4: " << msg.message << endl;
      cout << "TEST 4: " << msg.mtype << endl;

      if (msg.mtype == dh_mtype) //Message is from DataHub, need to send to ProbeA
      {
        strncpy(msg.message, "Hello Probe A", size);
        msgsnd(qid, (struct msgbuf *)&msg, size, 0);
        msgCounter++;
      }
      else
      {
        if (msg.mtype == alphaSeed)
        {
          //Receive message from Probe C & print
          cout << getpid() << "(Probe A): " << msg.message << endl;
          cout << endl;

          //Send Acknowledgement to ProbeA
          msg.mtype = dh_mtype;
          strncpy(msg.message, "Hello Probe A", size);
          msgsnd(qid, (struct msgbuf *)&msg, size, 0);
          msgCounter++;
        }
        if (msg.mtype == betaSeed)
        {
          //Receive message from Probe B & print
          cout << getpid() << "(Probe B): " << msg.message << endl;
          cout << endl;
          //If msgCounter is greater than or equal to 10000,
          //then terminate Probe B and set statusB to false
          if (msgCounter >= 10000)
          {
            cout << "Message counter has reached 10,000" << endl;
            force_patch(pidB);
            statusB = false;
          }
          else
          {
            msgCounter++;
          }
        }
        if (msg.mtype == rhoSeed)
        {
          //Receive message from Probe C & print
          cout << getpid() << "(Probe C): " << msg.message << endl;
          cout << endl;
          msgCounter++;
        }
        if (msg.mtype == 555) //Message saying Probe A has been terminated
        {
          statusA = false;
        }
        if (msg.mtype == 666) //Message is the pid from ProbeB
        {
          //Get pid of B
          pidB = atoi(msg.message);
          msgCounter++;
        }
        if (msg.mtype == 777) //Message saying Probe C has been terminated
        {
          statusC = false;
        }

        cout << "Counter: " << msgCounter << endl;
      }
      cout << "TEST A: " << statusA << endl;
      cout << "TEST B: " << statusB << endl;
      cout << "TEST C: " << statusC << endl;
      if (!statusA && !statusB && !statusC)
      {
        messageLoop = false;
      }
    }
  }

  //Delete Message Queue
  msgctl(qid, IPC_RMID, NULL);
  cout << "Message Queue has been deleted" << endl;

  return 0;
}