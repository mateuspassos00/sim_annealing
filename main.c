#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "tsp_sa.h"

#define NUM_CITIES 51

#define MAX_ITER 350
#define INIT_TEMP 1000.0
#define MIN_TEMP 0.0001
#define SA_MAX 286
#define COOLING_SCHEDULE 1
#define NUM_SWAPS 1

#define RUNS 10

void name_file(char *filename, int run) {
    char r[3];
    snprintf(r, 3,"%d", run);
    
    strcat(filename, "r");
    strcat(filename, r);
}

int main(void) {
    
    srand(time(NULL));
    
    printf("Loading cities...\n");
    city *cities;
    if(NUM_CITIES == 51) {cities = load_instances("datasets\\51", NUM_CITIES);}
    else {cities = load_instances("datasets\\100", NUM_CITIES);}

    printf("Generating distance matrix...\n");
    float **distances = generate_distances(cities, NUM_CITIES);
        
    for(int i = 0; i < RUNS; i++) {
        printf("Running SA for run #%d...\n", i + 1);
        int *init_sol = init_solution(NUM_CITIES);
        int *final_sol = tsp_sa(init_sol, INIT_TEMP, MIN_TEMP, SA_MAX, MAX_ITER, distances, NUM_CITIES, COOLING_SCHEDULE, NUM_SWAPS, i);

        char filename[20] = "";
        name_file(filename, i + 1);

        printf("Cost: %.2f\n", solution_cost(final_sol, distances, NUM_CITIES));
        print_solution(final_sol, NUM_CITIES);
        print_solution_to_file(final_sol, distances, filename, NUM_CITIES);
        
        destroy_solution(final_sol);
    }

    destroy_cities(cities, NUM_CITIES);
    destroy_distances(distances, NUM_CITIES);
}