/*
	Include Headers
*/
#include "common_headers.h"
#include "definitions.h"

/*
	Thread worker function recevies the socket descriptor from the scheduler.
	It provides the socket descriptor to the request_handle() function,
	to handle the request.
*/
void* thread_worker(void* arg)
{	
	thread_arg* args = (thread_arg*)arg;
	while(1)
	{	
		// Getting the request that this thread has to handle from the scheduler
		int conn_fd = get_from_scheduler(args->workers, args->scheduler);	

		// printf("thread = %d, conn_fd = %d\n", args->num_request, conn_fd);

		// Handle request
		request_handle(conn_fd);

		// Close connection
		close_or_die(conn_fd);
	}
	return NULL;
}