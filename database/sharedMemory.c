// Compile using -lrt -lpthread

#include <fcntl.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

typedef struct {
  pthread_mutex_t mutex;
  int counter;
} shared_t;

int main()
{
    //Create the shared memory region as file
  char* name = "/tmp-test";
  int fd = shm_open(name, O_RDWR | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
  if (fd < 0) {
    perror("shm_open");
    exit(1);
  }

  if (ftruncate(fd, sizeof(shared_t)) < 0) {
    perror("ftruncate");
    exit(1);
  }

  shared_t* sh = (shared_t*)mmap(0, sizeof(shared_t), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
  if (sh == NULL) {
    perror("mmap");
    exit(1);
  }

  // Setup the shared memory space.
  // These steps are needed to ensure that the mutex can be accessed by
  // multiple processes and can reside in shared memory.
  pthread_mutexattr_t mattr;
  pthread_mutexattr_init(&mattr);
  pthread_mutexattr_setpshared(&mattr, PTHREAD_PROCESS_SHARED);
  pthread_mutex_init(&(sh->mutex), &mattr);
  sh->counter = 1;

  for (int i = 0; i < 100; i++) {
    // Do the forking here.
    if (fork() == 0) { // Child
      pthread_mutex_lock(&sh->mutex);
      sh->counter++;
      pthread_mutex_unlock(&sh->mutex);
      exit(0); // Done
    }
  }

  // Stay here until we're done.
  while (sh->counter < 100);

  printf("Final counter value: %d\n", sh->counter);

  if (shm_unlink(name) < 0) {
    perror("shm_unlink");
    exit(1);
  }

  return 0;
}