// Queue for ModSim
#include <stdio.h> 
#include <stdlib.h> 

#define MAX_CAPACITY 25

typedef struct Arr {
	double arrival;
	int gate_origin;
	int gate_dest;
} Arr;

typedef struct QNode { 
	struct Arr key;
	struct QNode* next; 
} QNode; 

// The queue, head stores the head node of LL and tail stores the 
// last node of LL 
typedef struct Queue { 
	QNode *head, *tail;
	int capacity;
} Queue; 



// A utility function to create a new linked list node. 
QNode* newNode(Arr a) 
{ 
	QNode* temp = (QNode*)malloc(sizeof(QNode));

	temp->key.arrival = a.arrival;
	temp->key.gate_origin = a.gate_origin;
	temp->key.gate_dest = a.gate_dest;
	temp->next = NULL; 

	return temp; 
} 

// A utility function to create an empty queue 
Queue* createQueue() 
{ 
	Queue* q = (Queue*)malloc(sizeof(Queue)); 
	q->head = q->tail = NULL;
	q->capacity = 0;
	return q; 
}
void initializeQueue(Queue *Q){
	(Q) = (Queue *) malloc(sizeof(Queue));
	(Q)->head = NULL;
	(Q)->tail = NULL;
	(Q)->capacity = 0;
}
/**
 * @brief create an element of Queue
 * @param i the time of element is created
 * @param j the gate origin
 * @param k the gate destination
 */
Arr createArr(double i, int j, int k) {

	Arr a;
	a.arrival = i;
	a.gate_origin = j;
	a.gate_dest = k;
	
	return a;
};

/**
 * @brief push an element <Arr> type to corresponding Q
 * @param q queue to push
 * @param a element to be addes
 */
void push(Queue* q, Arr a) 
{ 
	// Create a new LL node 
	QNode* temp = newNode(a); 

	// If queue is empty, then new node is head and tail both 
	if (q->tail == NULL) { 
		q->head = q->tail = temp; 
		return; 
	} 

	// Add the new node at the end of queue and change tail 
	q->tail->next = temp; 
	q->tail = temp;
	q->capacity += 1;
} 

// Function to remove a key from given queue q 
Arr pop(Queue* q) 
{ 
	// Store previous head and move head one node ahead 
	QNode* temp = q->head;
	Arr arr_temp = temp->key;

	q->head = q->head->next; 

	// If head becomes NULL, then change tail also as NULL 
	if (q->head == NULL){
		q->tail = NULL;
	}
	q->capacity -= 1;
	return arr_temp;
}


// Function to remove a key based on gate_dest param, from given queue q 
Arr pop_gate_dest(Queue* q, int gate_dest) 
{
	Arr arr_temp;
	// Store previous head and move head one node ahead 

	if (q->head->key.gate_dest == gate_dest){
		return pop(q);
	}
	else {
		QNode* node_pointer = NULL;
		struct QNode* t = q->head;
		/* struct Queue* temp = createQueue();
		struct Arr a = createArr(t->key.arrival, t->key.gate_origin, t->key.gate_dest); */

		/* push(temp, a);
		while (t->next != NULL ) {
			if (t->next->key.gate_dest != gate_dest) {
				a = createArr(t->next->key.arrival, t->next->key.gate_origin, t->next->key.gate_dest);
				push(temp, a);
			}
			t = t->next;
		} */

		while (t->next != NULL){
			if (t->next->key.gate_dest != gate_dest){
				t = t->next;
			}
			else{
				// save pointer
				node_pointer = t->next;
				// save content
				arr_temp = node_pointer->key;
				// reposition pointer next
				t->next = node_pointer->next;
			}
		}
		q->capacity -= 1;
		free(node_pointer);
		return arr_temp;
	}
} 

int queueFull(Queue Q){
	return (Q.capacity >= MAX_CAPACITY);
}

/**
 * @brief search boolean there's a psger in a bus going to gate 'gate_destination'
 * 
 * @param gate_destination 
 * @param Q 
 * @return int 
 */
int passengerOnQueue(int gate_destination, Queue Q){
	int isFound = 0;
	QNode *t = Q.head;
  if (t == NULL){
    return 0;
  }
  
  do{
    if ((t->key).gate_dest == gate_destination){
      printf("ini POQ\n");
      return 1;
    }
    else{
      t = t->next;
    }
  }while(t != NULL);
  return 0;
}
// Driver Program to test anove functions 

/*
int main() 
{ 
	struct Queue* q = createQueue(); 
	// struct Arr* a = createArr(0.05, 1, 2); // = malloc(sizeof(struct Arr) * 3);
	// struct Arr* b = createArr(0.12, 2, 3);
	struct Arr a, b, c, d;
	struct QNode* succ;
	int i;
	a = createArr(11.7,2,4);
	d = createArr(0.9, 5, 6);
	b.arrival = 0.5;
	b.gate_origin = 1;
	b.gate_dest = 5;

	c = createArr(12.5, 4, 6);

	push(q, a); 
	push(q, b);
	push(q, c);  
	push(q, d);

	i = 1;
	succ = q->head;
	while (succ != NULL) {
		printf("%d : [%f, %d, %d]\n", i, succ->key.arrival, succ->key.gate_origin, succ->key.gate_dest);
		i++;
		succ = succ->next;
		
	}

	pop_gate_dest(q, 6);
	printf("\n\n");

	i = 1;
	succ = q->head;
	while (succ != NULL) {
		printf("%d : [%f, %d, %d]\n", i, succ->key.arrival, succ->key.gate_origin, succ->key.gate_dest);
		i++;
		succ = succ->next;
		
	}

	free(q);
	free(succ);
	return 0; 
} 
*/