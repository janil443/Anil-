#include<stdio.h>
#include<stdlib.h>


struct my_node {
	int data;
	struct my_node *next;
};
typedef struct my_node node;
#define NODES       10

node* alloc_new_node (node *head, int data);
void add_node_to_list(node **head, node *new_node);
void add_node_in_mid_of_list(node *head, node *new_node);
void dump_all_nodes(node *head);
node* reverse_list(node *head);

int main ()
{
	int input = -1;
	int data = 0;
  struct my_node *HEAD = NULL;

  int i;
  for(i =0; i < NODES ; i++)
  {
    //alloc_new_node(HEAD, i+1);
    //printf("HEAD= %p\n",HEAD);
    add_node_to_list(&HEAD, alloc_new_node(HEAD, i+1));
    //printf("HEAD= %p\n",HEAD);
  }
  dump_all_nodes(HEAD);
   HEAD = reverse_list(HEAD);
  dump_all_nodes(HEAD);
	return 0;
}

node* reverse_list(node *HEAD)
{
  node *prev = NULL;
  node *cur = NULL;
  node *next = NULL;
  cur = HEAD;

  while(cur != NULL)
  {
    next = cur->next;
    cur->next = prev;
    prev = cur;
    cur = next;

  }

  return prev;

}
node* alloc_new_node (node *head, int data)
{
  //printf("%s: \n",__func__);
	node *new_node;
	new_node = (node*)malloc(sizeof(*new_node));
	if(new_node == NULL)
	printf("%s: No Memory \n",__func__);
	new_node->next = NULL;
  new_node->data = data;
	return new_node;
}

void add_node_to_list(node **head, node *new_node)
{
	node *temp_node;
  //printf("%s: \n",__func__);
	if(*head == NULL) {
  //  printf("%s HEAD is null ",__func__);
		*head = new_node;
   // printf("%s ***************HEAD is %p ",__func__,*head);
  }else {
		temp_node = *head;
		while(temp_node->next != NULL)
			temp_node = temp_node->next;
		temp_node->next = new_node;
	}
	return;
}

void add_node_in_mid_of_list(node *head, node *new_node)
{
	node *fast_node;
	node *slow_node;

	if(head == NULL)
		head = new_node;
	else {
		fast_node = head;
		slow_node = head;

		while(fast_node->next != NULL && fast_node->next->next != NULL) {
			fast_node = fast_node->next->next;
			slow_node = slow_node->next;
		}
		new_node->next = slow_node->next;
		slow_node = new_node;
	}
	return;
}

void dump_all_nodes(node *head)
{
	node *pnode;
	int count = 0 ;
	if(head == NULL)
	{
		perror(" List is empty \n");
	}
	else {
		pnode = head;
    //printf("%s: Head =%p \n",__func__,head) ;
		while(pnode != NULL){
			printf("List member[%d] { address =%p, next_node_address =%p, data = %d }\n",++count,pnode,pnode->next,pnode->data);
      pnode = pnode->next;
		}
	}
	return;
}


