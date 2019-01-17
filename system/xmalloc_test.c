#include <xinu.h>
typedef struct my_struct
{
	bpid32 pool_id[1000];
	int buffer_size[1000];
	int total_buffers[1000];
	int allocated_bytes[1000];
	int allocated_buffers[1000];
	int fragmented_bytes[1000];
	int cc_counter[1000];
	
}my_struct;

struct my_struct struc;
//struct allocated_struct allocated;
int counter=0;
//char *ans;
int variable_id[100];
int variable_pool_id[100];
int variable_size[100];

int cc=0;
void xmalloc_init()
{
	//printf("%d",bufinit());
	
	int x=0;
	for(x=0;x<1000;x++)
	{
		struc.pool_id[x]=0;
		struc.buffer_size[x]=0;
		struc.total_buffers[x]=0;
		struc.allocated_bytes[x]=0;
		struc.allocated_buffers[x]=0;
		struc.fragmented_bytes[x]=0;
		struc.cc_counter[x]=0;
	}
	
	//bpid32 pool_id[7];
	//struct my_struct struc;
	//printf("im here\n");
	int i=0;
	int32 size=8;
	int32 no_of_buffers=15;
	for(i=0;i<10;i++)
	{
		struc.pool_id[i]=mkbufpool(size,no_of_buffers);
		struc.buffer_size[i]=size;
		struc.total_buffers[i]=no_of_buffers;
		//printf("POOL ID:%d\t BUFFER SIZE:%d\t TOTAL BUFFERS:%d\t\n",struc.pool_id[i],struc.buffer_size[i],struc.total_buffers[i]);
		size=size*2;
		//no_of_buffers=no_of_buffers-10;
	}
}
void* xmalloc(int32 size)
{
	/*if(size<=0)
	{
		return;	
	}*/
	if(size>0)
	{
		int i=0;
		for(i=0;i<10;i++)
		{
			
			if(size<struc.buffer_size[i] && struc.cc_counter[i]<90)
			{
				/*if(struc.cc_counter[i]==90)
				{
					continue;
				}*/
				if(struc.allocated_buffers[i]<struc.total_buffers[i])
				{
					//printf("pool id:%d\n",struc.pool_id[i]);
					struc.allocated_buffers[i]=struc.allocated_buffers[i]+1;
					struc.allocated_bytes[i]=struc.allocated_bytes[i]+size;
					struc.fragmented_bytes[i]=(struc.allocated_buffers[i]*struc.buffer_size[i])-struc.allocated_bytes[i];
					struc.cc_counter[i]=struc.cc_counter[i]+1;
					char *ptr=getbuf(struc.pool_id[i]);
					//struc.total_buffers[i]=struc.total_buffers[i]-1;	
					variable_id[cc]=(int)ptr;
					variable_pool_id[cc]=i;
					variable_size[cc]=size;
					cc=cc+1;
					//printf("%d\n",(int)ptr);
					return (void*)ptr;
				}
			}
		}
		return (void*)(-1);
	}	
	else
	{
		return (void*)(-1);
	}
}
void xheap_snapshot()
{
	int i=0;
	for(i=0;i<10;i++)
	{
		printf("pool_id=%d,  buffer_size=%d,  total_buffers=%d,  allocated_bytes=%d,  allocated_buffers=%d,  fragmented_bytes=%d\n", struc.pool_id[i],struc.buffer_size[i],struc.total_buffers[i],struc.allocated_bytes[i],struc.allocated_buffers[i],struc.fragmented_bytes[i]);
		if(struc.buffer_size[i]==0)
		{
			break;
		}	
	}
} 
void xfree(void *p)
{
	if(p)
	{	
		//printf("in free\n");
		//printf("memory_address: %d\n",(int)p);
		//bpid32 pool_id;
		//char *buffer_address=(char*)p;
		//buffer_address=sizeof(bpid32);
		//pool_id=*(bpid32*)buffer_address;
		//freebuf((char*))
		int get_pool_id=-1;
		int get_variable_size=-1;
		int i=0;
		int k=0;
		
		for(k=0;k<2;k++)
		{
			//printf("%d, %d, %d\n",variable_id[k],variable_pool_id[k],variable_size[k]);
		}
		for(i=0;i<100;i++)
		{
			if(variable_id[i]==(int)p)
			{
				get_pool_id=variable_pool_id[i];
				get_variable_size=variable_size[i];
				break;
			}
		}
		int j=0;
		for(j=0;j<10;j++)
		{
			if(get_pool_id==struc.pool_id[j])	
			{
				struc.allocated_bytes[j]=struc.allocated_bytes[j]-get_variable_size;
				struc.allocated_buffers[j]=struc.allocated_buffers[j]-1;		
				struc.fragmented_bytes[j]=(struc.allocated_buffers[j]*struc.buffer_size[j])-struc.allocated_bytes[j];
			}
		}	
	


		//printf("%d\n",(int)pool_id);
		//printf("%d\n",(int)freebuf((char*)p));
		
		freebuf((char*)p);
		p=NULL;
	}
}
