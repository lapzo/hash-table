typedef struct {
    char* key;
    char* value;
} ht_item;

typedef struct {
    ht_item** items;
    int base_size
    int count;
    int size;
} ht_info;    

// API
char* ht_search(ht_info* ht, const char* key);
void ht_insert(ht_info* ht, const char* key, const char* value);
void ht_delete(ht_info* ht, const char *key);


