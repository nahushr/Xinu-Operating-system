#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/file.h>
#include <unistd.h>
#include <stdbool.h>

void printfile();
int total_number;
int* start_arr;
int* end_arr;
FILE *fp[10000];
char *word;
char *file_name;
char *result=NULL;
//pthread_mutex_t lock;
int counter=0;
//pthread_cond_t count_threshold_cv=PTHREAD_COND_INITIALIZER;
#define BUF_SIZE 1024

struct node {
   int data;
   int key;
   struct node *next;
};

struct node *head = NULL;
struct node *current = NULL;
//https://www.tutorialspoint.com/data_structures_algorithms/linked_list_program_in_c.htm
//display the list

void insertFirst(int key, int data) {
   //create a link
   struct node *link = (struct node*) malloc(sizeof(struct node));
  
   link->key = key;
   link->data = data;
  
   //point it to old first node
   link->next = head;
  
   //point first to new first node
   head = link;
}

 void printList() {
   struct node *ptr = head;
   printf("\n[ ");
  
   //start from the beginning
   while(ptr != NULL) {
      printf("(%d,%d) ",ptr->key,ptr->data);
      ptr = ptr->next;
   }
  
   printf(" ]");
}

//delete first item
struct node* deleteFirst() {

   //save reference to first link
   struct node *tempLink = head;
  
   //mark next to first link as first 
   head = head->next;
  
   //return the deleted link
   return tempLink;
}

//is list empty
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

void sort() {

   int i, j, k, tempKey, tempData;
   struct node *current;
   struct node *next;
  
   int size = length();
   k = size ;
  
   for ( i = 0 ; i < size - 1 ; i++, k-- ) {
      current = head;
      next = head->next;
    
      for ( j = 1 ; j < k ; j++ ) {   

         if ( current->data < next->data ) {
            tempData = current->data;
            current->data = next->data;
            next->data = tempData;

            tempKey = current->key;
            current->key = next->key;
            next->key = tempKey;
         }
      
         current = current->next;
         next = next->next;
      }
   }   
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
int getkey(){
  struct node *ptr=head;
  int key_value=ptr->key;
  //head=head->next;
  return key_value;
}

 void *BusyWork(void *t)
 {
   // pthread_mutex_lock(&lock);

    int i;
    int tid;
    char* result;
    tid = (intptr_t)t;
  
    //printf("Thread %d printed.....................................................\n",tid);
    char* line;
    line=malloc(sizeof(1000000000000000000));
    size_t len=0;
    //printf("In: %d\n",  start_arr[tid]);

     fp[tid] = fopen(file_name, "r");  
     fseek(fp[tid], start_arr[tid], SEEK_CUR);   // jump to beginning
    // result_arr[tid].thread=tid;
    
    int cc=0;
    while(1)
    {
      getline(&line,&len,fp[tid]);
      cc=cc+1;
      if(strstr(line , word) !=NULL)
      {
	//if(tid==9)
        //printf("%s" , line);    
        insertFirst(cc, tid);
      }
      if(ftell(fp[tid])>=end_arr[tid])
      {
//        printf("broke\n");
        break;
      }
    }
    free(line);
    fclose(fp[tid]);
    counter=counter+1;
//    pthread_exit(NULL);
 }
 
 int main (int argc, char *argv[])
 {
	//int flag=0;
	//printf("here1");	
	if(!feof(stdin) && argc==2)
	{
		//flag=1;
		//if(argc==3) flag=1;		
		file_name="myfile889.txt";
		void *content=malloc(BUF_SIZE);
		FILE *fp667=fopen(file_name,"w");
		int read;
		while(read=fread(content,1,BUF_SIZE,stdin)){
	//		flag=1;
			fwrite(content, read, 1, fp667);
		}
		fclose(fp667);
		if(argc==2)
		{
			//single processor
			 char line1[1000];

              // initialsing the file pointer to read
	              FILE* fp1 = fopen(file_name,"r");

             // reading line by line and comparing each word in line
        	     while(fscanf(fp1 , "%[^\n]\n" , line1)!=EOF)
	            {
	                      if(strstr(line1 , argv[1]) !=NULL)
	                     {
	                             // print that line
	                             printf("%s\n" , line1);
	                      }
	                     else
        	            {
                	            continue;
	                    }	
        	   }
	            fclose(fp1);
	                exit(1);


		}
		
		
	}	
	//printf("here2");


	//exit(1);
	if(atoi(argv[1])==1)
	{
		//single processor
		//exit(1);
		char line1[1000];

              // initialsing the file pointer to read
              FILE* fp1 = fopen(argv[3],"r");

             // reading line by line and comparing each word in line
             while(fscanf(fp1 , "%[^\n]\n" , line1)!=EOF)
            {
                      if(strstr(line1 , argv[2]) !=NULL)
                     {
                             // print that line
                             printf("%s\n" , line1);
                      }
                     else
                    {
                            continue;
                    }
           }
            fclose(fp1);
                exit(1);

	}
	//printf("here3");
        if(argc==3 )
	{
		//printf("here");
		char line1[1000];

              // initialsing the file pointer to read
              FILE* fp1 = fopen(argv[2],"r");

             // reading line by line and comparing each word in line
             while(fscanf(fp1 , "%[^\n]\n" , line1)!=EOF)
            {
                      if(strstr(line1 , argv[1]) !=NULL)
                     {
                             // print that line
                             printf("%s\n" , line1);
                      }
                     else
                    {
                            continue;
                    }
           }
            fclose(fp1);
                exit(1);
	}


  //  pthread_mutex_init(&lock,NULL);
    pthread_t thread[atoi(argv[1])];
   
   // int num=atoi(argv[1]);
    int rc;
    int t;
    void *status;
    total_number=atoi(argv[1]);
    start_arr = calloc(total_number, sizeof(int));
    end_arr = calloc(total_number, sizeof(int));
  //  h = (heap_t *)calloc(1, sizeof (heap_t));
   

    if(argc==4)
    {
	 file_name=(char*)argv[3];
     }
    word=(char*)argv[2];

    for(t=0;t<total_number;t++)
    {
       fp[t] = fopen(file_name, "r");  
      if (fp[t] == NULL) { 
          printf("File Not Found!\n"); 
          exit(1);
      }
    } 
    fseek(fp[0], 0L, SEEK_END); 
    long int res = ftell(fp[0]); 
    
    int start=0;
    start_arr[0]=0;
    int block_size=(res/total_number);
    
    
    int end=end_arr[0];
    int x;
    fseek(fp[0],end,SEEK_SET);
    while(fgetc(fp[0])!= '\n')
    {
          end=end+1;
    }  
    end_arr[0]=end;
//    pthread_create(&thread[0], NULL, BusyWork, (void *)(intptr_t)t);

    for(x=1;x<total_number;x++)
    {
  	if(x==total_number-1)
	  {
	    start=end+1;
	    fseek(fp[0],0L,SEEK_END);
	    end=ftell(fp[0]);
	    start_arr[x]=start;
	    end_arr[x]=end;
	  }
	  else
	  {
	
		  start = end+1;
	          end=end+block_size;
	          fseek(fp[0],end,SEEK_SET);
	          while(fgetc(fp[0]) != '\n')
    		  {
		      end=end+1;
		  }
          start_arr[x]=start;
          end_arr[x]=end;
  	  }
//	pthread_create(&thread[x],NULL,BusyWork, (void *)(intptr_t)t);
    }
  

    /*Dividing the file*/


    /* Initialize and set thread detached attribute */
  for(t=0; t<total_number; t++) {
      pthread_create(&thread[t], NULL, BusyWork, (void *)(intptr_t)t);  
       }

    
   for(t=0; t<total_number; t++) {
       pthread_join(thread[t], NULL);
       }

    
  //   printf("Waiting here 1");
  
     if(counter==total_number){
//printf("hello");
//printList();
sort();
//printList();
reverse(&head);
//printList();
//printfile();
int func_xx=0;
//char *func_line;
//func_line=(char*)malloc(1000000000);
//FILE *fpx=fopen(file_name,"r");
//size_t func_len=0;
for(func_xx=0;func_xx<total_number;func_xx++)
{
	char *func_line;
	func_line=(char*)malloc(100000000);
	FILE *fpx=fopen(file_name,"r");
	size_t func_len=0;
//	printf("Thread %d printing with starting position %d, ending postition %d........................\n",func_xx,start_arr[func_xx]);
	fseek(fpx,start_arr[func_xx],SEEK_CUR);
	int func_counter=0;
	//printf("hello21");
	//fscanf(fpx,"%[^\n]",line);
 //       printf("Thread %d printing with starting position %d, ending postition %d, ftell position %d........................\n",func_xx,start_arr[func_xx],ftell(fpx));

	while(1)
	{
		getline(&func_line, &func_len, fpx);
		//int func_counter=0;
		func_counter=func_counter+1;
		//int key=getkey();
		//printf("%d\n",getkey());
		if(!isEmpty())
		{
			if(func_counter==getkey())
			{
				printf("%s", func_line);
				struct node *temp=deleteFirst();
			}
		}
		if(ftell(fpx)>=end_arr[func_xx])
		{
			fclose(fpx);
//			printf("^^^^^^^^^^^^broke");
			break;
		}
	}
   //  printf("*****************hello*************************");
}
//fclose(fpx);
//printf("terminated");
}
	pthread_exit(NULL);
}

