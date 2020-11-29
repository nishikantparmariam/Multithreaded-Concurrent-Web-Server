/*
    Include header files
*/
#include "common_headers.h"
#include "definitions.h"

/*
    int heap_comparator(heap* Heap, int i, int j)
    Custom comparator for heap that compares nodes based either on file name or parameter (file size) as per the policy SFF or SFNF
    Returns 1 iff value at index i has larger size or lexicographically bigger name than at index j
*/
int heap_comparator(heap* Heap, int i, int j){

    // returns 1 iff i > j in comparison

    if(Heap->by_file_name==1){

        // Comparing by file name (SFNF)
        if(strcmp(Heap->array[i].file_name,Heap->array[j].file_name) > 0) return 1;

        return 0;
    } else {

        // Comparing by file size (SFF)
        if(Heap->array[i].parameter > Heap->array[j].parameter) return 1;

        return 0;
    }    
}

/*
    void _swap(node* x, node* y)
    Function to swap values at two nodes
*/
void _swap(node* x, node* y){
    node temp = *x;
    *x = *y;
    *y = temp;
}

/*
    heap* init_heap(int heap_size, int by_file_name)
    Returns a newly initialized heap with the given max heap size and by_file_name bit that indicates sorting of nodes either by file name or by file size in the comparator function
*/
heap* init_heap(int heap_size, int by_file_name){

    // Allocate memory, exit if fails, and initialize values
    heap* newHeap = (heap*)malloc(sizeof(heap));
    if(!newHeap) exit(1);
    newHeap->max_size = heap_size;
    newHeap->curr_size = 0;
    newHeap->by_file_name = by_file_name;
    newHeap->array = (node*)malloc(heap_size*sizeof(node));
    

    if(!newHeap->array) exit(1);

    // Initialize the heap array with default values that will be overwritten as and when they are filled
    for(int i=0;i< heap_size; i++){
        newHeap->array[i].fd = 0;
        newHeap->array[i].parameter = 0;
        newHeap->array[i].file_name = (char *) malloc(1024*sizeof(char));
    }

    return newHeap;
}

/*
    void insert_in_heap(int conn_fd, off_t parameter, char* file_name, heap* Heap)
    Inserts new connection file desciptor from the client into the queue. It takes file name and size (parameter) as arguments
*/
void insert_in_heap(int conn_fd, off_t parameter, char* file_name, heap* Heap){
    
    // Check if the queue is not full
    assert(Heap->curr_size < Heap->max_size);

    // Insert at last position available in the heap array
    int index = Heap->curr_size;
    Heap->curr_size++;
    Heap->array[index].fd = conn_fd;
    Heap->array[index].parameter = parameter;     
    strcpy(Heap->array[index].file_name, file_name);
    
    // Heapify in bottom-up manner until min-heap property is not satisfied
    while(index!=0 && (heap_comparator(Heap, index/2, index)==1)){
        _swap(&Heap->array[index/2], &Heap->array[index]);
        index = index/2;
    }
}

/*
    void heapify(heap* Heap, int index)
    Heapifies heap in top-down manner from the given index of the heap array
*/
void heapify(heap* Heap, int index){

    // Left child and right child
    int left = 2*index + 1;
    int right = 2*index + 2;

    // Index of smallest node (according to the min-heap property)
    int smallest = index;

    // Update smallest if left is smaller
    if(left < Heap->curr_size && (heap_comparator(Heap, smallest, left)==1)){
        smallest = left;
    }

    // Update smallest if right is smaller
    if(right < Heap->curr_size && (heap_comparator(Heap, smallest, right)==1)){
        smallest = right;
    }

    // Update current node with smallest child and recurse in top-down manner
    if(smallest != index){
        _swap(&Heap->array[smallest], &Heap->array[index]);
        heapify(Heap, smallest);
    }

}

/*
    int extract_min(heap* Heap)
    Returns minimum element (file descriptor at the root of the heap).
*/
int extract_min(heap* Heap){

    // Check if the heap is not empty
    assert(Heap->curr_size > 0);
    
    // Swap last node with first node in the array
    _swap(&Heap->array[0], &Heap->array[Heap->curr_size-1]);

    // Get file descriptor
    int fd = Heap->array[Heap->curr_size-1].fd;    

    //Reduce heap size
    Heap->curr_size--;

    // Heapify from first node of the array in top-down manner

    heapify(Heap, 0);
    return fd;
}
