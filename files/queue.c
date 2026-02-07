#include "../include/queue.h"
#include <stdlib.h>

Queue* createQueue() {
    Queue* q = (Queue*)malloc(sizeof(Queue));
    q->front = q->rear = NULL;
    return q;
}

void enqueue(Queue* q, int x, int y) {
    Node* temp = (Node*)malloc(sizeof(Node));
    temp->p.x = x;
    temp->p.y = y;
    temp->next = NULL;

    if (q->rear == NULL) {
        q->front = q->rear = temp;
        return;
    }
    q->rear->next = temp;
    q->rear = temp;
}

Point dequeue(Queue* q) {
    if (q->front == NULL) {
        Point error = {-1, -1};
        return error;
    }
    Node* temp = q->front;
    Point p = temp->p;
    
    q->front = q->front->next;

    if (q->front == NULL) {
        q->rear = NULL;
    }

    free(temp);
    return p;
}

int isQueueEmpty(Queue* q) {
    return (q->front == NULL);
}

void freeQueue(Queue* q) {
    if (q == NULL) return;

    Node* current = q->front;
    while (current != NULL) {
        Node* temp = current;
        current = current->next;
        free(temp);
    }
    
    free(q);
}