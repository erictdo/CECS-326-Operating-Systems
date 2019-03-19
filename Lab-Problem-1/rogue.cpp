#include <cstdlib>
#include <unistd.h>
#include <time.h>
int main()
{
  char data[65536];
  struct timespec delay, left;
  delay.tv_sec = 0;
  delay.tv_nsec = 5000000;
  for (;;)
    nanosleep(&delay, &left); // students should avoid this function in this class
  exit(0);
}