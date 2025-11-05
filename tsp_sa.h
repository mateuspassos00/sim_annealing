#ifndef _TSP_SA_H
#define _TSP_SA_H

typedef struct coordinates {
    int x, y;
} coordinates;

typedef struct city {
    int label;
    coordinates *c;
} city;

city *load_instances(const char *filename, int num_cities);
float **generate_distances(city *cities, int num_cities);
int *init_solution(int num_cities);
float solution_cost(int *solution, float **distances, int num_cities);
int *tsp_sa(int *init_sol, float init_temp, float min_temp, int sa_max, int max_iter, float **distances, int num_cities, int cs, int num_swaps, int run);

void print_solution(int *solution, int num_cities);
void print_solution_to_file(int *solution, float **distances, const char *filename, int num_cities);
void print_distances(float **distances, int num_cities);
void print_cities(city *cities, int num_cities);

void destroy_solution(int *solution);
void destroy_distances(float **distances, int num_cities);
void destroy_cities(city *cities, int num_cities);

#endif