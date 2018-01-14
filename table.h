#include "utility.h"

typedef struct stable_ * stable;
typedef struct entry_ * entry;

struct entry_{string key;void *value;entry next;};

#define TABLESIZE 109
struct stable_{
	entry table[TABLESIZE];
	stable prev;
};


stable table_new(stable prev);
unsigned int hash(string s);
static int streq(string a, string b);
void *table_local_find(stable tab,string key,int index);
void *table_find(stable tab,string key);
void *table_insert(stable tab,string key,void *value);



