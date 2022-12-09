/*
Name: Eungjoe Kang
Student ID: 20210013
Description of the file:
  customer_manager2 is an API library for the customer data management,
  where the users can register the customer information and perform
  lookup operations to retrieve the purchase amount information.
  It is implemented using two hash tables that each uses
  user id and name as a key respectively .
*/

#define _GNU_SOURCE 1
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "customer_manager.h"

/*-------------------------------------------------------------------*/
#define START_BUCKET_SIZE 1024
#define MAX_BUCKET_SIZE 1048576

struct UserInfo {
  char *id;                   // customer id
  char *name;                 // customer name
  int purchase;               // purchase amount (> 0)
  struct UserInfo *next_id;   // ptr to next item by id
  struct UserInfo *next_name; // ptr to next item by name
};

struct DB {
  struct UserInfo **Table_id;   // hash table by id
  struct UserInfo **Table_name; // hash table by name
  int BucketCount; // current bucket size of the hash table
  int numItems;    // # of stored items, needed to determine
			             // whether the array should be expanded
			             // or not
};
/*-------------------------------------------------------------------*/
/* static int hash_function(const char *pcKey, int iBucketCount)
  - Return a hash code for pcKey that is between 0 and iBucketCount-1,
    inclusive. Adapted from the EE209 lecture notes. 
/*-------------------------------------------------------------------*/
enum {HASH_MULTIPLIER = 65599};

static int
hash_function(const char *pcKey, int iBucketCount)
{
  int i;
  unsigned int uiHash = 0U;
  for (i = 0; pcKey[i] != '\0'; i++)
    uiHash = uiHash * (unsigned int)HASH_MULTIPLIER
          + (unsigned int)pcKey[i];
  return (int)(uiHash % (unsigned int)iBucketCount);
}
/*-------------------------------------------------------------------*/
/* DB_T CreateCustomerDB(void)
  - Allocates memory for a new DB_T object and any underlying objects.
  - Returns a pointer to the memory block for the new DB_T object
    on success, and returns NULL if it fails to allocate the memory.
  - Prints out to stderr "Can't allocate a memory for DB_T" or
    "Can't allocate a memory for array of size (BucketCount)"
    if each of the memory allocation fails.                          */
/*-------------------------------------------------------------------*/
DB_T 
CreateCustomerDB(void)
{
  DB_T d;
  
  d = (DB_T) calloc(1, sizeof(struct DB));
  if (d == NULL) {
    fprintf(stderr, "Can't allocate a memory for DB_T\n");
    return NULL;
  }

  d->BucketCount = START_BUCKET_SIZE; // start with bucket size 1024

  // create hash table by id
  d->Table_id = (struct UserInfo **)calloc(d->BucketCount,
               sizeof(struct UserInfo *));
  // create hash table by name
  d->Table_name = (struct UserInfo **)calloc(d->BucketCount,
               sizeof(struct UserInfo *));

  if ((d->Table_id == NULL) || (d->Table_name == NULL)) {
    fprintf(stderr, "Can't allocate a memory for array of size %d\n",
	          d->BucketCount);   
    free(d);
    return NULL;
  }

  return d;
}
/*-------------------------------------------------------------------*/
/* void DestroyCustomerDB(DB_T d)
  - Frees all memory occupied by the given DB_T object, d
    and its underlying objects.
  - If the DB_T object d is NULL, it does nothing.                   */
/*-------------------------------------------------------------------*/
void
DestroyCustomerDB(DB_T d)
{
  struct UserInfo *p;
  struct UserInfo *p_next;
  if (d != NULL) {
    for (int h = 0; h < (d->BucketCount); h++)
        for (p = d->Table_id[h]; p != NULL; p = p_next) {
          p_next = p->next_id;
          free(p->id);
          free(p->name);
          free(p->next_id);
          free(p->next_name);
          free(p);
        }

    free(d->Table_id);
    free(d->Table_name);
    free(d);
  }
}
/*-------------------------------------------------------------------*/
/* int RegisterCustomer(DB_T d, const char *id,
		                    const char *name, const int purchase)
  - Registers a new customer whose id is the given id,
    name is the given name, and purchase amount is the given purchase
    to the given DB_T object, d.
  - On success, it returns 0, otherwise it returns -1.
    Failure includes the following:
    1) Any of d, id, or name is NULL
    2) purchase is zero or a negative number
    3) An item with the same id or name already exists               */
/*-------------------------------------------------------------------*/
int
RegisterCustomer(DB_T d, const char *id,
		 const char *name, const int purchase)
{
  // check for failure (d, id, or name is NULL)
  if ((d == NULL) || (id == NULL) || (name == NULL))
    return -1;
  // check for failure (purchase is zero or a negative number)
  else if (purchase <= 0)
    return -1;
  
  // check if an item with the same id or name already exists
  int h_id = hash_function(id, d->BucketCount);
  int h_name = hash_function(name, d->BucketCount);
  struct UserInfo *p;

  for (p = d->Table_id[h_id]; p != NULL; p = p->next_id)
    if (strcmp(p->id, id) == 0)
      return -1;

  for (p = d->Table_name[h_name]; p != NULL; p = p->next_name)
    if (strcmp(p->name, name) == 0)
      return -1;

  // create new user data
  struct UserInfo *new_user;
  new_user = (struct UserInfo *)calloc(1, sizeof(struct UserInfo));
  if (new_user == NULL) {
    fprintf(stderr, "Can't allocate a memory for UserInfo\n");
    return -1;
  }
  
  // store the new user data
  new_user->id = strdup(id);
  new_user->name = strdup(name);
  new_user->purchase = purchase;

  new_user->next_id = d->Table_id[h_id];
  new_user->next_name = d->Table_name[h_name];
  
  d->Table_id[h_id] = new_user;
  d->Table_name[h_name] = new_user;
  d->numItems++;
  return 0;
}
/*-------------------------------------------------------------------*/
/* int UnregisterCustomerByID(DB_T d, const char *id)
  - Unregisters a customer whose id is the given id
    from the given DB_T object, d.
    Frees all memory occupied by the item being unregistered.
  - On success, it returns 0, otherwise it returns -1.
    Failure includes the following:
    1) Any of d or id is NULL
    2) Item whose id is the given id does not exist                  */
/*-------------------------------------------------------------------*/
int
UnregisterCustomerByID(DB_T d, const char *id)
{
  // check for failure (d, or id is NULL)
  if ((d == NULL) || (id == NULL))
    return -1;  
  
  int match_found = 0;
  struct UserInfo *p;
  struct UserInfo *p_prev;

  // find match in hash table by id
  int h_id = hash_function(id, d->BucketCount);

  if (d->Table_id[h_id] != NULL) {
    p = d->Table_id[h_id];

    if (strcmp(p->id, id) == 0) { // match found at first node
      d->Table_id[h_id] = p->next_id;
      match_found = 1;
    }
    else { // match found somewhere other than the first node
      for (; p != NULL; p_prev = p, p = p->next_id)
        if (strcmp(p->id, id) == 0) {
          p_prev->next_id = p->next_id;
          match_found = 1;
          break;
        }
    }
  }

  if (match_found == 0) // matching id not found
    return -1;

  // find match in hash table by name
  int h_name = hash_function(p->name, d->BucketCount);
  p = d->Table_name[h_name];
  if (strcmp(p->id, id) == 0) // match found at first node
    d->Table_name[h_name] = p->next_id;
  else { // match found somewhere other than the first node
    for (; p != NULL; p_prev = p, p = p->next_name)
      if (strcmp(p->id, id) == 0) {
        p_prev->next_name = p->next_name;
        break;
      }
  }

  // free memory
  free(p->id);
  free(p->name);
  free(p->next_id);
  free(p->next_name);
  free(p);
  d->numItems--;
  return 0;
}
/*-------------------------------------------------------------------*/
/* int UnregisterCustomerByName(DB_T d, const char *name)
  - Unregisters a user whose name is the given name
    from the given DB_T object, d.
    Frees all memory occupied by the item being unregistered.
  - On success, it returns 0, otherwise it returns -1.
    Failure includes the following:
    1) Any of d or name is NULL
    2) Item whose name is the given name does not exist              */
/*-------------------------------------------------------------------*/
int
UnregisterCustomerByName(DB_T d, const char *name)
{
  // check for failure (d, or id is NULL)
  if ((d == NULL) || (name == NULL))
    return -1;  
  
  int match_found = 0;
  struct UserInfo *p;
  struct UserInfo *p_prev;

  // find match in hash table by name
  int h_name = hash_function(name, d->BucketCount);
  if (d->Table_name[h_name] != NULL) {
    p = d->Table_name[h_name];

    if (strcmp(p->name, name) == 0) { // match found at first node
      d->Table_name[h_name] = p->next_name;
      match_found = 1;
    }
    else { // match found somewhere other than the first node
      for (; p != NULL; p_prev = p, p = p->next_name)
        if (strcmp(p->name, name) == 0) {
          p_prev->next_name = p->next_name;
          match_found = 1;
          break;
        }
    }
  }

  if (match_found == 0) // matching name not found
    return -1;

  // find match in hash table by id
  int h_id = hash_function(p->id, d->BucketCount);
  p = d->Table_id[h_id];
  if (strcmp(p->name, name) == 0)// match found at first node
    d->Table_id[h_id] = p->next_id;
  else { // match found somewhere other than the first node
    for (; p != NULL; p_prev = p, p = p->next_id)
      if (strcmp(p->name, name) == 0) {
        p_prev->next_id = p->next_id;
        break;
      }
  }

  // free memory
  free(p->id);
  free(p->name);
  free(p->next_id);
  free(p->next_name);
  free(p);
  d->numItems--;
  return 0;
}
/*-------------------------------------------------------------------*/
/* int GetPurchaseByID(DB_T d, const char* id)
  - On success returns the purchase amount of the customer
    whose id is the given id from the given DB_T object, d,
    otherwise it returns -1.
  - Failure includes the following:
    1) Any of d or id is NULL
    2) Item whose id is the given id does not exist                  */
/*-------------------------------------------------------------------*/
int
GetPurchaseByID(DB_T d, const char* id)
{
  // check for failure (d, or id is NULL)
  if ((d == NULL) || (id == NULL))
    return -1;  

  // find match in hash table by id
  struct UserInfo *p;
  int h_id = hash_function(id, d->BucketCount);
  for (p = d->Table_id[h_id]; p != NULL; p = p->next_id)
    if (strcmp(p->id, id) == 0)
      return (p->purchase);

  // matching id not found
  return -1;
}
/*-------------------------------------------------------------------*/
/* int GetPurchaseByName(DB_T d, const char* name)
  - On success returns the purchase amount of the customer
    whose name is the given name from the given DB_T object, d,
    otherwise it returns -1.
  - Failure includes the following:
    1) Any of d or name is NULL
    2) Item whose name is the given name does not exist              */
/*-------------------------------------------------------------------*/
int
GetPurchaseByName(DB_T d, const char* name)
{
  // check for failure (d, or name is NULL)
  if ((d == NULL) || (name == NULL))
    return -1;  
  
  // find match in hash table by name
  struct UserInfo *p;
  int h_name = hash_function(name, d->BucketCount);
  for (p = d->Table_name[h_name]; p != NULL; p = p->next_name)
    if (strcmp(p->name, name) == 0)
      return (p->purchase);

  // matching id not found
  return -1;
}
/*-------------------------------------------------------------------*/
/* int GetSumCustomerPurchase(DB_T d, FUNCPTR_T fp)
  - On success, GetCustomerPurchase returns the sum of all numbers
    returned by the function fp by iterating each non-empty user item
    in the given DB_T object, d,
    and returns -1 if any of d or fp is NULL.                        */
/*-------------------------------------------------------------------*/
int
GetSumCustomerPurchase(DB_T d, FUNCPTR_T fp)
{
  // check for failure (d, or name is NULL)
  if ((d == NULL) || (fp == NULL))
    return -1;

  int sum = 0;
  char* id;
  char* name;
  int purchase;
  struct UserInfo *p;
  for (int h = 0; h < (d->BucketCount); h++)
      for (p = d->Table_id[h]; p != NULL; p = p->next_id) {
        id = p->id;
        name = p->name;
        purchase = p->purchase;
        sum += fp(id, name, purchase);
      }

  return sum;
}