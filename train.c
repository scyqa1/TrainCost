// 20032255  scyqa1  Qichen An

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#define min(x,y) (x<y?x:y)

// in order to exit when user just pressed enter(just get one char as user just type one char)   
// similar to prompt2 from lecture     
char *prompt(const char*mesg, const int limit)
{
    char *name;
    name = malloc(sizeof(char) * limit);             /* the length of name is no more than 99 characters, the same below */
    if(name == NULL)
    {
        return NULL;
    }

    printf("%s",mesg);

    int pos = 0;
    while(pos < limit-1)
    {
        char ch;
        scanf("%c", &ch);
        if(ch == '\n') // User pressed return/enter.
        {
        name[pos] = '\0';  // Terminate the string here.
        break;
        }
        name[pos] = ch;
        pos++;
    }

    name[limit-1] = '\0'; // Make sure last element is '\0'.
   
    return name;
}

// define structure Station which will be used to data of the first line of the file(endstation)
typedef struct
{
    char name[100];
}Station;
 
int amount;   /* the number of end station*/

// load data of the first line of the file, and create an array of structure Station, finally return a pointer to the array or NULL on error
Station *endstation(FILE *fp)
{
    Station *sta;
    amount=1;
    sta = malloc(sizeof(Station) * amount);
    if(sta == NULL)
    {
        printf("Unable to allocate memory.\n");
        fclose(fp);
        exit(3);
    }

    int pos = 0;    /* used to update strings of name */
    char ch;
    
    ch=fgetc(fp); 
    if(ch!=',')
    {
        free(sta);
        fclose(fp);
        printf("Invalid distances file.\n");
        exit(2);                                    /* if the file is invalid that start of file is not empty, exit */
    } 

    ch=fgetc(fp);
    if(ch==','||ch=='\n')
    {
        free(sta);
        fclose(fp);
        printf("Invalid distances file.\n");
        exit(2);                                    /* if the file is invalid that station name is empty, exit */
    }  
    while(ch != '\n')
    {
        if(ch == ',')
        {
            // expand
            ((sta+amount-1)->name)[pos] = '\0';  /* terminate strings */
            amount++;
            pos=0;
            Station *tmp = malloc(sizeof(Station) * amount);
        if(tmp == NULL)
        {
            free(sta);   /* free the current buffer before giving up. */
            printf("Unable to allocate memory.\n");
            fclose(fp);
            exit(3);
        }
        for(int i = 0; i < amount-1; i++)
            {
                for(int j=0; j < 100; j++)          
                ((tmp+i)->name)[j] = ((sta+i)->name)[j];
        }
            free(sta);
        sta = tmp;
            ch=fgetc(fp);
            if(ch==','||ch=='\n')
            {
                free(sta);
                fclose(fp);
                printf("Invalid distances file.\n");
                exit(2);                                    /* if the file is invalid that station name is empty, exit */
            }
            continue;
        }
        else
        {
            ((sta+amount-1)->name)[pos]=ch;
            pos++;
            ch=fgetc(fp);
        }
    }   
    ((sta+amount-1)->name)[pos] = '\0';  /* terminate the last string */
     
    return sta;
}

// define a single list whose first element is arc tail, and the other elements are arc head which is pointed to
struct arc
{
    char name[100];
    long int distance;   /* the distance before arc tail and arc head */
    struct arc *next;
};
typedef struct arc Arc;

int stop_read;       /* to control reading the file and check if the the end of file is valid */ 

// Given a list, free all the elements of the list and set the start to NULL
// This function is used when invalid file occurs
// similar to function from lecture
void list_free(Arc **start)
{
   Arc *cur = *start;
   while(cur != NULL)
   {
        Arc *temp = cur->next;
        free(cur);
        cur = temp;
   }
   *start = NULL;
}

// load data of each line after the first line, create a list, and return a pointer to the list or NULL on error 
Arc *creat_list(FILE *fp, Station *sta)
{
    Arc *head;         
    Arc *p1,*p2;
    int pos=0;
    char ch;

    head=p1=p2=malloc(sizeof(Arc));
    if(p1 == NULL)
    {
        printf("Unable to allocate memory.\n");
        free(sta);              /* free the current buffer before giving up. */
        fclose(fp);
        exit(3);
    }

    // read the station name at the begining of each line
    ch=fgetc(fp);
    if(ch==',')
    {
        free(sta);
        free(p1);
        fclose(fp);
        printf("Invalid distances file.\n");
        exit(2);                                    /* if the file is invalid that station name is empty, exit */
    }
    if(ch=='\n'||ch==EOF)          /* stop function creat when all useful data is read */ 
    {
        stop_read=1;
        free(p1);
        return NULL;
    }
    else
    stop_read=0;                 
    while(ch!=',')
    {
        (p1->name)[pos]=ch;
        pos++;
        ch=fgetc(fp);  
        if(ch=='\n') 
        { 
            printf("Invalid distances file.\n");
            free(sta);
            free(p1);
            fclose(fp);
            exit(2);                             /* if the file is invalid that the number of comma is not correct, exit */
        }     
    } 
    (p1->name)[pos]='\0';
    p1->distance=0;

    int comma=0;        /* used to check is the distance file is invalid according to the amount endstation and commas */

    while(ch!='\n')     /* stop when jump to new line */
    {
        ch=fgetc(fp);
        if(ch==','||ch=='\n')
        {
            comma++;
            continue;
        }
        else
        {                      
            p2=p1;
            p1=malloc(sizeof(Arc));
            if(p1 == NULL)
            {
                printf("Unable to allocate memory.\n");
                free(sta);                   /* free the current buffer before giving up. */
                list_free(&head);                       /* free the current buffer before giving up. */
                fclose(fp);
                exit(3);                
            }

            pos=0;
            char dis[100];
            while(ch!=',' && ch!='\n')    /* if the cell between two commas or the end of line and a comma do not have a value, do not create arc head */
            {
                if(ch<'0'||ch>'9')
                {
                    printf("Invalid distances file.\n");
                    free(sta);
                    free(p1);
                    list_free(&head);
                    fclose(fp);
                    exit(2);                                 /* if the file is invalid that the value is not digit, exit */
                }
                dis[pos]=ch;
                pos++;
                ch=fgetc(fp);
            }
            dis[pos]='\0';
            p1->distance=strtol(dis,NULL,10);               /* the distance is stored in decimal base */
            if(p1->distance==0)
            {
                printf("Invalid distances file.\n");
                free(sta);
                free(p1);
                list_free(&head);
                fclose(fp);
                exit(2);                                     /* if the file is invalid that the value is 0('-'has been avoid in the previous step), exit */
            }
            strcpy(p1->name, (sta+comma)->name);
            p2->next=p1;
            
            comma++;
        }  
    }
    p1->next=NULL;

    if(head==NULL)
    {
        head=p1;                   /* the condition that the station dosen't lead any station */
    }

    if(comma!=amount)
    {
        printf("Invalid distances file.\n");
        free(sta);
        list_free(&head);
        fclose(fp);
        exit(2);                                     /* if the file is invalid that the number of comma is not correct, exit */
    }
    return head;
}

int num;    /* the number of the start station at the first column */

// Given n(n=num) list, free all the elements and set the head pointer to NULL
void freelists(Arc **head, int number)
{
    for(int i=0;i<number;i++)
    {
        Arc *cur = head[i];
        while(cur != NULL)
        {
            Arc *temp = cur->next;
            free(cur);
            cur = temp;
        }
        head[i] = NULL;
    }
    free(head);
}

// bulid the graph, create list row by row, and connect lists. return a pointer of the first list or NULL for stop
Arc **graph(FILE *fp, Station *sta)
{
    num=1;
    Arc **head;
    Arc *test;
    head=malloc(sizeof(Arc *) * num);
    if(head == NULL)
    {
        printf("Unable to allocate memory.\n");
        free(sta);                   /* free the current buffer before giving up. */
        fclose(fp);
        exit(3);  
    }

    head[num-1]=creat_list(fp,sta);           /* a pointer to the first list*/

    while(1)
    {  
        char ch;
        num++;
        Arc **tmp=malloc(sizeof(Arc *) * num);
        if(tmp == NULL)
        {
            printf("Unable to allocate memory.\n");
            freelists(head,num-1);                  /* free the current buffer before giving up. */
            free(sta);                   /* free the current buffer before giving up. */
            fclose(fp);
            exit(3);  
        }
        for(int i=0;i<num-1;i++)
        {
            tmp[i]=head[i];
        }
        test=creat_list(fp,sta);
        if(!stop_read)
        {
        tmp[num-1]=test; 
        free(head);
        head=tmp; 
        }
        else
        {
            free(tmp);
            while((ch=fgetc(fp))!=EOF)
            {               
                if(ch!='\n')
                {
                    printf("Invalid distances file.\n");
                    free(sta);
                    freelists(head,num-1);
                    fclose(fp);
                    exit(2);                              /* if the file is invalid that the end of file is invalid, exit */
                } 
            }
            break;
        }               
    }
    num--;
    
    return head;  
}

// define a structure Dijkstra which will be used to calculate the minimum distance
typedef struct
{
    char name[100];
    int mindis;                  /* a temporary minimum distance */
    int ismin;                   /* if mindis the final minimum distance, set ismin 1    */
    char pass[100];              /* the station that go through the minimum path need to pass */
}Dijkstra;   

void path(Dijkstra *route, Station *sta, Arc **head, char *star, char *end)
{
        
    for(int i=0;i<amount;i++)
    {
        strcpy(route[i].name,(sta+i)->name);        
        route[i].mindis=0;
        route[i].ismin=0;
        route[i].pass[0]='\0';
    }
    // according to the start station and its departure single list, fill known number of mindis
    for(int i=0;i<num;i++)
    {
        if(!strcmp(star,(*(head+i))->name))
        {
            Arc *p=*(head+i);
            do
            {
                for(int j=0;j<amount;j++)
                {
                    if(!strcmp(route[j].name,p->name))
                    {
                        route[j].mindis=p->distance;
                    }
                }
                p=p->next;
            }while(p!=NULL);
        }
    }
    // complete the Dijkstra algorithm
    while(1)
    {
        // search for the minumum value of undetermined mindis
        Dijkstra *a=route;
        for(int i=1;i<amount;i++)
        {
            if(route[i].ismin==1)
            continue;
            if(a->mindis==0 || a->ismin==1)
            a=&route[i];
            else
            {
                if(min(a->mindis,route[i].mindis)==route[i].mindis && route[i].mindis!=0)
                a=&route[i];
            }
        }
         if(a->mindis==0||a->ismin==1)
         break;                               /* if all mindis is determined or there is no path, stop repeating */

        a->ismin=1;        /* determine the value is minimum distance of one station */
        // according to the station, update other undetermined station' minsid
        for(int i=0;i<num;i++)
        {
            if(!strcmp(a->name,(*(head+i))->name))
            {
                Arc *p=(*(head+i))->next;
                while(p!=NULL)
                {
                    for(int j=0;j<amount;j++)
                    {
                        if(!strcmp(route[j].name,p->name))
                        {
                            if(!strcmp(star,p->name))
                            continue;
                            if(route[j].mindis==0)
                            {
                                route[j].mindis=a->mindis+p->distance;
                                strcpy(route[j].pass,a->name);
                            }
                            else
                            {
                                int d=route[j].mindis;
                                route[j].mindis=min(route[j].mindis , a->mindis+p->distance);
                                if(d==route[j].mindis)
                                continue;
                                else
                                strcpy(route[j].pass,a->name);
                            }
                        }
                    }
                    p=p->next;
                }
            }
        }
    }
}

// output via station in reverse order according to pass[100], return the number of via station
int printstation(int n, char *station, Dijkstra *pointer)
{
    char tmp[100];

    for(int i=0;i<amount;i++)
    {
        if(!strcmp(station,pointer[i].name))
        strcpy(tmp,pointer[i].pass);
    }
    if(strlen(tmp)!=0) 
    {
        n=printstation(n,tmp,pointer);  
        printf("%s\n",tmp);
        n++;
    }
    return n;
}  

// output the result
void output(Dijkstra *route, char *star, char *end)
{ 
    for(int i=0;i<amount;i++)
    {
        if(!strcmp(end,route[i].name))
        {
            int viastation=0;
            if(route[i].mindis==0)
            {
                printf("No possible journey.\n");
                    
            }
            else
            {
                printf("From %s\n",star);
                for(int i=0;i<amount;i++)
                {
                    if(!strcmp(end,route[i].name))
                    {
                        if(strlen(route[i].pass)==0)
                        printf("direct\n");
                        else
                        {
                            printf("via\n");
                            Dijkstra *pointer=route;
                            viastation=printstation(viastation,route[i].name,pointer);       /* output the via stations and receive the number of via stations */
                        }
                    }
                }
                printf("To %s\n",end);
                printf("Distance %d km\n",route[i].mindis);
                float cost=route[i].mindis*1.2+viastation*25;
                printf("Cost %d RMB\n",(int)ceil(cost));                    
            }
        }
    }
}

void min_distance(FILE *fp, Station *sta, Arc **head)
{
    char *star;
    char *end;
    while(1)
    {
        int control;          /* used to control loop, for example: invalid start station or end station name or same name of start station and end station */ 

        star=prompt("Start station: ",100);
        if(star == NULL)
        {
            printf("Unable to allocate memory.\n");
            free(sta);                   /* free the current buffer before giving up. */
            freelists(head,num);
            fclose(fp); 
            exit(3);
        }
        // stop repeating when user enter nothing
        if(strlen(star)==0)
        {
            free(star);
            break;
        }
        // seek if input station is one of departure stations
        for(int i=0;i<num;i++)
        {
            if(!strcmp(star,(*(head+i))->name))
            {
                control=0;
                break;
            }
            else
            control=1;
        }
        if(control)
        {
            printf("No such station.\n");
            continue;
        }

         
        end=prompt("End station: ",100);
        if(end == NULL)
        {
            printf("Unable to allocate memory.\n");
            free(sta);                   /* free the current buffer before giving up. */
            freelists(head,num);
            free(star);
            fclose(fp); 
            exit(3);
        }
        // seek if input station is one of arrival stations
        for(int i=0;i<amount;i++)
        {
            if(!strcmp(end,(sta+i)->name))
            {
                control=0;
                break;
            }
            else
            control=1;
        }
        if(control)
        {
            printf("No such station.\n");
            continue;
        }   
        // check if start station is same as end station
        if(!strcmp(star,end))
        {
            printf("No journey, same start and end station.\n");
            control=1;
        }
        else
        control=0;

        if(control)
        {
            continue;
        } 

        // create an array of structure Dijkstra
        Dijkstra route[amount];
        path(route, sta, head, star, end);

        // output the result according to Dijkstra structure
        output(route,star,end);

        free(star);
        free(end);           
    }
}

int main(int argc, char *argv[])
{
    if(argc!=2)
    {
        printf("Invalid command line arguments. Usage: train <disances.txt>\n");
        return 4;
    }
    FILE *fp=fopen(argv[1],"r");
    if(fp==NULL)
    {
        perror("Cannot open file.");
        return 1;
    }

    // receive a pointer to the arrival stations 
    Station *sta = endstation(fp);          /* if unable to allocate memory, close file and exit in the function */

    // receive a pointer to the departure stations and each station is head of a sigle list
    Arc **head=graph(fp, sta);             /* if unable to allocate memory, free, close file and exit in the function */    

    min_distance(fp, sta, head);

    fclose(fp);
    free(sta);
    freelists(head,num);  

    return 0;  
}