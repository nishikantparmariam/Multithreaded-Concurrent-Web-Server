#include "common_headers.h"
#include "definitions.h"
/*
    scheduler* init_scheduler(char* policy, int buffer_size): 
    Initializes a schedular of type schedular*. Sets the data structure of 
    the schedular according to the scheduling policy
    Returns the schedular
*/
scheduler* init_scheduler(char* policy, int buffer_size) {

    scheduler* d = (scheduler*)malloc(sizeof(scheduler));
    if(d == NULL) {
        return NULL;
    }
    d->policy = policy;
    d->buffer_size = buffer_size;
    d->curr_size = 0;

    // Decide the data structure based on scheduling policy
    if (strcmp("SFF", policy) == 0) {        
        d->Heap = init_heap(buffer_size, 0); 
        d->Queue = NULL;       
    } else if (strcmp("FIFO", policy) == 0) {        
        d->Queue = init_queue(buffer_size);        
        d->Heap = NULL;
    } else if(strcmp("SFNF", policy) == 0){
        d->Heap = init_heap(buffer_size, 1); 
        d->Queue = NULL;       
    }
    return d;  

}

/*
    thread_pool* init_thread_pool(int num_threads): 
    Creates a thread pool of size num_threads and initializes the locks of the thread pool
    Returns the thread pool
    
*/

thread_pool* init_thread_pool(int num_threads) {

    thread_pool* workers = (thread_pool*)malloc(sizeof(thread_pool));
    if(workers == NULL) {
        return NULL;
    }
    workers->num_threads = num_threads;
    workers->working_threads = 0;
    workers->pool = malloc(num_threads * sizeof(pthread_t));

    Pthread_mutex_init(&workers->LOCK, NULL);
    Pthread_cond_init(&workers->FILL, NULL);
    Pthread_cond_init(&workers->EMPTY, NULL);

    return workers;   

}

/*
    void start_threads(scheduler* d, thread_pool* workers): 
    This function creates worker threads and invoke the thread_worker function from all threads 

*/

void start_threads(scheduler* d, thread_pool* workers) {
    for (int i = 0; i < workers->num_threads; i++) {
        thread_arg* arg = (thread_arg*)malloc(sizeof(thread_arg));
        if(arg == NULL) {
            printf("Thread number %d: Creation failed", i);
            continue;
        }
        arg->workers = workers;
        arg->scheduler = d;
        arg->num_request = i;
        Pthread_create(&workers->pool[i], NULL, thread_worker, arg);
    }
}

/*
    void schedule_new_request(scheduler* d, int conn_fd): 
    Depending on the policy
    If the policy is SFF or SFNF, then get the corresponding file property from the request_file_properties(conn_fd), 
    and insert in the heap data structure, with the parameter returned from the request_file_properties. 
    If the policy is FIFO: Just insert the request in the queue.

*/

void schedule_new_request(scheduler* d, int conn_fd) {

    if (strcmp(d->policy, "SFF") == 0 || strcmp("SFNF", d->policy) == 0) {

        file_prop* fileProp = request_file_properties(conn_fd);           
        printf("File Size for %d : %ld and name %s\n", conn_fd,fileProp->file_size,fileProp->file_name);
        insert_in_heap(conn_fd, fileProp->file_size,fileProp->file_name,d->Heap);        
    
    } else if (strcmp("FIFO", d->policy) == 0) {

        insert_in_queue(conn_fd, d->Queue);

    } 
    d->curr_size++;

}

/*
    int pick_request(scheduler* d): 
    Depending on the policy, extracts conn file descriptor of the correct request
    Returns the file descriptor
    
*/ 

int pick_request(scheduler* d) {

    int conn_fd;
    if (strcmp(d->policy, "SFF") == 0 || strcmp("SFNF", d->policy) == 0) {
        conn_fd = extract_min(d->Heap);        
    } else if (strcmp("FIFO", d->policy) == 0) {
        conn_fd = get_from_queue(d->Queue);
    }
    d->curr_size--;
    return conn_fd;
}

/*
    int is_scheduler_full(scheduler* d)     
    Returns 1 if the schedular is full 
    Else Returns 0 
*/ 
int is_scheduler_full(scheduler* d) {
    return d->curr_size == d->buffer_size;
}

/*
    int is_scheduler_empty(scheduler* d)
    Returns 1 if the schedular is empty 
    Else Returns 0 
*/ 

int is_scheduler_empty(scheduler* d) {
    return d->curr_size == 0;
}

/*
    give_to_scheduler(thread_pool* workers, scheduler* d, int conn_fd):
    It queries the scheduler to check if the queue or heap data stucture 
    is full or not.
    If not full, it stores the current socket descriptor in the data
    structure.
    Else puts the caller thread to sleep.
*/
void give_to_scheduler(thread_pool* workers, scheduler* d, int conn_fd) {

    Pthread_mutex_lock(&workers->LOCK); 
    while(is_scheduler_full(d)) {
        Pthread_cond_wait(&workers->FILL, &workers->LOCK);
    }
    schedule_new_request(d, conn_fd);
    Pthread_cond_signal(&workers->EMPTY);
    Pthread_mutex_unlock(&workers->LOCK);

}

/*
    get_from_scheduler(thread_pool* workers, scheduler* d):
    It queries the scheduler to check if the queue or heap data stucture 
    is empty or not.
    If not empty, it retrieves the required socket descriptor.
    Else puts the caller thread to sleep.
*/
int get_from_scheduler(thread_pool* workers, scheduler* d) {

    Pthread_mutex_lock(&workers->LOCK);    
    while(is_scheduler_empty(d)) {        
        Pthread_cond_wait(&workers->EMPTY, &workers->LOCK);        
    }
    int conn_fd = pick_request(d);

    printf("Request Scheduled for FD: %d\n", conn_fd);
    
    Pthread_cond_signal(&workers->FILL);
    Pthread_mutex_unlock(&workers->LOCK);

    return conn_fd;
}
