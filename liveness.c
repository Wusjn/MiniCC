#include "eeyore.h"
#include <stdlib.h>

static int iter_fin;

void live_iter(flownode tar){
	if(tar==NULL||tar->visited==1) return;
	tar->visited=1;
	if(tar->kind==end_flow) return;
	live_iter(tar->succ[0]);
	live_iter(tar->succ[1]);
	if(tar->kind==inter_flow){
		for(int i=0;i<2;i++){
			if(tar->succ[i]!=NULL)
				bitmap_or(tar->new_out,tar->succ[i]->in);
		}
		if(!bitmap_equal(tar->new_out,tar->out))
			iter_fin=0;
		bitmap temp;
		temp=tar->out;
		tar->out=tar->new_out;
		tar->new_out=temp;
		bitmap_clear(tar->new_out);
		bitmap_copy(tar->in,tar->out);
		bitmap_minus(tar->in,tar->def);
		bitmap_or(tar->in,tar->use);
	}
	return;
}

void live_trans(interstat tar){
	bitmap_copy(tar->out,tar->next->out);
	bitmap_reset(tar->out,tar->next->def);
	for(int i=0;i<3;i++){
		bitmap_set(tar->out,tar->next->use[i]);
	}
	return;
}

void live_expand(flownode tar){
	if(tar==NULL||tar->visited==1)
		return;
	tar->visited=1;
	if(tar->kind==end_flow) return;
	live_expand(tar->succ[0]);
	live_expand(tar->succ[1]);
	if(tar->kind==inter_flow){
		bitmap_copy(tar->end->out,tar->out);
		interstat cur=tar->end->prev;
		for(;cur!=tar->begin->prev;cur=cur->prev){
			live_trans(cur);
		}
	}
	return;
}

int live_scan(flownode tar){
	if(tar->kind!=begin_flow)
		return -1;
	iter_fin=0;
	while(!iter_fin){
		iter_fin=1;
		live_iter(tar);
		flow_clear_history(tar);
	}
	live_expand(tar);
	flow_clear_history(tar);
	return 0;
}

void livezone_adjust(livezone *zones,bitmap tar,int line){
	for(int i=0;i<tar->bitsize;i++){
		if(bitmap_get(tar,i)){
			if(zones[i]->begin<0)
				zones[i]->begin=line;
			zones[i]->end=line;
		}
	}
	return;
}

int zone_less(const void *a,const void *b){
	livezone a_=*((livezone *)a);
	livezone b_=*((livezone *)b);
	int res;
	if((res=(a_->begin-b_->begin))!=0)
		return res;
	else return a_->number-b_->number;
}

int livezone_create(interstat tar){
	if(tar->kind!=begin_inter)
		return -1;
	funcinfo info=tar->u.begin;
	for(int i=0;i<info->varnum;i++){
		info->zones[i]->number=i;
		info->zones[i]->begin=-1;
		info->zones[i]->end=-1;
	}
	for(interstat cur=tar->next;cur!=NULL;cur=cur->next){
		livezone_adjust(info->zones,cur->out,cur->statnum);
	}
	info->ori_zones=(livezone *)checked_malloc(sizeof(livezone)*info->varnum);
	memcpy(info->ori_zones,info->zones,sizeof(livezone)*info->varnum);
	qsort(info->zones,info->varnum,sizeof(livezone),zone_less);
	return 0;
}

void livezone_print(livezone *zones,int varnum){
	fprintf(stderr,"\n");
	for(int i=0;i<varnum;i++){
		fprintf(stderr,"var %d : begin %d end %d\n",
			zones[i]->number,zones[i]->begin,zones[i]->end);
	}
	return;
}

int live_analyse(interstat eeyore){
	for(interstat cur=eeyore;cur!=NULL;cur=cur->nextblk){
		if(cur->kind!=begin_inter)
			continue;
		live_scan(cur->u.begin->graph);
		livezone_create(cur);
		livezone_print(cur->u.begin->zones,cur->u.begin->varnum);
	}
	return 0;
}
