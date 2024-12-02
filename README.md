# Conways Game of life
The Conway's Game of Life or simply Life, is a cellular automaton devised by the British mathematician John Horton Conway.

## About the game:
* It is a zero player game
* It's evolution is determined by its initial state, requiring no further input
* The universe of the Game of Life is an infinite, two-dimensional orthogonal grid of square cells
* The cells can have 2 state, live or dead
* Every cell interacts with its eight neighbors
* Any live cell with fewer than two live neighbours dies
* Any live cell with two or three live neighbours lives on to the next generation
* Any live cell with more than three live neighbours dies
* Any dead cell with exactly three live neighbours becomes a live cell

## Implementation details:
* We have used MPI (Message Passing Interface) to implement Conway's Game of Life by distributing the grid among multiple processors for parallel computation
* Processes exchange boundary information to ensure correct neighbor updates across sub-grid boundaries using MPI communication primitives
* The input is a text file that contains the initial state of the game
* Addition the number of generations, grid size and output file name are passed as parameters
* For visualization we have used OpenGL 

## Patterns

## MPI Execution
* Clone the repo and move into the project directory
* To compile the code run the below command
  ```
    ./build.sh compile
  ```
* Once compilations is successful and without errors the job can be submitted to the HPC cluster using the below command:
  ```
    sbatch job.sl
  ```
* An output text file will be created
* For debugging errors and failures conways_game.out file can be used

## OpenGL Execution
* 
*
  
