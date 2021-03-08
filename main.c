#include "simlib.h"
#include "queue.c"

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

#define SAMPST_WAIT_GATE_1 8
#define SAMPST_WAIT_GATE_2 9
#define SAMPST_WAIT_GATE_3 10

#define SAMPST_PPL_SYS 11

#define STREAM_INTERARRIVAL_1 1
#define STREAM_INTERARRIVAL_2 2
#define STREAM_INTERARRIVAL_3 3
#define STREAM_UNLOADING 4
#define STREAM_LOADING 5
#define STREAM_RANDOM_GATE 6

#define BUS_SPEED 30
#define NUM_STATIONS 3

short sampst_delay_gate[NUM_STATIONS + 1],
      sampst_queue_gate[NUM_STATIONS + 1],
      sampst_wait_gate[NUM_STATIONS + 1];

int num_stations,
    current_station,
    isBusArrived,
    isBusLeaving,
    isAlreadyOnSeat,
    isAlreadyOff,
    isReadyToLoad,
    isDepartSet;
double  startTimeWaitInStation,
        mean_interarrival[NUM_STATIONS + 1],
        routeTime[NUM_STATIONS + 1],
        length_simulation,
        prob_stations_1_2[2 + 1];
//Queue queue_on_bus, queue_gate[NUM_STATIONS + 1];
Queue *queue_on_bus, *queue_gate[NUM_STATIONS + 1];


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
  current_station = 3;
  isBusArrived = 0;
  isBusLeaving = 0;
  isReadyToLoad = 0;
  isAlreadyOnSeat = 1;
  isAlreadyOff = 1;
  isDepartSet = 0;
  sampst_delay_gate[1] = SAMPST_DELAY_GATE_1;
  sampst_delay_gate[2] = SAMPST_DELAY_GATE_2;
  sampst_delay_gate[3] = SAMPST_DELAY_GATE_3;

  sampst_queue_gate[1] = SAMPST_QUEUE_GATE_1;
  sampst_queue_gate[2] = SAMPST_QUEUE_GATE_2;
  sampst_queue_gate[3] = SAMPST_QUEUE_GATE_3;

  sampst_wait_gate[1] = SAMPST_WAIT_GATE_1;
  sampst_wait_gate[2] = SAMPST_WAIT_GATE_2;
  sampst_wait_gate[3] = SAMPST_WAIT_GATE_3;
  /**
   * @brief routeTime[1] = mean time travelled from Station 1 to 2, and so on
   * 
   */
  queue_gate[1] = createQueue();
  queue_gate[2] = createQueue();
  queue_gate[3] = createQueue();
  queue_on_bus = createQueue();
  /* initializeQueue(&queue_gate[1]);
  initializeQueue(&queue_gate[2]);
  initializeQueue(&queue_gate[3]);
  initializeQueue(&queue_on_bus); */

  routeTime[1] = (1.0 * 3600) / BUS_SPEED;
  routeTime[2] = (4.5 * 3600) / BUS_SPEED;
  routeTime[3] = (4.5 * 3600) / BUS_SPEED;

  mean_interarrival[1] = (double) (3600 / 14);
  mean_interarrival[2] = (double) (3600 / 10);
  mean_interarrival[3] = (double) (3600 / 24);

  event_schedule(length_simulation, EVENT_END_SIMUL);
  event_schedule(sim_time, EVENT_UNLOAD);
  event_schedule(sim_time + expon(mean_interarrival[1], STREAM_INTERARRIVAL_1), EVENT_ADD_QUEUE_1);
  event_schedule(sim_time + expon(mean_interarrival[2], STREAM_INTERARRIVAL_2), EVENT_ADD_QUEUE_2);
  event_schedule(sim_time + expon(mean_interarrival[3], STREAM_INTERARRIVAL_3), EVENT_ADD_QUEUE_3);
}

void arrive(int current_gate){
  /**
   * @brief Schedule next arrival
   * 
   */
  int igate = current_gate - 7;
  event_schedule(sim_time + expon(mean_interarrival[igate], igate), current_gate);
  /**
   * 
   * int gate_destination = random_integer(<isi probabilities disini>, 2);
   * queueElement q_temp = createElement(sim_time, current_gate, gate_destination);
   * listInsert(q_temp, queue[current_gate]);
   * popQueue(<statiun destination>, queue);
   * code di atas akan mengganti 2 baris di bawah ini
   */
  int gate_destination;
  if (igate == 3){
    gate_destination = gaterandom();
  }
  else{
    gate_destination = 3;
  }
  Arr new_passenger = createArr(sim_time, igate, gate_destination);
  push(queue_gate[igate], new_passenger);
  list_file(LAST, igate);
  load(igate);
}

void depart(){
  /**
   * Set to the next station
   * Set the bus haven't arrived to Next station
   * Schedule event for next arrival
   */
  printf("Current passenger in Bus %d\n", list_size[QUEUE_BUS]);
  event_schedule(sim_time + routeTime[current_station], EVENT_UNLOAD);
  current_station = (current_station % 3) + 1;
  isBusArrived = 0;
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
    event_schedule(sim_time + WAIT_IN_STATION * 60, EVENT_BUS_LEAVE);
    isBusArrived = 1;
    isBusLeaving = 0;
    isReadyToLoad = 0;
    isDepartSet = 0;
    // mark the time for calculating how long the bus wait in Each Station
    startTimeWaitInStation = sim_time;

    // Stats [A] Update stats for Number of Queue in each Station
    sampst(list_size[current_station], sampst_queue_gate[current_station]);
    // Stats [C] Update stats for Number passenger in Bus
    sampst(list_size[QUEUE_BUS], SAMPST_SEAT_BUS);
    printf("Arrived at GATE %d\n", current_station);
  }
  
  // Check if any passenger in bus to be unloaded in this bus
  // Check if time < 5 min
  if (isAlreadyOff == 1){
    if (isBusLeaving != 1){
      // check if corresponding station has queue
      // remove the queue with POP, and push BUS queue
      if (passengerOnQueue(current_station, *queue_on_bus) == 1 && isReadyToLoad != 1){

        // CODE FOR UNLOADING PASSENGER OFF THE BUS
        // ...
        Arr unloaded_passenger = pop_gate_dest(queue_on_bus, current_station);
        list_remove(FIRST, QUEUE_BUS);

        isAlreadyOff = 0;

        // Stats[F] Update Stats HERE for HOW LONG PEOPLE in system = unload time - arrival to gate
        sampst(sim_time - unloaded_passenger.arrival, SAMPST_PPL_SYS);
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
    printf("Off the bus %.4f\n", sim_time);
    isAlreadyOff = 1;
    event_schedule(sim_time, EVENT_UNLOAD);
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
        sampst(sim_time - startTimeWaitInStation, sampst_wait_gate[current_station]);
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
            if (list_size[QUEUE_BUS] < 25){
              // pop queue on station
              isAlreadyOnSeat = 0;
              Arr passenger = pop(queue_gate[current_station]);
              list_remove(FIRST, current_station);
              
              // push element to bus
              // ...
              push(queue_on_bus, passenger);
              list_file(LAST, QUEUE_BUS);
              
              // Stats[B] input the delay of each passenger IN STATION to be loaded in corresponding station
              sampst(sim_time - passenger.arrival, sampst_delay_gate[current_station]);
              printf("ada yang naik nunggu: %.3lf time: %.4f\n", passenger.arrival, sim_time);
              // Check if there's queue
              event_schedule(sim_time + (double) uniform(LOAD_MIN, LOAD_MAX, STREAM_LOADING), EVENT_LOAD);
            }
            else{
              //printf("Bus Full %d\n", queue_on_bus.capacity);
            }
          }
          else{
            //printf("The bus not yet ready to be loaded\n");
          }
        }
        else{
          //printf("The bus not yet arrived\n");
        }
      }
      else{
        //printf("No passenger in gate\n");
      }
    }
  }
  else{
    printf("seated %.4f\n", sim_time);
    isAlreadyOnSeat = 1;
  }
}

void flagBusLeaving(){
  isBusLeaving = 1;
}

int gaterandom(){
  int sc_temp = uniform(0.0, 1000.0, STREAM_RANDOM_GATE);
    if (sc_temp < 583.0){
        return 1;
    }
    return 2;
}

void report(){
  // 
  printf("Average, maximum, minimum number in each queue\n");
  for (int i = 1; i <= NUM_STATIONS; i++){
    sampst(list_size[i], -sampst_queue_gate[i]);
    printf("Gate %d \t%.4f \t%.4lf \t%.4lf\n", sampst_queue_gate[i], transfer[1], transfer[3], transfer[4]);
  }
  printf("Average and maximum DELAYS in each queue\n");
  for (int i = 1; i <= NUM_STATIONS; i++){
    sampst(list_size[i], -sampst_delay_gate[i]);
    printf("Gate %d \t%.4f \t%.4lf\n", sampst_delay_gate[i], transfer[1], transfer[3]);
  }
  printf("Average and maximum Seat occupied in Bus\n");
  sampst(list_size[0], -SAMPST_SEAT_BUS);
  printf("Average \t%.4f\n", transfer[1]);
  printf("Maximum \t%.4f\n", transfer[3]);
}
int main(){
  length_simulation = 80.0 * 3600;
  init_simlib();

  
  init_model();
  
  do{
    timing();
    if (next_event_type == EVENT_DEPART){
      printf("EVENT_DEPART from gate %d \ttime: %.4f\n\n", current_station, sim_time);
    }
    else if (next_event_type == EVENT_UNLOAD){
      printf("\nEVENT_UNLOAD in gate %d \ttime: %.4f\n", current_station, sim_time);
    }
    else if (next_event_type == EVENT_LOAD){
      printf("EVENT_LOAD in gate %d \ttime: %.4f\n", current_station, sim_time);
    }
    else if (next_event_type == EVENT_BUS_LEAVE){
      printf("EVENT_LEAVING from gate %d \ttime: %.4f\n", current_station, sim_time);
    }

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
  printf("yang mau ke gate 1: %d\n", passengerOnQueue(1, *queue_on_bus));
  printf("yang mau ke gate 2: %d\n", passengerOnQueue(2, *queue_on_bus));
  printf("yang mau ke gate 3: %d\n", passengerOnQueue(3, *queue_on_bus));

  //sampst(list_size[i], -sampst_queue_gate[i]);  
  printf("yang ANTRI di gate 1: %d %d\n", queue_gate[1]->capacity, list_size[1]);
  printf("yang ANTRI di gate 2: %d %d\n", queue_gate[2]->capacity, list_size[2]);
  printf("yang ANTRI di gate 3: %d %d\n", queue_gate[3]->capacity, list_size[3]);
}
