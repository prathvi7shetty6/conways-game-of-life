#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <string.h>
#include <math.h>
#include <mpi.h>
#include <unistd.h>

typedef struct GridSection {
    int rows;
    int cols;
    int scale;
} GridSection;

typedef struct NeighborRanks {
    int north;
    int south;
    int west;
    int east;
    int northwest;
    int northeast;
    int southwest;
    int southeast;
} NeighborRanks;

typedef struct CmdArguments {
    int generation;
    int grid_size;
    char * input_file;
    char * output_file;
} CmdArguments;

int update_grid_cell(int row, int col, int alive_neighbors, char** current, char** next);
int count_alive_cells(int row, int col, char** current);
int handle_grid_edges(GridSection* grid_section, char** current, char** next);
void determine_neighbors(MPI_Comm communicator, NeighborRanks* neighbors);
char** allocate_grid(int rows, int cols);
char** load_file(const char* filepath, int rank, int num_processes, GridSection* grid_section);
void save_file(const char* output_filepath, int rank, int total_processes, int grid_size, GridSection* grid_section, char** grid_data);
void execute_game_of_life(char** current_grid, int rank, int num_processes, MPI_Comm communicator, NeighborRanks* neighbors, GridSection* grid_section, CmdArguments* arguments);
void processArguments(int argc, char** argv, CmdArguments* args);

#endif //UTILS_H