#include<stdio.h>
#include <stdlib.h>
#include "header.h"
void main()
{
    initialize();
    if(return_ref_ptr()==NULL||return_LUT()==NULL)
    {
        exit(0);
    }
    int flag=1;
    int size,input,status;
    void *ptr=NULL;
    lint adr;
    int i=10;
    while(flag)
    {
        printf("\n*************************************************\n");
	printf("PRESS:\n1)For allocating memory \n2)For dealloacting memory\n3)For showing ASL\n");
        printf("4)Initializing the chunk again\n5)To show the stats\n6)To exit the program\n");
	printf("*************************************************\n");
        scanf("%d",&input);
        switch(input)
        {
        case 1:
            printf("ENTER THE ALOCATION SIZE ");
            scanf("%d",&size);
            printf("\n");
            if(size<1)
            {
                printf("Please ask for something more than this\nRequest sizes can not be less than 1\n");
                break;
            }
            ptr=mymalloc(size);
            if(ptr==NULL)
            {
                printf("EXTERNAL FRAGMENTATION!!\n");
                printf("ALLOCATION SIZE %d, ",size);
                int x=return_free_size();
                x=(1<<28)-x;
                double ext_frag;
                ext_frag=(double)x/(double)(1<<28);
                ext_frag*=100;
                printf("CURRENTLY ALLOCATED HEAP AREA %lf\n",ext_frag);
                flag=0;
            }
            break;
        case 2:
            printf("ENTER THE DEALLOCATION ADDRESS\n");
            scanf("%lu",&adr);
            void *ptr;
            ptr=(void*)(adr+(lint)return_ref_ptr());
            status=myfree(ptr);
            if(status)
            {
                printf("DEALLOACTION SUCCESSFUL!!!\n");
            }
            break;
        case 3:
            show_asl();
            break;
        case 4:
            initialize();
            if(return_ref_ptr()==NULL||return_LUT==NULL)
            {
                printf("INITIALIZATION FAILED\n");
                flag=0;
            }
            else
            {
                printf("INITIALIZATION SUCESSFUL\n");
            }
            break;
        case 5:
            printstats();
            break;
        case 6:
            flag=0;
            break;
        default :
            printf("Wrong Choice Entered\n");
            i--;
            if(i)
            {
                printf("WARNING!!! The program will exit after %d more wrong entry\n",i);
            }
            else
            {
                printf("EXITING PROGRAM\n");
                flag=0;
            }
            break;
        }
    }
    free_all();
}

