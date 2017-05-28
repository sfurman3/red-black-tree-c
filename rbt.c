//////////////////////////////////////////////////////////////////////////////
// rbt.c                                                                    //
//////////////////////////////////////////////////////////////////////////////
// rbt.c contains implementations of the functions declared in rbt.h.
#include "rbt.h"

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <ctype.h>

#define DOUBLE_BLACK 2 // The DOUBLE-BLACK color for an RBT node.

#define DOUBLE_BLACK_PTR ((void *)1) // The DOUBLE-BLACK pointer for an RBT leaf.

// Text coloring macros (ANSI character escapes)
#define RBT_BOLD_BLACK "\033[34;1m"
#define RBT_BOLD_RED   "\033[31;1m"
#define RBT_BOLD_CYAN  "\033[36;1m"
#define RBT_NO_STYLE   "\033[0m"
#define RBT_ERROR      RBT_BOLD_RED "Error: " RBT_NO_STYLE

// ALLOC_TRACK is a debugging macro for tracking the number of nodes allocated
// and freed during execution (in order to detect memory leaks).
#ifdef ALLOC_TRACK
unsigned int NUM_NODES; // Current number of allocated nodes.
#endif // ALLOC_TRACK

//////////////////////////////////////////////////////////////////////////////
// Tree Height                                                              //
//////////////////////////////////////////////////////////////////////////////
int RBT_height(RBT root) {
    if (root == BLACK_LEAF) {
        return 0;
    }
    if (root->left == BLACK_LEAF && root->right == BLACK_LEAF) {
        return 0;
    }
    int left_height = 1 + RBT_height(root->left);
    int right_height = 1 + RBT_height(root->right);
    if (left_height > right_height) {
        return left_height;
    }
    return right_height;
}

int RBT_black_height(RBT root) {
    if (root == BLACK_LEAF) {
        return 0;
    }
    if (root->left == BLACK_LEAF) {
        return 1;
    }
    if (root->left->color == BLACK) {
        return 1 + RBT_black_height(root->left);
    }
    return RBT_black_height(root->left);
}

//////////////////////////////////////////////////////////////////////////////
// rep_ok: Functions for checking that an RBT satisfies the representation  //
// invariant for Red-Black Trees:                                           //
//   + The root of the tree is BLACK.                                       //
//   + If a node is RED, both children are BLACK.                           //
//   + Every path from a node down to any leaf contains the same number of  //
//     black nodes.                                                         //
//////////////////////////////////////////////////////////////////////////////
// helper: Checks that no nodes in an RBT are doubly-black. If any are
// doubly-black raises SIGABRT.
void RBT_double_blackness_ok(RBT root) {
    if (root == BLACK_LEAF) {
        return;
    }
    if (root == DOUBLE_BLACK_PTR || root->color == DOUBLE_BLACK) {
        printf(RBT_ERROR "tree should not contain doubly-black nodes\n");
        raise(SIGABRT);
    }
    RBT_double_blackness_ok(root->left);
    RBT_double_blackness_ok(root->right);
}

// helper: Raises SIGABRT if the red-red RBT invariant is violated.
void RBT_red_red_ok(RBT root) {
    if (root == BLACK_LEAF) {
        return;
    }
    if (root->color == RED) {
        if (!(root->left == BLACK_LEAF || root->left->color == BLACK)) {
            printf(RBT_ERROR "tree does not satisfy red-red invariant\n");
            raise(SIGABRT);
        }
        if (!(root->right == BLACK_LEAF || root->right->color == BLACK)) {
            printf(RBT_ERROR "tree does not satisfy red-red invariant\n");
            raise(SIGABRT);
        }
    }
    RBT_red_red_ok(root->left);
    RBT_red_red_ok(root->right);
}

// helper: Raises SIGABRT if the black-height RBT invariant is violated.
void RBT_black_height_ok(RBT root) {
    if (root == BLACK_LEAF) {
        return;
    }
    int left_height = RBT_black_height(root->left);
    int right_height = RBT_black_height(root->right);
    if (root->left == BLACK_LEAF || root->left->color == BLACK) {
        left_height++;
    }
    if (root->right == BLACK_LEAF || root->right->color == BLACK) {
        right_height++;
    }
    if (left_height != right_height) {
        printf(RBT_ERROR
                "tree does not satisfy black-height invariant.\n"
                "    black_height(tree->left):  %d\n"
                "    black_height(tree->right): %d\n",
                left_height, right_height);
        RBT_pretty_print(root);
        raise(SIGABRT);
    }
    RBT_black_height_ok(root->left);
    RBT_black_height_ok(root->right);
}

// Check the representation invariant for Red-Black Trees:
//   + The root of the tree is BLACK.
//   + If a node is RED, both children are BLACK.
//   + Every path from a node down to any leaf contains the same number of
//     black nodes.
//
// If the invariant is violated, raises SIGABRT. Otherwise, returns the
// original tree (unchanged).
//
// NOTE: Compiling with "-D REP_OK" causes RBT_rep_ok to be applied to the inputs
// and outputs of certain, externally-visible function below.
RBT RBT_rep_ok(RBT root) {
    // check the root color invariant
    if (root == BLACK_LEAF) {
        return root;
    }
    if (root == DOUBLE_BLACK_PTR || root->color == DOUBLE_BLACK) {
        printf(RBT_ERROR "root should not be doubly-black\n");
        raise(SIGABRT);
    }
    if (root->color != BLACK) {
        printf(RBT_ERROR "tree does not satisfy black root invariant\n");
        raise(SIGABRT);
    }
    // check that no nodes are doubly-black
    RBT_double_blackness_ok(root);
    // check the red-red invariant
    RBT_red_red_ok(root);
    // check black-height invariant
    RBT_black_height_ok(root);

    return root;
}

//////////////////////////////////////////////////////////////////////////////
// RBT Creation and Insertion                                               //
//////////////////////////////////////////////////////////////////////////////
// helper: eliminates the first red-red violation found between a child and
// grandchild node of the given RBT. Returns the new root (if changed).
// Otherwise, returns the original root.
// Assumes root is not NULL.
// NOTE: to avoid memory leaks ALWAYS assign the result to the provided root.
//   e.g. tree = RBT_eliminate_red_red(tree);
RBT RBT_eliminate_red_red(RBT root) {
    RBT left = root->left;
    RBT right = root->right;
    if (left != BLACK_LEAF && left->color == RED) {
        RBT left_left = left->left;
        if (left_left != BLACK_LEAF && left_left->color == RED) {
            if (right != BLACK_LEAF && right->color == RED) {
                // case 1 : RED uncle -> recolor
                left->color = BLACK;
                right->color = BLACK;
                root->color = RED;
                return root;
            }
            // case 2 : BLACK uncle -> rotate & recolor
            root->left = left->right;
            left->right = root;
            root->color = RED;
            left->color = BLACK;
            return left; // left is the new root
        }
        RBT left_right = left->right;
        if (left_right != BLACK_LEAF && left_right->color == RED) {
            if (right != BLACK_LEAF && right->color == RED) {
                // case 1 : RED uncle -> recolor
                left->color = BLACK;
                right->color = BLACK;
                root->color = RED;
                return root;
            }
            // case 2 : BLACK uncle -> rotate & recolor
            root->left = left_right->right;
            left_right->right = root;
            left->right = left_right->left;
            left_right->left = left;
            root->color = RED;
            left_right->color = BLACK;
            return left_right; // left_right is the new root
        }
    }
    if (right != BLACK_LEAF && right->color == RED) {
        RBT right_left = right->left;
        if (right_left != BLACK_LEAF && right_left->color == RED) {
            if (left != BLACK_LEAF && left->color == RED) {
                // case 1 : RED uncle -> recolor
                left->color = BLACK;
                right->color = BLACK;
                root->color = RED;
                return root;
            }
            // case 2 : BLACK uncle -> rotate & recolor
            root->right = right_left->left;
            right_left->left = root;
            right->left = right_left->right;
            right_left->right = right;
            root->color = RED;
            right_left->color = BLACK;
            return right_left; // right_left is the new root
        }
        RBT right_right = right->right;
        if (right_right != BLACK_LEAF && right_right->color == RED) {
            if (left != BLACK_LEAF && left->color == RED) {
                // case 1 : RED uncle -> recolor
                left->color = BLACK;
                right->color = BLACK;
                root->color = RED;
                return root;
            }
            // case 2 : BLACK uncle -> rotate & recolor
            root->right = right->left;
            right->left = root;
            root->color = RED;
            right->color = BLACK;
            return right; // right is the new root
        }
    }
    // neither left nor right is RED
    return root;
}

// helper: recursive part of RBT_add.
RBT RBT_add_inner(RBT root, RBT node, unsigned int capacity) {
    if (root == NULL) {
        node->capacity = capacity;
        node->prev_dist = 0;
        node->left = NULL;
        node->right = NULL;
        node->next = NULL;
        node->in_use = false;
        node->color = BLACK; // new nodes default to BLACK
        return node;
    }

    unsigned int c = root->capacity;
    if (capacity == c) { // add the new node to the linked-list
        node = RBT_add_inner(NULL, node, capacity);
        node->next = root->next;
        root->next = node;
        return root; // don't need to check for violations (linked-list)
    } else if (capacity < c) {
        RBT left = root->left;
        RBT new_left = RBT_add_inner(left, node, capacity);
        if (left == NULL) { // new_left is a newly allocated node with no children
            new_left->color = RED; // such added nodes are always RED
        }
        root->left = new_left;
    } else {
        RBT right = root->right;
        RBT new_right = RBT_add_inner(right, node, capacity);
        if (right == NULL) { // new_right is a newly allocated node with no children
            new_right->color = RED; // such added nodes are always RED
        }
        root->right = new_right;
    }

    // eliminate any red-red violations and return the new root
    return RBT_eliminate_red_red(root);
}

RBT RBT_add(RBT root, RBT node, unsigned int capacity) {
    #ifdef REP_OK
    RBT_rep_ok(root);
    #endif
    if (node == NULL) {
        return root;
    }
    RBT new_tree = RBT_add_inner(root, node, capacity);
    new_tree->color = BLACK;
    #ifdef ALLOC_TRACK
    NUM_NODES++;
    #endif // ALLOC_TRACK
    #ifdef REP_OK
    RBT_rep_ok(new_tree);
    #endif
    return new_tree;
}

RBT RBT_new(RBT root, unsigned int capacity) {
    #ifdef REP_OK
    return RBT_rep_ok(RBT_add(NULL, root, capacity));
    #else
    return RBT_add(NULL, root, capacity);
    #endif
}

//////////////////////////////////////////////////////////////////////////////
// RBT Freeing                                                              //
//////////////////////////////////////////////////////////////////////////////
void RBT_free_list(RBT head) {
    while (head != NULL) {
        RBT next = head->next;
        free(head);
        #ifdef ALLOC_TRACK
        NUM_NODES--;
        #endif // ALLOC_TRACK
        head = next;
    }
}

void RBT_free(RBT root) {
    if (root == NULL) {
        return;
    }

    RBT_free(root->left);
    RBT_free_list(root->next);
    RBT_free(root->right);

    free(root);
    #ifdef ALLOC_TRACK
    NUM_NODES--;
    #endif // ALLOC_TRACK
}

//////////////////////////////////////////////////////////////////////////////
// RBT Removal                                                              //
//////////////////////////////////////////////////////////////////////////////
// helper: Propagates a doubly-black node (generated by a node removal) up the
// tree. May result in a rotation, which absorbs the doubly-black node, or an
// upward propagation, which means the returned root will be doubly-black.
// Returns the new root.
// NOTE: to avoid memory leaks ALWAYS assign the result to the provided root.
//   e.g. tree = RBT_propagate_double_blackness(tree);
RBT RBT_propagate_double_blackness(RBT root) {
    if (root == BLACK_LEAF || root == DOUBLE_BLACK_PTR) {
        return root;
    }
    RBT left = root->left;
    RBT right = root->right;
    if (left == DOUBLE_BLACK_PTR || (left != BLACK_LEAF && left->color == DOUBLE_BLACK)) {
        // { right != NULL }
        if (right->color == RED) {
            // Case C: rotate & recolor
            // { root->color == RED } /* because of red-red invariant */
            root->right = right->left;
            right->left = root;
            root->color = RED;
            right->color = BLACK;
            // right is the new root
            right->left = RBT_propagate_double_blackness(root);
            return right;
        }
        // { right->color == BLACK }
        // blacken the doubly-black node
        if (left == DOUBLE_BLACK_PTR) {
            root->left = BLACK_LEAF;
        } else {
            left->color = BLACK;
        }
        RBT right_left = right->left;
        if (right_left != BLACK_LEAF && right_left->color == RED) {
            // Case A: rotate & recolor
            root->right = right_left->left;
            right_left->left = root;
            right->left = right_left->right;
            right_left->right = right;
            right_left->color = root->color;
            root->color = BLACK;
            // right_left is the new root
            return right_left;
        }
        RBT right_right = right->right;
        if (right_right != BLACK_LEAF && right_right->color == RED) {
            // Case A: rotate & recolor
            root->right = right_left;
            right->left = root;
            right->color = root->color;
            root->color = BLACK;
            right_right->color = BLACK;
            // right is the new root
            return right;
        }
        // Case B: propagate blackness upward
        if (root->color == BLACK) {
            root->color = DOUBLE_BLACK;
        } else {
            root->color = BLACK;
        }
        right->color = RED;
        return root;
    }
    if (right == DOUBLE_BLACK_PTR || (right != BLACK_LEAF && right->color == DOUBLE_BLACK)) {
        // { left != NULL }
        RBT left = root->left;
        if (left->color == RED) {
            // Case C: rotate & recolor
            // { root->color == RED } /* because of red-red invariant */
            root->left = left->right;
            left->right = root;
            root->color = RED;
            left->color = BLACK;
            // left is the new root
            left->right = RBT_propagate_double_blackness(left->right);
            return left;
        }
        // { left->color == BLACK }
        // blacken the doubly-black node
        if (right == DOUBLE_BLACK_PTR) {
            root->right = BLACK_LEAF;
        } else {
            right->color = BLACK;
        }
        RBT left_right = left->right;
        if (left_right != BLACK_LEAF && left_right->color == RED) {
            // Case A: rotate & recolor
            root->left = left_right->right;
            left->right->right = root;
            left->right = left_right->left;
            left_right->left = left;
            left_right->color = root->color;
            root->color = BLACK;
            // left_right is the new root
            return left_right;
        }
        RBT left_left = left->left;
        if (left_left != BLACK_LEAF && left_left->color == RED) {
            // Case A: rotate & recolor
            root->left = left_right;
            left->right = root;
            left->color = root->color;
            root->color = BLACK;
            left_left->color = BLACK;
            // left is the new root
            return left;
        }
        // Case B: propagate blackness upward
        if (root->color == BLACK) {
            root->color = DOUBLE_BLACK;
        } else {
            root->color = BLACK;
        }
        left->color = RED;
    }
    return root;
}

// helper (special-case): Propagates a doubly-black node (generated by a node
// removal) up the RBT. Used by RBT_remove_root(...) for the final removal case
// in which double blackness must be propagated up to the root's right subtree
// from the `prevswap` node.
// Assumes `current` and `prevswap` are not NULL.
// NOTE: to avoid memory leaks ALWAYS assign the result to the provided root.
//   e.g. tree->right = RBT_propagate_double_blackness_prevswap(..., tree->right);
RBT RBT_propagate_double_blackness_prevswap(RBT prevswap, RBT current) {
    if (current == prevswap) {
        return RBT_propagate_double_blackness(current);
    }
    current->left = RBT_propagate_double_blackness_prevswap(prevswap, current->left);
    return RBT_propagate_double_blackness(current);
}

// helper: Removes a root with an empty linked list (i.e. root->next == NULL)
// and stores it in `removed`. The root is detached from the RBT and a
// replacement root is returned.
// Propagates double-blackness to the root (if necessary).
// Assumes: root is not NULL and `root->next == NULL`.
// NOTE: to avoid memory leaks ALWAYS assign the result to the provided root.
//   e.g. tree = RBT_remove_empty_root(tree, ...);
RBT RBT_remove_empty_root(RBT root, RBT *removed) {
    *removed = root;
    RBT left = root->left;
    RBT right = root->right;
    if (left == NULL) {
        if (right == NULL) {            /*   (root)    */
            if (root->color == RED) {   /*   /    \    */
                return BLACK_LEAF;      /* NULL   NULL */
            }
            // { root->color == BLACK }
            return DOUBLE_BLACK_PTR;
        }
        // right is the new root
        root->right = NULL;             /*   (root)    */
        if (right->color == RED) {      /*   /    \    */
            right->color = BLACK;       /* NULL  (...) */
            return right;
        }
        right->color = DOUBLE_BLACK;
        return right;
    }
    if (right == NULL) {                /*    (root)   */
        // left is the new root         /*    /    \   */
        root->left = NULL;              /* (...)  NULL */
        if (left->color == RED) {
            left->color = BLACK;
            return left;
        }
        left->color = DOUBLE_BLACK;
        return left;
    }
    if (left->left == BLACK_LEAF &&     /*       (root)    */
        left->right == BLACK_LEAF) {    /*       /    \    */
        root->left = NULL;              /*    (lft)  (...) */
        root->right = NULL;             /*    /   \        */
        left->right = right;            /* NULL   NULL     */
        if (left->color == BLACK) {
            left->left = DOUBLE_BLACK_PTR;
        }
        left->color = root->color;
        // left is the new root
        return RBT_propagate_double_blackness(left);
    }
    // { right != NULL }
    RBT prevswap = right;
    RBT swap = prevswap->left;
    if (swap == NULL) {                 /*    (root)    */
        prevswap->left = left;          /*    /    \    */
        root->left = NULL;              /* (...)  (prv) */
        root->right = NULL;             /*        /     */
        if (prevswap->color == BLACK) { /*      NULL    */
            // blacken prevswap->right
            RBT prv_right = prevswap->right;
            if (prv_right == BLACK_LEAF) {
                prevswap->right = DOUBLE_BLACK_PTR;
            } else if (prv_right->color == RED) {
                prv_right->color = BLACK;
            } else {
                // prv_right->color == BLACK
                prv_right->color = DOUBLE_BLACK;
            }
        }
        prevswap->color = root->color;
        // prevswap is the new root
        return RBT_propagate_double_blackness(prevswap);
    }
    while (swap->left != NULL) {        /*      (root)    */
        prevswap = swap;                /*      /    \    */
        swap = swap->left;              /*   (...)  (...) */
    }                                   /*           /    */
    prevswap->left = swap->right;       /*         ...    */
    swap->left = left;                  /*         /      */
    swap->right = right;                /*      (prv)     */
    root->left = NULL;                  /*       /        */
    root->right = NULL;                 /*    (swp)       */
    if (swap->color == BLACK) {         /*    /   \       */
        // blacken prevswap's new left  /* NULL  (...)    */
        RBT prv_left = prevswap->left;
        if (prv_left == BLACK_LEAF) {
            prevswap->left = DOUBLE_BLACK_PTR;
        } else if (prv_left->color == RED) {
            prv_left->color = BLACK;
        } else {
            // prv_left->color == BLACK
            prv_left->color = DOUBLE_BLACK;
        }
    }
    swap->color = root->color;
    // propagate double-blackness up from prevswap to the right subtree
    swap->right = RBT_propagate_double_blackness_prevswap(prevswap, swap->right);
    // swap is the new root
    return RBT_propagate_double_blackness(swap);
}

// helper: remove the root node and store it in `removed`
// If root has a non-NULL linked-list, then the first element is removed and
// returned. Otherwise, root is detached from the RBT and a replacement root is
// returned.
// Propagates double-blackness to the root (if necessary).
// Assumes: root is not NULL.
RBT RBT_remove_root(RBT root, RBT *removed) {
    RBT target = root->next;
    if (target != NULL) { // root has multiple nodes with the target capacity
        // remove a node from root's linked list and store it in `removed`
        root->next = target->next;
        target->next = NULL;
        *removed = target;
        return root;
    }
    // root is the last node with the target capacity
    return RBT_remove_empty_root(root, removed);
}

// helper: recursive part of RBT_remove_at_least.
// If the returned tree contains a doubly-black node, it will always be the
// root.
RBT RBT_remove_at_least_inner(RBT root, unsigned int capacity, RBT *removed) {
    if (root == NULL) {
        *removed = NULL;
        return NULL;
    }

    unsigned int c = root->capacity;
    if (capacity == c) { // root has the target capacity
        // remove the root node and return the new root
        return RBT_remove_root(root, removed);
    } else if (capacity < c) { // root->left may have a better fitting node
        RBT newleft = RBT_remove_at_least_inner(root->left, capacity, removed);
        if (*removed == NULL) { // no nodes are a better fit than root
            // remove the root node and return the new root
            return RBT_remove_root(root, removed);
        }
        root->left = newleft;
        return RBT_propagate_double_blackness(root);
    }
    // root is too small to fit `capacity`
    RBT newright = RBT_remove_at_least_inner(root->right, capacity, removed);
    if (*removed == NULL) { // no nodes in root->right are large enough
        return root;
    }
    root->right = newright;
    return RBT_propagate_double_blackness(root);
}

RBT RBT_remove_at_least(RBT root, unsigned int capacity, RBT *removed) {
    #ifdef REP_OK
    RBT_rep_ok(root);
    #endif
    if (removed == NULL) {
        return root;
    }

    RBT newroot = RBT_remove_at_least_inner(root, capacity, removed);
    if (newroot == DOUBLE_BLACK_PTR) { // the tree is an empty DOUBLE-BLACK root
        // Unblacken the root
        newroot = BLACK_LEAF;
    } else if (newroot != BLACK_LEAF) {
        // Blacken/unblacken the root
        newroot->color = BLACK;
    }
    #ifdef REP_OK
    return RBT_rep_ok(newroot);
    #endif
    return newroot;
}

// helper: Finds a node within `root` that is physically equivalent to `node`,
// removes it, and stores it in `removed`.
// If root has a non-NULL linked-list, then the first element is removed and
// returned. Otherwise, root is detached from the RBT and a replacement root is
// returned.
// Propagates double-blackness to the root (if necessary).
// Assumes: root is not NULL.
RBT RBT_remove_node_root(RBT root, RBT node, RBT *removed) {
    if (node != root) { // `node` can only be in `root`'s linked list
        RBT prev_target = root;
        RBT target = prev_target->next;
        while (target != NULL) {
            if (node == target) {
                prev_target->next = target->next;
                target->next = NULL;
                *removed = target;
                return root;
            }
            prev_target = target;
            target = prev_target->next;
        }
        // { `node` is neither in `root` nor `root`'s linked list }
        *removed = NULL;
        return root;
    }
    // { node == root }
    RBT next = root->next;
    if (next != NULL) { // there are other blocks with the same size as `root`
        // we can replace `root` with `node`
        *removed = root;
        next->left = root->left;
        next->right = root->right;
        next->color = root->color;
        root->left = NULL;
        root->right = NULL;
        root->next = NULL;
        return next;
    }
    // we have to remove `root` from the tree
    root = RBT_remove_empty_root(root, removed);
    return root;
}

// helper: recursive part of RBT_remove_node.
// If the returned tree contains a doubly-black node, it will always be the
// root.
RBT RBT_remove_node_inner(RBT root, RBT node, unsigned int capacity, RBT *removed) {
    if (root == NULL) {
        *removed = NULL;
        return NULL;
    }

    unsigned int c = root->capacity;
    if (capacity == c) { // root has the target capacity
        // remove the root node and return the new root
        return RBT_remove_node_root(root, node, removed);
    } else if (capacity < c) { // root->left may have the target capacity
        root->left = RBT_remove_node_inner(root->left, node, capacity, removed);
        return RBT_propagate_double_blackness(root);
    }
    // root->right may have the target capacity
    root->right = RBT_remove_node_inner(root->right, node, capacity, removed);
    return RBT_propagate_double_blackness(root);
}

RBT RBT_remove_node(RBT root, RBT node, RBT *removed) {
    #ifdef REP_OK
    RBT_rep_ok(root);
    #endif
    if (removed == NULL) {
        return root;
    }
    if (node == NULL) {
        *removed = NULL;
        return root;
    }

    RBT newroot = RBT_remove_node_inner(root, node, node->capacity, removed);
    if (newroot == DOUBLE_BLACK_PTR) { // the tree is an empty DOUBLE-BLACK root
        // Unblacken the root
        newroot = BLACK_LEAF;
    } else if (newroot != BLACK_LEAF) {
        // Blacken/unblacken the root
        newroot->color = BLACK;
    }
    #ifdef REP_OK
    return RBT_rep_ok(newroot);
    #endif
    return newroot;
}

//////////////////////////////////////////////////////////////////////////////
// RBT Printing                                                             //
//////////////////////////////////////////////////////////////////////////////
void RBT_list_print(RBT head) {
    while (head != NULL) {
        printf("%u ", head->capacity);
        head = head->next;
    }
}

void RBT_in_order_print(RBT root) {
    if (root == NULL) {
        return;
    }
    RBT_in_order_print(root->left);
    printf("%u ", root->capacity);
    RBT_list_print(root->next);
    RBT_in_order_print(root->right);
}

// helper: Print a given node and its metadata.
void RBT_print_node(RBT root) {
    switch (root->color) {
        case BLACK: printf(RBT_BOLD_BLACK); break;
        case RED: printf(RBT_BOLD_RED); break;
        default: printf(RBT_BOLD_RED); break;
    }
    printf("%d", root->capacity);
    if (isalnum(root->capacity)) {
        printf(" (%c)", root->capacity);
    }
    printf(RBT_NO_STYLE);
    // print the black-height
    printf(" (bh: %d)", RBT_black_height(root));
    printf("\n");
}

// helper: recursive part of RBT_pretty_print.
// Print any subtrees within the given tree.
//
// NOTE: The implementation below is based heavily on that of "VasyaNovikov" at
// the following link:
// http://stackoverflow.com/questions/4965335/how-to-print-binary-tree-diagram
void RBT_pretty_print_inner(RBT root, char *prefix, bool isTail) {
    if (root == NULL) {
        return;
    }
    if (root == DOUBLE_BLACK_PTR) {
        printf("%s", prefix);
        printf("%s", isTail ? " └── " : " ├── ");
        printf(RBT_BOLD_CYAN "uh-oh: double-black leaf\n" RBT_NO_STYLE);
        return;
    }
    // Print the root node
    printf("%s", prefix);
    printf("%s", isTail ? " └── " : " ├── ");
    RBT_print_node(root);

    // Print any subtrees
    char *str = NULL;
    int string_length = snprintf(NULL, 0, "%s%s", prefix, isTail ? "     " : " │   ");
    if (string_length > 0) {
        str = malloc(string_length + 1);
        snprintf(str, string_length, "%s%s", prefix, isTail ? "     " : " │   ");
    }
    if (str != NULL) {
        if (root->right == NULL) {
            RBT_pretty_print_inner(root->left, str, true);
        } else if (root->left == NULL) {
            RBT_pretty_print_inner(root->right, str, true);
        } else {
            RBT_pretty_print_inner(root->right, str, false);
            RBT_pretty_print_inner(root->left, str, true);
        }
    } else {
        printf(RBT_ERROR "memory exceeded. Cannot display tree.");
    }
    free(str);
}

// NOTE: The implementation below is based heavily on that of "VasyaNovikov" at
// the following link:
// http://stackoverflow.com/questions/4965335/how-to-print-binary-tree-diagram
void RBT_pretty_print(RBT root) {
    if (root == NULL) {
        return;
    }
    // Print the root node
    printf(" ");
    RBT_print_node(root);

    // Print any subtrees
    if (root->right == NULL) {
        RBT_pretty_print_inner(root->left, "", true);
    } else if (root->left == NULL) {
        RBT_pretty_print_inner(root->right, "", true);
    } else {
        RBT_pretty_print_inner(root->right, "", false);
        RBT_pretty_print_inner(root->left, "", true);
    }
}

//////////////////////////////////////////////////////////////////////////////
// RBT Allocation Tracking
//////////////////////////////////////////////////////////////////////////////
unsigned int RBT_num_nodes() {
    #ifdef ALLOC_TRACK
    return NUM_NODES;
    #endif // ALLOC_TRACK
    return 0;
}
