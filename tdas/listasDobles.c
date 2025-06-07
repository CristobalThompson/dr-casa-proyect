//listasDobles.c
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "listasDobles.h"


Node * createNode(void * data) {
    Node * new = (Node *)malloc(sizeof(Node));
    assert(new != NULL);
    new->data = data;
    new->prev = NULL;
    new->next = NULL;
    return new;
}

List * create_List() {
    List* lista = (List*)malloc(sizeof(List));
    assert(lista != NULL);
    lista->head = NULL;
    lista->tail = NULL;
    lista-> current = NULL;
    lista->contador = 0;
    return lista;
}

void * first_List(List * list) {
    if (list->head == NULL) return NULL;
    list->current = list->head;
    return list->head->data;
}

void * next_List(List * list) {
    if (list->head == NULL || list->current == NULL || list->current->next == NULL) return NULL;
    list->current = list->current->next;
    return list->current->data;
}

void * last_List(List * list) {
    if (list->head == NULL) return NULL;
    list->current = list->head;
    while (list->current->next != NULL)
    {
        list->current = list->current->next;
    }
    return list->current->data;
}

void * prev_List(List * list) {
    if (list->current != NULL) list->current = list->current->prev;
    if (list->current != NULL) return list->current->data;
    return NULL;
}

void push_Front(List * list, void * data) {
    Node* newNodo = createNode(data);
    if (list->head == NULL)
    {
        list->current = newNodo;
        list->tail = newNodo;
    }
    else
    {
        list->head->prev = newNodo;
        newNodo->next = list->head;
    }
    list->head = newNodo;
    ++(list->contador);
}

void push_Back(List * list, void * data) {
    list->current = list->tail;
    push_Current(list, data);
}

void push_Current(List * list, void * data) {
    Node* izq = list->current;
    Node* nuevo = createNode(data);
    if (list->head == NULL)
    {
        nuevo->prev = NULL;
        nuevo->next = NULL;
        list->head = nuevo;
        list->tail = nuevo;
    }
    else if (list->current->next == NULL)
    {
        nuevo->next = NULL;
        nuevo->prev = izq;
        list->tail = nuevo;
        izq->next = nuevo;
    }
    else
    {
        Node* der = list->current->next;
        nuevo->next = der;
        der->prev = nuevo;
        nuevo->prev = izq;
        izq->next = nuevo;
    }
    ++(list->contador);
}

void * pop_Front(List * list) {
    list->current = list->head;
    return pop_Current(list);
}

void * pop_Back(List * list) {
    list->current = list->tail;
    return pop_Current(list);
}

void * pop_Current(List * list) {
    void* data = list->current->data;

    if (list->current->prev == NULL && list->current->next == NULL)
    {
        list->head = list->tail = list->current = NULL;
    }
    else if (list->current->prev == NULL)
    {
        list->head = list->head->next;
        list->head->prev = NULL;
        list->current = list->head;
    }
    else if (list->current->next == NULL)
    {
        list->tail = list->current->prev;
        list->tail->next = NULL;
        list->current = NULL;
    }
    else
    {
        list->current->prev->next = list->current->next;
        list->current->next->prev = list->current->prev;
        list->current = list->current->next;
    }
    --(list->contador);
    return data;
}

void clean_List(List * list) {
    while (list->head != NULL) {
        pop_Front(list);
    }
}

int size_List(List * list){
    return list->contador;
}