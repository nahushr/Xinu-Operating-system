#ifndef _FUTURE_H_
#define _FUTURE_H_  

#define HPID -1
#define TPID -2

struct queue{
  pid32 pid;
  struct qnode *next;
};

typedef enum{
  FUTURE_EMPTY,
  FUTURE_WAITING,
  FUTURE_READY
} future_state_t;

typedef enum{
  FUTURE_EXCLUSIVE,
  FUTURE_SHARED,
  FUTURE_QUEUE
} future_mode_t;

typedef struct{
  int value;
  future_state_t state;
  future_mode_t mode;
  pid32 pid;
  struct queue* set_queue;
  struct queue* get_queue;
} future_t;

extern sid32 process_to_call;
extern sid32 process_set;
extern sid32 process_get;

future_t* future_alloc(future_mode_t mode);
syscall future_free(future_t*);
syscall future_get(future_t*, int*);
syscall future_set(future_t*, int);

uint future_producer(future_t*, int);
uint future_consumer(future_t*);

struct queue* insert_in_queue(pid32); // struct to create a new node in the queue
struct queue *doubly_queue_init(); //standard call to initialize the system queue
int is_empty(struct queue*); //function to check whether the queue is empty or no
void add_in_queue(struct queue*, pid32);
pid32 remove_from_queue(struct queue*);

#endif /* _FUTURE_H_ */

