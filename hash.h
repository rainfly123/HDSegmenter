#include<stdio.h>
#include <stdlib.h>
#include <stdint.h>

/**
* @code by xiechc
* @code date 2013/04/17
*/

struct hash_node
{
 uint64_t  _value;
 uint32_t _key;
};

void hash_initial(struct hash_node** _table_p);

void hash_insert(struct hash_node** _table_p, uint32_t key, uint64_t value);

uint64_t hash_search(struct hash_node** _table_p, uint32_t key);

void hash_destory(struct hash_node** _table_p);
