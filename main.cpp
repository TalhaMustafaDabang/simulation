
    #include <cstdlib>
#include <iostream>

#include <stdio.h>
#include <stdlib.h>
#include "simlib.h"


//DIRECTIVES

#define EVENT_ARRIVAL 1
#define EVENT_DEPARTURE 2
#define EVENT_CLOSE_DOORS 3
#define SAMPST_DELAYS 1
#define STREAM_INTERARRIVAL 1
#define STREAM_SERVICE 2


using namespace std;


//GLOBAL VARIRABLES /NON-SIMLIB VARIABLES/
int min_tellers,max_tellers,num_tellers,shortest_length,shortest_queue;

float mean_interarrival,mean_service,length_doors_open;

//File *infile,*outfile;




//NON-SIMLIB FUNCTIONS

void arrive(void);
void departure(int teller);
void jockey(int teller);
void report(void);




void main()
{

infile = fopen("mtbank.in","r");
outfile = fopen("mtbank.out","w");

fscanf(infile,"%d %d %f %f %f",&min_tellers,&max_tellers,&mean_interarrival,&mean_service,&length_doors_open);

fprintf(outfile,"Multi Tellers Bank with separate queues & jockying \n\n ");

fprintf(outfile,"Number of teller : %16d to %3d \n\n",min_tellers,max_tellers);

fprintf(outfile,"Mean Interarrival Time %11.3f minutes \n\n",mean_interarrival);

fprintf(outfile,"Mean Service Time %16.3f minutes \n\n",mean_service);

fprintf(outfile,"Bank Closes After %16.3f minutes \n\n",length_doors_open);

for(num_tellers=min_tellers;num_tellers<=max_tellers;++num_tellers)
{

//INITIALIZE SIMLIB
init_simlib();
maxatr=4;
event_schedule(expon(mean_interarrival,STREAM_INTERARRIVAL),EVENT_ARRIVAL);
event_schedule(60*length_doors_open,EVENT_CLOSED_DOORS);
while(list_size[LIST_EVENT]!=0)
{
       timing();
       switch(next_event_time)
       {
        case EVENT_ARRIVAL:
        arrive();
        break;

        case EVENT_DEPARTURE:
        depart((int)transfer[3]);
        break;

        case EVENT_CLOSED_DOORS:
        event_cancel(EVNET_ARRIVAL);
        break;



       }

report();

}

}

fclose(infile);
fclose(outfile);

}


void arrive(void)
{
 int teller;
 event_schedule(sim_time+expon(mean_interarrival,STREAM_INTERARRIVAL_TIME),EVENT_ARRIVAL);
 for(teller=1;teller<=num_tellers;++teller)
 {
           if(list_size[num_tellers+teller]==0)
           {
            sampst(0.0,SAMPST_DELAYS);
            list_files(FIRST,num_tellers+teller);
            trnasfer[3]=teller;
            event_schedule(sim_time+expon(mean_service,STREAM_SERVICE),EVENT_DEPARTURE);
            return;
           }

 }

 shortest_length=list_size[1];
 shortest_queue=1;
 for(teller=2;teller<=num_tellers;++teller)
 {
  if(list_size[teller]<shortest_length){
      shortest_length=list_size[teller];
      shortest_queue=teller;

      }

   transfer[1]=sim_time;
   list_file(LAST,shortest_queue);


 }
}//arrive end



void depart(int teller)
{
     if(list_size[teller]==0)
     {
      list_remove(FIRST,num_tellers+teller);
     }
     else
     {
         list_remove(FIRST,teller);
         sampst(sim_time-transfer[1],SAMPST_DELAYS);
         transfer[3]=teller;
         event_schedule(sim_time+expon(mean_service,STREAM_SERVICE),EVENT_DEPARTURE);

     }

     jockey(teller);

 }//depart end



void jockey(int teller)
{
    int jumper,min_distance,ni,nj,other_teller,distance;
    jumper=0;
    min_distance=1000;
    ni=list_size[teller]+list_size[num_tellers+teller];
    for(other_teller=1;other_teller<=num_tellers;++other_teller)
    {
        nj=list_size[other_teller]+list_size[num_tellers+other_teller];
        distance=abs(teller-other_teller);
        if((other_teller!=teller)&&(nj>ni+1)&&(distance<min_distance))
        {
            jumper=other_teller;
            min_distance=distance;
        }
    }

    if(jumper>0)
    {
        list_remove(LAST,jumper);
        if(list_size[num_tellers+teller]>0)
        {
            list_file(LAST,teller);

        }
        else{
            sampst(sim_time-transfer[1],SAMPST_DELAYS);
            list_file(FIRST,num_tellers+teller);
            transfer[3]=teller;
            event_schedule(sim_time+expon(mean_service,STREAM_SERVICE),EVENT_DEPARTURE);

        }
    }


}//jockey end


void report(void)
{
    int teller;
    float avg_num_in_queue;
    avg_num_in_queue=0.0;
    for(teller=1;teller<=num_tellers;++teller)
    {
        avg_num_in_queue+=filest(teller);
        fprintf(outfile,"\n\n with %2d tellers, avrage number in queue = %10.3f",num_tellers,avg_num_in_queue);
        fprintf(outfile,"\n\n delays in queue in minutes: \n");
        out_sampst(outfile,SAMPST_DELAYS,SAMPST_DELAYS);
    }
}//report

    cout << "Hello world!" << endl;
    return 0;
}
