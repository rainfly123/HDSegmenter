#include<stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "hash.h"
/**
* @code by xiechc
* @code date 2013/04/17
*/

#define HASHTABLE_SIZE 100


uint8_t hash_func(uint32_t src)
{ 
    return (src % HASHTABLE_SIZE);
}

void hash_initial(struct hash_node** _table_p)
{
 int size = sizeof(struct hash_node) ;
 *_table_p = (struct hash_node*) calloc(HASHTABLE_SIZE, size);
}

void hash_insert(struct hash_node** _table_p, uint32_t key, uint64_t value)
{
    int h = hash_func(key);
    struct hash_node* _table = *_table_p;
    _table[h]._value = value;
    return;
}

uint64_t hash_search(struct hash_node** _table_p, uint32_t key)
{
    int h = hash_func(key);
    struct hash_node* _table = *_table_p;
    return _table[h]._value;
}

void hash_destory(struct hash_node** _table_p)
{
    free(*_table_p);
}
 
/**
* test program!
*/
#ifdef HDEBUG
int main()
{
 struct hash_node* n;
 hash_initial(&n);

 hash_insert(&n,1,1);
 hash_insert(&n,3,3);
 
 uint64_t _value = hash_search(&n, 1);
 printf("value:=%d", _value);
 _value = hash_search(&n, 3);
 printf("value:=%d", _value);
 hash_destory(&n);
}
#endif
