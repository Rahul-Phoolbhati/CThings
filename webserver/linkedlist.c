#include<stdio.h>
#include<stdlib.h>
#include<errno.h>
#include<string.h>


typedef struct Node{
    int data;
    struct Node* next; // until } reached compiler doesn't register the typpedef name
} LinkedNode;

void printList(LinkedNode* head){
    LinkedNode* temp = head;

    while(temp!= NULL){
        printf("%d -> ", temp->data);
        temp = temp->next;
    }
    printf("NULL\n");
}

LinkedNode* createNode(int data){
    LinkedNode* newNode = (LinkedNode*)malloc(sizeof(LinkedNode));
    if(newNode == NULL){
        printf("Node Creation failed due to memory allocation error: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }
    newNode->data = data;
    newNode->next = NULL;
    return newNode;
}

// void insertAtHead(){}
//void insertAfterNode(){}
//void removeNode(int value){}

int main(){

    LinkedNode* head;
    LinkedNode* a = createNode(19);
    LinkedNode* b = createNode(30);

    head = a;    
    a->next = b;
    b->next = NULL;

    printList(head);
    
    return 0;
}