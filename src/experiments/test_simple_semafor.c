#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <unistd.h>

int main(void)
{
  key_t key;
  int semid;

  /* IPC ключ ресурса */
  key = ftok("/etc/fstab", getpid());

  /* создать только один семафор: */
  semid = semget(key, 1, 0666 | IPC_CREAT);

  printf("%d\n", key);
  printf("%d\n", semid);

  return 0;
}
