/*
Name: Eungjoe Kang
Student ID: 20210013
Description of the file:
  customer_manager1 is an API library for the customer data management,
  where the users can register the customer information and perform
  lookup operations to retrieve the purchase amount information.
  It is implemented using a dynamically resizable array.
*/

#define _GNU_SOURCE 1
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "customer_manager.h"

/*-------------------------------------------------------------------*/
#define UNIT_ARRAY_SIZE 1024

struct UserInfo {
  char *name;                // customer name
  char *id;                  // customer id
  int purchase;              // purchase amount (> 0)
};

// Database of
struct DB {
  struct UserInfo *pArray;   // pointer to the array
  int curArrSize;            // current array size (max # of elements)
  int numItems;              // # of stored items, needed to determine
			                       // whether the array should be expanded
			                       // or not
};
/*-------------------------------------------------------------------*/
/* DB_T CreateCustomerDB(void)
  - Allocates memory for a new DB_T object and any underlying objects.
  - Returns a pointer to the memory block for the new DB_T object
    on success, and returns NULL if it fails to allocate the memory.
  - Prints out to stderr "Can't allocate a memory for DB_T" or
    "Can't allocate a memory for array of size (curArrSize)"
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
  d->curArrSize = UNIT_ARRAY_SIZE; // start with 1024 elements
  d->pArray = (struct UserInfo *)calloc(d->curArrSize,
               sizeof(struct UserInfo));
  if (d->pArray == NULL) {
    fprintf(stderr, "Can't allocate a memory for array of size %d\n",
	          d->curArrSize);   
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
  if (d != NULL) {
    for (int h = 0; h < (d->curArrSize); h++)
        // check if element is empty
        if (d->pArray[h].name != NULL) {
          free(d->pArray[h].name);
          free(d->pArray[h].id);
        }

    free(d->pArray);
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
  for (int h = 0; h < d->curArrSize; h++)
    if (d->pArray[h].name != NULL)
      if ((strcmp(d->pArray[h].id, id) == 0)
        ||(strcmp(d->pArray[h].name, name) == 0))
        return -1;

  int new_user_idx = 0;
      
  // expand the array if empty element does not exist
  if (d->numItems == d->curArrSize) {
    d->curArrSize += UNIT_ARRAY_SIZE;
    d->pArray = (struct UserInfo *)realloc(d->pArray,
                        sizeof(struct UserInfo)*d->curArrSize);
    if (d->pArray == NULL) {
      fprintf(stderr, "Can't allocate a memory for array of size %d\n",
	    d->curArrSize);
      return -1;
    }

    // initialize expanded part of the array
    for (int h = d->curArrSize - UNIT_ARRAY_SIZE;
         h < d->curArrSize; h++)
      d->pArray[h].name = NULL;
    
    // index of the empty element
    new_user_idx = d->curArrSize - UNIT_ARRAY_SIZE; 
  }
  // find empty element if array is not full
  else if (d->numItems < d->curArrSize) {
    for (int h = 0; h < d->curArrSize; h++ )
      if (d->pArray[h].name == NULL) {
        new_user_idx = h; // index of the empty element
        break;
      }
  }
  else
    assert(0);
    
  // store the new user data
  d->pArray[new_user_idx].name = strdup(name);
  d->pArray[new_user_idx].id = strdup(id);
  d->pArray[new_user_idx].purchase = purchase;
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
  
  // find matching id and unregister
  for (int h = 0; h < d->curArrSize; h++)
    if (d->pArray[h].name != NULL) // check if element is empty
      if (strcmp(d->pArray[h].id, id) == 0) {
        free(d->pArray[h].name);
        free(d->pArray[h].id);
        d->pArray[h].name = NULL; // initialize
        d->numItems--;
        return 0;
      }

  // matching id not found
  return -1;
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
  
  // find matching id and unregister
  for (int h = 0; h < d->curArrSize; h++)
    if (d->pArray[h].name != NULL) // check if element is empty
      if (strcmp(d->pArray[h].name, name) == 0) {
        free(d->pArray[h].name);
        free(d->pArray[h].id);
        d->pArray[h].name = NULL; // initialize
        d->numItems--;
        return 0;
      }

  // matching id not found
  return -1;
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
  
  // find matching id
  for (int h = 0; h < d->curArrSize; h++)
    if (d->pArray[h].name != NULL) // check if element is empty
      if (strcmp(d->pArray[h].id, id) == 0)
        return (d->pArray[h].purchase);

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
  
  // find matching name
  for (int h = 0; h < d->curArrSize; h++)
    if (d->pArray[h].name != NULL) // check if element is empty
      if (strcmp(d->pArray[h].name, name) == 0)
        return (d->pArray[h].purchase);

  // matching name not found
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

  for (int h = 0; h < d->curArrSize; h++) 
      // check if element is empty
      if (d->pArray[h].name != NULL) {
        id = d->pArray[h].id;
        name = d->pArray[h].name;
        purchase = d->pArray[h].purchase;
        sum += fp(id, name, purchase);
      }

  return sum;
}
