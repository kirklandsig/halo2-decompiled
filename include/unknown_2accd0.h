/* UNKNOWN_2ACCD0.H: saved game files on the Xbox hard disk
   (src/unknown_2accd0.cpp); the file references they use are in files.h and
   the asynchronous tasks that run them in job_queue.h */

#ifndef UNKNOWN_2ACCD0_H
#define UNKNOWN_2ACCD0_H

#include "unknown_11c920.h"
#include <xtl.h>
#include "files.h"
#include "job_queue.h"

/* the state of a saved game file operation */
struct s_saved_game_file_task
{
	/* written by the job thread that runs the operation, while its caller waits */
	volatile bool done;
	volatile bool unknown1;
	volatile bool succeeded;
	byte unknown3;
	long state;
	char path[256];
	real progress;
};

#endif
