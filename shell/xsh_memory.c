#include <xinu.h>
#include <string.h>
#include <stdio.h>
//#include <mymem.h>

shellcmd xsh_memory(int nargs, char*args[])
{
	xmalloc_init();
	
	void *a=xmalloc(223);
	xheap_snapshot();
	xfree(a);
	
	void *b=xmalloc(445);
	b="abc";
	xheap_snapshot();
	xfree(b);

	void *c=xmalloc(667);
	xheap_snapshot();
	
	void *d=xmalloc(1000);
	xheap_snapshot();

	xfree(c);
	xfree(d);
	
	xheap_snapshot();


	return 0;
}
