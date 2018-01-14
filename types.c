#include "types.h"

void panic(string msg){
	fprintf(stderr,"%s\n",msg);
	exit(0);
}

TYPE array_type(TYPE elem,int len){
	TYPE r=(TYPE)checked_malloc(sizeof(*r));
	r->kind=array_TYPE;
	r->width=elem->width*len;
	r->u.array.elem=elem;
	r->u.array.len=len;
	return r;
}

TYPE record_type(string name,stable fields,stable types,int size){
	TYPE r=(TYPE)checked_malloc(sizeof(*r));
	r->kind=record_TYPE;
	r->width=size;
	r->u.record.name=name;
	r->u.record.fields=fields;
	r->u.record.types=types;
	return r;
}

TYPE point_type(TYPE sub){
	TYPE r=(TYPE)checked_malloc(sizeof(*r));
	r->kind=pointer_TYPE;
	r->width=4;
	r->u.subtype=sub;
	return r;
}

TYPE args_type(TYPE head,TYPE tail){
	TYPE r=(TYPE)checked_malloc(sizeof(*r));
	r->kind=args_TYPE;
	r->u.args.head=head;
	r->u.args.tail=tail;
	return r;
}

TYPE get_elem_type(TYPE array){
	if(array->kind!=array_TYPE)
		return NULL;
	return array->u.array.elem;
}

VARINFO get_field_VARINFO(TYPE record,string name){
	if(record->kind!=record_TYPE)
		return NULL;
	return (VARINFO)table_find(record->u.record.fields,name);
}

TYPE get_record_type(stable types,string name){
	return (TYPE)table_find(types,name);
}

VARINFO get_info(stable tab,string name){
	return (VARINFO)table_find(tab,name);
}

VARINFO combinfo(int tag,int offset,TYPE type){
	VARINFO r=(VARINFO)checked_malloc(sizeof(*r));
	r->tag=tag;
	r->offset=offset;
	r->type=type;
	return r;
}

int type_equal(TYPE t1,TYPE t2){
	if(t1==NULL&&t2==NULL) return 1;
	else if(t1==NULL) return 0;
	else if(t2==NULL) return 0;

	if(t1->kind!=t2->kind)
		return 0;
	if(t1->kind==args_TYPE){
		return type_equal(t1->u.args.head,t2->u.args.head)
			&&type_equal(t1->u.args.tail,t2->u.args.tail);
	}
	else if(t1->kind==record_TYPE){
		return t1->u.record.fields==t2->u.record.fields;	//equal by name
	}
	else if(t1->kind==array_TYPE){
		return type_equal(t1->u.array.elem,t2->u.array.elem);
	}
	else if(t1->kind==pointer_TYPE){
		return type_equal(t1->u.subtype,t2->u.subtype);
	}
	else return 1;
}

void add_var(stable tab,string name,VARINFO info){
	VARINFO tar;
	if((tar=(VARINFO)table_insert(tab,name,info))!=NULL){
		if(tar->type->kind==args_TYPE
			&&info->type->kind==args_TYPE
			&&(tar->tag==0||info->tag==0)
			&&type_equal(info->type,tar->type)){
			if(info->tag!=0) tar->tag==1;
		}
		else{
			fprintf(stderr,"symbol \"%s\" :  ",name);
			panic("symbol redeclare");
		}
	}
}

void add_type(stable tab,string name,TYPE type){
	if(table_insert(tab,name,type)!=NULL){
		fprintf(stderr,"symbol \"%s\" :  ",name);
		panic("symbol redeclare");
	}
}

void refill_type(TYPE type,int offset){
	if(type->kind!=record_TYPE)
		panic("not recod_TYPE");
	type->width=offset;
}
