#include <stdio.h>
#include "simlib.h"

#define MAX_LOOP 1000
#define STREAM_RANDOM 2

int gaterandom(void);

int main(){
    int temp, n, stream;
    int count = 0;
    double p = 0.0;
    double prob[] = {0.583, 0.417};
    //printf("n stream ");
    //scanf("%d %d", &n, &stream);

    for(int i = 0; i < MAX_LOOP; i++){
        temp = gaterandom();
        if (temp == 1){
            count++;
        }
    }

    p = (double) count / MAX_LOOP; 
    printf("Probabilities: %lf %lf\n", prob[0], prob[1]);
    printf("%.4lf %.4lf\n", p, 1-p);

    return 0;
}

int random_gate(){
    int sc_temp = uniform(0.0, 1000.0, STREAM_RANDOM);
    if (sc_temp < 583.0){
        return 1;
    }
    return 2;
}