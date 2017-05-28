//////////////////////////////////////////////////////////////////////////////
// rbt.h                                                                    //
//////////////////////////////////////////////////////////////////////////////
// rbt.h contains declarations of functions for creating and performing
// operations on Red-Black Trees as well as a definition of the RBT data type.
// It is based on Professor Lyn Turbak's "Red-Black Trees" handout, containing
// specifications for RBT's and their operations, which can be found at the
// following link:
// http://cs.wellesley.edu/~cs231/spring01/red-black.pdf

// The implementation of RBT_pretty_print is based heavily on that of
// "VasyaNovikov" at the following link:
// http://stackoverflow.com/questions/4965335/how-to-print-binary-tree-diagram

// Conditional Compilation:
// To enable, compile with "-D flag_name".
//
//   - ALLOC_TRACK      (mildly slows performance)
//     + Enable the RBT_num_nodes() function.
//
//   - REP_OK           (severely slows performance)
//     + Apply an internal representation invariant check to every RBT argument
//       and return value (at runtime). Raises SIGABRT if violated.

#ifndef RBT_H
#define RBT_H

#include <stdbool.h>

#define RED   1 // The RED color for an RBT node.
#define BLACK 0 // The BLACK color for an RBT node.

// A leaf node. Leaf nodes are NULL pointers and are, by definition, BLACK.
#define BLACK_LEAF NULL

// Red-Black Tree data type.
// Every RBT node has a data block for dynamically allocating memory.
typedef struct RBT {
    struct RBT *left;  // pointer to the left child
    struct RBT *right; // pointer to the right child
    struct RBT *next;  // pointer to the next node with the same capacity
    unsigned int capacity  : 30; // number of bytes in the block (excluding the header)
    unsigned int prev_dist : 30; // distance (in bytes) to the previous header
    unsigned int in_use    :  1; // usage status of a block
    unsigned int color     :  2; // color of the RBT node (RED / BLACK)
}__attribute__((packed)) *RBT;

// RBT_new returns a new RBT with the given `root` and initialized with
// `capacity` (and no children).
// This is equivalent to RBT_add(NULL, root, capacity).
RBT RBT_new(RBT root, unsigned int capacity);

// RBT_add inserts a new RBT node (pointed to by `node`) into RBT `root` and
// initializes it with the given capacity. All other fields in `node` are
// initialized to 0, NULL, or false (except for color, which should not be
// modified). The returned RBT points to the new root (which may or may not be
// the original root). If `root` is NULL, then it is treated as an empty tree,
// and `node` is the new root. If `node` is NULL, then `root` is returned.
//
// NOTE: Because all fields are initialized to 0, any existing field values in
// `node` are overwritten. For this reason, before calling RBT_add, ALWAYS SAVE
// any value that should be preserved. Then reassign it to `node->...`.
// NOTE: to avoid memory leaks ALWAYS assign the result to the provided root.
//   e.g. tree = RBT_add(tree, ...);
RBT RBT_add(RBT root, RBT node, unsigned int capacity);

// RBT_remove_at_least removes the smallest RBT node whose capacity is at least
// that requested, storing a pointer to it in the variable `removed`. The
// returned RBT points to the new root. If no such node exists, then the
// original root is returned and a NULL pointer is stored in `removed`. If the
// `removed` argument is NULL then the original root is returned (without
// modifying the tree).
//
// NOTE: to avoid memory leaks ALWAYS assign the result to the provided root.
//   e.g. tree = RBT_remove_at_least(tree, ..., ...);
// NOTE: the returned root is NULL if a node is removed from a singleton RBT.
RBT RBT_remove_at_least(RBT root, unsigned int capacity, RBT *removed);

// RBT_remove_node removes the given node from the RBT with the given root and
// stores it in the RBT variable `removed`. The new root is returned. If `node`
// cannot be found in the tree, then the original root is returned and a NULL
// pointer is stored in `removed`.
//
// If `removed` is NULL, `root` is returned.
// If `node` is NULL, `root` is returned and `removed` is set to NULL.
//
// NOTE: to avoid memory leaks ALWAYS assign the result to the provided root.
//   e.g. tree = RBT_remove_node(tree, ..., ..., ...);
RBT RBT_remove_node(RBT root, RBT node, RBT *removed);

// RBT_height returns the height of the RBT.
// Tree height is defined as the *length* of the longest path from the root to
// any non-leaf node. This is the same as the number of non-root, non-leaf
// nodes on this path.
//
// NOTE: "Leaf" nodes are NULL pointers (see BLACK_LEAF above).
int RBT_height(RBT root);

// RBT_black_height returns the black-height of the RBT.
// Black-height is defined as the number of black nodes from a given node
// down to any descendant leaf (not including the initial node but including
// leaves).
//
// NOTE: "Leaf" nodes are NULL pointers (see BLACK_LEAF above).
int RBT_black_height(RBT root);

// RBT_print_node prints an RBT node and its metadata.
void RBT_print_node(RBT root);

// RBT_list_print prints all values in the linked list within an RBT node.
void RBT_list_print(RBT head);

// RBT_in_order_print prints all values in an RBT in order (including
// duplicates).
void RBT_in_order_print(RBT root);

// RBT_pretty_print prints a "pretty" representation of an RBT.
// The output has the same format as the "tree" command line program.
void RBT_pretty_print(RBT root);

// Returns the number of nodes currently allocated (when compiled with -D
// ALLOC_TRACK). Returns 0 otherwise.
unsigned int RBT_num_nodes();

//////////////////////////////////////////////////////////////////////////////
// Functions for use with malloc, calloc, etc.                              //
//////////////////////////////////////////////////////////////////////////////
// The following functions are for freeing RBTs that were allocated completely
// using standard library dynamic allocators and whose blocks can thus be freed
// with calls to free. Behavior is undefined if the nodes were either not
// allocated with one such allocator or if the nodes were already freed.

// Free all RBTs in a linked list of RBTs starting with `head`.
// Assumes `head` and all subsequent RBTs in the list have no children.
void RBT_free_list(RBT head);

// Free all nodes in a RBT with the given root, including it's children and any
// linked-lists within each node.
void RBT_free(RBT root);

#endif /* RBT_H */

