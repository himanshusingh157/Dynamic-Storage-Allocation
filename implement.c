#include "header.h"
#include<stdio.h>
#include <stdlib.h>
#define max_size 268435456
void* ref_ptr;
list ASL[56];
char *LUT;//Look up table
splitnode *parent=NULL;
splitnode *last=NULL;
int no_alloc;
int no_dealloc;
lint tot_req_size;
lint tot_alloc_size;
int splits;
int recomb;
void initialize() //All the global variable array list are initialized
{
    no_alloc=0;
    no_dealloc=0;
    tot_req_size=0;
    tot_alloc_size=0;
    splits=0;
    recomb=0;
    if(ref_ptr!=NULL)
    {
        free(ref_ptr);
    }
    ref_ptr=NULL;
    ref_ptr=malloc(max_size*sizeof(char));
    if(ref_ptr==NULL)
    {
        printf("INITIALIZATION FAILED!!\n");
        return;
    }
    if(LUT!=NULL)
    {
        free(LUT);
    }
    LUT=NULL;
    LUT=(char*)calloc(max_size,1); //LOOK UP TABLE
    if(LUT==NULL)
    {
        printf("CANNOT INITIALZE MEMORY FOR LOOKUP TABLE\n");
        return;
    }
    freeasl();
    int i,j;
    ASL[0].size=1;
    for (i=1;i<56;i++)
    {
        ASL[i].next=NULL;
        if(i%2==1)
        {
            j=(i+1)/2;
            ASL[i].size=1<<j;
        }
        else
        {
            j=i/2-1;
            ASL[i].size=3*(1<<j);
        }
    }
    node* tempnode=newnode(ref_ptr);
    if(tempnode==NULL)
    {
        exit(0);
    }
    ASL[55].next=tempnode;
    changetag(0,0);
    changekval(0,28);
    freesplit();
    addsnode((1<<28),0,0);
}

void show_asl()   //printing the ASL list
{
    int i;
    node *temp;
    int totmem;
    int j=0;
    totmem=0;
    printf("\n\n SHOWING ASL LIST\n");
    for(i=0;i<56;i++)
    {
        j=0;
        temp=ASL[i].next;
        if(temp==NULL)
        {
            printf("Available blocks of %d byte are 0\n",ASL[i].size);
        }
        else
        {
            while(temp!=NULL)
            {
                j++;
                temp=temp->next;
            }
            printf("Available blocks of %d byte are %d\n",ASL[i].size,j);
        }
        totmem=totmem+(j*ASL[i].size);
    }
    printf("\nTotal available free memeory is %d bytes\n",totmem);
}

int maxi(int i,int j)  //return maximum of two integers here basically to compare sizes
{
    if(i>j)
    {
        return i;
    }
    else
    {
        return j;
    }
}

//These function are for changing and returning TAG, TYPE, KVAL from Lookup table
void changetag(lint start,int tag)
{
    LUT[start]=(LUT[start]&127)|(tag<<7);
}
void changekval(lint start,int kval)
{
    LUT[start]=(LUT[start]&224)|kval;
}
void changetype(lint start,int type)
{
    LUT[start]=(LUT[start]&159)|(type<<5);
}
int returntag(lint start)
{
    int tag;
    tag=LUT[start]&128;
    tag=tag>>7;
    return tag;
}
int returnkval(lint start)
{
    int kval;
    kval=LUT[start]&31;
    return kval;
}
int returntype(lint start)
{
    int type;
    type=LUT[start]&96;
    type=type>>5;
    return type;
}

int return_free_size() //To get the details how much space is remaining whenever external fragmentation occurs
{
    int i;
    node *temp;
    int totmem;
    int j=0;
    totmem=0;
    for(i=0;i<56;i++)
    {
        j=0;
        temp=ASL[i].next;
        if(temp!=NULL)
        {
            while(temp!=NULL)
            {
                j++;
                temp=temp->next;
            }
        }
        totmem=totmem+(j*ASL[i].size);
    }
    return totmem;
}

void* mymalloc(int alloc_size) //allocate memory to appropriate size for a given fixed request size
{
    no_alloc++;
    tot_req_size+=alloc_size;
    void *ptr;
    if(alloc_size>max_size)
    {
        printf("Cannot allocate memory of size more than 256 MB!!\n");
        return NULL;
        tot_req_size-=alloc_size;
    }
    else if(alloc_size==max_size)
    {
        if(returntag(0)==0&&ASL[55].next!=NULL)
        {
            ptr=ASL[55].next->address;
            changetag(0,1);
            freenode(55,ref_ptr);
            printf("256 MB allocated at address 0\n");
            tot_alloc_size-=ASL[55].size;
            return ptr;
        }
        else
        {
            printf("Sorry!! Cannot allocate 256MB size of memory\n");
            tot_req_size-=alloc_size;
            return NULL;
        }
    }
    else
    {
        if(alloc_size==1&&ASL[0].next==NULL)
        {
            alloc_size=2;
        }
        int i,j;
        lint add;
        i=0;
        while(ASL[i].size<alloc_size)
        {
            i++;        //store kval
        }
        if(ASL[i].next!=NULL)
        {
            ptr=ASL[i].next->address;
            add=(lint)ptr-(lint)ref_ptr;
            printf("Space Allocated %d byte at address %lu\n",ASL[i].size,add);
            tot_alloc_size+=ASL[i].size;
            changetag(add,1);
            freenode(i,ptr);
            return ptr;
        }
        else
        {
            while(ASL[i].next==NULL)
            {
                j=i;
                while(ASL[j].next==NULL)
                {
                    j++;
                    if(j>55)
                    {
                        printf("SORRY!! This much space is not available\n");
                        tot_req_size-=alloc_size;
                        return NULL;
                    }
                }
                splitmemory(j);
            }
            ptr=ASL[i].next->address;
            add=(lint)ptr-(lint)ref_ptr;
            printf("Space Allocated %d byte at address %lu\n",ASL[i].size,add);
            tot_alloc_size+=ASL[i].size;
            changetag(add,1);
            freenode(i,ptr);
            return ptr;
        }
    }
}

node* newnode(void *addr)  //returns a node
{
    node *temp=(node*)malloc(sizeof(node));
    temp->prev=NULL;
    temp->next=NULL;
    temp->address=addr;
    return temp;
}

int addnode(int i,node *temp1)   //Add node to the ASL list
{
    if(ASL[i].next==NULL)
    {
        ASL[i].next=temp1;
    }
    else
    {
        node*temp2;
        temp2=ASL[i].next;
        ASL[i].next=temp1;
        temp1->next=temp2;
        temp2->prev=temp1;
    }
}

void splitmemory(int j) //A given node of ASL list is splitted into the corresponding buddies
{
    node *temp;
    lint temp_adr,temp2_adr;
    node *x,*y;
    int k;
    void *ptr;
    if(ASL[j].size%3==0)
    {
        temp=ASL[j].next;
        ptr=temp->address;
        temp_adr=((lint)temp->address)-((lint)ref_ptr);
        k=returnkval(temp_adr);
        temp2_adr=temp_adr+(lint)(1<<k+1);
        changekval(temp_adr,k+1);
        changekval(temp2_adr,k);
        changetype(temp_adr,1);
        changetype(temp2_adr,2);
        addsnode(returnsize(temp_adr),temp_adr,1);
        addsnode(returnsize(temp2_adr),temp2_adr,2);
        changetag(temp_adr,0);
        changetag(temp2_adr,0);
        x=newnode((void*)(temp_adr+(lint)ref_ptr));
        y=newnode((void*)(temp2_adr+(lint)ref_ptr));
        int z=maxi(j-3,0);
        addnode(j-1,x);
        addnode(z,y);
        freenode(j,ptr);
    }
    else
    {
        temp=ASL[j].next;
        ptr=temp->address;
        temp_adr=((lint)temp->address)-((lint)ref_ptr);
        k=returnkval(temp_adr);
        temp2_adr=temp_adr+(lint)(3<<k-2);
        changekval(temp_adr,k-2);
        changekval(temp2_adr,k-2);
        changetype(temp_adr,3);
        changetype(temp2_adr,3);
        addsnode(returnsize(temp_adr),temp_adr,3);
        addsnode(returnsize(temp2_adr),temp2_adr,3);
        changetag(temp_adr,0);
        changetag(temp2_adr,0);
        freenode(j,ptr);
        x=newnode((void*)(temp_adr+(lint)ref_ptr));
        y=newnode((void*)(temp2_adr+(lint)ref_ptr));
        if(k>2)
        {
            addnode(j-1,x);
            addnode(j-4,y);
        }
        else
        {
            if(k=2)
            {
                addnode(j-1,x);
                addnode(j-3,y);
            }
        }
    }
    splits++;
}

int myfree(void *add)  //deallocating the given address
{
    lint address;
    address=(lint)add-(lint)ref_ptr;
    node *temp;
    lint buddy_address;
    int k,t,s,i;
    if(returntag(address)==0)
    {
        printf("ADDRESS AT THIS LOCATION IS ALREADY FREE\n");
        return 0;
    }
    else
    {
        k=returnkval(address);
        t=returntype(address);
        s=returnsize(address);
        i=findindex(address);//return buddy index
        buddy_address=findbuddy(address);
        if(avail(i,buddy_address)==1)//find if buddy is free
        {
            combine(address,buddy_address);
        }
        else
        {
            changetag(address,0);//else add it to the list
            if(t==1||t==2)
            {
                node* tempnode;
                tempnode=newnode(add);
                addnode(2*k-1,tempnode);
            }
            else
            {
                if(s%3==0)
                {
                    node* tempnode;
                    tempnode=newnode(add);
                    addnode(2*k+2,tempnode);
                }
                else
                {
                    node* tempnode;
                    tempnode=newnode(add);
                    addnode(2*k-1,tempnode);
                }
            }
        }
    }
    checklist();
    no_dealloc++;
    return 1;
}

void combine(lint adr,lint bud_adr) //whenever a buddy is free is recombine at block with its buddy
{
    lint left_adr;
    int type,k,size1,size2;
    size1=returnsize(adr);
    size2=returnsize(bud_adr);
    type=returnstype(size1,adr);
    void *address;
    if(type==1)
    {
        deletesnode(size1,adr);
        deletesnode(size2,bud_adr);
        k=returnkval(adr);
        address=(void*)(bud_adr+(lint)ref_ptr);
        if(k==1)
        {
            freenode(0,address);
        }
        else
        {
            freenode(2*k-3,address);
        }
        node *tempnode;
        address=(void*)(adr+(lint)ref_ptr);
        tempnode=newnode(address);
        addnode(2*k,tempnode);
        changetype(adr,3);
        changekval(adr,k-1);
        changetag(adr,0);

    }
    else
    {
        if(type==2)
        {
            deletesnode(size1,adr);
            deletesnode(size2,bud_adr);
            k=returnkval(bud_adr);
            address=(void*)(bud_adr+(lint)ref_ptr);
            freenode(2*k-1,address);
            node *tempnode;
            tempnode=newnode(address);
            addnode(2*k,tempnode);
            changetype(bud_adr,3);
            changekval(bud_adr,k-1);
            changetag(bud_adr,0);
        }
        else
        {
            deletesnode(size1,adr);
            deletesnode(size2,bud_adr);
            left_adr=findleft(adr,bud_adr);
            k=returnkval(left_adr);
            if(left_adr==bud_adr)
            {
                address=(void*)(left_adr+(lint)ref_ptr);
                freenode(2*k+2,address);
            }
            else
            {
                address=(void*)(bud_adr+(lint)ref_ptr);
                freenode(maxi(2*k-1,0),address);
            }
            node *tempnode;
            address=(void*)(left_adr+(lint)ref_ptr);
            tempnode=newnode(address);
            addnode(2*k+3,tempnode);
            changekval(left_adr,k+2);
            changetag(left_adr,0);
            type=returnstype((1<<k+2),left_adr);
            changetype(left_adr,type);
        }
    }
    recomb++;
}

void checklist()  //check the ASL list if there could be any recombination
{
    int i,s,t,k;
    lint adr,bud_adr;
    node *temp;
    for(i=1;i<55;i++)
    {
        if(ASL[i].next!=NULL)
        {
            temp=ASL[i].next;
            while(temp!=NULL)
            {
                s=ASL[i].size;
                adr=((lint)temp->address)-((lint)ref_ptr);
                t=returntype(adr);
                if(t==1)
                {
                    k=returnkval(adr);
                    bud_adr=adr+(lint)(1<<k);
                    if(avail(i-2,bud_adr)==1)  //if buddy is available then add recombine it
                    {
                        combine(adr,bud_adr);
                        freenode(i,temp->address);
                    }
                }
                else
                {
                    if(t==2)
                    {
                        k=returnkval(adr);
                        bud_adr=adr-(lint)(2<<k);
                        if(avail(i+2,bud_adr)==1)
                        {
                            combine(adr,bud_adr);
                            freenode(i,temp->address);
                        }
                    }
                    else
                    {
                        if(s%3==0)
                        {
                            k=returnkval(adr);
                            bud_adr=adr+(lint)(3<<k);
                            int p=maxi(i-3,0);
                            if(avail(p,bud_adr)==1)
                            {
                                combine(adr,bud_adr);
                                freenode(i,temp->address);
                            }
                        }
                        else
                        {
                            k=returnkval(adr);
                            bud_adr=adr-(lint)(3<<k);
                            if(avail(i+3,bud_adr)==1)
                            {
                                combine(adr,bud_adr);
                                freenode(i,temp->address);
                            }
                        }
                    }
                }
                temp=temp->next;
            }
        }
    }
}

void freenode(int p,void *adr)  //remove a node in ASL list with the given address
{
    node *temp1,*temp2;
    temp1=ASL[p].next;
    if(temp1==NULL)
    {
        printf("ASL at index %d is already empty\n",p);
        return;
    }
    else
    {
        while(temp1->address!=adr)
        {
            temp1=temp1->next;
            if(temp1==NULL)
            {
                printf("NO NODE WITH THIS ADDRESS FOUND!!");
                return;
            }
        }
        temp2=temp1->prev;
        if(temp2==NULL)
        {
            ASL[p].next=temp1->next;
            free(temp1);
            if(ASL[p].next!=NULL)
            {
                temp1=ASL[p].next;
                temp1->prev=NULL;
            }
        }
        else
        {
            temp2->next=temp1->next;
            free(temp1);
            temp1=temp2->next;
            if(temp1!=NULL)
            {
                temp1->prev=temp2;
            }
        }

    }
}

int returnsize(lint addr) //return the size by looking into the look up table
{
    int kval,type,size;
    kval=returnkval(addr);
    type=returntype(addr);
    if(type==1||type==2)
    {
        size=(1<<kval);
        return size;
    }
    else
    {
        int x;
        x=(int)(addr>>kval)&(lint)3;
        if(x==3)
        {
            size=(1<<kval);
            return size;
        }
        else
        {
            size=3*(1<<kval);
            return size;
        }
    }
}

void addsnode(int s,lint add,int ty)  //Add node to the splitlist when any split happens
{
    splitnode *temp;
    temp=(splitnode*)malloc(sizeof(splitnode));
    temp->size=s;
    temp->address=add;
    temp->type=ty;
    temp->next=NULL;
    if(parent==NULL)
    {
        parent=temp;
        last=temp;
    }
    else
    {
        last->next=temp;
        last=temp;
    }
}

lint findbuddy(lint adr)  //returns the address of the buddy
{
    int k=returnkval(adr);
    int type=returntype(adr);
    return adr^(type<<k);
}

int returnstype(int si,lint add)  //Return the type of the parent block whenever buddy of type 3 combine
{
    splitnode *temp;
    temp=parent;
    while(temp->size!=si||temp->address!=add)
    {
        temp=temp->next;
    }
    //if not found
    return temp->type;
}

int avail(int i,lint bud)  //Returns if buddy is free or not
{
    node *temp1;
    temp1=ASL[i].next;
    if(temp1==NULL)
    {
        return 0;
    }
    else
    {
        void *buddy;
        buddy=(void*)(bud+(lint)ref_ptr);
        while(temp1->address!=buddy)
        {
            temp1=temp1->next;
            if(temp1==NULL)
            {
                return 0;
            }
        }
        return 1;
    }
}

void deletesnode(int si,lint add) //delete the node of the splitlist
{
    if(parent->size==si&&parent->address==add)
    {
        free(parent);
        parent=NULL;
        last=NULL;
        return;
    }
    splitnode *temp1,*temp2;
    temp1=parent;
    while(temp1->size!=si||temp1->address!=add)
    {
        temp2=temp1;
        temp1=temp1->next;
        if(temp1==NULL)
        {
            printf("NO NODE WITH THIS SIZE AND ADDRESS FOUND!!\n");
            return;
        }
    }
    temp2->next=temp1->next;
    if(last==temp1)
    {
        last=temp2;
    }
    free(temp1);
}

lint findleft(lint addr,lint buddy)  //When two type 3 buddies recombine, find the left to do the operation
{
    int size1,size2;
    size1=returnsize(addr);
    size2=returnsize(buddy);
    if(size1>size2)
    {
        return addr;
    }
    else
    {
        return buddy;
    }
}

int findindex(lint add)  //returns buddy index in ASL list
{
    int k,t,s;
    t=returntype(add);
    s=returnsize(add);
    k=returnkval(add);
    if(t==1)
    {
        if(k==1)
        {
            return 0;
        }
        else
        {
            return 2*k-3;
        }
    }
    else if(t==2)
    {
        if(k==0)
        {
            return 1;
        }
        else
        {
            2*k+1;
        }
    }
    else
    {
        if(s%3==0)
        {
            if(k==0)
            {
                return 0;
            }
            else
            {
                return 2*k-1;
            }
        }
        else
        {
            return 2*k+2;
        }
    }
}

void freeasl() //Free the ASL list whenever initialize is called in between execution of program
{               //or whenever exiting the program
    int i;
    node *temp1,*temp2;
    for(i=0;i<56;i++)
    {
        temp1=ASL[i].next;
        while(temp1!=NULL)
        {
            temp2=temp1->next;
            free(temp1);
            temp1=temp2;
        }
        ASL[i].next=NULL;
    }
}

void freesplit() //Free all the split nodes if exiting in between program
{
    splitnode *temp;
    last=NULL;
    while(parent!=NULL)
    {
        temp=parent;
        parent=temp->next;
        free(temp);
    }
}
void free_all()
{
    free(ref_ptr);
    free(LUT);
    freeasl();
    freesplit();
}
void* return_ref_ptr()
{
    return ref_ptr;
}
char* return_LUT()
{
    return LUT;
}
void printstats()
{
    printf("SHOWING STATS UPTIL NOW:\nTOTAL NUMBER OF ALLOCATION REQUEST = %d\n",no_alloc);
    printf("TOTAL NUMNBER OF DEALLOCATION REQUEST = %d\n",no_dealloc);
    if(tot_req_size==0)
    {
        printf("INTERNAL FRAGMENTATION IS 0 bytes\n");
        printf("NUMBER OF BUDDIES SPLITS = 0\nNUMBER OF RECOMBINATION = 0\n");
        return;
    }
    printf("NUMBER OF BUDDIES SPLITS = %d\nNUMBER OF RECOMBINATION = %d\n",splits,recomb);
    double int_frag;
    int_frag=(double)(tot_alloc_size-tot_req_size)/(double)tot_req_size;
    printf("INTERNAL FRAGMENTATION IS %ld bytes (%lf %)",tot_alloc_size-tot_req_size,int_frag*100);
}
