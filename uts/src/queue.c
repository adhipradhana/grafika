#include "queue.h"

// A utility function to create a new linked list node. 
struct QNode* newNode(int x, int y) { 
    struct QNode *temp = (struct QNode*)malloc(sizeof(struct QNode)); 
    temp->x = x;
    temp->y = y; 
    temp->next = NULL; 
    return temp;  
} 

// A utility function to create an empty queue 
struct Queue* createQueue() { 
    struct Queue *q = (struct Queue*)malloc(sizeof(struct Queue)); 
    q->front = q->rear = NULL; 
    return q; 
} 
  
// The function to add a key k to q 
void enQueue(struct Queue *q, int x, int y) { 
    // Create a new LL node 
    struct QNode *temp = newNode(x, y); 
  
    // If queue is empty, then new node is front and rear both 
    if (q->rear == NULL) 
    { 
       q->front = q->rear = temp;  
       return; 
    } 
  
    // Add the new node at the end of queue and change rear 
    q->rear->next = temp; 
    q->rear = temp; 
} 
  
// Function to remove a key from given queue q 
struct QNode *deQueue(struct Queue *q) { 
    // If queue is empty, return NULL. 
    if (q->front == NULL) 
       return NULL; 
  
    // Store previous front and move front one node ahead 
    struct QNode *temp = q->front; 
    q->front = q->front->next; 
  
    // If front becomes NULL, then change rear also as NULL 
    if (q->front == NULL) 
       q->rear = NULL; 
    return temp; 
} 