#include <stdio.h>
#include <stdlib.h>
#include "mymalloc.h"

#define SIZE 25000

char RAM[SIZE]={'\0'};

struct node
{ 
	int size; 
	int address;
	struct node* next; 
}; 

int usableLimit=SIZE; //upper limit of the usable area to user

struct node *FirstNodeUsed=NULL,*tempUsed; //data of the used list

struct node *firstNodeFree=NULL,*tempFree; //data of the free list

void addNodeUsed(int address,int size) //to the used list
{
	
	if(FirstNodeUsed==NULL)
	{
		usableLimit=usableLimit-16; //allocating space for this node (16 bytes)
		FirstNodeUsed=(struct node*)(RAM+usableLimit); //allocating memory from back end of the array 
		FirstNodeUsed->next=NULL;
		FirstNodeUsed->size=size;
		FirstNodeUsed->address=address;
		tempUsed=FirstNodeUsed;
	}
	else{	
		struct node *newNode;
		usableLimit=usableLimit-16; //allocating space for this node (16 bytes)
		newNode=(struct node*)(RAM+usableLimit); //allocating memory from back end of the array 
		newNode->next=NULL;
		newNode->size=size;
		newNode->address=address;
		tempUsed->next=newNode;
		tempUsed=tempUsed->next;
	}
	
}

void addNodeFree(int address,int size) //to the free list
{
	
	if(firstNodeFree==NULL)
	{
		usableLimit=usableLimit-16; //allocating space for this node (16 bytes)
		firstNodeFree=(struct node*)(RAM+usableLimit); //allocating memory from back end of the array 
		firstNodeFree->next=NULL;
		firstNodeFree->size=size;
		firstNodeFree->address=address;
		tempFree=firstNodeFree;
	}
	else{
		
		struct node *newNodeFree;
		usableLimit=usableLimit-16; //allocating space for this node (16 bytes)
		newNodeFree=(struct node*)(RAM+usableLimit); //allocating memory from back end of the array 
		newNodeFree->next=NULL;
		newNodeFree->size=size;
		newNodeFree->address=address;
		tempFree->next=newNodeFree;
		tempFree=tempFree->next;
	}
	
}	

void MyFree(void *pointer)
{
	int arrayIndex=(char*)pointer-RAM; //to store the index value of the RAM array
	struct node *temp2=FirstNodeUsed; //used list starting node to temp2
	struct node *tempPrev2=FirstNodeUsed;

	//REMOVE the corresponding data FROM THE USED LIST and ADDING corresponding data to free list
	while(temp2!=NULL)
	{	
		if(temp2->address==arrayIndex)
		{
			if(temp2==FirstNodeUsed) //if its the first node of the list
			{
				FirstNodeUsed=FirstNodeUsed->next;
				addNodeFree(temp2->address,temp2->size); //ADDING THE NEWLY FREED AREA TO FREE LIST
				break;
			}
			else if(temp2->next==NULL) //last node of the list
			{
				tempPrev2->next=NULL;
				addNodeFree(temp2->address,temp2->size); //ADDING THE NEWLY FREED AREA TO FREE LIST
				break;
			}
			else //middle node of the list
			{
				tempPrev2->next=temp2->next;
				addNodeFree(temp2->address,temp2->size); //ADDING THE NEWLY FREED AREA TO FREE LIST
				break;
			}
		}
		
		tempPrev2=temp2;
		temp2=temp2->next;		
	}	
}

void *MyMalloc(size_t n)  
{			

	//CHECK THE FREE LIST
	void *result;
	
	if(FirstNodeUsed==NULL) //to handle, when every slot is freed and wants to allocate memory again
	{	
		usableLimit=SIZE;
		firstNodeFree=NULL;
		tempUsed=NULL;
		tempFree=NULL;
	}
	
	struct node *temp1=firstNodeFree;
	struct node *tempPrev1=firstNodeFree;
	int isAllocationSuccessful=0,top=0;
	
	if(n>SIZE) 
	{
		printf("Memory Allocation Failed, Error - Not Enough Space.\n");
		return (NULL);
	}
	
	if(temp1==NULL) //when whole memory is free (initial state)
	{
		result=RAM;
		addNodeUsed(0,n);		
		addNodeFree(n,(usableLimit-n)-16);
	}
	else
	{
		while(temp1!=NULL)
		{
			if(temp1->size>=n) //first fit method
			{	
			
				isAllocationSuccessful=1; //to detect successful allocations
				if(temp1->size==n || (temp1->size-n)<16) //best fit || Internal fragmentation 
				{
					result=RAM+temp1->address;
					addNodeUsed(temp1->address,n); //adding to the used list
				
				
					//removing that data from free list
					if(temp1==firstNodeFree) //if its the first node of the list
		     		{
						temp1->size=0; //never remove the first one because we need that node to keep the algorithm funtional
						break;
			   		}	
			   		else if(temp1->next==NULL) //last node of the list
			  		{
						tempPrev1->next=NULL;
						break;
			  		}
			  		
			  		else if(temp1->next==NULL) //middle node of the list
			  		{
						tempPrev1->next=temp1->next;
						break;
			  		}
				}
			
				else 
				{
					result=RAM+temp1->address; //storing the starting address of the block we reserved to a void pointer "result"
				
					//update the used list
					addNodeUsed(temp1->address,n);
			
					//update the free list 
					if(temp1==firstNodeFree) //first Node contains the upper usable limit
					{
						temp1->address=temp1->address+n;
						temp1->size=usableLimit-temp1->address;		
						break;
					}
					else //for every other node
					{
						top=temp1->address+temp1->size;
						temp1->address=temp1->address+n;
						temp1->size=top-temp1->address;	
						break;
					}
					
				
				}
			}		
		tempPrev1=temp1;
		temp1=temp1->next;		
		}
		if(!isAllocationSuccessful) //to detect unsuccessful allocations (It is 1 when memory is allocated)
		{
			printf("Memory Allocation Failed, Error - Not Enough Contiguous Space. \n");
			return (NULL);
		}	
	}
	
	return (result);	
}
