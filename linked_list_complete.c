#include<stdio.h>
#include<stdlib.h>


struct my_node {
	int data;
	struct my_node *next;
};
typedef struct my_node node;

node* alloc_new_node (node *head, int data);
void add_node_to_list(node *head, node *new_node);
void add_node_in_mid_of_list(node *head, node *new_node);

int main ()
{
	int input = -1;
	int data = 0;
top:
	printf("1. Insert node random data \n 2. Insert node with data provided \n 3. Insert node to head with data provided \n 4. Insert node at given position \n 5. Print the middle node \n 6. Print the specified position node \n 7. Insert the node at middle \n 8. Print all list \n 9. Free all nodes \n 10. Exit \n");

	scanf("%d %d ",&input, &data);

	if(input > 10) {
		printf(" Invalid input \n");
		goto top;
	}
	switch(input)
	{
		case 1:
			break;
		case 2:
			break;
		case 3:
			break;
		case 4:
			break;
		case 5:
			break;
		case 6:
			break;
		case 7:
			break;
		case 8:
			break;
		case 9:
			break;
		case 10:
			break;

	}


	return 0;
}


node* alloc_new_node (node *head, int data)
{
	node *new_node;
	new_node = (node*)malloc(sizeof(*new_node));
	if(new_node == NULL)
	printf("%s: No Memory \n",__func__);
	new_node->next = NULL;
	return new_node;
}

void add_node_to_list(node *head, node *new_node)
{
	node *temp_node;
	if(head == NULL)
		head = new_node;
	else {
		temp_node = head;
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
		while(pnode->next != NULL){
			printf("List member[%d] { address =%p, next_node_address =%p, data = %d }\n",++count,pnode,pnode->next,pnode->data);
		}
	}
	return;
}


