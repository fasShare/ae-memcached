#ifndef ASSOC_H
#define ASSOC_H
#include "items.h"
/* associative array */
void assoc_init(void);
item *assoc_find(char *key);
int assoc_insert(char *key, item *item);
void assoc_delete(char *key);
#endif
