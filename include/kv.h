#include <xinu.h>
#include <string.h>
#include <stdbool.h>
struct node
{
	int key;
	char *data;
	char *char_key;
	struct node *next;
};
struct cache_monitoring
{
	int total_hits;
	int total_accesses;
	int total_set_success;
	int cache_size;
	int num_keys;
	int total_evictions;
};
void* xmalloc(int32);
void xmalloc_init();
void xfree(void *);

char *ptr;
char **str_to_ret;

void reverse(struct node** head_ref);
unsigned long hash_function(char *string_to_hash);
void insertFirst(char *key, char *data);
struct node* deleteFirst();
bool isEmpty();
int length();
struct node* find(char *key);
struct node* delete(char *key);
void sort();
void reverse(struct node** head_ref);
int kv_init();
int kv_set(char *key, char *value);
char *kv_get(char *key);
bool kv_delete(char *key);
void kv_reset();
int get_cache_info(char *kind);
char** most_popular_keys(int k);

int ccounter=0;
struct cache_monitoring cache_metadata;
struct node *head=NULL;
