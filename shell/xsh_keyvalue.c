#include <xinu.h>
#include <stdbool.h>
shellcmd xsh_keyvalue(int nargs, char*args[])
{
	//kv_init2();
	int retval;
	int set_errors=0;
	char* valtmp=NULL;
	int get_errors=0;
	kv_init();
	printf("kvset:- %d\n",kv_set("1mykey889","myvalue"));
	printf("kvset:- %d\n",kv_set("2mykey2","myvalue2"));
	printf("kvset:- %d\n",kv_set("3mykey3","myvalue3"));

    printf("kvset:- %d\n",kv_set("1mykey889","myvalue33"));
    printf("kvset:- %d\n",kv_set("2mykey2","myvalue2"));
    printf("kvset:- %d\n",kv_set("3mykey3","myvalue3"));

    printf("kvset:- %d\n",kv_set("1mykey889","myvalue34"));
    printf("kvset:- %d\n",kv_set("2mykey2","myvalue2"));
    printf("kvset:- %d\n",kv_set("3mykey3","myvalue3"));

    printf("kvset:- %d\n",kv_set("1mykey889","myvalue344"));


    printf("kvget:- %s\n",kv_get("1mykey889"));
    printf("kvget:- %s\n",kv_get("2mykey2"));
    printf("kvget:- %s\n",kv_get("3mykey3"));
    printf("kvget:- %s\n",kv_get("1mykey"));
	if(kv_delete("mykey")==true)
	{
		printf("------------key deleted------------\n");
	}
	else
	{
		printf("---------------key not deleted---------\n");
	}

    char** abc=most_popular_keys(5);
    int i=0;
	printf("---------------\n");
    for(i=0;i<3;i++)
   {
	printf("%s\n",abc[i]);
    }
	printf("----------------\n");
    printf("total_hits: %d\n",get_cache_info("total_hits"));
    printf("total_accesses: %d\n",get_cache_info("total_accesses"));
    printf("total_set_success: %d\n",get_cache_info("total_set_success"));
    printf("cache_size: %d\n",get_cache_info("cache_size"));
    printf("num_keys: %d\n",get_cache_info("num_keys"));
    printf("total_evictions: %d\n",get_cache_info("total_evictions"));

	return 0;
}
