#include "simlib.h"

#define EVENT_ADD_QUEUE_1 8
#define EVENT_ADD_QUEUE_2 9
#define EVENT_ADD_QUEUE_3 10
#define EVENT_UNLOAD 11
#define EVENT_LOAD 12
#define EVENT_BUS_LEAVE 13
#define EVENT_DEPART 14
#define EVENT_ADJUST 15
#define EVENT_END_SIMUL 0

#define QUEUE_GATE_1 1
#define QUEUE_GATE_2 2
#define QUEUE_GATE_3 3
#define QUEUE_BUS 4

#define LIST_BUS_DEST_1 4
#define LIST_BUS_DEST_2 5
#define LIST_BUS_DEST_3 6

#define LOAD_MIN 15.0
#define LOAD_MAX 25.0
#define WAIT_IN_STATION 5.0

#define UNLOAD_MIN 16.0
#define UNLOAD_MAX 24.0

#define TIMEST_DELAY_GATE_1 1
#define TIMEST_DELAY_GATE_2 2
#define TIMEST_DELAY_GATE_3 3

#define TIMEST_QUEUE_GATE_1 4
#define TIMEST_QUEUE_GATE_2 5
#define TIMEST_QUEUE_GATE_3 6

#define TIMEST_SEAT_BUS 7

#define TIMEST_WAIT_GATE_1 8
#define TIMEST_WAIT_GATE_2 9
#define TIMEST_WAIT_GATE_3 10

#define SAMPST_PPL_SYS 11
#define SAMPST_ONE_LAP 12

#define STREAM_INTERARRIVAL_1 1
#define STREAM_INTERARRIVAL_2 2
#define STREAM_INTERARRIVAL_3 3
#define STREAM_UNLOADING 4
#define STREAM_LOADING 5
#define STREAM_RANDOM_GATE 6

#define DEFAULT_STATION 3
#define BUS_SPEED 30.0
#define NUM_STATIONS 3

short lap,
      timest_delay_gate[NUM_STATIONS + 1],
      timest_queue_gate[NUM_STATIONS + 1],
      timest_wait_gate[NUM_STATIONS + 1];

int num_stations,
    current_station,
    isBusArrived,
    isBusLeaving,
    isAlreadyOnSeat,
    isAlreadyOff,
    isReadyToLoad,
    counterArrival,
    isDepartSet;
double  subtotalArrival, tempArrival,
        timeElapsed,
        startTimeWaitInStation,
        startTimeOneLap,
        mean_interarrival[NUM_STATIONS + 1],
        routeTime[NUM_STATIONS + 1],
        length_simulation;

void init_model();
void unload();
void load();
void depart();
void arrive(int current_gate);
void flagBusLeaving();
int gaterandom();
void report();

void init_model(){
  /**
   * - initiate the passenger incoming to Station
   * - simulation starts from Station 3
   * - initiate the array for easy access to sampst
   */
  current_station = DEFAULT_STATION;
  isBusArrived = 0;
  isBusLeaving = 0;
  isReadyToLoad = 0;
  isAlreadyOnSeat = 1;
  isAlreadyOff = 1;
  isDepartSet = 0;
  lap = 0;
  counterArrival = 0;
  subtotalArrival = 0.0;
  timeElapsed = 0.0;

  timest_delay_gate[1] = TIMEST_DELAY_GATE_1;
  timest_delay_gate[2] = TIMEST_DELAY_GATE_2;
  timest_delay_gate[3] = TIMEST_DELAY_GATE_3;

  timest_queue_gate[1] = TIMEST_QUEUE_GATE_1;
  timest_queue_gate[2] = TIMEST_QUEUE_GATE_2;
  timest_queue_gate[3] = TIMEST_QUEUE_GATE_3;

  timest_wait_gate[1] = TIMEST_WAIT_GATE_1;
  timest_wait_gate[2] = TIMEST_WAIT_GATE_2;
  timest_wait_gate[3] = TIMEST_WAIT_GATE_3;
  /**
   * @brief routeTime[1] = mean time travelled from Station 1 to 2, and so on
   * 
   */

  routeTime[1] = (1.0 * 3600) / BUS_SPEED;
  routeTime[2] = (4.5 * 3600) / BUS_SPEED;
  routeTime[3] = (4.5 * 3600) / BUS_SPEED;
  
  mean_interarrival[1] = (3600.0 / 14);
  mean_interarrival[2] = (3600.0 / 10);
  mean_interarrival[3] = (3600.0 / 24);

  event_schedule(length_simulation, EVENT_END_SIMUL);
  event_schedule(sim_time, EVENT_UNLOAD);
  event_schedule(sim_time + expon(mean_interarrival[1], STREAM_INTERARRIVAL_1), EVENT_ADD_QUEUE_1);
  event_schedule(sim_time + expon(mean_interarrival[2], STREAM_INTERARRIVAL_2), EVENT_ADD_QUEUE_2);
  event_schedule(sim_time + expon(mean_interarrival[3], STREAM_INTERARRIVAL_3), EVENT_ADD_QUEUE_3);
  //event_schedule(sim_time, EVENT_ADD_QUEUE_1);
  //event_schedule(sim_time, EVENT_ADD_QUEUE_2);
  //event_schedule(sim_time, EVENT_ADD_QUEUE_3);
}

void arrive(int current_gate){
  /**
   * @brief Schedule next arrival
   * 
   */
  int igate = current_gate - 7;
  event_schedule(sim_time + expon(mean_interarrival[igate], igate), current_gate);
 
  int gate_destination;
  if (igate == 3){
    gate_destination = gaterandom();
    //printf("gate_destination %d\n", gate_destination);
  }
  else{
    gate_destination = 3;
  }
  
  transfer[1] = gate_destination;
  transfer[2] = sim_time;
  transfer[3] = 0;
  transfer[4] = 0;
  /* if (igate == 1){
    printf("pass %d %.4lf\n", igate, sim_time);
    subtotalArrival += (sim_time - tempArrival);
    tempArrival = sim_time;
    counterArrival += 1;
  } */
  list_file(LAST, igate);
  //if (igate == current_station){
    load(igate);
  //}
}

void depart(){
  /**
   * Set to the next station
   * Set the bus haven't arrived to Next station
   * Schedule event for next arrival
  */

  // Mark the stats for calculating ONE LAP
  if (current_station == DEFAULT_STATION && lap > 0){
    timest(sim_time - startTimeOneLap, SAMPST_ONE_LAP);
    startTimeOneLap = sim_time;
  }
  lap += 1;
  printf("DEPARTING \t\t\ttime: %.4f\n", sim_time);
  printf("Passenger in Bus: %d %d %d\n", list_size[LIST_BUS_DEST_1], list_size[LIST_BUS_DEST_2], list_size[LIST_BUS_DEST_3]);
  sampst(sim_time - startTimeWaitInStation, timest_wait_gate[current_station]);

  // Stats [C] Update stats for Number passenger in Bus
  timest(list_size[LIST_BUS_DEST_1] + list_size[LIST_BUS_DEST_2] + list_size[LIST_BUS_DEST_3], TIMEST_SEAT_BUS);
  //printf("Seat Bus:\t%d\n", list_size[LIST_BUS_DEST_1] + list_size[LIST_BUS_DEST_2] + list_size[LIST_BUS_DEST_3]);
  
  event_schedule(sim_time + routeTime[current_station], EVENT_UNLOAD);
  current_station = (current_station % 3) + 1;
  isBusArrived = 0;
  isReadyToLoad = 0;

  printf("Queue IN GATE: \t%d \t%d \t%d\n", list_size[1], list_size[2], list_size[3]);
  event_cancel(EVENT_DEPART);
  // cancel all event except unload
}

void unload(){
  /**
   *  - Check if the bus just arrived,
   *  - Set event for the next leaving station
   *
   */
  if (isBusArrived == 0){

    timeElapsed = sim_time;
    isBusArrived = 1;
    isBusLeaving = 0;
    isAlreadyOff = 1;
    isReadyToLoad = 0;
    isDepartSet = 0;

    // mark the time for calculating how long the bus wait in Each Station
    startTimeWaitInStation = sim_time;

    // Stats [A] Update stats for Number of Queue in each Station
    timest(list_size[current_station], timest_queue_gate[current_station]);
    
    printf("\nARRIVE_AT_GATE %d\t\ttime: %.4lf\n", current_station, sim_time);
  }
  
  // Check if any passenger in bus to be unloaded in this bus
  // Check if time < 5 min
  if (isAlreadyOff == 1){
    if (isBusLeaving == 0){
      // check if corresponding station has queue
      // remove the queue with POP, and push BUS queue
      if (list_size[current_station + 3] > 0){
        // CODE FOR UNLOADING PASSENGER OFF THE BUS
        isAlreadyOff = 0;
        event_schedule(sim_time + (double) uniform(UNLOAD_MIN, UNLOAD_MAX, STREAM_UNLOADING), EVENT_UNLOAD);
      }
      // else, BUS ready to LOAD
      else {
        isReadyToLoad = 1;
        event_schedule(sim_time, EVENT_LOAD);
      }
    }
    // Bus is leaving, schedule departure, but redirect to LOAD
    else{
      isReadyToLoad = 1;
      event_schedule(sim_time, EVENT_LOAD);
    }
  }
  // There a passenger just have been of the bus
  else{
    // Stats[F] Update Stats HERE for HOW LONG PEOPLE in system = unload time - arrival to gate
    list_remove(FIRST, current_station + 3);
    double timeArrivalPerson = transfer[2];
    sampst(sim_time - timeArrivalPerson, SAMPST_PPL_SYS);
    isAlreadyOff = 1;
    printf("UNLOADED\t\t\ttime: %.4f\n", sim_time);
    event_schedule(sim_time, EVENT_UNLOAD);
    //reset 5 minutes rules
    tempArrival = sim_time;
    event_cancel(EVENT_BUS_LEAVE);
    event_schedule(sim_time + (WAIT_IN_STATION * 60.0), EVENT_BUS_LEAVE);
  }

}

void load(int request_gate){
  // check waktu sekarang > next_event_type
  // check whether the passenger already sitting, we may proceed the Bus Leaving
  if (isAlreadyOnSeat == 1){
    if (isBusLeaving == 1){
      // Calculate the time the bus is waiting on each stations
      if (isDepartSet == 0){
        isDepartSet = 1;
        
        event_schedule(sim_time, EVENT_DEPART); 
      }
      else{
        //printf("Multi Attempt to Set EVENT_DEPART\n");
      }
    }
    else{
      if (list_size[current_station] > 0 /*&& request_gate == current_station && isReadyToLoad == 1*/){
        if (request_gate == current_station){
          if (isReadyToLoad == 1){

            if (list_size[LIST_BUS_DEST_1] + list_size[LIST_BUS_DEST_2] + list_size[LIST_BUS_DEST_3] < 20){
              //printf("PICKING UP\t\t %d \ttime: %.4f\n", current_station, sim_time);
              isAlreadyOnSeat = 0;
              
              // Check if there's queue
              double test = uniform(LOAD_MIN, LOAD_MAX, STREAM_LOADING);
              printf("LOAD UP %.4lf\t%.4lf\n", test, sim_time);
              event_schedule(sim_time + test, EVENT_LOAD);
            }
            else{
              if (sim_time - tempArrival < WAIT_IN_STATION * 60){
              }
                counterArrival += 1;
              event_cancel(EVENT_BUS_LEAVE);
              event_schedule(sim_time, EVENT_BUS_LEAVE);
              event_schedule(sim_time, EVENT_LOAD);
            }
          }
        }
      }
    }
  }
  else{
    // pop queue on station
    list_remove(FIRST, current_station);
    int gate_dest = transfer[1];
    double arrival = transfer[2];
    printf("LOADED \t\t%.4lf\n",sim_time);
    // push element to bus
    transfer[1] = gate_dest;
    transfer[2] = arrival;
    list_file(LAST, gate_dest + 3);

    // Stats[B] input the delay of each passenger IN STATION to be loaded in corresponding station
    timest(sim_time - arrival, timest_delay_gate[current_station]);
    isAlreadyOnSeat = 1;
    event_schedule(sim_time, EVENT_LOAD);
    // reset Bus Leave
    event_cancel(EVENT_BUS_LEAVE);
    event_schedule(sim_time + (WAIT_IN_STATION * 60.0), EVENT_BUS_LEAVE);
  }
}

void flagBusLeaving(){
  isBusLeaving = 1;
  printf("LEAVING \t\t\ttime: %.4f\n", sim_time);
}

int gaterandom(){
  int sc_temp = uniform(0.0, 1000.0, STREAM_RANDOM_GATE);
    if (sc_temp < 583.0){
        return 1;
    }
    return 2;
}

void report(){

  printf("\nNUMBER in each queue\n");
  printf("\t\t Avg\t\t Max\n");
  for (int i = 1; i <= NUM_STATIONS; i++){
    timest(list_size[i], -timest_queue_gate[i]);
    printf("Gate %d \t\t%.2lf \t\t%.0lf\n", i, transfer[1], transfer[2]);
  }
  
  printf("\nDELAYS in each queue\n");
  printf("\t\t Avg\t\tMax\n");
  for (int i = 1; i <= NUM_STATIONS; i++){
    timest(list_size[i], -timest_delay_gate[i]);
    printf("Gate %d \t\t%.1lf \t\t%.1lf\n", timest_delay_gate[i], transfer[1], transfer[2]);
  }

  timest(list_size[0], -TIMEST_SEAT_BUS);
  printf("\n\t\t\t Avg\t Max\n");
  printf("SEAT occupied in Bus\t%.1lf\t %.0lf\n", transfer[1], transfer[2]);

  printf("\nWAITING TIME in each Terminal\n");
  printf("\t\t Avg\t\t Max\t\t Min\n");
  //for (int i = 1; i <= NUM_STATIONS; i++){
    sampst(list_size[0], -timest_wait_gate[1]);
    printf("Gate %d \t\t%.1lf \t\t%.1lf \t\t%.1lf\n", 1, transfer[1], transfer[3], transfer[4]);
    sampst(list_size[0], -timest_wait_gate[2]);
    printf("Gate %d \t\t%.1lf \t\t%.1lf \t\t1%.1lf\n", 2, transfer[1], transfer[3], transfer[4]);
    sampst(list_size[0], -timest_wait_gate[3]);
    printf("Gate %d \t\t%.1lf \t\t%.1lf \t\t%.1lf\n", 3, transfer[1], transfer[3], transfer[4]);
  //}

  timest(list_size[0], -SAMPST_ONE_LAP);
  printf("\n\t\t Avg\t Max\t Min\n");
  printf("1-Lap Time \t%.1lf\t %.1lf\t %.1lf\n", transfer[1], transfer[2], transfer[3]);
  printf("Last Lap %d\n", lap);

  sampst(list_size[0], -SAMPST_PPL_SYS);
  printf("\n\t\t Avg\t Max\t\t Min\n");
  printf("INDV In-system \t%.1lf\t %.1lf\t %.1lf\n", transfer[1], transfer[3], transfer[4]);
}
int main(){
  length_simulation = 80.0 * 3600;
  init_simlib();
  maxatr = 4;
  init_model();
  
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
        load(current_station);
        break;
      case EVENT_BUS_LEAVE:
        flagBusLeaving();
        break;
      case EVENT_END_SIMUL:
        report();
        break;
    }
  }while (next_event_type != EVENT_END_SIMUL);
  printf(" < 5 mins: %d\n", counterArrival);
  
}