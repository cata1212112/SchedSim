typedef struct proc {
    int pid;
    int arrival;
    int remaining;
    int priority;
    int wait_time;
    int last_active;
    int start_time;
    int time_quantum;
    int response_time;
}proces; /// PCB

typedef struct node {
    proces info;
    struct node* next;
    struct node* prev;
}node;

typedef struct queue {
    node* head;
    node* tail;
    proces curent;
    int num_procese;
}queue;

void init(queue *q); /// initialiare
node* newNode(proces info); /// creez nod nou
void pop(queue *q); /// sterg primul elemnt din lista
void delete_pid(queue *q, int pid); /// sterg elementul care are pidul = pid
void push(queue *q, node* Nod); /// introduc un elemnt la sfarsitul listei
int empty(queue *q); /// verific daca lista e goala
proces front(queue *q); /// copia primului element din lista
proces get_min_priority(queue *q); /// copia elementului cu prioritatea maxima din lista
proces get_min_remaining(queue *q); /// copia elementului cu timpul ramas minim din lista
proces get_max_remaining(queue *q); /// copia elementului cu timpul ramas maxim din lista
void PrintQueue(queue *q); // functie pentru debugging de printare a cozii
proces get_max_response_ration(queue *q); /// copia elementului cu cel mai mare response ration;

