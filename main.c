#include "simlib.h"

/**
 * @brief defining EVENT TYPE
 * 
 */

#define EVENT_ADD_QUEUE_1 8
#define EVENT_ADD_QUEUE_2 9
#define EVENT_ADD_QUEUE_3 10
#define EVENT_UNLOAD 11
#define EVENT_LOAD 12
#define EVENT_BUS_LEAVE 13
#define EVENT_DEPART 14
#define EVENT_END_SIMUL 0

#define QUEUE_GATE_1 1
#define QUEUE_GATE_2 2
#define QUEUE_GATE_3 3
#define QUEUE_BUS 4

#define LOAD_MIN 15
#define LOAD_MAX 25
#define WAIT_IN_STATION 5

#define UNLOAD_MIN 16
#define UNLOAD_MAX 24

#define SAMPST_DELAY_GATE_1 1
#define SAMPST_DELAY_GATE_2 2
#define SAMPST_DELAY_GATE_3 3

#define SAMPST_QUEUE_GATE_1 4
#define SAMPST_QUEUE_GATE_2 5
#define SAMPST_QUEUE_GATE_3 6

#define SAMPST_SEAT_BUS 7

#define STREAM_INTERARRIVAL_1 1
#define STREAM_INTERARRIVAL_2 2
#define STREAM_INTERARRIVAL_3 3
#define STREAM_UNLOADING 4
#define STREAM_LOADING 5
#define STREAM_RANDOM_GATE 6

#define BUS_SPEED 30.0
#define NUM_STATIONS 3

short sampst_delay_gate[NUM_STATIONS + 1],
      sampst_queue_gate[NUM_STATIONS + 1];

int num_stations,
    current_station,
    isBusArrived,
    isBusLeaving,
    isReadyToLoad;
double  mean_interarrival[NUM_STATIONS + 1],
        routeTime[NUM_STATIONS + 1],
        length_simulation,
        prob_stations_1_2[2 + 1];

void init_model();
void unload();
void load();
void depart();
void arrive(int current_gate);
void flagBusLeaving();

void init_model(){
  /**
   * - initiate the passenger incoming to Station
   * - simulation starts from Station 3
   * - initiate the array for easy access to sampst
   */
  current_station = 3;
  isBusArrived = 0;
  isBusLeaving = 0;
  isReadyToLoad = 0;

  sampst_delay_gate[1] = SAMPST_DELAY_GATE_1;
  sampst_delay_gate[2] = SAMPST_DELAY_GATE_2;
  sampst_delay_gate[3] = SAMPST_DELAY_GATE_3;

  sampst_queue_gate[1] = SAMPST_QUEUE_GATE_1;
  sampst_queue_gate[2] = SAMPST_QUEUE_GATE_2;
  sampst_queue_gate[3] = SAMPST_QUEUE_GATE_3;
  /**
   * @brief routeTime[1] = mean time travelled from Station 1 to 2, and so on
   * 
   */
  
  routeTime[1] = (1.0 / BUS_SPEED) * 3600;
  routeTime[2] = (4.5 / BUS_SPEED) * 3600;
  routeTime[3] = (4.5 / BUS_SPEED) * 3600;

  mean_interarrival[1] = (double) (3600 / 14);
  mean_interarrival[2] = (double) (3600 / 10);
  mean_interarrival[3] = (double) (3600 / 24);

  event_schedule(sim_time + expon(mean_interarrival[1], STREAM_INTERARRIVAL_1), EVENT_ADD_QUEUE_1);
  event_schedule(sim_time + expon(mean_interarrival[2], STREAM_INTERARRIVAL_2), EVENT_ADD_QUEUE_2);
  event_schedule(sim_time + expon(mean_interarrival[3], STREAM_INTERARRIVAL_3), EVENT_ADD_QUEUE_3);
}

void arrive(int current_gate){
  /**
   * @brief Schedule next arrival
   * 
   */
    event_schedule(sim_time + expon(mean_interarrival[current_gate], current_gate), current_gate);

  // Insert passenger into queue
  // ...

  /**
   * 
   * int gate_destination = random_integer(<isi probabilities disini>, 2);
   * queueElement q_temp = createElement(sim_time, current_gate, gate_destination);
   * listInsert(q_temp, queue[current_gate]);
   * popQueue(<statiun destination>, queue);
   * code di atas akan mengganti 2 baris di bawah ini
   */
  transfer[1] = sim_time;
  list_file(LAST, QUEUE_GATE_1);
}

void depart(){
  /**
   * Set to the next station
   * Set the bus haven't arrived to Next station
   * Schedule event for next arrival
   */
  current_station = (current_station % 3) + 1;
  isBusArrived = 0;
  event_schedule(sim_time + routeTime[current_station], EVENT_UNLOAD);
}

void unload(){
  /**
   *  - Check if the bus just arrived,
   *  - Set event for the next leaving station
   *
   */
  if (isBusArrived == 0){
    event_schedule(WAIT_IN_STATION * 60, EVENT_BUS_LEAVE);
    isBusArrived = 1;

    // Update stats for Number of Queue in each Station
    sampst(list_size[current_station], sampst_queue_gate[current_station]);
  }
  
  // Check if any passenger in bus to be unloaded in this bus
  // Check if time < 5 min
  if (isBusLeaving != 1){
    // check if corresponding station has queue
    // remove the queue with POP, and push BUS queue
    if (list_size[QUEUE_BUS] > 0 && passengerOnQueue(current_station, queue_on_bus)){

      // CODE FOR UNLOADING PASSENGER OFF THE BUS
      // ...
      list_remove(FIRST, QUEUE_BUS);

      // Update Stats HERE
      // ...
      
    }

    // check if there's more queue, schedule event
    if (passengerOnQueue(current_station, queue_on_bus)){
      event_schedule(sim_time + (double) uniform(UNLOAD_MIN, UNLOAD_MAX, STREAM_UNLOADING), EVENT_UNLOAD);
    }
    // else, BUS ready to LOAD
    else {
      event_schedule(sim_time, EVENT_LOAD);
      isReadyToLoad = 1;
    }
  }
  // Bus is leaving, schedule departure
  else{
    event_schedule(sim_time, EVENT_DEPART);
    event_cancel(EVENT_UNLOAD);
  }
}

void load(){
  // check waktu sekarang > next_event_type
  if (isBusLeaving != 1){
    if (queueNotFull(queue_on_bus) && list_size[current_station] > 0){
      // pop queue on station
      // ...
      list_remove(FIRST, current_station);
      
      // push element to bus
      // ...
      list_file(LAST, QUEUE_BUS);
      
      // Update Stats HERE
      // ...

      // Check if there's queue
      if (list_size[current_station] > 0){
        // Schedule for loading
        event_schedule(sim_time + (double) uniform(LOAD_MIN, LOAD_MAX, STREAM_LOADING), EVENT_LOAD);
      }
    }
    // If there's no queue at the moment, trigger wait moment every 2s
    event_schedule(sim_time + (double) 2.0, EVENT_LOAD);
  }
  else{
    event_schedule(sim_time, EVENT_DEPART);
    event_cancel(EVENT_LOAD);
  }
}

void flagBusLeaving(){
  isBusLeaving = 1;
}

int main(){
  init_simlib();

  event_schedule(length_simulation, EVENT_END_SIMUL);
  do{
    timing();

    switch(next_event_type){
      case EVENT_ADD_QUEUE_1:
      case EVENT_ADD_QUEUE_2:
      case EVENT_ADD_QUEUE_3:
        arrive(next_event_type);
        break;
      case EVENT_DEPART:
        depart();
        break;
      case EVENT_UNLOAD:
        unload();
        break;
      case EVENT_LOAD:
        load();
        break;
      case EVENT_BUS_LEAVE:
        flagBusLeaving();
        break;
    }
  }while (next_event_type != EVENT_END_SIMUL);
}


