/*******************************************************************/
/******* Author    : Mahmoud Abdelraouf Mahmoud    *****************/
/******* Date      : 13 Apr 2023                   *****************/
/******* Version   : 0.1                           *****************/
/******* File Name : Generic Linked List Thread.c  *****************/
/*******************************************************************/
/**-----------------< Includes section -----------------*/
/**< System includes */
#include <stdlib.h>
/**< Project includes */
#include "glthread.h"

/**-----------------< Functions implementation section -----------------*/
void init_glthread(glthread_t *glthread) {
  glthread->left = NULL;  // Set the left pointer of the glthread to NULL
  glthread->right = NULL; // Set the right pointer of the glthread to NULL
}

void glthread_add_next(glthread_t *curr_glthread, glthread_t *new_glthread) {
  if(!new_glthread || !curr_glthread) {
      return;
  }

  if (!curr_glthread->right) {
    curr_glthread->right = new_glthread;
    new_glthread->left = curr_glthread;
    return;
  }

  glthread_t *temp = curr_glthread->right;
  curr_glthread->right = new_glthread;
  new_glthread->left = curr_glthread;
  new_glthread->right = temp;
  temp->left = new_glthread;
}

void glthread_add_before(glthread_t *curr_glthread, glthread_t *new_glthread) {

  if (!curr_glthread->left) {
    new_glthread->left = NULL;
    new_glthread->right = curr_glthread;
    curr_glthread->left = new_glthread;
    return;
  }

  glthread_t *temp = curr_glthread->left;
  temp->right = new_glthread;
  new_glthread->left = temp;
  new_glthread->right = curr_glthread;
  curr_glthread->left = new_glthread;
}

void remove_glthread(glthread_t *curr_glthread) {

  // If the current node does not have a left neighbor
  if (!curr_glthread->left) {
    // If the current node has a right neighbor
    if (curr_glthread->right) {
      curr_glthread->right->left =
          NULL;                    // Update the left neighbor of the right node
      curr_glthread->right = 0; // Remove the link to the right node
      return;
    }
    return;
  }
  // If the current node does not have a right neighbor
  if (!curr_glthread->right) {
    curr_glthread->left->right =
        NULL;                   // Update the right neighbor of the left node
    curr_glthread->left = NULL; // Remove the link to the left node
    return;
  }
  // If the current node has both left and right neighbors
  curr_glthread->left->right =
      curr_glthread->right; // Update the right neighbor of the left node
  curr_glthread->right->left =
      curr_glthread->left;     // Update the left neighbor of the right node
  curr_glthread->left = 0;  // Remove the link to the left node
  curr_glthread->right = 0; // Remove the link to the right node
}

void delete_glthread_list(glthread_t *base_glthread) {
  glthread_t *glthreadptr = NULL;

  // Iterate over the linked list and remove each glthread_t structure
  ITERATE_GLTHREAD_BEGIN(base_glthread, glthreadptr) {
    remove_glthread(glthreadptr);
  }
  ITERATE_GLTHREAD_END(base_glthread, glthreadptr);
}

void glthread_add_last(glthread_t *base_glthread, glthread_t *new_glthread) {
  glthread_t *glthreadptr = NULL, *prev_glthreadptr = NULL;

  // Iterate over the linked list to find the last glthread_t structure
  ITERATE_GLTHREAD_BEGIN(base_glthread, glthreadptr) {
    prev_glthreadptr = glthreadptr;
  }
  ITERATE_GLTHREAD_END(base_glthread, glthreadptr);

  // If a previous glthread_t structure is found, add the new_glthread after it
  if (prev_glthreadptr)
    glthread_add_next(prev_glthreadptr, new_glthread);
  // If no previous glthread_t structure is found, add the new_glthread as the
  // first element
  else
    glthread_add_next(base_glthread, new_glthread);
}

unsigned int get_glthread_list_count(glthread_t *base_glthread) {
  unsigned int count = 0;         // Initialize count to 0
  glthread_t *glthreadptr = NULL; // Initialize glthreadptr to NULL

  // Iterate over the linked list and increment count for each glthread_t
  // structure
  ITERATE_GLTHREAD_BEGIN(base_glthread, glthreadptr) {
    count++; // Increment count
  }
  ITERATE_GLTHREAD_END(base_glthread, glthreadptr);

  return count; // Return the count of glthread_t structures
}

void glthread_priority_insert(glthread_t *base_glthread, glthread_t *glthread,
                              int (*comp_fn)(void *, void *), int offset) {
  glthread_t *curr = NULL, *prev = NULL; // Initialize pointers to current and
  // previous glthread_t structures

  init_glthread(glthread); // Initialize the glthread

  if (IS_GLTHREAD_LIST_EMPTY(base_glthread)) { // Check if the list is empty
    glthread_add_next(
        base_glthread,
        glthread); // Add the glthread as the only node in the list
    return;
  }

  /**< Only one node */
  if (base_glthread->right &&
      !base_glthread->right->right) { // Check if there is only one node in the list
    if (comp_fn(
            GLTHREAD_GET_USER_DATA_FROM_OFFSET(base_glthread->right, offset),
            GLTHREAD_GET_USER_DATA_FROM_OFFSET(glthread, offset)) == -1) {
      glthread_add_next(base_glthread->right,
                        glthread); // Add the glthread after the existing node
    } else {
      glthread_add_next(
          base_glthread,
          glthread); // Add the glthread as the new head of the list
    }
    return;
  }

  if (comp_fn(GLTHREAD_GET_USER_DATA_FROM_OFFSET(glthread, offset),
              GLTHREAD_GET_USER_DATA_FROM_OFFSET(base_glthread->right,
                                                 offset)) == -1) {
    glthread_add_next(base_glthread,
                      glthread); // Add the glthread as the new head of the list
    return;
  }

  ITERATE_GLTHREAD_BEGIN(base_glthread, curr) { // Iterate over the linked list

    if (comp_fn(GLTHREAD_GET_USER_DATA_FROM_OFFSET(glthread, offset),
                GLTHREAD_GET_USER_DATA_FROM_OFFSET(curr, offset)) !=
        -1) {      // Check the priority of the current glthread
      prev = curr; // Update the previous pointer
      continue;    // Continue to the next iteration
    }

    glthread_add_next(curr,
                      glthread); // Add the glthread before the current one
    return;
  }
  ITERATE_GLTHREAD_END(base_glthread, curr);

  /**< Add in the end */
  glthread_add_next(prev, glthread); // Add the glthread at the end of the list
}

void *glthread_search(glthread_t *base_glthread,
                       void *(*thread_to_struct_fn)(glthread_t *), void *key,
                       int (*comparison_fn)(void *, void *)) {
  glthread_t *curr = NULL;

  ITERATE_GLTHREAD_BEGIN(base_glthread, curr) { // Iterate over the linked list
    void *current_user_data = thread_to_struct_fn(
        curr); // Convert the current glthread_t to user-defined structure
    if (comparison_fn(current_user_data, key) ==
        0) { // Compare the user-defined structure with the key
      return current_user_data; // Return the user-defined structure if a match
                                // is found
    }
  }
  ITERATE_GLTHREAD_END(base_glthread, curr);

  return NULL; // Return NULL if the key is not found in the linked list
}
