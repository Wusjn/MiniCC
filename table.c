#include "table.h"

stable table_new(stable prev){
	stable newtable=(stable)checked_malloc(sizeof(*newtable));
	int i=0;
	memset(newtable->table,0,sizeof(newtable->table));
	newtable->prev=prev;
	return newtable;
}

unsigned int hash(string s){
	unsigned int h=0;
	for(;*s;s++){
		h = h*65599 + *s;
	}
	return h;
}

static int streq(string a, string b)
{
	return !strcmp(a,b);
}

void* table_local_find(stable tab,string key,int index){
	for(entry cure=tab->table[index];cure!=NULL;cure=cure->next){
		if(streq(key,cure->key)) return cure->value;
	}
	return NULL;
}

void *table_find(stable tab,string key){
	int index=hash(key)%TABLESIZE;
	for(stable curt=tab;curt!=NULL;curt=curt->prev){
		for(entry cure=curt->table[index];cure!=NULL;cure=cure->next){
			if(streq(key,cure->key)) return cure->value;
		}
	}
	return NULL;
}

void *table_insert(stable tab,string key,void* value){
	int index=hash(key)%TABLESIZE;
	void *tar;
	if((tar=table_local_find(tab,key,index))!=NULL) return tar;
	entry new_entry=(entry)checked_malloc(sizeof(*new_entry));
	new_entry->key=key;
	new_entry->value=value;
	new_entry->next=tab->table[index];
	tab->table[index]=new_entry;
	return NULL;
}



