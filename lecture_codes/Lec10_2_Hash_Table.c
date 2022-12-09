#include <stdio.h>

// Hash Function
unsigned int hash(const char *x) {
    int i;
    unsigned int h = 0U;
    for (i=0; x[i]!='\0'; i++)
        h = h * 65599 + (unsigned char)x[i];
    return h % 1024;
}

enum {BUCKET_COUNT = 1024};


// Hash Table: Data Structure
struct Node {
    const char *key;
    int value;
    struct Node *next;
};

struct Table {
    struct Node *array[BUCKET_COUNT];
};


// Hash Table: Create
struct Table *Table_create(void) {
    struct Table *t;
    t = (struct Table*)calloc(1, sizeof(struct Table));
    return t;
}


// Hash Table: Add
void Table_add(struct Table *t, const char *key, int value)
{
    struct Node *p = (struct Node*)malloc(sizeof(struct Node));
    int h = hash(key);
    p->key = key;
    p->value = value;
    p->next = t->array[h];
    t->array[h] = p;
}


// Hash Table: Search
int Table_search(struct Table *t, const char *key, int *value)
{
    struct Node *p;
    int h = hash(key);
    for (p = t->array[h]; p != NULL; p = p->next)
        if (strcmp(p->key, key) == 0) {
            *value = p->value;
            return 1;
        }
    return 0;
}


// Hash Table: Free
void Table_free(struct Table *t) {
    struct Node *p;
    struct Node *nextp;
    int b;
    for (b = 0; b < BUCKET_COUNT; b++)
        for (p = t->array[b]; p != NULL; p = nextp) {
            nextp = p->next;
            free(p);
        }
    free(t);
}


int Table_delete(struct Table *t, const char *key) {
    struct Node *p, *q;
    int matched = 0;
    int h = hash(key);

    // Loop to delete the first node with the matching key
    while (t->array[h] && strcmp(t->array[h]->key, key) == 0) {
        q = t->array[h];
        t->array[h] = t->array[h]->next;
        free(q->key);
        free(q);
        matched = 1;
    }

    // Search to delete non-first nodes with the matching key
    for (p = t->array[h]; p != NULL;) {
        if (p->next && strcmp(p->next->key, key) == 0) {
            q = p->next;
            p->next = q->next;
            free(q->key);
            free(q);
            matched = 1;
        }
        else
            p = p->next;
    }
    return matched;
}