#include <xinu.h>
#include <future.h>
#include <stdio.h>
#include <string.h>

sid32 process_to_call;
sid32 process_get;
sid32 process_set;

shellcmd xsh_prodcons(int nargs, char *args[])
{
	//printf("%s\n",args[1]);
	//printf("%s\n",args[2]);
	if(nargs==2 && strcmp(args[1], "-f", 20)==0)
	{
		future_t* f_exclusive= future_alloc(FUTURE_EXCLUSIVE);
		future_t* f_shared = future_alloc(FUTURE_SHARED);
		future_t* f_queue = future_alloc(FUTURE_QUEUE);

		//FUTURE_EXCLUSIVE
		resume( create(future_consumer, 1024, 20, "fcons1", 1, f_exclusive) );
		resume( create(future_producer, 1024, 20, "fprod1", 2, f_exclusive, 1) );


		//FUTURE_SHARED
		resume( create(future_consumer, 1024, 20, "fcons2", 1, f_shared) );
		resume( create(future_consumer, 1024, 20, "fcons3", 1, f_shared) );
		resume( create(future_consumer, 1024, 20, "fcons4", 1, f_shared) );
		resume( create(future_consumer, 1024, 20, "fcons5", 1, f_shared) );
		resume( create(future_producer, 1024, 20, "fprod2", 2, f_shared, 2) );


		//FUTURE_QUEUE
		resume( create(future_consumer, 1024, 20, "fcons6", 1, f_queue) );
		resume( create(future_consumer, 1024, 20, "fcons7", 1, f_queue) );
		resume( create(future_consumer, 1024, 20, "fcons8", 1, f_queue) );
		resume( create(future_consumer, 1024, 20, "fcons9", 1, f_queue) );
		resume( create(future_producer, 1024, 20, "fprod3", 2, f_queue, 3) );
		resume( create(future_producer, 1024, 20, "fprod4", 2, f_queue, 4) );
		resume( create(future_producer, 1024, 20, "fprod5", 2, f_queue, 5) );
		resume( create(future_producer, 1024, 20, "fprod6", 2, f_queue, 6) );
	

		future_free(f_exclusive);
		future_free(f_shared);
		future_free(f_queue);
		
		sleep(1); // waiting for the processes to complete hence adding a sleep to the current process so that it prints in the same process
		future_free(f_exclusive);
                future_free(f_shared);
                future_free(f_queue);

		return SHELL_OK;
	}
	else
	{
		fprintf(stderr, " %s incorrect arguments \n", args[0]);
		return SHELL_OK;
	}
}

