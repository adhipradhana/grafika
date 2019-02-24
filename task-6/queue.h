// A C program to demonstrate linked list based implementation of queue 
// Reference : https://www.geeksforgeeks.org/queue-set-2-linked-list-implementation/
#ifndef _QUEUE_H
#define _QUEUE_H

#include <stdlib.h> 
#include <stdio.h> 
  
struct QNode { 
    int x, y; 
    struct QNode *next; 
}; 

struct Queue { 
    struct QNode *front, *rear; 
}; 

struct QNode* newNode(int x, int y);

struct Queue* createQueue();

void enQueue(struct Queue *q, int x, int y);

struct QNode *deQueue(struct Queue *q);

#endif