#include <xinu.h>
void *my_xmalloc(ulong nbytes)
{
	if(nbytes==0)
	{
		return NULL;
	}
	else
	{
		//take a chunk of memory
		struct memblk *memory_block;
		nbytes=nbytes+sizeof(struct memblk);
		memory_block=(struct memblk*)getmem(nbytes);
		if((uint32)memory_block==SYSERR)
		{
			return NULL; //memory allocation failed 
		}
		else
		{
			memory_block->mnext=memory_block;
			memory_block->mlength=nbytes;
			return (void *)(memory_block+1);
		}
	}
}

