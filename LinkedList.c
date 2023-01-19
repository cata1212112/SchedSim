#include <stdlib.h>
#include "LinkedList.h"
#include <limits.h>
#include <stdio.h>

void init(queue *q) {
    q -> head = q -> tail = NULL;
}

node* newNode(proces info) {
    node* nod = malloc(sizeof(node));
    nod -> info = info;
    nod -> next = NULL;
    nod -> prev = NULL;
    return nod;
}

int empty(queue *q) {
    if (q -> head == NULL) return 1;
    return 0;
}

void push(queue *q, node* Nod) {
    if (empty(q)) {
        q -> tail = q -> head = Nod;
    } else {
        q -> tail -> next = Nod;
        Nod->prev = q -> tail;
        q -> tail = Nod;

    }
}

void pop(queue *q) {
    if (q -> head == q -> tail) {
        q -> head = q -> tail = NULL;
    } else {
        q -> head = q -> head -> next;
        q -> head -> prev = NULL;
    }
}

void delete_pid(queue *q, int pid) {
    node *start = q -> head;

    while (start != NULL) {
        if (start->info.pid == pid) {
            if (q->head == q->tail) {
                q -> head = q ->tail = NULL;
            } else if (start != q -> tail && start != q -> head) {
                start->prev -> next = start->next;
                start->next -> prev = start -> prev;

            } else if (start == q -> tail){
                q -> tail = start->prev;
                q -> tail -> next = NULL;
            } else {
                q -> head = q -> head -> next;
                q -> head -> prev = NULL;
            }
        }
        start = start -> next;
    }
}

proces front(queue *q) {
    proces prc;
    prc = q -> head -> info;
    return prc;
}


proces get_min_priority(queue *q) {
    proces prc;
    int min_pri = INT_MAX;
    node* start = q -> head;
    while (start != NULL) {
        if (min_pri > start->info.priority) {
            min_pri = start->info.priority;
            prc = start->info;
        }
        start = start -> next;
    }
    return prc;
}

proces get_min_remaining(queue *q) {
    proces prc;
    int min = INT_MAX;
    node* start = q -> head;

    while (start != NULL) {
        if (min > start->info.remaining) {
            min = start->info.remaining;
            prc = start->info;
        } else if (min == start->info.remaining) {
            if (prc.arrival > start->info.arrival) {
                min = start->info.remaining;
                prc = start->info;
            }
        }
        start = start -> next;
    }
    return prc;
}


proces get_max_remaining(queue *q) {
    proces prc;
    int maxi = 0;
    node* start = q -> head;

    while (start != NULL) {
        if (maxi < start->info.remaining) {
            maxi = start->info.remaining;
            prc = start->info;
        }
        start = start -> next;
    }
    return prc;
}

void PrintQueue(queue *q) {
    node* start = q -> head;

    while (start != NULL) {
        printf("Pid:%d Timp:%d ", start ->info . pid, start -> info.remaining);
        start = start -> next;
    }
}

proces get_max_response_ration(queue *q) {
    node* start = q -> head;
    double maxi = 1 + (double)start -> info.wait_time / start ->info.remaining;
    proces prc = start -> info;

    while (start != NULL) {
        if (maxi < 1 + (double)start -> info.wait_time / start ->info.remaining) {
            maxi = 1 + (double)start -> info.wait_time / start ->info.remaining;
            prc = start->info;
        }
        start = start -> next;
    }
    return prc;
}

