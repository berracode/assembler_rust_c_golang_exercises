#ifndef KTHASH_TABLE_H
#define KTHASH_TABLE_H
#include <stdlib.h>


typedef struct ktprefix_code_t {
    char *character;
    char *bits;
} ktprefix_code_t;

typedef struct kthash_table_t{
    ktprefix_code_t *prefix_codes;
    size_t size;
} kthash_table_t;

kthash_table_t* create_prefix_table(size_t initial_capacity);

void insert_prefix_code(kthash_table_t *table, char *character, char *bits);

void free_prefix_table(kthash_table_t *table);


#endif