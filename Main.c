/* External definitions for single-server queueing system. */

#include <stdio.h>  
#include <math.h>
#include "lcgrand.h"  /* Header file for random-number generator. */

#define Q_LIMIT 100  /* Limit on queue length. */
#define BUSY      1  /* Mnemonics for server's being busy */
#define IDLE      0  /* and idle. */

//Calculating and defining the gap between packets generated
#define AUDIO_PACKET_GAP 120 * 8/(64 * 1024) 
#define VIDEO_PACKET_GAP 1000 * 8/(384 * 1024) 
#define DATA_PACKET_GAP 583 * 8/(256 * 1024)
// Defining means
#define mean_on_audio 0.36
#define mean_off_audio 0.64
#define mean_on_video 0.33
#define mean_off_video 0.73
#define mean_on_data 0.35
#define mean_off_data 0.65 

int   next_event_type, num_custs_delayed, num_delays_required, num_events,
      num_in_q, server_status;
float area_num_in_q, area_server_status, mean_interarrival, mean_service,
      sim_time, time_arrival[Q_LIMIT + 1], time_last_event, time_next_event[3],
      total_of_delays;
// on_off arrays holds both on and off timings staring with on and the next is off and on ,so on.
float *on_off_audio = malloc(sizeof(float) * 100);
float *on_off_video = malloc(sizeof(float) * 100);
float *on_off_data = malloc(sizeof(float) * 100);
float *packet_timings_audio = malloc(sizeof(float) * 1000000);
float *packet_timings_video = malloc(sizeof(float) * 100000);
float *packet_timings_data = malloc(sizeof(float) * 100000);;

FILE  *infile, *outfile;

void on_off_generate(void);
void packets_generate(void);
void  initialize(void);
void  timing(void);
void  arrive(void);
void  depart(void);
void  report(void);
void  update_time_avg_stats(void);
float expon(float mean);


main()  /* Main function. */
{
    /* Open input and output files. */

    infile  = fopen("mm1.in",  "r");
    outfile = fopen("mm1.out", "w");

    /* Specify the number of events for the timing function. */

    num_events = 2;

    /* Read input parameters. */

    fscanf(infile, "%f %f %d", &mean_interarrival, &mean_service,
           &num_delays_required);

    /* Write report heading and input parameters. */

    fprintf(outfile, "Single-server queueing system\n\n");
    fprintf(outfile, "Mean interarrival time%11.3f minutes\n\n",
            mean_interarrival);
    fprintf(outfile, "Mean service time%16.3f minutes\n\n", mean_service);
    fprintf(outfile, "Number of customers%14d\n\n", num_delays_required);

    /* Initialize the simulation. */

    initialize();

    /* Run the simulation while more delays are still needed. */

    while (num_custs_delayed < num_delays_required)     
    {
        /* Determine the next event. */

        timing();

        /* Update time-average statistical accumulators. */

        update_time_avg_stats();

        /* Invoke the appropriate event function. */

        switch (next_event_type) 
        {
            case 1:
                arrive();
                break;
            case 2:
                depart();
                break;
        }
    }

    /* Invoke the report generator and end the simulation. */

    report();

    fclose(infile);
    fclose(outfile);

    return 0;
}

void on_off_generate(){
    // Generating ON/OFF times for Audio
    for (int i = 0; i < 100; ++i){
        if (i != 0){
            if (i%2 == 0){
                on_off_audio[i] = on_off_audio[i - 1] + expon(mean_on_audio);    
            } else {
                on_off_audio[i] = on_off_audio[i - 1] + expon(mean_off_audio);
            }
        } else {
            on_off_audio[i] = 0.0 + expon(mean_on_audio);
        }
    }
    // Generating ON/OFF times for Video
    for (int i = 0; i < 100; ++i){
        if (i != 0){
            if (i%2 == 0){
                on_off_video[i] = on_off_video[i - 1] + expon(mean_on_video);    
            } else {
                on_off_video[i] = on_off_video[i - 1] + expon(mean_off_video);
            }
        } else {
            on_off_video[i] = 0.0 + expon(mean_on_video);
        }
    }
    // Generating ON/OFF times for Data
    for (int i = 0; i < 100; ++i){
        if (i != 0){
            if (i%2 == 0){
                on_off_data[i] = on_off_data[i - 1] + expon(mean_on_data);    
            } else {
                on_off_data[i] = on_off_data[i - 1] + expon(mean_off_data);
            }
        } else {
            on_off_data[i] = 0.0 + expon(mean_on_data);
        }
    }
}

void packets_generate(){
    float time = 0.0;
    //generate audio packets from on/off timings
    for(int i = 0, i < 100, i = i + 2){
        while(time < on_off_audio[i+1]){
            packet_timings_audio
        }
    }
}




void initialize(void)  /* Initialization function. */
{
    /* Initialize the simulation clock. */

    sim_time = 0.0;

    /* Initialize the state variables. */

    server_status   = IDLE;
    num_in_q        = 0;
    time_last_event = 0.0;

    /* Initialize the statistical counters. */

    num_custs_delayed  = 0;
    total_of_delays    = 0.0;
    area_num_in_q      = 0.0;
    area_server_status = 0.0;

    /* Initialize event list.  Since no customers are present, the departure
       (service completion) event is eliminated from consideration. */

    time_next_event[1] = sim_time + expon(mean_interarrival);
    time_next_event[2] = 1.0e+30;
}


void timing(void)  /* Timing function. */
{
    int   i;
    float min_time_next_event = 1.0e+29;

    next_event_type = 0;

    /* Determine the event type of the next event to occur. */

    for (i = 1; i <= num_events; ++i)
        if (time_next_event[i] < min_time_next_event)
        {   
            min_time_next_event = time_next_event[i];
            next_event_type     = i;
        }

    /* Check to see whether the event list is empty. */

    if (next_event_type == 0)
    {
        /* The event list is empty, so stop the simulation. */

        fprintf(outfile, "\nEvent list empty at time %f", sim_time);
        exit(1);
    }

    /* The event list is not empty, so advance the simulation clock. */

    sim_time = min_time_next_event;
}


void arrive(void)  /* Arrival event function. */
{
  // to be completed by students
}


void depart(void)  /* Departure event function. */
{
  // to be completed by studnts
}


void report(void)  /* Report generator function. */
{
    /* Compute and write estimates of desired measures of performance. */

    // to be completed by students
}


void update_time_avg_stats(void)  /* Update area accumulators for time-average
                                     statistics. */
{
    float time_since_last_event;

    /* Compute time since last event, and update last-event-time marker. */

    time_since_last_event = sim_time - time_last_event;
    time_last_event       = sim_time;

    /* Update area under number-in-queue function. */

    area_num_in_q      += num_in_q * time_since_last_event;

    /* Update area under server-busy indicator function. */

    area_server_status += server_status * time_since_last_event;
}


float expon(float mean)  /* Exponential variate generation function. */
{
    /* Return an exponential random variate with mean "mean". */

    // to be completed by students
}

