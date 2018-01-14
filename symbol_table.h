

typedef char * string;

void *checked_malloc(int len);
string String(char *s);


typedef struct binder_ * binder;
typedef struct stable_ * stable;
typedef struct entry_ * entry;


struct binder_
	{
	enum{var_binder,func_binder}kind;
	//var_binder: 1->defn 0->decl
	//func_binder: 1->defn 0->decl,not defn yet -1->decl,but already defn
	int flag;
	void *defin;
};

struct entry_{string key;binder value;entry next;};

#define TABLESIZE 109
struct stable_{
	entry table[TABLESIZE];
	stable prev;
};


stable table_new(stable prev);
unsigned int hash(string s);
static int streq(string a, string b);
binder table_local_find(stable tab,string key,int index);
binder table_find(stable tab,string key);
binder table_insert(stable tab,string key,binder value);



