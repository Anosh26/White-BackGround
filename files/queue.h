#ifndef QUEUE_H
#define QUEUE_H

typedef struct {
    int x, y;
} Point;

typedef struct Node {
    Point p;
    struct Node* next;
} Node;

typedef struct {
    Node *front, *rear;
} Queue;

Queue* createQueue();
void enqueue(Queue* q, int x, int y);
Point dequeue(Queue* q);
int isQueueEmpty(Queue* q);
void freeQueue(Queue* q);

#endif