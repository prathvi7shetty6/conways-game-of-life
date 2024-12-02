#include "utils.h"

CmdArguments program_arguments;

void processArguments(int argc, char** argv, CmdArguments* args) {
    int opt;

    args->generation = 0;
    args->grid_size = 0;
    args->input_file = NULL;
    args->output_file = NULL;

    while ((opt = getopt(argc, argv, "g:n:i:o:")) != -1) {
        switch (opt) {
            case 'g': 
                args->generation = atoi(optarg);
                if(args->generation <= 0) {
                    fprintf(stderr, "Invalid number of iterations\n");
                    exit(EXIT_FAILURE);
                }
                break;
            case 'n': 
                args->grid_size = atoi(optarg);
                if(args->grid_size <= 0) {
                    fprintf(stderr, "Invalid grid size\n");
                    exit(EXIT_FAILURE);
                }
                break;
            case 'i': 
                args->input_file = optarg;
                if(args->input_file == NULL || access(args->input_file, F_OK) == -1) {
                    fprintf(stderr, "Input file does not exist\n");
                    exit(EXIT_FAILURE);
                }
                break;
            case 'o': 
                args->output_file = optarg;
                if(args->output_file == NULL) {
                    fprintf(stderr, "Invalid output file\n");
                    exit(EXIT_FAILURE);
                }
                break;
            default: 
                fprintf(stderr, "Invalid option: %c\n", opt);
                exit(EXIT_FAILURE);
        }
    }
}

int update_grid_cell(int row, int col, int alive_neighbors, char** current, char** next) {
    int changed = 0;
    if (current[row][col] == '1') {
        if (alive_neighbors < 2 || alive_neighbors > 3) {
            next[row][col] = '0';
            changed = 1;
        } else {
            next[row][col] = '1';
        }
    } else {
        if (alive_neighbors == 3) {
            next[row][col] = '1';
            changed = 1;
        } else {
            next[row][col] = '0';
        }
    }
    return changed;
}

int count_alive_cells(int row, int col, char** current) {
    int alive_count = 0;
    int row_offsets[] = {-1, -1, -1, 0, 0, 1, 1, 1};
    int col_offsets[] = {-1, 0, 1, -1, 1, -1, 0, 1};

    for (int i = 0; i < 8; i++) {
        int neighbor_row = row + row_offsets[i];
        int neighbor_col = col + col_offsets[i];
        if (current[neighbor_row][neighbor_col] == '1') {
            alive_count++;
        }
    }
    return alive_count;
}

int handle_grid_edges(GridSection* grid_section, char** current, char** next) {
    int changed = 0;
    int edge_rows[] = {1, grid_section->rows};
    int edge_cols[] = {1, grid_section->cols};

    for (int col = 1; col <= grid_section->cols; ++col) {
        for (int i = 0; i < 2; ++i) {
            int row = edge_rows[i];
            int alive_neighbors = count_alive_cells(row, col, current);
            if (update_grid_cell(row, col, alive_neighbors, current, next) == 1) {
                changed = 1;
            }
        }
    }

    for (int row = 1; row <= grid_section->rows; ++row) {
        for (int i = 0; i < 2; ++i) {
            int col = edge_cols[i];
            int alive_neighbors = count_alive_cells(row, col, current);
            if (update_grid_cell(row, col, alive_neighbors, current, next) == 1) {
                changed = 1;
            }
        }
    }
    return changed;
}

void determine_neighbors(MPI_Comm comm, NeighborRanks* neighbor_ranks) {
    int displacement = 1;

    // Determine the ranks of neighbors in the north-south and west-east directions
    MPI_Cart_shift(comm, 0, displacement, &neighbor_ranks->north, &neighbor_ranks->south);
    MPI_Cart_shift(comm, 1, displacement, &neighbor_ranks->west, &neighbor_ranks->east);

    // Calculate diagonal neighbors based on north, south, west, and east ranks
    neighbor_ranks->northwest = (neighbor_ranks->north != MPI_PROC_NULL && neighbor_ranks->west != MPI_PROC_NULL) ? neighbor_ranks->north - 1 : MPI_PROC_NULL;
    neighbor_ranks->northeast = (neighbor_ranks->north != MPI_PROC_NULL && neighbor_ranks->east != MPI_PROC_NULL) ? neighbor_ranks->north + 1 : MPI_PROC_NULL;
    neighbor_ranks->southwest = (neighbor_ranks->south != MPI_PROC_NULL && neighbor_ranks->west != MPI_PROC_NULL) ? neighbor_ranks->south - 1 : MPI_PROC_NULL;
    neighbor_ranks->southeast = (neighbor_ranks->south != MPI_PROC_NULL && neighbor_ranks->east != MPI_PROC_NULL) ? neighbor_ranks->south + 1 : MPI_PROC_NULL;

    // Set invalid ranks to MPI_PROC_NULL
    int* neighbors[] = {&neighbor_ranks->north, &neighbor_ranks->south, &neighbor_ranks->west, &neighbor_ranks->east,
                        &neighbor_ranks->northwest, &neighbor_ranks->northeast, &neighbor_ranks->southwest, &neighbor_ranks->southeast};

    for (int i = 0; i < 8; ++i) {
        if (*neighbors[i] < 0) {
            *neighbors[i] = MPI_PROC_NULL;
        }
    }
}


char** allocate_grid(int rows, int cols) {
    char** grid = malloc(rows * sizeof(char*));
    char* memory_block = malloc(rows * cols * sizeof(char));
    memset(memory_block, '0', rows * cols);
    
    for (int i = 0; i < rows; ++i) {
        grid[i] = memory_block + (i * cols);
    }
    return grid;
}

char** load_file(const char* filepath, int rank, int num_processes, GridSection* grid_section) {
    int initial_row = (rank / grid_section->scale) * grid_section->rows;
    int initial_col = (rank % grid_section->scale) * grid_section->cols;

    MPI_File mpi_input_file;
    MPI_File_open(MPI_COMM_WORLD, filepath, MPI_MODE_RDONLY, MPI_INFO_NULL, &mpi_input_file);

    char* temp_line = (char*)malloc(grid_section->cols * sizeof(char));
    char** proc_grid = allocate_grid(grid_section->rows + 2, grid_section->cols + 2);

    for (int current_row = 0; current_row < grid_section->rows; ++current_row) {
        int file_row_offset = (initial_row + current_row) * grid_section->cols * grid_section->scale;
        int file_offset = file_row_offset + initial_col;

        MPI_File_seek(mpi_input_file, file_offset, MPI_SEEK_SET);
        MPI_File_read(mpi_input_file, temp_line, grid_section->cols, MPI_CHAR, MPI_STATUS_IGNORE);

        for (int col = 0; col < grid_section->cols; ++col) {
            proc_grid[current_row + 1][col + 1] = temp_line[col];
        }
    }

    free(temp_line);
    MPI_File_close(&mpi_input_file);

    return proc_grid;
}

void save_file(const char* output_filepath, int rank, int total_processes, int grid_size, GridSection* grid_section, char** grid_data) {
    MPI_File fd;
    MPI_Offset file_offset;
    char* buffer_row = (char*)malloc(grid_section->cols * sizeof(char));
    if (!buffer_row) {
        fprintf(stderr, "ERROR: Memory allocation for buffer_row failed.\n");
        MPI_Abort(MPI_COMM_WORLD, 1);
    }

    int lead_row = (rank / grid_section->scale) * grid_section->rows;
    int lead_column = (rank % grid_section->scale) * grid_section->cols;

    if (MPI_File_open(MPI_COMM_WORLD, output_filepath, 
                      MPI_MODE_CREATE | MPI_MODE_WRONLY | MPI_MODE_APPEND, 
                      MPI_INFO_NULL, &fd) != MPI_SUCCESS) {
        fprintf(stderr, "ERROR: MPI_File_open failed for rank %d.\n", rank);
        free(buffer_row);
        MPI_Abort(MPI_COMM_WORLD, 1);
    }

    if (MPI_File_get_position(fd, &file_offset) != MPI_SUCCESS) {
        fprintf(stderr, "ERROR: MPI_File_get_position failed for rank %d.\n", rank);
        MPI_File_close(&fd);
        free(buffer_row);
        MPI_Abort(MPI_COMM_WORLD, 1);
    }

    for (int current_row = 0; current_row < grid_section->rows; ++current_row) {
        int row_position = (lead_row + current_row) * grid_size;
        MPI_Offset position_to_write = file_offset + (row_position + lead_column);

        if (MPI_File_seek(fd, position_to_write, MPI_SEEK_SET) != MPI_SUCCESS) {
            fprintf(stderr, "ERROR: MPI_File_seek failed for rank %d.\n", rank);
            MPI_File_close(&fd);
            free(buffer_row);
            MPI_Abort(MPI_COMM_WORLD, 1);
        }

        for (int col = 0; col < grid_section->cols; ++col) {
            buffer_row[col] = grid_data[current_row + 1][col + 1];
        }

        if (MPI_File_write(fd, buffer_row, grid_section->cols, MPI_CHAR, MPI_STATUS_IGNORE) != MPI_SUCCESS) {
            fprintf(stderr, "ERROR: MPI_File_write failed for rank %d.\n", rank);
            MPI_File_close(&fd);
            free(buffer_row);
            MPI_Abort(MPI_COMM_WORLD, 1);
        }
    }

    if (rank == total_processes - 1) {
        if (MPI_File_write(fd, "\n", 1, MPI_CHAR, MPI_STATUS_IGNORE) != MPI_SUCCESS) {
            fprintf(stderr, "ERROR: MPI_File_write failed while writing newline for rank %d.\n", rank);
        }
    }

    free(buffer_row);
    MPI_File_close(&fd);
}

void execute_game_of_life(char** current_grid, int rank, int num_processes, MPI_Comm communicator, NeighborRanks* neighbors, GridSection* grid_section, CmdArguments* arguments) {

    MPI_Datatype column, row;
    MPI_Request send[8], recv[8];
    MPI_Status status[8];

    MPI_Type_vector(grid_section->rows, 1, grid_section->cols + 2, MPI_CHAR, &column);
    MPI_Type_commit(&column);
    MPI_Type_contiguous(grid_section->cols, MPI_CHAR, &row);
    MPI_Type_commit(&row);

    char** next_grid = allocate_grid(grid_section->rows + 2, grid_section->cols + 2);
    char** temp;

    for (int iter = 0; iter < arguments->generation; ++iter) {
        MPI_Irecv(&current_grid[0][1], 1, row, neighbors->north, 0, communicator, &recv[0]);
        MPI_Isend(&current_grid[1][1], 1, row, neighbors->north, 0, communicator, &send[0]);

        MPI_Irecv(&current_grid[grid_section->rows + 1][1], 1, row, neighbors->south, 0, communicator, &recv[1]);
        MPI_Isend(&current_grid[grid_section->rows][1], 1, row, neighbors->south, 0, communicator, &send[1]);

        MPI_Irecv(&current_grid[1][0], 1, column, neighbors->west, 0, communicator, &recv[2]);
        MPI_Isend(&current_grid[1][1], 1, column, neighbors->west, 0, communicator, &send[2]);

        MPI_Irecv(&current_grid[1][grid_section->cols + 1], 1, column, neighbors->east, 0, communicator, &recv[3]);
        MPI_Isend(&current_grid[1][grid_section->cols], 1, column, neighbors->east, 0, communicator, &send[3]);

        MPI_Irecv(&current_grid[0][0], 1, MPI_CHAR, neighbors->northwest, 0, communicator, &recv[4]);
        MPI_Isend(&current_grid[1][1], 1, MPI_CHAR, neighbors->northwest, 0, communicator, &send[4]);

        MPI_Irecv(&current_grid[0][grid_section->cols + 1], 1, MPI_CHAR, neighbors->northeast, 0, communicator, &recv[5]);
        MPI_Isend(&current_grid[1][grid_section->cols], 1, MPI_CHAR, neighbors->northeast, 0, communicator, &send[5]);

        MPI_Irecv(&current_grid[grid_section->rows + 1][0], 1, MPI_CHAR, neighbors->southwest, 0, communicator, &recv[6]);
        MPI_Isend(&current_grid[grid_section->rows][1], 1, MPI_CHAR, neighbors->southwest, 0, communicator, &send[6]);

        MPI_Irecv(&current_grid[grid_section->rows + 1][grid_section->cols + 1], 1, MPI_CHAR, neighbors->southeast, 0, communicator, &recv[7]);
        MPI_Isend(&current_grid[grid_section->rows][grid_section->cols], 1, MPI_CHAR, neighbors->southeast, 0, communicator, &send[7]);

        for(int row = 2; row <= grid_section->rows - 1; row++) {
            for(int col = 2; col <= grid_section->cols - 1; col++) {
                int alive_neighbors = count_alive_cells(row, col, current_grid);
                update_grid_cell(row, col, alive_neighbors, current_grid, next_grid);
            }
        }

        MPI_Waitall(8, recv, status);

        handle_grid_edges(grid_section, current_grid, next_grid);

        temp = current_grid;
        current_grid = next_grid;
        next_grid = temp;

        MPI_Waitall(8, send, status);

        save_file(arguments->output_file, rank, num_processes, arguments->grid_size, grid_section, current_grid);

        MPI_Barrier(MPI_COMM_WORLD);
    }

    MPI_Type_free(&column);
    MPI_Type_free(&row);
    free(next_grid[0]);
    free(next_grid);
}

int main(int argc, char** argv) {
    int ndims = 2, reorder = 1;
    int dims[ndims], periods[ndims];
    int rank, num_processes;
    MPI_Comm communicator;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &num_processes);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    processArguments(argc, argv, &program_arguments);
    if (rank == 0) {
        FILE* output_file = fopen(program_arguments.output_file, "w");
        fprintf(output_file, "%d\n", program_arguments.grid_size);
        fclose(output_file);
    }

    dims[0] = dims[1] = (int) sqrt(num_processes);
    periods[0] = periods[1] = 0;
    int err = 0;
    MPI_Dims_create(num_processes, ndims, dims);
    err = MPI_Cart_create(MPI_COMM_WORLD, ndims, dims, periods, reorder, &communicator);
    if (err != 0) {
        printf("ERROR[%d] creating cartesian topology of processes\n", err);
    }
    NeighborRanks neighbors;
    determine_neighbors(communicator, &neighbors);

    GridSection grid_section;
    grid_section.scale = sqrt((double)num_processes);
    grid_section.rows = grid_section.cols = program_arguments.grid_size / grid_section.scale;

    char** grid = load_file(program_arguments.input_file, rank, program_arguments.grid_size, &grid_section);
    save_file(program_arguments.output_file, rank, num_processes, program_arguments.grid_size, &grid_section, grid);
    MPI_Barrier(MPI_COMM_WORLD);

    double start, end, time_elapsed;
    double global_time = 0.0;

    start = MPI_Wtime();

    execute_game_of_life(grid, rank, num_processes, communicator, &neighbors, &grid_section, &program_arguments);
    
    end = MPI_Wtime();
    time_elapsed = end - start;
    MPI_Reduce(&time_elapsed, &global_time, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD);

    free(grid[0]);
    free(grid);
    
    MPI_Finalize();

    return 0;
}
