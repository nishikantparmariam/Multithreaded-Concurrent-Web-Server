#ifndef __common_headers_h__
#define __common_headers_h__

#ifdef __linux__
#include <semaphore.h>
#endif

#include "common_threads.h"
#include "definitions.h"

#include "request.h"
#include "io_helper.h"
#include <pthread.h>
#include <assert.h>
#include <sched.h>
#include <stdio.h>
#include <pthread.h>
#include <stdbool.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/time.h>

#endif