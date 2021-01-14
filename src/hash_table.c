#include "hash_table.h"
#include "prime.c"

#include <string.h>
#include <stdlib.h>
#include <math.h>

static ht_item* ht_create_item(const char* x, const char *y) {
    ht_item* k = malloc(sizeof(ht_item));

    k->key = strdup(x);
    k->value = strdup(y);
    return k;
}


static ht_info* ht_new_sized(const int base_size) {
    ht_info* ht = xmalloc(sizeof(ht_info));
    ht->base_size = base_size;

    ht->size = next_prime(ht->base_size);
    
    ht->count = 0;
    ht->items = xcalloc((size_t)ht->size, sizeof(ht_item*));
    return ht;
}

ht_info* new_ht(void) {
   /*
   ht_info* ht = malloc(sizeof(ht_info)); 
   ht->size = 128;  
   ht->count = 0;
   ht->items = calloc((size_t)ht->size, sizeof(ht_item*));
   return ht;
   */
   return ht_new_sized(HT_INITIAL_BASE_SIZE);
   
}
static void ht_del_item(ht_item* k) {
    free(k->key);
    free(k->value);
    free(k);
}

void del_ht(ht_info* ht) {
    for (int i = 0; i < ht->size; i++) {
       ht_item* item = ht->items[i]; 
       if (item != NULL) ht_del_item(item);
    }
    free(ht->items);
    free(ht);
}

/* Realization of hash function */
static int ht_hash(const char* s, const int a, const int m) {
    long hash = 0;
    const int len_s = strlen(s);
    for (int i = 0; i < len_s; i++) {
        hash += (long)pow(a, len_s - (i + 1)) * s[i];
        hash = hash % m;
    }
    return (int)hash;
}
        

static int ht_get_hash(const char *s, const int num_buckets, const int attempt) {
    const int hash_a = ht_hash(s, HT_PRIME_1, num_buckets);
    const int hash_b = ht_hash(s, HT_PRIME_2, num_buckets);
    return (hash_a + (attempt * (hash_b + 1))) % num_buckets;
}

static ht_item HT_DELETED_ITEM = {NULL, NULL};

char* ht_search(ht_info* ht, const char* key) {
    int index = ht_get_hash(key, ht->size, 0);
    ht_item* item = ht->items[index];
    int i = 1;
    while (item != NULL) {
        if (item != &HT_DELETED_ITEM) {
        if (strcmp(item->key, key) == 0) {
        
            return item->value;
       } 
    }
        index = ht_get_hash(key, ht->size, i);
        item = ht->items[index];
        i++;
 }
   return NULL;
}
static void ht_resize_down(ht_info* ht) {
    const int new_size = ht->base_size / 2;
    ht_resize(ht, new_size);
}
static void ht_resize_up(ht_info* ht) {
    const int new_size = ht->base_size * 2;
    ht_resize(ht, new_size);
}



void ht_insert(ht_info* ht, const char* key, const char* value) {
    const int load = ht->count * 100 / ht->size;
    if (load > 70) ht_resize_up(ht);
    ht_item* item = ht_create_item(key, value);
    int index = ht_get_hash(item->key, ht->size, 0);
    ht_item* cur_item = ht->items[index];
    int i = 1;
    while (cur_item != NULL && cur_item != &HT_DELETED_ITEM) {
        index = ht_get_hash(item->key, ht->size, 1);
        cur_item = ht->items[index];
        i++;
    }
    ht->items[index] = item;
    ht->count++;
}

void ht_delete(ht_info* ht, const char* key) {
    const int load = ht->count * 100 / ht->size;
    if (load < 10) ht_resize_down(ht);
    int index = ht_get_hash(key, ht->size, 0);
    ht_item* item = ht->items[index];
    int i = 1;
    while (item != NULL) {
        if (item != &HT_DELETED_ITEM) {
            if (strcmp(item->key, key) == 0) {
                ht_del_item(item);
                ht->items[index] = &HT_DELETED_ITEM;
            }
        }
        index = ht_get_hash(key, ht->size, i);
        item = ht->items[index];
        i++;
    }
    ht->count--;
}
        
static void ht_resize(ht_info* ht, const int base_size) {
    if (base_size < HT_INITIAL_BASE_SIZE) {
        return;
    }
    ht_info* new_ht = ht_new_sized(base_size);
    for (int i = 0; i < ht->size; i++) {
        ht_item* item = ht->items[i];
        if (item != NULL && item != &HT_DELETED_ITEM) {
            ht_insert(new_ht, item->key, item->value);
            }
        }
    ht->base_size = new_ht->base_size;
    ht->count = new_ht->count;

    const int tmp_size = ht->size;
    ht->size = new-ht->size;
    new_ht->size = tmp_size;

    ht_item** tmp_items = ht->items;
    ht->items = new_ht->items;
    new_ht->items = tmp_items;

    ht_delete(new_ht);
}


