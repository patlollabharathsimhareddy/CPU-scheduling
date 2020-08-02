#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef SIZE
#define SIZE 500
#endif

#ifndef N
#define N 200
#endif

struct process
{
	int pnumber;
	int priority, arr_time, burst_time;
	int rem_time, start_time;
};

struct heap
{
	struct process *p;
	struct heap *parent, *left, *right;
};

int global_time = 0;

struct heap* insert(struct heap*, int, struct process*);
void preorder(struct heap*);
void convert_to_bin(int, char*);
void strrev(char *);
struct heap* extract_min(struct heap*, int, struct process**);


int main(int argc, char const *argv[])
{
	int n,i,j,list_indx;
	int nodes=0;
	struct process *running=NULL, *temp_proc, *temp_running;
	struct heap *root = NULL, *last=NULL;
	int proc_list[SIZE], time_list[SIZE];
	int p_num = 0, avail_time;
	struct process *procs[SIZE];

	list_indx = 0;

	scanf("%d", &n);
	for(i=0;i<n;i++){
		procs[i] = (struct process*)(malloc(sizeof(struct process)));
		scanf("%d%d%d%d", &procs[i]->pnumber, &procs[i]->priority, 
			&procs[i]->arr_time, &procs[i]->burst_time);
		procs[i]->rem_time = procs[i]->burst_time;
		procs[i]->start_time = 0;
	}
	for(i=0;i<n;i++){
		if(running){
			avail_time = procs[i]->arr_time - running->start_time;
			while(running && running->rem_time<=avail_time){
				avail_time -= running->rem_time;
				global_time += running->rem_time;
				time_list[list_indx++] = global_time;
				free(running);
				running = NULL;
				if(root){
					preorder(root);
					printf("\n");	
				}
				root = extract_min(root, p_num, &running);
				if(p_num>0)
					p_num -=1;
				if(running){
					running->start_time = global_time;
					proc_list[list_indx] = running->pnumber;
				}
			}
			if(running){
				running->rem_time -= avail_time;	
				running->start_time = procs[i]->arr_time;			
			}
			global_time = procs[i]->arr_time;
		}

		// After the time details have been updated
		if(!running){
			running = procs[i];
			if(i<n-1){
				for(j=i+1;j<n && (procs[j]->arr_time == procs[j-1]->arr_time);j++){
					if(procs[j]->priority < running->priority){
						p_num++;
						root = insert(root, p_num, running);
						running = procs[j];
					}
					else{
						p_num++;
						root = insert(root, p_num, procs[j]);
					}
				}
				i = j-1;
			}
			
			running->start_time = global_time;
			proc_list[list_indx] = running->pnumber;
			global_time = running->arr_time;
		}
		else if(running->priority <= procs[i]->priority){
			p_num += 1;
			root = insert(root, p_num, procs[i]);
			temp_proc = running;
			temp_running = running;
			if(i<n-1){
				for(j=i+1;j<n && (procs[j]->arr_time == procs[j-1]->arr_time);j++){
					if(running && procs[j]->priority < running->priority){
						p_num++;
						running->rem_time -= (procs[i]->arr_time - running->start_time);
						root = insert(root, p_num, running);
						temp_running = procs[j];
						running = NULL;
					}
					else if(procs[j]->priority < temp_running->priority){
						p_num++;
						root = insert(root, p_num, temp_running);
						temp_running = procs[j];
					}
					else{
						p_num++;
						root = insert(root, p_num, procs[j]);
					}
				}
				i = j-1;
			}
			if(!running){
				running = temp_running;
				running->start_time = running->arr_time;
				time_list[list_indx++] = running->arr_time;
				proc_list[list_indx] = running->pnumber;
				global_time = running->arr_time;
			}	
			else {
				global_time = procs[i]->arr_time;
			}
							
		}
		else{
			p_num += 1;
			running->rem_time -= (procs[i]->arr_time - running->start_time);
			if(avail_time == 0)
				list_indx--;
			root = insert(root, p_num, running);
			running = procs[i];
			if(i<n-1){
				for(j=i+1;j<n && (procs[j]->arr_time == procs[j-1]->arr_time);j++){
					if(procs[j]->priority < running->priority){
						p_num++;
						root = insert(root, p_num, running);
						running = procs[j];
					}
					else{
						p_num++;
						root = insert(root, p_num, procs[j]);
					}
				}
				i = j-1;
			}	
			running->start_time = running->arr_time;
			time_list[list_indx++] = running->arr_time;
			proc_list[list_indx] = running->pnumber;
			global_time = running->arr_time;
		}
	}

	// All processes have been read
	// Now it runs like priority scheduling without pre-emption
	while(root){
		if(running){
			global_time += running->rem_time;
			time_list[list_indx++] = global_time;
			free(running);
			running = NULL;
		}
		if(root){
			preorder(root);
			printf("\n");
		}
		root = extract_min(root, p_num, &running);
		if(p_num>0)
			p_num -= 1;
		proc_list[list_indx] = running->pnumber;
	}
	global_time += running->rem_time;
	time_list[list_indx] = global_time;	
	free(running);
	for(i=0;i<=list_indx;i++)
		printf("%d ", proc_list[i]);
	printf("\n");
	for(i=0;i<=list_indx;i++)
		printf("%d ",time_list[i]);
	printf("\n");
	return 0;
}


struct heap* insert(struct heap *root, int n, struct process *p){
	struct heap *temp = (struct heap*)(malloc(sizeof(struct heap)));
	int len;
	char bin[SIZE];
	struct heap *flag;
	struct process *t;
	int i;

	temp->p = p;
	temp->parent = temp->left = temp->right = NULL;

	if(!root){
		return temp;
	}

	convert_to_bin(n,bin);
	len = strlen(bin);
	flag = root;
	for(i=1;i<len-1;i++){
		if(bin[i]=='0'){
			flag = flag->left;
		}
		else{
			flag = flag->right;
		}
	}
	if(bin[len-1]=='0'){
		flag->left = temp;
	}
	else{
		flag->right = temp;
	}
	temp->parent = flag;
	while(temp != root && temp->p->priority<=temp->parent->p->priority){
		if (temp->p->priority < temp->parent->p->priority){
			t = temp->p;
			temp->p = temp->parent->p;
			temp->parent->p = t;
			temp = temp->parent;
		}	
		else if((temp->p->priority == temp->parent->p->priority) && (temp->p->arr_time<temp->parent->p->arr_time)){
			t = temp->p;
			temp->p = temp->parent->p;
			temp->parent->p = t;
			temp = temp->parent;	
		}	
		else{
			temp = root;
		}	
	}
	return root;
}

struct heap* extract_min(struct heap *root, int n, struct process **min){
	char bin[SIZE];
	int len, i;
	struct process *temp, *temp_p;
	struct heap *l,*r, *flag;
	int l_min, r_min;

	if(n<=0){
		*min = NULL;
		return NULL;
	}

	if(n==1){
		temp = root->p;
		free(root);
		*min = temp;
		return NULL;
	}

	convert_to_bin(n, bin);
	len = strlen(bin);
	flag = root;
	for(i=1;i<len;i++){
		if(bin[i] == '0')
			flag = flag->left;
		else
			flag = flag->right;
	}
	*min = root->p;
	temp = root->p;	// minimum node
	root->p = flag->p;
	if(bin[len-1]=='0')
		flag->parent->left = NULL;
	else
		flag->parent->right = NULL;
	free(flag);
	// Because we are considering arrival time too, we need to equate it
	flag = root;
	l = flag->left;
	r = flag->right;
	while((l && (flag->p->priority>=l->p->priority)) || (r&& (flag->p->priority>=r->p->priority))){
		l_min = 0;
		r_min = 0;
		if(l && l->p->priority<flag->p->priority){
			l_min = 1;
		}
		else if(l && (l->p->priority == flag->p->priority) &&(l->p->arr_time<flag->p->arr_time)){
			l_min = 1;
		}
		if(l_min && (r && r->p->priority<l->p->priority)){
			r_min = 1;
		}
		else if(l_min && (r && r->p->priority == l->p->priority &&r->p->arr_time<l->p->arr_time)){
			r_min = 1;
		}
		else if(!l_min && (r && r->p->priority<flag->p->priority)){
			r_min = 1;
		}
		else if(!l_min && (r && r->p->priority == flag->p->priority &&r->p->arr_time<flag->p->arr_time)){
			r_min = 1;
		}
		if(r_min){
			temp_p = r->p;
			r->p = flag->p;
			flag->p = temp_p;
			flag = flag->right;
			l = flag->left;
			r = flag->right; 
		}
		else if(l_min){
			temp_p = l->p;
			l->p = flag->p;
			flag->p = temp_p;
			flag = flag->left;		
			l = flag->left;
			r = flag->right; 
		}
		else{
			l = NULL;
			r = NULL;
		}
	}
	return root;
}

void convert_to_bin(int n, char *bin){
	int i=0;
	while(n){
		bin[i++] = (char)(n%2) + 48;
		n/=2;
	}
	bin[i]='\0';
	strrev(bin);
	return;
}

void strrev(char *bin){
	int i,j,len = strlen(bin);
	char temp;
	i=0;
	j = len-1;

	while(i<j){
		temp = bin[i];
		bin[i++] = bin[j];
		bin[j--] = temp;
	}
}

void preorder(struct heap *root){
	if(root){
		printf("%d ",root->p->pnumber );
		preorder(root->left);
		preorder(root->right);
	}
}