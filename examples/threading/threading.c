#include "threading.h"
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>

// Optional: use these functions to add debug or error prints to your application
#define DEBUG_LOG(msg,...)
//#define DEBUG_LOG(msg,...) printf("threading: " msg "\n" , ##__VA_ARGS__)
#define ERROR_LOG(msg,...) printf("threading ERROR: " msg "\n" , ##__VA_ARGS__)



void* threadfunc(void* thread_param)
{

    // TODO: wait, obtain mutex, wait, release mutex as described by thread_data structure
    // hint: use a cast like the one below to obtain thread arguments from your parameter
    //struct thread_data* thred_args_toprocess = (struct thread_data *) thread_param;
    // obtain data from thread_param
    struct thread_data* thred_args_toprocess = (struct thread_data *) thread_param;
    usleep(thred_args_toprocess->wait_to_obtain_ms * 1000);
    // waiting to obtain mutex
    // obtain and lock mutex
    int mutex_lock_status = pthread_mutex_lock(thred_args_toprocess->mutex);
    if( mutex_lock_status != 0 ) {
        thred_args_toprocess->thread_complete_success = false;
        return thread_param;
    }
    // wait to release mutex
    usleep(thred_args_toprocess->wait_to_release_ms * 1000);
    // release mutex
    int mutex_unlock_status = pthread_mutex_unlock(thred_args_toprocess->mutex);
    if( mutex_unlock_status != 0 ) {
        thred_args_toprocess->thread_complete_success = false;
        return thread_param;
    }
    thred_args_toprocess->thread_complete_success = true;

    return thread_param;
}


bool start_thread_obtaining_mutex(pthread_t *thread, pthread_mutex_t *mutex,int wait_to_obtain_ms, int wait_to_release_ms)
{
    /**
     * TODO: allocate memory for thread_data, setup mutex and wait arguments, pass thread_data to created thread
     * using threadfunc() as entry point.
     *
     * return true if successful.
     *
     * See implementation details in threading.h file comment block
     */
    // allocate memory for thread_data
    struct thread_data* thread_data_pointer = malloc(sizeof(struct thread_data));
    // setup mutex and wait arguments
    thread_data_pointer->wait_to_obtain_ms = wait_to_obtain_ms;
    thread_data_pointer->wait_to_release_ms = wait_to_release_ms;
    thread_data_pointer->mutex = mutex;
    // pass thread_data to created thread
    int thread_created = pthread_create(thread, NULL, threadfunc, thread_data_pointer);
    if( thread_created == 0 ) {
        return true;
    }

    return false;
}

