#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <math.h>

#include "tsp_sa.h"

city *load_instances(const char *filename, int num_cities) {
    FILE *fp = fopen64(filename, "rt"); assert(fp != NULL);

    city *cities = (city *) malloc(num_cities * sizeof(city));
    int x, y, label;
    char line[256];
            
    while (fgets(line, sizeof(line), fp)) {
        if (sscanf(line, "%d %d %d", &label, &x, &y) == 3) {            
            coordinates *c = (coordinates *) malloc(sizeof(coordinates));
            c->x = x;
            c->y = y;

            cities[label - 1].c = c;
            cities[label - 1].label = label;            
        }
    }

    fclose(fp);

    return cities;
}

float distance(coordinates *a, coordinates *b) {
    float diff_x = a->x - b->x;
    float diff_y = a->y - b->y;

    return sqrtf(diff_x * diff_x + diff_y * diff_y);
}

void fill_with_distances(city *cities, float *row_distances, int num_elem_row) {
    for(int i = 0; i < num_elem_row; i++) {
        row_distances[i] = distance(cities[i].c, cities[num_elem_row - 1].c);
    }
}

float **generate_distances(city *cities, int num_cities) {
    float **distances = (float **) malloc(num_cities * sizeof(float*));
    for(int i = 0; i < num_cities; i++) {
        distances[i] = (float *) malloc((i + 1) * sizeof(float)); // matriz triangular (TSP simétrico)
        fill_with_distances(cities, distances[i], i + 1);
    }

    return distances;
}

void normalize_distances(float **distances, int num_cities) {
    float min = distances[0][0], max = distances[0][0];
    for(int i = 0; i < num_cities; i++) {
        for(int j = 0; j < num_cities; j++) {
            if (distances[i][j] < min) min = distances[i][j];
            else if(distances[i][j] > max) max = distances[i][j];
        }
    }

    for(int i = 0; i < num_cities; i++) {
        for(int j = 0; j < num_cities; j++) {
            distances[i][j] /= (max - min);
        }
    }
}

void shuffle(int *array, int n) {
    for (int i = n - 1; i > 0; i--) {
        int j = rand() % (i + 1); // random index between 0 and i
        // swap array[i] and array[j]
        int temp = array[i];
        array[i] = array[j];
        array[j] = temp;
    }
}

int *init_solution(int num_cities) {
    int *solution = (int *) malloc(num_cities * sizeof(int));
    for(int i = 0; i < num_cities; i++) solution[i] = i + 1; // solução indexada em 1
    
    shuffle(solution, num_cities);
    
    return solution;
}

float solution_cost(int *solution, float **distances, int num_cities) {
    float cost = 0;
    for(int i = 0; i < num_cities - 1; i++) {
        // solução indexada em 1 e distância em 0
        int row = solution[i] - 1;
        int col = solution[i + 1] - 1;

        // matriz triangular.. pode ser que algum elemento inexistente seja acessado
        if(row < col) cost += distances[col][row]; 
        else cost += distances[row][col];
    }

    return cost;
}

float cs_2(float T0, float TN, int i, int N) {
    float A = ((T0 - TN) * (N + 1)) / N;
    float B = T0 - A;

    return (A / (i + 1)) + B;
}

int *perturbate(int *cur_solution, int num_cities, int num_swaps) {
    // fazendo uma cópia
    int *tmp_solution = (int *) malloc(num_cities * sizeof(int));
    for(int i = 0; i < num_cities; i++) tmp_solution[i] = cur_solution[i];

    // trocando alguns elementos (swapping)
    for(int i = 0; i < num_swaps; i++) {
        int rand_1 = rand() % num_cities;
        int rand_2 = rand() % num_cities;
        
        int tmp = tmp_solution[rand_1];
        tmp_solution[rand_1] = tmp_solution[rand_2];
        tmp_solution[rand_2] = tmp;
    }

    return tmp_solution;
}

void copyarray(int *v, int *w, int n) {
    for(int i = 0; i < n; i++) v[i] = w[i];
}

void create_log(char *filename, int run) {
    char r[3];    
    snprintf(r, 3,"%d", run);        
    
    strcat(filename, "conv_r");
    strcat(filename, r);

    FILE *fp = fopen64(filename, "a");

    fprintf(fp, "iteration,current_cost,best_cost,temperature\n");

    fclose(fp);
}

void log_convergence(char *filename, int it, int iterT, int sa_max, float cur_cost, float best_cost, float T) {
    FILE *fp = fopen64(filename, "a");

    fprintf(fp, "%d,%.2f,%.2f,%.4f\n", iterT + it * sa_max, cur_cost, best_cost, T);

    fclose(fp);
}

int *tsp_sa(int *init_solution, float init_temp, float min_temp, int sa_max, int max_iter, float **distances, int num_cities, int cs, int num_swaps, int run) {
    int *best_solution = (int *) malloc(num_cities * sizeof(int));
    copyarray(best_solution, init_solution, num_cities);
    float best_cost = solution_cost(best_solution, distances, num_cities);
    
    int iterT = 0; // número de iterações na temperatura T
    float T = init_temp; // temperatura atual
    
    int *cur_solution = init_solution;
    float cur_cost = best_cost;
      
    char filename[10] = "";
    create_log(filename, run);
    for(int i = 0; i < max_iter; i++) {
        while(iterT < sa_max) {
            iterT++;            
            
            int *tmp_solution = perturbate(cur_solution, num_cities, num_swaps);
            float tmp_cost = solution_cost(tmp_solution, distances, num_cities);
            
            float delta = tmp_cost - cur_cost;

            if(delta <= 0) { 
                free(cur_solution);
                cur_solution = tmp_solution;
                cur_cost = tmp_cost;
                if(cur_cost <= best_cost) {
                    copyarray(best_solution, cur_solution, num_cities);
                    best_cost = cur_cost;
                }
            } else {
                float x = ((float) (rand() % RAND_MAX)) / RAND_MAX; // escolhendo x entre [0, 1]
                float prob_worse = expf(-delta / T);
                if(x < prob_worse) {
                    free(cur_solution);
                    cur_solution = tmp_solution;
                    cur_cost = tmp_cost;
                } else free(tmp_solution);
            }

            log_convergence(filename, i, iterT, sa_max, cur_cost, best_cost, T);
        }                
        
        if(cs == 1) T *= 0.98;
        else T = cs_2(init_temp, min_temp, i + 1, max_iter);
        iterT = 0;
    }

    return best_solution;
}

void print_solution(int *solution, int num_cities) {
    for(int i = 0; i < num_cities - 1; i++) printf("%d, ", solution[i]);
    printf("%d\n", solution[num_cities - 1]);
}

void print_solution_to_file(int *solution, float **distances, const char *filename, int num_cities) {
    FILE *fp = fopen64(filename, "wt"); assert(fp != NULL);
    
    fprintf(fp, "Cost: %.2f\n", solution_cost(solution, distances, num_cities));
    
    for(int i = 0; i < num_cities - 1; i++) fprintf(fp, "%d\n", solution[i]);
    fprintf(fp, "%d\n", solution[num_cities - 1]);

    fclose(fp);
}

void print_cities(city *cities, int num_cities) {
    for(int i = 0; i < num_cities; i++) {
        printf("%d %d %d\n", cities[i].label, cities[i].c->x, cities[i].c->y);
    }
}

void print_distances(float **distances, int num_cities) {
    for(int i = 0; i < num_cities; i++) {
        for(int j = 0; j < i + 1; j++) {
            printf("%f ", distances[i][j]);
        }
        printf("\n");
    }
}

void destroy_solution(int *solution) {
    free(solution);
}

void destroy_distances(float **distances, int num_cities) {
    for(int i = 0; i < num_cities; i++) free(distances[i]);
    free(distances);
}

void destroy_cities(city *cities, int num_cities) {
    for(int i = 0; i < num_cities; i++) free(cities[i].c);
    free(cities);
}