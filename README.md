# Tugas1-ModSim

This project demonstrates how the queue is simulated using M/M/1 queue. The process is deterministic, meaning the state of the model could only go on one-way. The problem is quoted as below.

The simulation is event-based, therefore, there must be event termination handling, based on the priority and urgencies. For example, at location A, **unload** event can not be evoked nor run after **depart** event has been triggered, unless the former timestamp + **unloading** process time < **depart** timestamp.

This project is written in C language and using **SimLib** library. In case you're creating your own data structure (such as Queue), please consider your queue will be accessed and modified concurrently by several process/thread. I recommend using the simple queue provided by **SimLib**, like ```list_file``` and ```list_remove```.

"Consider a car rental system shown in figure below, with all distances given in miles. People arrive at location i (where i = 1, 2, 3) with independent exponential interarrival times at respective rates of 14, 10, and 24 per hour. Each location has a FIFO queue with unlimited capacity. There is one bus with a capacity of 20 people and a speed of 30 miles per hour. The bus is initially at location 3 (car rental), and leaves immediately in a counterclockwise direction. All people arriving at a terminal want to go to the car rental. All people arriving at the car rental want to go to terminals 1 and 2 with respective probabilities 0.583 and 0.417. When a bus arrives at a location, the following rules apply:
![image](https://user-images.githubusercontent.com/44569953/134843112-a6b3f487-cf16-46f2-b955-105300f4d0d5.png)

1. People are first unloaded in a FIFO manner. The time to unload one person is distributed uniformly between 16 and 24 seconds.
2. People are then loaded on the bus up to its capacity, with a loading time per person that is distributed uniformly between 15 and 25 seconds.
3. The bus always spends at least 5 minutes at each location. If no loading or unloading is in process after 5 minutes. the bus will leave immediately."
