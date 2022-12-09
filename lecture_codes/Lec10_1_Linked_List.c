#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// linked list: structures linked through pointer

//Linked List: Data Structure
struct Node {
    const char *key;
    int value;
    struct Node *next;
};

// First Node
struct Table {
    struct Node *first;
};


// Linked List: Create
struct Table *Table_create(void)
{
    struct Table *t;
    t = (struct Table *) malloc(sizeof(struct Table));
    t->first = NULL;
    return t;
}


// Linked List: Add
void Table_add(struct Table *t, const char *key, int value)
{
    struct Node *p = (struct Node*)malloc(sizeof(struct Node));
    /* we omit error checking here (e.g., p == NULL) */
    p->key = key;
    p->value = value;
    p->next = t->first;
    t->first = p;
}


// Linked List: Search
int Table_search(struct Table *t, const char *key, int *value)
{
    struct Node *p;
    for (p = t->first; p != NULL; p = p->next)
        if (strcmp(p->key, key) == 0) {
            *value = p->value;
            return 1;
        }
    return 0;
}


// Linked List: Free
void Table_free(struct Table *t) {
    struct Node *p;
    struct Node *nextp;
    for (p = t->first; p != NULL; p = nextp) {
        nextp = p->next;
        free(p);
    }
    free(t);
}

