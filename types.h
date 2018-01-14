#include "table.h"

typedef struct TYPE_ * TYPE;
typedef struct VARINFO_ * VARINFO;
typedef struct Access_ * Access;


struct TYPE_{
	enum {int_TYPE,float_TYPE,array_TYPE,record_TYPE,pointer_TYPE,args_TYPE}kind;
	int width;
	union{
		struct {TYPE elem;int len;} array;
		struct {string name;stable fields,types;} record;
		struct {TYPE head;TYPE tail;} args;
		TYPE subtype;
	}u;
};

struct VARINFO_{
	int tag;	//when is a function, tag=1 represents that it has been defined
	int offset;
	TYPE type;
};

struct Access_{
	int tag;
	int offset;
	TYPE type;
};


void panic(string msg);
TYPE array_type(TYPE elem,int len);
TYPE record_type(string name,stable fields,stable types,int size);
TYPE point_type(TYPE sub);
TYPE args_type(TYPE head,TYPE tail);
TYPE get_elem_type(TYPE array);
VARINFO get_field_VARINFO(TYPE record,string name);
TYPE get_record_type(stable types,string name);
VARINFO get_info(stable tab,string name);
int type_equal(TYPE t1,TYPE t2);
VARINFO combinfo(int tag,int offset,TYPE type);
void add_var(stable tab,string name,VARINFO info);
void add_type(stable tab,string name,TYPE type);
int type_equal(TYPE t1,TYPE t2);
void refill_type(TYPE type,int offset);

