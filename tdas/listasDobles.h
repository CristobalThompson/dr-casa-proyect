#ifndef Lista_h
#define Lista_h

typedef struct Node {
    void *data;
    struct Node *next;
    struct Node *prev;
} Node;

typedef struct List {
    Node *head;
    Node *tail;
    Node *current;
    int contador;
} List;

List * create_List(void);

void * first_List(List * list);

void * next_List(List * list);

void * last_List(List * list);

void * prev_List(List * list);

void push_Front(List * list, void * data);

void push_Back(List * list, void * data);

void push_Current(List * list, void * data);

void * pop_Front(List * list);

void * pop_Back(List * list);

void * pop_Current(List * list);

void clean_List(List * list);

int size_List(List * list);

#endif /* Lista_h */