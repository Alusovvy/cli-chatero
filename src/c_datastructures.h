#include <stdlib.h>
#include <stdio.h>

typedef struct {
	char* key;
	void* item;
} item;

item* get_item(item* items, size_t size, const char* key) {
	for (size_t i=0; i<size; i++) {
		if (strcmp(item[i].key, key) == 0) {
			return &items[i];
		}
	}

	return NULL;
}


