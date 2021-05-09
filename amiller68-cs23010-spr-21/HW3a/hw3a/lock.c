#include "lib/lock.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#define UNUSED(x) (void)(x)

//Create a lock of a specified type
lock_t *new_lock(char type, int n)
{
    lock_t *L = (lock_t *) malloc(sizeof(lock_t));

    if(!L)
    {
        printf("ERR: Could not allocate space for lock_t instance\n");
        return NULL;
    }

    L->type = type;
    L->counter = 0;

    switch(type)
    {
        case 't':
            L->l = (void *) new_tas(n);
            L->init_thread = tas_init;
            L->lock = tas_lock;
            L->unlock = tas_unlock;
            return L;
        case 'p':
            L->l = (void *) new_mutex(n);
            L->init_thread = mutex_init;
            L->lock = mutex_lock;
            L->unlock = mutex_unlock;
            return L;
        case 'a':
            L->l = (void *) new_alock(n);
            L->init_thread = alock_init;
            L->lock = alock_lock;
            L->unlock = alock_unlock;
            return L;
        case 'm':
            L->l = (void *) new_mcs(n);
            L->init_thread = mcs_init;
            L->lock = mcs_lock;
            L->unlock = mcs_unlock;
            return L;
        default:
            printf("ERR: Invalid lock type\n");
            free(L);
            return NULL;
    }
}

//Destroy a lock of a specified type
int destroy_lock(lock_t *L)
{
    switch(L->type)
    {
        case 't':
            free(L);
            return destroy_tas((tas_t *) L->l);
        case 'p':
            free(L);
            return destroy_mutex((pthread_mutex_t *) L->l);
        case 'a':
            free(L);
            return destroy_alock((alock_t *) L->l);
        case 'm':
            free(L);
            return destroy_mcs((mcs_t *) L->l);
        default:
            printf("ERR: Invalid lock type\n");
            free(L);
            return 1;
    }
}

tas_t *new_tas(int n)
{
    UNUSED(n);
    tas_t *lock = (tas_t *) malloc(sizeof(tas_t));
    lock->state = false;
    return lock;
}

int destroy_tas(tas_t *lock)
{
    free(lock);
    return 0;
}

void tas_init(void *lock)
{
    UNUSED(lock);
    return;
}

void tas_lock(void *lock)
{

    while(__sync_bool_compare_and_swap(&((tas_t *) lock)->state, true, true)){}
    return;
}

void tas_unlock(void *lock)
{
    ((tas_t *) lock)->state = false;
    return;
}

pthread_mutex_t *new_mutex(int n)
{
    UNUSED(n);
    pthread_mutex_t *lock = (pthread_mutex_t *) malloc(sizeof(pthread_mutex_t));
    pthread_mutex_init(lock, NULL);
    return lock;
}

int destroy_mutex(pthread_mutex_t *lock)
{
    pthread_mutex_destroy((pthread_mutex_t *) lock);
    return 0;
}

void mutex_init(void *lock)
{
    UNUSED(lock);
    return;
}

void mutex_lock(void *L)
{
    pthread_mutex_lock((pthread_mutex_t *) L);
    return;
}

void mutex_unlock(void *L)
{
    pthread_mutex_unlock((pthread_mutex_t *) L);
    return;
}

#define PAD 1

alock_t *new_alock(int n)
{
    alock_t *lock = (alock_t *) malloc(sizeof(alock_t));
    volatile bool *flag = (bool *) calloc(n * PAD, sizeof(bool));
    pthread_key_create(&lock->myIndex, NULL);
    lock->flag = flag;
    flag[0] = true;
    lock->tail = 0;
    lock->size = n;
    return lock;
}


int destroy_alock(alock_t *lock)
{
    pthread_key_delete(lock->myIndex);
    free((void*) lock->flag);
    free(lock);
    return 0;
}

void alock_init(void *lock)
{
    UNUSED(lock);
    return;
}

void alock_lock(void *L)
{
    uintptr_t slot = (uintptr_t) ((int) __sync_fetch_and_add(&((alock_t *) L)->tail, 1)) % ((alock_t *) L)->size;
    pthread_setspecific(((alock_t *) L)->myIndex, (void *) slot);
    while (!((alock_t *) L)->flag[slot]){}
    return;
}

void alock_unlock(void *L)
{
    int slot = (int) ((uintptr_t) pthread_getspecific(((alock_t *) L)->myIndex));
    ((alock_t *) L)->flag[slot] = false;
    int index = (slot + 1) % ((alock_t *) L)->size;
    ((alock_t *) L)->flag[(slot + 1) % ((alock_t *) L)->size] = true;
    return;
}

mcs_t *new_mcs(int n)
{
    //printf("Initializing lock\n");
    UNUSED(n);
    mcs_t *lock = (mcs_t *) malloc(sizeof(mcs_t));
    lock->tail = NULL;
    //pthread_key_create(&lock->thread_init, NULL);
    pthread_key_create(&lock->myNode, free);
    //printf("Initialized lock\n");
    return lock;
}

int destroy_mcs(mcs_t *lock)
{
    pthread_key_delete(lock->myNode);
    free(lock);
    return 0;
}

void mcs_init(void *lock)
{
    //printf("thread-%ld: Creating Initial myNode...\n", pthread_self());
    void *ptr = malloc(sizeof(qnode_t));
    //printf("thread-%ld: Created myNode = %p\n", pthread_self(), ptr);
    pthread_setspecific(((mcs_t *) lock)->myNode, (void *) ptr);
    //printf("thread-%ld: Initialized myNode = %p\n", pthread_self(), ptr);
    return;
}

void mcs_lock(void *L)
{
    //printf("thread-%ld: Locking up...\n", pthread_self());
    volatile qnode_t *qnode = (volatile qnode_t *) pthread_getspecific(((mcs_t *) L)->myNode);
    //printf("thread-%ld: Got myNode = %p\n", pthread_self(), qnode);

    __sync_synchronize();

    volatile qnode_t *pred = (volatile qnode_t *) __atomic_exchange_n (&((mcs_t *) L)->tail, qnode, __ATOMIC_ACQUIRE);
    //printf("thread-%ld: Got pred = %p\n", pthread_self(), pred);
    if (pred)
    {
        qnode->locked = true;
        pred->next = qnode;

        // wait until predecessor gives up the lock
        while (qnode->locked) {}
    }

    return;
}


void mcs_unlock(void *L)
{
    //printf("thread-%ld: Unlocking...\n", pthread_self());
    volatile qnode_t *qnode = (volatile qnode_t *) pthread_getspecific(((mcs_t *) L)->myNode);
    //printf("thread-%ld: myNode = %p\n", pthread_self(), qnode);

    if (qnode->next == NULL)
    {
        //printf("thread-%ld: My tail = %p\n", pthread_self(), ((mcs_t *) L)->tail);
        if (__sync_bool_compare_and_swap(&((mcs_t *) L)->tail, qnode, NULL))
        {
            return;
        }
        // wait until predecessor fills in its next field
        while (qnode->next == NULL) {}
    }

    qnode->next->locked = false;
    qnode->next = NULL;
    return;
}
