#include "simlib.h"

/**
 * @brief defining EVENT TYPE
 * 
 */

#define EVENT_ADD_QUEUE_1 8
#define EVENT_ADD_QUEUE_2 9
#define EVENT_ADD_QUEUE_3 10

#define STREAM_INTERARRIVAL 3

#define QUEUE_GATE_1 1
#define QUEUE_GATE_2 2
#define QUEUE_GATE_3 3

#define EVENT_END_SIMUL 0

#define STREAM_INTERARRIVAL 1
#define STREAM_ARRIVAL_AT_GATE 2
#define NUM_STATIONS 3

int num_stations,
    current_station,
    isBusArrived,
    route[NUM_STATIONS + 1];
double  mean_interarrival[NUM_STATIONS + 1],
        length_simulation,
        prob_stations_1_2[2 + 1];

void init_model(){
  /**
   * @brief initiate the passenger incoming to Station
   * @brief simulation starts from Station 3
   * 
   */
  current_station = 3;
  isBusArrived = 0;

  event_schedule(sim_time + expon(mean_interarrival[1], STREAM_INTERARRIVAL), EVENT_ADD_QUEUE_1);
  event_schedule(sim_time + expon(mean_interarrival[2], STREAM_INTERARRIVAL), EVENT_ADD_QUEUE_2);
  event_schedule(sim_time + expon(mean_interarrival[3], STREAM_INTERARRIVAL), EVENT_ADD_QUEUE_3);
}

void arrive(int current_gate){
  /**
   * @brief Schedule next arrival
   * 
   */
    event_schedule(sim_time + expon(mean_interarrival[current_gate], STREAM_INTERARRIVAL), current_gate);

  // Insert passenger into queue

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
   */
  current_station = (current_station % 3) + 1;
  isBusArrived = 0;
}

void unload(){
  /**
   *  Check if the bus just arrived,
   *  Set event for the next leaving station
   *
   */
  if (isBusArrived == 0){
    event_schedule(<5 menit>, EVENT_DEPART);
    isBusArrived = 1;
  }
  /**
   * If already set, continue unload
   * Set event for the next unload
   */
  event_schedule(sim_time + expon)
}

void load(){
  // check waktu sekarang > next_event_type
  event_cancel();
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
    }
  }while (next_event_type != EVENT_END_SIMUL);
}


