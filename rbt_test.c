#include "rbt.h"

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <signal.h>

#define ERROR "\033[31;1mError: \033[0m"
#define DOUBLE_WORD_SIZE (sizeof(long))

void bst_tests() {
    ////////////////////////////////////////////////////////////////////
    // Insert and free 100 of the same value into the root's linked list
    ////////////////////////////////////////////////////////////////////
    RBT tree = RBT_new(malloc(sizeof(struct RBT)), 100);
    for (unsigned int i = 0; i < 100; i++) {
        if((tree = RBT_add(tree, malloc(sizeof(struct RBT)), 100)) == NULL) {
            printf("A node should have been added\n");
            exit(1);
        }
    }
    #ifdef ALLOC_TRACK
    unsigned int num_allocated = RBT_num_nodes();
    printf("%d nodes allocated", num_allocated);
    #endif // ALLOC_TRACK

    printf("\n");
    RBT_free(tree);
    tree = NULL;
    #ifdef ALLOC_TRACK
    printf("%d nodes freed\n", num_allocated - RBT_num_nodes());
    #endif // ALLOC_TRACK

    ////////////////////////////////////////////////
    // Insert and free 100 random values in [0, 100)
    ////////////////////////////////////////////////
    tree = RBT_new(malloc(sizeof(struct RBT)), 100);
    for (unsigned int i = 0; i < 100; i++) {
        int next_val = abs(rand() % 100);
        if((tree = RBT_add(tree, malloc(sizeof(struct RBT)), next_val)) == NULL) {
            printf("A node should have been added\n");
            exit(1);
        }
    }
    #ifdef ALLOC_TRACK
    num_allocated = RBT_num_nodes();
    printf("\n%d nodes allocated\n", num_allocated);
    #endif // ALLOC_TRACK

    RBT_free(tree);
    tree = NULL;
    #ifdef ALLOC_TRACK
    printf("%d nodes freed\n", num_allocated - RBT_num_nodes());
    #endif // ALLOC_TRACK

    ////////////////////////////////////////////////////
    // Insert and free 100,000 random values in [0, 100)
    ////////////////////////////////////////////////////
    tree = RBT_new(malloc(sizeof(struct RBT)), 100);
    for (unsigned int i = 0; i < 100000; i++) {
        int next_val = abs(rand() % 100);
        if((tree = RBT_add(tree, malloc(sizeof(struct RBT)), next_val)) == NULL) {
            printf("A node should have been added\n");
            exit(1);
        }
    }
    #ifdef ALLOC_TRACK
    num_allocated = RBT_num_nodes();
    printf("\n%d nodes allocated\n", num_allocated);
    #endif // ALLOC_TRACK

    RBT_free(tree);
    tree = NULL;
    #ifdef ALLOC_TRACK
    printf("%d nodes freed\n", num_allocated - RBT_num_nodes());
    #endif // ALLOC_TRACK

    ////////////////////////////////////////////////////
    // Test removal of nodes from the root's linked list
    ////////////////////////////////////////////////////
    tree = RBT_new(malloc(sizeof(struct RBT)), 10);
    for (unsigned int i = 0; i < 9; i++) {
        if((tree = RBT_add(tree, malloc(sizeof(struct RBT)), 10)) == NULL) {
            printf("A node should have been added\n");
            exit(1);
        }
    }
    #ifdef ALLOC_TRACK
    num_allocated = RBT_num_nodes();
    printf("\n%d nodes allocated\n", num_allocated);
    #endif // ALLOC_TRACK

    RBT removed;
    for (unsigned int i = 0; i < 10; i++) {
        tree = RBT_remove_at_least(tree, 1, &removed);
        if (removed == NULL) {
            printf("A node should have been removed\n");
            exit(1);
        }
        #ifdef ALLOC_TRACK
        /*printf("node: %p capacity: %d requested: 1\n", removed, removed->capacity);*/
        #endif
        RBT_free(removed);
    }
    RBT_remove_at_least(tree, 1, &removed);
    if (removed != NULL) {
        printf("Tree should not contain any more nodes\n");
        exit(1);
    }
    if (tree != NULL) {
        printf("Tree should not contain any more nodes\n");
        exit(1);
    }
    #ifdef ALLOC_TRACK
    printf("%d nodes removed\n", num_allocated - RBT_num_nodes());
    #endif // ALLOC_TRACK

    ///////////////////////////////////////////////////////
    // Test removal of nodes from various parts of the tree
    ///////////////////////////////////////////////////////
    tree = RBT_new(malloc(sizeof(struct RBT)), 10);
    if((tree = RBT_add(tree, malloc(sizeof(struct RBT)), 13)) == NULL) {
        printf("A node should have been added\n");
        exit(1);
    }
    if((tree = RBT_add(tree, malloc(sizeof(struct RBT)), 5)) == NULL) {
        printf("A node should have been added\n");
        exit(1);
    }
    if((tree = RBT_add(tree, malloc(sizeof(struct RBT)), 7)) == NULL) {
        printf("A node should have been added\n");
        exit(1);
    }
    for (unsigned int i = 6; i < 15; i++) {
        if((tree = RBT_add(tree, malloc(sizeof(struct RBT)), i)) == NULL) {
            printf("A node should have been added\n");
            exit(1);
        }
    }
    #ifdef ALLOC_TRACK
    num_allocated = RBT_num_nodes();
    printf("\n%d nodes allocated\n", num_allocated);
    #endif // ALLOC_TRACK

    for (unsigned int i = 0; i < 13; i++) {
        tree = RBT_remove_at_least(tree, 1, &removed);
        if (removed == NULL) {
            printf("A node should have been removed\n");
            exit(1);
        }
        #ifdef ALLOC_TRACK
        /*printf("node: %p capacity: %d requested: 1\n", removed, removed->capacity);*/
        #endif
        RBT_free(removed);
    }
    RBT_remove_at_least(tree, 1, &removed);
    if (removed != NULL) {
        printf("Tree should not contain any more nodes\n");
        exit(1);
    }
    if (tree != NULL) {
        printf("Tree should not contain any more nodes\n");
        exit(1);
    }
    #ifdef ALLOC_TRACK
    printf("%d nodes removed\n", num_allocated - RBT_num_nodes());
    #endif // ALLOC_TRACK

    ////////////////////////////////////////////////////////////////
    // Test insertion and (random) removal of random 1,000,000 items
    ////////////////////////////////////////////////////////////////
    tree = RBT_new(malloc(sizeof(struct RBT)), 100);
    int num_nodes = 1000000; // 1,000,000
    for (unsigned int i = 0; i < num_nodes; i++) {
        int next_val = abs(rand() % 100);
        if((tree = RBT_add(tree, malloc(sizeof(struct RBT)), next_val)) == NULL) {
            printf("A node should have been added\n");
            exit(1);
        }
    }
    #ifdef ALLOC_TRACK
    num_allocated = RBT_num_nodes();
    printf("\n%d nodes allocated\n", num_allocated);
    #endif // ALLOC_TRACK

    while (RBT_black_height(tree) != 0) {
        RBT removed = NULL;
        tree = RBT_remove_at_least(tree, rand() % 100, &removed);
        if (removed != NULL) {
            if (removed->left != NULL) {
                raise(SIGABRT);
            }
            if (removed->right != NULL) {
                raise(SIGABRT);
            }
            if (removed->next != NULL) {
                raise(SIGABRT);
            }
            if (removed == tree) {
                raise(SIGABRT);
            }
            RBT_free(removed);
        }
    }
    removed = NULL;
    #ifdef ALLOC_TRACK
    printf("%d nodes freed\n\n", num_allocated - RBT_num_nodes());
    #endif // ALLOC_TRACK
}

int compare(const void *a, const void *b) {
    if (*(char *)a > *(char *)b) {
        return 1;
    }
    if (*(char *)a == *(char *)b) {
        return 0;
    }
    return -1;
}

/* Check that inserting "ALGORITHM" (in that order) creates the expected tree.
 * Prints the nodes using an "in-order" traversal (for manually checking
 * correctness).
 *
 *          (I)
 *         /   \
 *       G       O
 *      / \     / \
 *    (A) (H) (L) (R)
 *              \   \
 *               M   T
 *
 * Height: 3
 * Black height: 2
 * Note: (...) means the node is black). */
void rbt_insertion_test_1() {
    char algorithm[] = "ALGORITHM";
    RBT tree = RBT_new(malloc(sizeof(struct RBT)), algorithm[0]);
    for (int i = 1; i < strlen(algorithm); i++) {
        tree = RBT_add(tree, malloc(sizeof(struct RBT)), algorithm[i]);
    }
    printf("Got:      ");
    RBT_in_order_print(tree);

    qsort(algorithm, strlen(algorithm), sizeof(algorithm[0]), &compare);
    printf("\nExpected: ");
    for (int i = 0; i < strlen(algorithm); i++) {
        printf("%d ", algorithm[i]);
    }
    printf("\n");

    printf("Expected: ");
    for (int i = 0; i < strlen(algorithm); i++) {
        printf("%2c ", algorithm[i]);
    }
    printf("\n");
    int height;
    if ((height = RBT_black_height(tree)) != 2) {
        printf(ERROR "tree black-height should be 2. Got: %d\n", height);
    }
    if ((height = RBT_height(tree)) != 3) {
        printf(ERROR "tree height should be 3. Got: %d\n", height);
    }
    #ifdef REP_OK
    RBT_pretty_print(tree);
    printf("Expected:\n"
           "        (I)           \n"
           "       /   \\         \n"
           "     G       O        \n"
           "    / \\     / \\     \n"
           "  (A) (H) (L) (R)     \n"
           "            \\   \\   \n"
           "             M   T    \n\n"
           "Black height: 2       \n"
           "Note: (...) means the node is black).\n\n");
    #endif
}

/* Check that inserting "ALGORITHM" (in that order) creates the expected tree:
 *
 *          (I)
 *         /   \
 *       G       O
 *      / \     / \
 *    (A) (H) (L) (R)
 *              \   \
 *               M   T
 *
 * Black height: 2
 * Note: (...) means the node is black). */
void rbt_insertion_test_2() {
    RBT tree = RBT_new(malloc(sizeof(struct RBT)), 'A');
    if (!(tree->capacity == 'A' && tree->color == BLACK &&
                tree->left == BLACK_LEAF && tree->right == BLACK_LEAF &&
                tree->next == NULL)) {
        printf(ERROR "A inserted improperly\n");
        return;
    }

    tree = RBT_add(tree, malloc(sizeof(struct RBT)), 'L');
    if (!(tree->capacity == 'A' && tree->color == BLACK &&
                tree->left == BLACK_LEAF && tree->right != BLACK_LEAF &&
                tree->next == NULL)) {
        printf(ERROR "L inserted improperly 1\n");
        return;
    }
    if (!(tree->right->capacity == 'L' && tree->right->color == RED &&
                tree->right->left == BLACK_LEAF && tree->right->right == BLACK_LEAF &&
                tree->right->next == NULL)) {
        printf(ERROR "L inserted improperly 2\n");
        return;
    }

    tree = RBT_add(tree, malloc(sizeof(struct RBT)), 'G');
    if (!(tree->capacity == 'G' && tree->color == BLACK &&
                tree->left != BLACK_LEAF && tree->right != BLACK_LEAF &&
                tree->next == NULL)) {
        printf(ERROR "G inserted improperly 1\n");
        return;
    }
    if (!(tree->left->capacity == 'A' && tree->left->color == RED &&
                tree->left->left == BLACK_LEAF && tree->left->right == BLACK_LEAF &&
                tree->left->next == NULL)) {
        printf(ERROR "G inserted improperly 2\n");
        return;
    }
    if (!(tree->right->capacity == 'L' && tree->right->color == RED &&
                tree->right->left == BLACK_LEAF && tree->right->right == BLACK_LEAF &&
                tree->right->next == NULL)) {
        printf(ERROR "G inserted improperly 3\n");
        return;
    }

    tree = RBT_add(tree, malloc(sizeof(struct RBT)), 'O');
    if (!(tree->capacity == 'G' && tree->color == BLACK &&
                tree->left != BLACK_LEAF && tree->right != BLACK_LEAF &&
                tree->next == NULL)) {
        printf(ERROR "O inserted improperly 1\n");
        return;
    }
    if (!(tree->left->capacity == 'A' && tree->left->color == BLACK &&
                tree->left->left == BLACK_LEAF && tree->left->right == BLACK_LEAF &&
                tree->left->next == NULL)) {
        printf(ERROR "O inserted improperly 2\n");
        return;
    }
    if (!(tree->right->capacity == 'L' && tree->right->color == BLACK &&
                tree->right->left == BLACK_LEAF && tree->right->right != BLACK_LEAF &&
                tree->right->next == NULL)) {
        printf(ERROR "O inserted improperly 3\n");
        return;
    }
    if (!(tree->right->right->capacity == 'O' && tree->right->right->color == RED &&
                tree->right->right->left == BLACK_LEAF &&
                tree->right->right->right == BLACK_LEAF &&
                tree->right->right->next == NULL)) {
        printf(ERROR "O inserted improperly 4\n");
        return;
    }

    tree = RBT_add(tree, malloc(sizeof(struct RBT)), 'R');
    if (!(tree->capacity == 'G' && tree->color == BLACK &&
                tree->left != BLACK_LEAF && tree->right != BLACK_LEAF &&
                tree->next == NULL)) {
        printf(ERROR "R inserted improperly 1\n");
        return;
    }
    if (!(tree->left->capacity == 'A' && tree->left->color == BLACK &&
                tree->left->left == BLACK_LEAF && tree->left->right == BLACK_LEAF &&
                tree->left->next == NULL)) {
        printf(ERROR "R inserted improperly 2\n");
        return;
    }
    if (!(tree->right->capacity == 'O' && tree->right->color == BLACK &&
                tree->right->left != BLACK_LEAF && tree->right->right != BLACK_LEAF &&
                tree->right->next == NULL)) {
        printf(ERROR "R inserted improperly 3\n");
        return;
    }
    if (!(tree->right->right->capacity == 'R' && tree->right->right->color == RED &&
                tree->right->right->left == BLACK_LEAF &&
                tree->right->right->right == BLACK_LEAF &&
                tree->right->right->next == NULL)) {
        printf(ERROR "R inserted improperly 4\n");
        return;
    }
    if (!(tree->right->left->capacity == 'L' && tree->right->left->color == RED &&
                tree->right->left->left == BLACK_LEAF &&
                tree->right->left->right == BLACK_LEAF &&
                tree->right->left->next == NULL)) {
        printf(ERROR "R inserted improperly 5\n");
        return;
    }

    tree = RBT_add(tree, malloc(sizeof(struct RBT)), 'I');
    if (!(tree->capacity == 'G' && tree->color == BLACK &&
                tree->left != BLACK_LEAF && tree->right != BLACK_LEAF &&
                tree->next == NULL)) {
        printf(ERROR "I inserted improperly 1\n");
        return;
    }
    if (!(tree->left->capacity == 'A' && tree->left->color == BLACK &&
                tree->left->left == BLACK_LEAF && tree->left->right == BLACK_LEAF &&
                tree->left->next == NULL)) {
        printf(ERROR "I inserted improperly 2\n");
        return;
    }
    if (!(tree->right->capacity == 'O' && tree->right->color == RED &&
                tree->right->left != BLACK_LEAF && tree->right->right != BLACK_LEAF &&
                tree->right->next == NULL)) {
        printf(ERROR "I inserted improperly 3\n");
        return;
    }
    if (!(tree->right->right->capacity == 'R' && tree->right->right->color == BLACK &&
                tree->right->right->left == BLACK_LEAF &&
                tree->right->right->right == BLACK_LEAF &&
                tree->right->right->next == NULL)) {
        printf(ERROR "I inserted improperly 4\n");
        return;
    }
    if (!(tree->right->left->capacity == 'L' && tree->right->left->color == BLACK &&
                tree->right->left->left != BLACK_LEAF &&
                tree->right->left->right == BLACK_LEAF &&
                tree->right->left->next == NULL)) {
        printf(ERROR "I inserted improperly 5\n");
        return;
    }
    if (!(tree->right->left->left->capacity == 'I' &&
                tree->right->left->left->color == RED &&
                tree->right->left->left->left == BLACK_LEAF &&
                tree->right->left->left->right == BLACK_LEAF &&
                tree->right->left->left->next == NULL)) {
        printf(ERROR "I inserted improperly 6\n");
        return;
    }

    tree = RBT_add(tree, malloc(sizeof(struct RBT)), 'T');
    if (!(tree->capacity == 'G' && tree->color == BLACK &&
                tree->left != BLACK_LEAF && tree->right != BLACK_LEAF &&
                tree->next == NULL)) {
        printf(ERROR "T inserted improperly 1\n");
        return;
    }
    if (!(tree->left->capacity == 'A' && tree->left->color == BLACK &&
                tree->left->left == BLACK_LEAF && tree->left->right == BLACK_LEAF &&
                tree->left->next == NULL)) {
        printf(ERROR "T inserted improperly 2\n");
        return;
    }
    if (!(tree->right->capacity == 'O' && tree->right->color == RED &&
                tree->right->left != BLACK_LEAF && tree->right->right != BLACK_LEAF &&
                tree->right->next == NULL)) {
        printf(ERROR "T inserted improperly 3\n");
        return;
    }
    if (!(tree->right->right->capacity == 'R' && tree->right->right->color == BLACK &&
                tree->right->right->left == BLACK_LEAF &&
                tree->right->right->right != BLACK_LEAF &&
                tree->right->right->next == NULL)) {
        printf(ERROR "T inserted improperly 4\n");
        return;
    }
    if (!(tree->right->left->capacity == 'L' && tree->right->left->color == BLACK &&
                tree->right->left->left != BLACK_LEAF &&
                tree->right->left->right == BLACK_LEAF &&
                tree->right->left->next == NULL)) {
        printf(ERROR "T inserted improperly 5\n");
        return;
    }
    if (!(tree->right->left->left->capacity == 'I' &&
                tree->right->left->left->color == RED &&
                tree->right->left->left->left == BLACK_LEAF &&
                tree->right->left->left->right == BLACK_LEAF &&
                tree->right->left->left->next == NULL)) {
        printf(ERROR "T inserted improperly 6\n");
        return;
    }
    if (!(tree->right->right->right->capacity == 'T' &&
                tree->right->right->right->color == RED &&
                tree->right->right->right->left == BLACK_LEAF &&
                tree->right->right->right->right == BLACK_LEAF &&
                tree->right->right->right->next == NULL)) {
        printf(ERROR "T inserted improperly 7\n");
        return;
    }


    tree = RBT_add(tree, malloc(sizeof(struct RBT)),  'H');
    if (!(tree->capacity == 'G' && tree->color == BLACK &&
                tree->left != BLACK_LEAF && tree->right != BLACK_LEAF &&
                tree->next == NULL)) {
        printf(ERROR "H inserted improperly 1\n");
        return;
    }
    if (!(tree->left->capacity == 'A' && tree->left->color == BLACK &&
                tree->left->left == BLACK_LEAF && tree->left->right == BLACK_LEAF &&
                tree->left->next == NULL)) {
        printf(ERROR "H inserted improperly 2\n");
        return;
    }
    if (!(tree->right->capacity == 'O' && tree->right->color == RED &&
                tree->right->left != BLACK_LEAF && tree->right->right != BLACK_LEAF &&
                tree->right->next == NULL)) {
        printf(ERROR "H inserted improperly 3\n");
        return;
    }
    if (!(tree->right->right->capacity == 'R' && tree->right->right->color == BLACK &&
                tree->right->right->left == BLACK_LEAF &&
                tree->right->right->right != BLACK_LEAF &&
                tree->right->right->next == NULL)) {
        printf(ERROR "H inserted improperly 4\n");
        return;
    }
    if (!(tree->right->left->capacity == 'I' && tree->right->left->color == BLACK &&
                tree->right->left->left != BLACK_LEAF &&
                tree->right->left->right != BLACK_LEAF &&
                tree->right->left->next == NULL)) {
        printf(ERROR "H inserted improperly 5\n");
        return;
    }
    if (!(tree->right->left->left->capacity == 'H' &&
                tree->right->left->left->color == RED &&
                tree->right->left->left->left == BLACK_LEAF &&
                tree->right->left->left->right == BLACK_LEAF &&
                tree->right->left->left->next == NULL)) {
        printf(ERROR "H inserted improperly 6\n");
        return;
    }
    if (!(tree->right->right->right->capacity == 'T' &&
                tree->right->right->right->color == RED &&
                tree->right->right->right->left == BLACK_LEAF &&
                tree->right->right->right->right == BLACK_LEAF &&
                tree->right->right->right->next == NULL)) {
        printf(ERROR "H inserted improperly 7\n");
        return;
    }
    if (!(tree->right->left->right->capacity == 'L' &&
                tree->right->left->right->color == RED &&
                tree->right->left->right->left == BLACK_LEAF &&
                tree->right->left->right->right == BLACK_LEAF &&
                tree->right->left->right->next == NULL)) {
        printf(ERROR "H inserted improperly 8\n");
        return;
    }

    tree = RBT_add(tree, malloc(sizeof(struct RBT)),  'M');
    if (!(tree->capacity == 'I' && tree->color == BLACK &&
                tree->left != BLACK_LEAF && tree->right != BLACK_LEAF &&
                tree->next == NULL)) {
        printf(ERROR "M inserted improperly 1\n");
        return;
    }
    if (!(tree->left->capacity == 'G' && tree->left->color == RED &&
                tree->left->left != BLACK_LEAF && tree->left->right != BLACK_LEAF &&
                tree->left->next == NULL)) {
        printf(ERROR "M inserted improperly 2\n");
        return;
    }
    if (!(tree->right->capacity == 'O' && tree->right->color == RED &&
                tree->right->left != BLACK_LEAF && tree->right->right != BLACK_LEAF &&
                tree->right->next == NULL)) {
        printf(ERROR "M inserted improperly 3\n");
        return;
    }
    if (!(tree->right->right->capacity == 'R' && tree->right->right->color == BLACK &&
                tree->right->right->left == BLACK_LEAF &&
                tree->right->right->right != BLACK_LEAF &&
                tree->right->right->next == NULL)) {
        printf(ERROR "M inserted improperly 4\n");
        return;
    }
    if (!(tree->right->left->capacity == 'L' && tree->right->left->color == BLACK &&
                tree->right->left->left == BLACK_LEAF &&
                tree->right->left->right != BLACK_LEAF &&
                tree->right->left->next == NULL)) {
        printf(ERROR "M inserted improperly 5\n");
        return;
    }
    if (!(tree->left->right->capacity == 'H' &&
                tree->left->right->color == BLACK &&
                tree->left->right->left == BLACK_LEAF &&
                tree->left->right->right == BLACK_LEAF &&
                tree->left->right->next == NULL)) {
        printf(ERROR "M inserted improperly 6\n");
        return;
    }
    if (!(tree->right->right->right->capacity == 'T' &&
                tree->right->right->right->color == RED &&
                tree->right->right->right->left == BLACK_LEAF &&
                tree->right->right->right->right == BLACK_LEAF &&
                tree->right->right->right->next == NULL)) {
        printf(ERROR "M inserted improperly 7\n");
        return;
    }
    if (!(tree->right->left->capacity == 'L' &&
                tree->right->left->color == BLACK &&
                tree->right->left->left == BLACK_LEAF &&
                tree->right->left->right != BLACK_LEAF &&
                tree->right->left->next == NULL)) {
        printf(ERROR "M inserted improperly 8\n");
        return;
    }
    if (!(tree->right->left->right->capacity == 'M' &&
                tree->right->left->right->color == RED &&
                tree->right->left->right->left == BLACK_LEAF &&
                tree->right->left->right->right == BLACK_LEAF &&
                tree->right->left->right->next == NULL)) {
        printf(ERROR "M inserted improperly 9\n");
        return;
    }
}

// Test operations on RBTs.
int main(void) {
    printf("struct RBT: %lu bytes (%lu double-words)\n", sizeof(struct RBT),
            sizeof(struct RBT) / DOUBLE_WORD_SIZE);

    clock_t begin = clock();
    srand(time(0));
    bst_tests();
    printf("PASSED: bst_tests\n");
    rbt_insertion_test_1();
    printf("PASSED: rbt_insertion_test_1\n");
    rbt_insertion_test_2();
    printf("PASSED: rbt_insertion_test_2\n");
    clock_t end = clock();
    double time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
    printf("\nTime elapsed: %g seconds\n", time_spent);

    return 0;
}
