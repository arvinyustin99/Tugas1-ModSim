// Queue for ModSim
#include <stdio.h> 
#include <stdlib.h> 


struct Arr {
	double arrival;
	int gate_origin;
	int gate_dest;
};

struct QNode { 
	struct Arr key;
	struct QNode* next; 
}; 

// The queue, head stores the head node of LL and tail stores the 
// last node of LL 
struct Queue { 
	struct QNode *head, *tail; 
}; 



// A utility function to create a new linked list node. 
struct QNode* newNode(struct Arr a) 
{ 
	struct QNode* temp = (struct QNode*)malloc(sizeof(struct QNode));

	temp->key.arrival = a.arrival;
	temp->key.gate_origin = a.gate_origin;
	temp->key.gate_dest = a.gate_dest;
	temp->next = NULL; 

	return temp; 
} 

// A utility function to create an empty queue 
struct Queue* createQueue() 
{ 
	struct Queue* q = (struct Queue*)malloc(sizeof(struct Queue)); 
	q->head = q->tail = NULL; 
	return q; 
} 

struct Arr createArr(double i, int j, int k) {

	struct Arr a;
	a.arrival = i;
	a.gate_origin = j;
	a.gate_dest = k;
	
	return a;
};

// The function to add a key k to q 
void push(struct Queue* q, struct Arr a) 
{ 
	// Create a new LL node 
	struct QNode* temp = newNode(a); 

	// If queue is empty, then new node is head and tail both 
	if (q->tail == NULL) { 
		q->head = q->tail = temp; 
		return; 
	} 

	// Add the new node at the end of queue and change tail 
	q->tail->next = temp; 
	q->tail = temp; 
} 

// Function to remove a key from given queue q 
void pop(struct Queue* q) 
{ 
	// If queue is empty, return NULL. 
	if (q->head == NULL)
		{}
	else {
		// Store previous head and move head one node ahead 
		struct QNode* temp = q->head;

		q->head = q->head->next; 

		// If head becomes NULL, then change tail also as NULL 
		if (q->head == NULL)
			q->tail = NULL; 

		temp = NULL;
		free(temp);
	} 
} 


// Function to remove a key based on gate_dest param, from given queue q 
void pop_gate_dest(struct Queue* q, int gate_dest) 
{ 
	// If queue is empty, return NULL. 
	if (q->head == NULL)
		{}
	else {
		// Store previous head and move head one node ahead 

		if (q->head->key.gate_dest == gate_dest)
			pop(q);
		else {
			struct Queue* temp = createQueue();
			struct QNode* t = q->head;
			struct Arr a = createArr(t->key.arrival, t->key.gate_origin, t->key.gate_dest);
			int i;

			push(temp, a);
			while (t->next != NULL ) {
				if (t->next->key.gate_dest != gate_dest) {
					a = createArr(t->next->key.arrival, t->next->key.gate_origin, t->next->key.gate_dest);
					push(temp, a);
				}
				t = t->next;
			}

			*q = *temp;
			t = NULL;
			free(temp);
			free(t);
		}
	} 
} 

// Driver Program to test anove functions 
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
