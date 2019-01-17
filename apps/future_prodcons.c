#include <xinu.h>
#include <future.h>
#include <stdlib.h>
#include <stdio.h>

uint future_producer(future_t* f, int n)
{
	printf("Produced %d \n" ,n);
	future_set(f,n);
	return OK;
}

uint future_consumer(future_t* f)
{
	int i;
	while((int)future_get(f,&i)>=1)
	{
		printf("Consumed %d\n",i);
		return OK;
	}
}


void add_in_queue(struct queue* front, pid32 process_id)
{
	//https://github.com/kulsv/
	struct queue *temp_node;
	struct queue *tail_node;
	struct queue *new_node;

	//now initialize both temp and tail pointing to the head of the queue
	temp_node=front;
	tail_node=front;

	//allocate a certain memory to the new node to be eentered in the queue
	new_node=(struct queue*)getmem(sizeof(struct queue));
	new_node->pid=process_id;
	new_node->next=NULL; //since its added at the end of the queue

	//point the temp on the new node too now
	while(temp_node!=NULL)
	{
		if(temp_node->next==NULL)
		{
			tail_node=temp_node;
			break;
		}
		else
		{
			temp_node=temp_node->next;
			//iterating temp_node over the list
		}
	}

	if(tail_node->pid==0)
	{
		tail_node->pid=new_node->pid;
		tail_node->next=new_node->next;
		front=tail_node;
	}
	else
	{
		tail_node->next=new_node;
	}
	return process_id;
}


pid32 remove_from_queue(struct queue* front)
{
	pid32 pid;
	struct queue* head;
	head=front->next;
	pid=front->pid;
	front=head;
	return pid;
}

	
