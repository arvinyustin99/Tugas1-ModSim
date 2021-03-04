#include "simlib.h"

/**
 * @brief defining EVENT TYPE
 * 
 */

#define EVENT_ADD_QUEUE_1 8
#define EVENT_ADD_QUEUE_2 9
#define EVENT_ADD_QUEUE_3 10
#define EVENT_ARRIVAL_1 1
#define EVENT_DEPART_1 2
#define EVENT_ARRIVAL_2 3
#define EVENT_DEPART_2 4
#define EVENT_ARRIVAL_3 5
#define EVENT_DEPART_3 6

#define EVENT_END_SIMUL 0

#define STREAM_INTERARRIVAL 1
#define STREAM_ARRIVAL_AT_GATE 2
#define NUM_STATIONS 3

int num_stations,
    current_station,
    route[NUM_STATIONS + 1];
double  mean_interarrival[NUM_STATIONS + 1],
        length_simulation,
        prob_stations_1_2[2 + 1];

void arrive(int current_gate){
    event_schedule(sim_time + expon(mean_interarrival[1]), EVENT_ADD_QUEUE_1);
    event_schedule(sim_time + expon(mean_interarrival[2]), EVENT_ADD_QUEUE_2);
    event_schedule(sim_time + expon(mean_interarrival[3]), EVENT_ADD_QUEUE_3);
    
}

int main(){
  init_simlib();


  event_schedule(length_simulation, EVENT_END_SIMUL);
  do{
    timing();

    switch(next_event_type){
      case EVENT_QUEUE_1:
      case EVENT_QUEUE_2:
      case EVENT_QUEUE_3:
        arrive(next_event_type);
        break;
      case EVENT_LOAD:
        load(current_station);
        break;
      case EVENT_UNLOAD:
        unload(current_station);
        break;
    }
  }while (next_event_type != EVENT_END_SIMUL);
}


