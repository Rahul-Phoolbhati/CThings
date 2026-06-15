#include<stdio.h>
#include<stdlib.h>


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

int main(){

    LinkedNode* head;
    LinkedNode a,b,c;
    a.data = 1;
    b.data = 3;
    c.data = 5;

    head = &a;
    a.next = &b;
    b.next = &c;
    c.next = NULL;

    printList(head);
    
    return 0;
}