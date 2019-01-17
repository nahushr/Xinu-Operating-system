#include <xinu.h>
#include <future.h>
future_t* future_alloc(future_mode_t mode)
{
    future_t* f =  (struct queue*)getmem(sizeof(future_t));                       //allocating future
    f->state = FUTURE_EMPTY;
    f->mode = mode;
    f->value = 0; //initializing the value to 0 to check the inacttivity of the future

    //allocating memory to the set and get queue of the process
    f->set_queue =  (struct queue*)getmem (sizeof (struct queue));          
    f->set_queue->next = NULL;
    f->get_queue =  (struct queue*)getmem (sizeof (struct queue));          
    f->get_queue->next = NULL;

    return f;
}


syscall future_free(future_t* f){
  //  printf("here");
    freemem(&f ,sizeof(future_t));// free the entire future;
//    printf("%d",(int)(freemem((char*)(f->get_queue), sizeof(f->get_queue))));
  //  printf("%d",(int)(freemem((char*)(f->set_queue), sizeof(f->set_queue))));
    return OK;
}

syscall future_get(future_t* f, int* value)
{
    intmask interrupt_handler=disable();

    f->state = FUTURE_WAITING;
    pid32 process_id=getpid();
    if(f->mode == FUTURE_EXCLUSIVE)
    {           
        if(f->state != FUTURE_READY)
        {
            if(f->pid > 0)
            {
		//checking if there are more than one consumer in the system
                return SYSERR;                      
            }
            f->pid = process_id;                      
            suspend(process_id);//suspend the process and then resume it from the producer
            *value = f->value;                      // take the value of the future once the producer produces it
            f->state = FUTURE_EMPTY;
	    //future_free(f);
        }
    }
    else if(f->mode == FUTURE_SHARED)
    {       
        if(f->state != FUTURE_READY)
        {          
            add_in_queue(f->get_queue,process_id);              
            suspend(process_id); //suspend the process till the producer produces the value
            *value = f->value;                      //get the value of the future once the producer produces it 
            if(f->get_queue->next == NULL)
            {
                f->state = FUTURE_EMPTY;
	//	future_free(f);                
            }
        }
    }    
    else if(f->mode == FUTURE_QUEUE)
    {       
            if(f->state != FUTURE_READY)
            {          
                struct queue *q = f->set_queue;
                pid32 process_id=getpid();
                if(q->pid !=0)
                {                        
                    struct queue* temp_queue =f->get_queue->next;
                    pid32 pid = remove_from_queue(f->set_queue);       
                    resume(pid);
                    f->get_queue->pid = temp_queue->pid;
                    f->get_queue->next = temp_queue->next;
                }
                else
                {
                    add_in_queue(f->get_queue,process_id);              
                    f->state = FUTURE_WAITING;
                    suspend(process_id); //suspend the process for the producer to print the value 
                }   
                *value = f->value;                      

                //check if the future is empty and set the state of the future accordingly
                if(f->get_queue !=NULL || f->get_queue->pid != 0)
                {
                    f->state = FUTURE_WAITING;
                }
                else
                {
                    f->state = FUTURE_EMPTY;//set the future state as empty if the all future elements are consumed by the consumer
	//	    future_free(f);
                }
            }
    }
    else 
    {
        *value = f->value;
    }
    
    restore(interrupt_handler);
    return OK;
}



syscall future_set(future_t* f, int value)
{
    intmask interrupt_handler= disable();
            
        if(f->mode == FUTURE_EXCLUSIVE)
        { 
            if(f->state == FUTURE_EMPTY || f->state == FUTURE_WAITING)
            {                        
                pid32 process_id=getpid();     
                if(f->set_queue->pid > 0)
                {                      
                    ///printf("Cannot Set \n"); //checking if multiple producers are trying to produce at the same time 
                    return SYSERR;
                }
                f->value = value;                           
                f->state = FUTURE_READY;
                f->set_queue->pid = process_id; //add the process in the set queue
                f->set_queue->next = NULL;
                resume(f->pid);                        
            }
        }
        else if(f->mode == FUTURE_SHARED)
        {   
            if(f->state == FUTURE_EMPTY || f->state == FUTURE_WAITING)
            {
                pid32 process_id=getpid();                  
                if(f->set_queue->pid > 0)
                {            
                    return SYSERR;
                }
                f->state = FUTURE_READY;
                f->value =value;
                f->set_queue->pid = process_id;
                f->set_queue->next = NULL;
                while(f->get_queue->pid != 0)
                {
                    struct queue* temp_queue =f->get_queue->next;
                    pid32 pid = remove_from_queue(f->get_queue);
                    resume(pid);
                    f->get_queue->pid = temp_queue->pid;
                    f->get_queue->next = temp_queue->next;
                }
            }
        }
        else if(f->mode == FUTURE_QUEUE)
        {   
            if(f->state == FUTURE_EMPTY || f->state == FUTURE_WAITING)
            {
                pid32 process_id=getpid();                   
                struct queue *q = f->get_queue;
                if(q->pid !=0)
                {          
                    f->state = FUTURE_READY;                  
                    f->value = value;                                                       
                    struct queue* temp_queue =f->get_queue->next; 
                    pid32 pid = remove_from_queue(f->get_queue);        
                    f->get_queue->pid = temp_queue->pid;
                    f->get_queue->next = temp_queue->next;
                    resume(pid);
                }
                else
                {
                    add_in_queue(f->set_queue,process_id);          
                    f->state = FUTURE_WAITING;
                    suspend(process_id); //waiting for consumer to read the from the process
                }
            }   
        }
        else
        {
            restore(interrupt_handler); //restore all interupts after the checking is done 
            return SYSERR;
        }
    
    restore(interrupt_handler); //restore interrupts
    return OK;
}
