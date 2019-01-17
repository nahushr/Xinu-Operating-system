#include <xinu.h>
#include <string.h>
#include <stdio.h>

shellcmd xsh_hello(int nargs, char*args[])
{
printf("hello operating systems");
return 0;
}

