typedef unsigned long int lint;
typedef struct node //nodes of ASL
{
    struct node *prev;
    void *address;
    struct node *next;
}node;
typedef struct list  //ASL list
{
    int size;
    node *next;
}list;
typedef struct splitnode //contains the details of block which are splitted
{
    int size;
    lint address;
    int type;
    struct splitnode *next;
}splitnode;
void initialize();
void show_asl();
int maxi(int ,int );
void changetag(lint ,int );
void changekval(lint,int );
void changetype(lint ,int );
int returntag(lint );
int returnkval(lint );
int returntype(lint );
int return_free_size();
void* mymalloc(int );
node* newnode(void *);
int addnode(int ,node *);
void splitmemory(int );
int myfree(void *);
void combine(lint ,lint );
void checklist();
void freenode(int ,void *);
int returnsize(lint );
void addsnode(int ,lint ,int );
lint findbuddy(lint );
int returnstype(int ,lint );
int avail(int ,lint );
void deletesnode(int ,lint );
lint findleft(lint ,lint );
int findindex(lint );
void freesplit();
void freeasl();
void free_all();
void* return_ref_ptr();
char* return_LUT();
void printstats();
