#include "dbConn.h"
//Shared memory
#include <pthread.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h> 

struct shared_conn_t {
    pthread_mutex_t mutex;
    int outFD;
    int inFD;
};

const char* sharedMemPath = "/db-shared-mem"; //Can't have more than 1 slash, http://linux.die.net/man/3/shm_open

SharedDBConnection sh_conn_create(int outFD, int inFD) {
    SharedDBConnection result;
    //Create the shared memory region
    int fd = shm_open(sharedMemPath, O_RDWR | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR); //Read-write mode; create it if it doesn't exist; truncate it if it exists
    if (fd < 0) {
        return NULL;
    }
    //Give it a size (starts with 0 bytes)
    if (ftruncate(fd, sizeof (*result)) < 0) {
        return NULL;
    }
    /*
     * Map the connection in the shared memory region:
     * - Let the kernel decide where exactly to map it
     * - Give the region the same size as the object
     * - Give read and write access
     * - Share the mapping
     * - Map what is in fd
     * - Start with offset 0
     */
    result = (SharedDBConnection) mmap(0, sizeof (*result), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (result == NULL) {
        return NULL;
    }
    //Set up the connection mutex to work with multiple processes in shared memory
    pthread_mutexattr_t mattr;
    pthread_mutexattr_init(&mattr);
    pthread_mutexattr_setpshared(&mattr, PTHREAD_PROCESS_SHARED);
    pthread_mutex_init(&(result->mutex), &mattr);
    //Ready to share
    result->outFD = outFD;
    result->inFD = inFD;
    return result;
}

int sh_conn_destroy(SharedDBConnection c) {
    if (close(c->outFD) == -1) {
        return 0;
    }
    if (close(c->inFD) == -1) {
        return 0;
    }
    if (shm_unlink(sharedMemPath) < 0) {
        return 0;
    }
    return 1;
}

int sh_conn_get_out_fd(SharedDBConnection c) {
    return c->outFD;
}

int sh_conn_get_in_fd(SharedDBConnection c) {
    return c->inFD;
}

int sh_conn_open(SharedDBConnection c) {
    return !pthread_mutex_lock(&c->mutex);
}

int sh_conn_close(SharedDBConnection c) {
    return !pthread_mutex_unlock(&c->mutex);
}