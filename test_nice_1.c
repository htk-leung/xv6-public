#include "types.h"
#include "stat.h"
#include "user.h"


int main(int argc, char *argv[])
{
  // get pid, value
  int pid = -1;
  int val = 0;

  if(argc < 2 || argc > 3) // error : no enough arguments
  {
    printf(1, "Usage : nice <pid> <value> or nice <value>");
    exit();
  }

  if(argc == 3)
  {
    pid = atoi(argv[1]);
    val = atoi(argv[2]);
  }
  else if(argc == 2)
  {
    pid = getpid();
    val = atoi(argv[1]);
  }

  if(val < 1 || pid > 5) // error : invalid val
  {
    return -2;
  }

  int oldval = changenice(pid, val);
  printf(1, "pid : %d\toldval : %d\n", pid, oldval);

  exit();
}