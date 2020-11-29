/*
    Include headers 
*/
#include "common_headers.h"
#include "definitions.h"

/*
    queue* init_queue(int queue_size)
    Returns a newly initialized Queue
*/
queue* init_queue(int queue_size){

    // Allocate memory to the queue, exit if fails
    queue* newQueue = (queue*)malloc(sizeof(queue));
    if(newQueue==NULL) exit(1);    

    // Initialize values
    newQueue->curr_size = 0;
    newQueue->max_size = queue_size;
    newQueue->fill = 0;
    newQueue->use = 0;

    // Allocate memory for queue array
    newQueue->array = (node*)malloc(sizeof(node)*queue_size);    

    if(newQueue->array == NULL) exit(1);

    // Make all file descriptors as zero initially, which will be overwritten later as and when requests comes
    for(int i=0; i < queue_size; i++){
        newQueue->array[i].fd = 0;
    }        

    return newQueue;
}

/*
    void insert_in_queue(int conn_fd, queue* Queue)
    Inserts the given connection file desciptor of the client at the back of the Queue
*/
void insert_in_queue(int conn_fd, queue* Queue){

    // Check if queue is not full
    assert(Queue->curr_size < Queue->max_size);

    // Insert and update fill pointer and current size
    Queue->array[Queue->fill].fd = conn_fd;
    Queue->fill = (Queue->fill + 1) % Queue->max_size;
    Queue->curr_size++;

}

/*
    int get_from_queue(queue* Queue)
    Removes one element from the front of the Queue
*/
int get_from_queue(queue* Queue){

    //Check if the queue is not empty
    assert(Queue->curr_size > 0);

    // Remove one element from use pointer and update Queue size
    int conn_fd = Queue->array[Queue->use].fd;
    Queue->use = (Queue->use + 1) % Queue->max_size;
    Queue->curr_size--;

    return conn_fd;
}