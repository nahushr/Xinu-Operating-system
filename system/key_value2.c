#include <kv.h>
#include <string.h>
void movetofront(struct node **head_ref) 
{ 
    if (*head_ref == NULL || (*head_ref)->next == NULL) 
        return; 
    struct node *secLast = NULL; 
    struct node *last = *head_ref; 
    while (last->next != NULL) 
    { 
        secLast = last; 
        last = last->next; 
    } 
    secLast->next = NULL; 
    last->next = *head_ref; 
    *head_ref = last; 
} 
unsigned long hash_function(char *string_to_hash)
{
	unsigned long hash_heuristic=5381;
	int i;
	while((i=*string_to_hash++))
	{
		hash_heuristic=((hash_heuristic << 5)+hash_heuristic)+i;
	}
	return hash_heuristic;
}

void printList() {
   struct node *ptr = head;
   printf("\n[ ");
   //start from the beginning
   while(ptr != NULL) {
      printf("(%d,%s)\n ",ptr->key,ptr->data);
      ptr = ptr->next;
   }
   printf(" ]\n");
}

void insertFirst(char *key, char *data) {
   int hashed_key=(int)hash_function(key);
   cache_metadata.total_set_success++;
   if(ccounter<=95)
   {
   		struct node *link = (struct node*)xmalloc(sizeof(struct node));
        cache_metadata.cache_size=cache_metadata.cache_size+(int)sizeof(struct node);
        cache_metadata.num_keys++;
		link->key = hashed_key;
		link->data = data;
		link->char_key=key;
		link->next =head;
		head=link;
		ccounter++;
   }
   else
   {
		cache_metadata.total_evictions++;
		struct node *findelement=find(key);
		if(findelement==NULL)
		{
			reverse(&head);
			head->key=hashed_key;
			head->data=data;

			reverse(&head);
			movetofront(&head);
		}
		else
		{
			//move the current node to the front of the linkedlist
			struct node *temp=head;
			struct node *temp2=head;
			int ccc=0;
			int f_flag=0;
			while(1)
			{
				if(temp->key==findelement->key)
				{
					//do nothing
					if(ccc==0)
					{
						f_flag=1;
					}
					break;
				}
				if(ccc>0)
				{
					temp2=temp2->next;
				}
				temp=temp->next;
				ccc++;
			}
			if(f_flag==0)
			{
				temp2->next=findelement->next;
				findelement->next=head;
				head=findelement;
			}
		}
    }
}

struct node* deleteFirst() {
   struct node *tempLink = head;
   head = head->next;
   xfree((void*)tempLink);
   freemem((char*)tempLink,(uint32)sizeof(tempLink));
   return tempLink;
}

bool isEmpty() {
   return head == NULL;
}

int length() {
   int length = 0;
   struct node *current;	
   for(current = head; current != NULL; current = current->next) {
      length++;
   }
   return length;
}
struct node* find(char *key) {
   int hashed_key=hash_function(key);
   struct node* current = head;
   if(head == NULL) {
      return NULL;
   }
   while(current->key != hashed_key) {
      if(current->next == NULL) {
         return NULL;
      } 
      else {
         current = current->next;
      }
   }      
   return current;
}

struct node* delete(char *key) {
	int hashed_key=hash_function(key);
   struct node* current = head;
   struct node* previous = NULL;
   if(head == NULL) {
      return NULL;
   }
   while(current->key != hashed_key) 
   {
      if(current->next == NULL) {
        xfree((void*)current);
	freemem((char*)current,(uint32)sizeof(current));
         return NULL;
      } else {
         previous = current;
         current = current->next;
      }
   }
  
   if(current == head) {
      head = head->next;
   } else {
      previous->next = current->next;
   }    
   xfree((void*)current);
   freemem((char*)current,(uint32)sizeof(current));
   return current;
}
void reverse(struct node** head_ref) {
   struct node* prev   = NULL;
   struct node* current = *head_ref;
   struct node* next;
	while (current != NULL) {
      next  = current->next;
      current->next = prev;   
      prev = current;
      current = next;
   }
   *head_ref = prev;
}

int kv_init()
{
	xmalloc_init();
	str_to_ret=(void*)getmem(10*sizeof(char *));
	cache_metadata.total_hits=0;
	cache_metadata.total_accesses=0;
	cache_metadata.total_set_success=0;
	cache_metadata.cache_size=0;
	cache_metadata.num_keys=0;
	cache_metadata.total_evictions=0;
    return 1;
}

int kv_set(char *key, char *value)
{
	if(sizeof(key)<=0 && sizeof(value)<0)
	{
		return 1;
	}
	else
	{
		char *temp_key=NULL;
		if(strlen(key)>1000)
		{
			strncpy(temp_key,key,1000);
		}
		else
		{
			temp_key=key;
		}
		struct node *findelement=find(key);
		if(findelement==NULL)
		{
			//element is not present in cache simply add;
			insertFirst(temp_key,value);
			return 0;
		}
		else
		{
			//element already exist apply lru and add it in front of the list again
			delete(temp_key);
			insertFirst(temp_key,value);
			return 0;
		}
	}
	return 1;
}
char *kv_get(char *key)
{
	//update total_accesses
	cache_metadata.total_accesses++;
	struct node *getelement=find(key);
	if(getelement!=NULL)
	{
		cache_metadata.total_hits++;	
		char *ret_data=getelement->data;
		struct node *temp=head;
		struct node *temp2=head;
		int ccc=0;
		int f_flag=0;
		while(1)
		{
			if(temp->key==getelement->key)
			{
				if(ccc==0)
				{
					f_flag=1;
				}
				break;
			}
			if(ccc>0)
			{
				temp2=temp2->next;
			}
			temp=temp->next;
			ccc++;
		}
		if(f_flag==0)
		{
			temp2->next=getelement->next;
			getelement->next=head;
			head=getelement;	
		}
		return ret_data;
	}
	else
	{
		return NULL;
	}
	return NULL;
}
bool kv_delete(char *key)
{
	struct node* findelement=find(key);
	if(findelement!=NULL)
	{
		//key found successfully delete it 
		delete(key);
		return true;
	}
	else
	{
		//key not found cant delete it from the cache
		return false;
	}
	return false;
}
void kv_reset()
{
	while(!isEmpty())
	{
		deleteFirst();	
	}
}
int get_cache_info(char *kind)
{
	if(strncmp(kind,"total_hits",strlen(kind))==0)
	{
		return cache_metadata.total_hits;	
	}
	else if(strncmp(kind,"total_accesses",strlen(kind))==0)
	{
		return cache_metadata.total_accesses;
	}
	else if(strncmp(kind,"total_set_success",strlen(kind))==0)
	{
		return cache_metadata.total_set_success;
	}
	else if(strncmp(kind,"cache_size",strlen(kind))==0)
	{
		return cache_metadata.cache_size;
	}
	else if(strncmp(kind,"num_keys",strlen(kind))==0)
	{
		return cache_metadata.num_keys;
	}
	else if(strncmp(kind,"total_evictions",strlen(kind))==0)
	{
		return cache_metadata.total_evictions;
	}
	else
	{
		return -1;
	}
	return -1;
}
char** most_popular_keys(int k)
{
	struct node *temp=head;
	char *ptrr;
	int i=0;
	while(i<k || temp!=NULL)
	{
		ptrr=temp->char_key;
		str_to_ret[i]=ptrr;
		temp=temp->next;
		i++;
	}
	return str_to_ret;
}
