#include<stdio.h>
#include<malloc.h>
#include<stdlib.h>
#include<pthread.h>
#include<time.h>


int count=0;//Global variable 
int print_data;//Save out data

pthread_t threads[2];//Thread name is threads
pthread_mutex_t mutex=PTHREAD_MUTEX_INITIALIZER;//Mutex name is mutex, do initiaialize
pthread_cond_t buffer_has_space=PTHREAD_COND_INITIALIZER;//Condition name is buffer_has_space
pthread_cond_t buffer_has_data=PTHREAD_COND_INITIALIZER;//Condition name is buffer_has_space


typedef struct Node
{
    int data;
    struct Node* next;// To make linked list
}Node;

Node *head, *tail, *basicNode;//Head pointer, tail pointer, basicNode pointer

Node* makeNode()//Initialize head, tail, basicNode NULL
{
    head=NULL;
    tail=NULL;
    basicNode=NULL;
}

void insertNode(int data)//Insert start at tail
{
    basicNode=(Node *)malloc(sizeof(Node));//Dynamc memory allocation
    basicNode->data=data;//basicNode of data is data
    
    basicNode->next=NULL;//basicNode of next is NULL because of insert at tail

    if(head==NULL)//If first time, linked list has nothing
    {
        head=basicNode;//head points basicNode
        tail=basicNode;//tail points basicNode
        
        return;
    }
    
    else//If is not first time
    {
        tail->next=basicNode;//tail of next points basicNode
        tail=basicNode;//tail points basiscNode
    }
}

void deleteNode()//Delete start at head
{
    Node* node;
    print_data=head->data;//print_data is head of data
    if(head==NULL)//Nothing
        return;
            
    else
    {
        node=head;
        head=head->next;
        
        free(node);//Dynamc memory allocation free
        return;
            
    }
}


void producer()
{
    int i;
    for(i=0; i<1000; i++)
    {
        pthread_mutex_lock(&mutex);//mutex lock because of global variable will changed
        if(count==100)
            pthread_cond_wait(&buffer_has_space, &mutex);//mutex unlock and wait until buffer_has_space signal happen
        
        printf("in: %d\n", i);
        insertNode(i);//Insert i
        count++;//global variable

        pthread_cond_signal(&buffer_has_data);

        pthread_mutex_unlock(&mutex);
    }
}

void consumer()
{
    int i, data;
    for(i=0; i<1000; i++)
    {
        pthread_mutex_lock(&mutex);//mutex lock because of global variable will changed
        if(count==0)
            pthread_cond_wait(&buffer_has_data, &mutex);
        
        printf("out: %d \n", i);
        deleteNode();//delete head
        count--;
        
        pthread_cond_signal(&buffer_has_space);

        pthread_mutex_unlock(&mutex);//mutex unlock
        printf("data = %d \n", print_data);
    }
}
int main(void)
{    
    int i;
    
    makeNode();
    pthread_create(&threads[0], NULL, (void *)producer, NULL);
    pthread_create(&threads[1], NULL, (void *)consumer, NULL);
    
    
    pthread_join(threads[0], NULL);
    pthread_join(threads[1], NULL);

    return 0;     
} 