#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <time.h>


#define QUEUE_SIZE 5

typedef struct _arg_t arg_t;

struct _fifo_t {
    int val;
    struct _fifo_t *next, *prev;
};
typedef struct _fifo_t fifo_t;

struct _mem_t {
    fifo_t fifo[QUEUE_SIZE];
    fifo_t *head, *tail;

    //sem_t fifo_lock;
    //sem_t full, empty;
};
typedef struct _mem_t mem_t;

static mem_t *sh_mem;



/**
 * Allocates sh_mem and initialize it in anonymous shared memory.
 * Fifo is implemented as circular buffer.
 */
static int allocAndInit(void)
{
    fifo_t *f;
    size_t i;

    sh_mem = (mem_t *)mmap(0, sizeof(*sh_mem), PROT_READ | PROT_WRITE,
                           MAP_SHARED | MAP_ANON, -1, 0);
    if (!sh_mem){
        perror("Allocation of shared memory failed: ");
        return -1;
    }

    // create circular buffer at beggining of shared memory
    f = sh_mem->fifo;
    for (i = 0; i < QUEUE_SIZE; i++){
        f->next = f + 1;
        f->prev = f - 1;
        f++;
    }
    sh_mem->fifo[0].prev = &sh_mem->fifo[QUEUE_SIZE - 1];
    sh_mem->fifo[0].prev->next = sh_mem->fifo;

    return 0;
}

int main(int argc, char *argv[])
{
    size_t i;
    int pid;

    if (allocAndInit() != 0){
        return -1;
    }

    fifo_t *f;

    f = sh_mem->fifo;

    for (i = 0; i < QUEUE_SIZE; i++){
        f->next->val = 1;
        f++;
    }

    f = sh_mem->fifo;

    for (i = 0; i < QUEUE_SIZE; i++){
        printf("%d\n", f->next->val);
        f++;
    }

    return 0;
}
