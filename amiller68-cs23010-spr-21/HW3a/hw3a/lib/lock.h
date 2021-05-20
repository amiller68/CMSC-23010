#ifndef _LOCK_H_
#define _LOCK_H_

#include <stdbool.h>
#include <pthread.h>

/* Top Level Locking interface */
typedef struct lock
{
    //A reference to the type
    char type;

    //How high are we counting today?
    int B;

    //Used in sleeper test
    int t;

    //A shared counter between threads;
    volatile long counter;

    //A pointer to a lock object, casted as void
    void *l;

    //if needed, Initialize thread specfific structs
    void (*init_thread)(void *);

    void (*lock)(void *);
    void (*unlock)(void *);
} lock_t;

//Create a lock of a specified type
lock_t *new_lock(char type, int n);
//Destroy a lock of a specified type
int destroy_lock(lock_t *L);
int destroy_lock_pool(lock_t *L_pool);
lock_t *new_lock_pool(int size, char type, int n);



/*Lock Implementations*/

//Test and Set Lock Declaration
typedef struct tas
{
    volatile bool state;
} tas_t;

tas_t *new_tas(int n);

int destroy_tas(tas_t *lock);

void tas_init(void *lock);

void tas_lock(void *L);

void tas_unlock(void *L);


//Mutex Wrapper Declarations
pthread_mutex_t *new_mutex(int n);

int destroy_mutex(pthread_mutex_t *lock);

void mutex_init(void *lock);

void mutex_lock(void *L);

void mutex_unlock(void *L);

//Anderson's Array Lock Deckaration
typedef struct alock
{
    volatile int tail;
    pthread_key_t myIndex;
    volatile bool *flag;
    int size;
} alock_t;

alock_t *new_alock(int n);

int destroy_alock(alock_t *lock);

void alock_init(void *lock);

void alock_lock(void *L);

void alock_unlock(void *L);

//MCS Lock declaration

struct qnode;

typedef struct qnode
{
    volatile bool locked;
    volatile struct qnode *next;
} qnode_t;

typedef struct mcs
{
    volatile qnode_t *tail;
    pthread_key_t myNode;
} mcs_t;

mcs_t *new_mcs(int n);

int destroy_mcs(mcs_t *lock);

void mcs_init(void *lock);

void mcs_lock(void *L);

void mcs_unlock(void *L);

#endif
