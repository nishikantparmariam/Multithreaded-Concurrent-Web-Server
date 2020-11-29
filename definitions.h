#ifndef __definitions_h__
#define __definitions_h__

// Constants
#define MAXBUF 16384
#define MAXFILETYPE 8192

// Node structure for Heap and Queue
typedef struct __node_t {
    int fd;
    off_t parameter;
    char* file_name;    
} node;


// Heap structure
typedef struct __heap_t {
    int curr_size;
    int max_size;
    int by_file_name;
    node* array;
} heap;

// Methods for Heap
void _swap(node* x, node* y);
heap* init_heap(int heap_size, int by_file_name);
void insert_in_heap(int conn_fd, off_t parameter, char* file_name, heap* Heap);
void heapify(heap* Heap, int index);
int extract_min(heap* Heap);
int heap_comparator(heap* Heap, int i, int j);


// Queue structure
typedef struct __queue_t {
    int max_size;
    int curr_size;
    int fill;
    int use;
    node* array;
} queue;

// Methods for Queue
queue* init_queue(int queue_size);
void insert_in_queue(int conn_fd, queue* Queue);
int get_from_queue(queue* Queue);


// Thread pool structure
typedef struct __thread_pool_t {

    int num_threads;
    int working_threads;
    pthread_t* pool;
    pthread_mutex_t LOCK;
    pthread_cond_t FILL;
    pthread_cond_t EMPTY;

} thread_pool;

// Scheduler structure
typedef struct __scheduler_t {

    char* policy;
    int buffer_size;
    int curr_size;
    heap* Heap;
    queue* Queue;

} scheduler;

// Arguments passed to workder threads
typedef struct __thread_arg {

    scheduler* scheduler;
    thread_pool* workers;
    int num_request;

} thread_arg;

// Helper structure for file properties
typedef struct __file_prop_t {
    char* file_name;
    off_t file_size;
} file_prop;

// Scheduler methods
scheduler* init_scheduler(char* policy, int buffer_size);
thread_pool* init_thread_pool(int num_threads);
void start_threads(scheduler* d, thread_pool* workers);
void schedule_new_request(scheduler* d, int conn_fd);
int pick_request(scheduler* d);
int is_scheduler_full(scheduler* d);
int is_scheduler_empty(scheduler* d);
void give_to_scheduler(thread_pool* workers, scheduler* d, int conn_fd);
int get_from_scheduler(thread_pool* workers, scheduler* d);

// Worker thread
void* thread_worker(void* arg);

// Helper methods
int get_file_name(int fd, char* filename);
off_t get_file_size(int fd);
int request_parse_uri_modified(char *uri, char *filename, char *cgiargs);
file_prop* request_file_properties(int fd);
int is_uri_safe(char uri[]);

#endif