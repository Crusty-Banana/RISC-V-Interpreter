#include <stdio.h>
#include <stdlib.h>
#include "linkedlist.h"
#include "hashtable.h"

struct hashtable {
    // TODO: define hashtable struct to use linkedlist as buckets
    linkedlist_t **buckets;
    int length;
};

/**
 * Hash function to hash a key into the range [0, max_range)
 */
static int hash(int key, int max_range) {
    // TODO: feel free to write your own hash function (NOT REQUIRED)
    key = (key > 0) ? key : -key;
    return key % max_range;
}

hashtable_t *ht_init(int num_buckets) {
    // TODO: create a new hashtable
    hashtable_t *table = malloc(sizeof(hashtable_t));
    table->buckets = malloc(sizeof(linkedlist_t*) * num_buckets);
    table->length = num_buckets;

    for (int i = 0; i < num_buckets; i++) {
        table->buckets[i] = ll_init();
    }

    return table;
}

void ht_add(hashtable_t *table, int key, int value) {
    // TODO: create a new mapping from key -> value.
    // If the key already exists, replace the value.
    int index = hash(key, table->length);
    ll_add(table->buckets[index], key, value);

}

int ht_get(hashtable_t *table, int key) {
    // TODO: retrieve the value mapped to the given key.
    // If it does not exist, return 0
    int index = hash(key, table->length);
    return ll_get(table->buckets[index], key);
}

int ht_size(hashtable_t *table) {
    // TODO: return the number of mappings in this hashtable
    int number_of_mapping = 0;
    for (int i = 0; i < table->length; i++) {
        number_of_mapping += ll_size(table->buckets[i]);
    }
    return number_of_mapping;
}
