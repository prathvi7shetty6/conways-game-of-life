# Conways Game of life
The Conway's Game of Life or simply Life, is a cellular automaton devised by the British mathematician John Horton Conway. It is a zero-player game where the evolution is determined entirely by its initial state, requiring no further input. 

## About the game:
* The universe of the Game of Life is an infinite, two-dimensional orthogonal grid of square cells
* Each cell on the grid can exist in one of two states: **alive** or **dead**.
* Every cell interacts with its eight neighbors
* **Underpopulation**: Any live cell with fewer than two live neighbours dies
* **Overpopulation**: Any live cell with two or three live neighbours lives on to the next generation
* **Survival**: Any live cell with more than three live neighbours dies
* **Reproduction**: Any dead cell with exactly three live neighbours becomes a live cell

## Implementation details:
* We have used MPI (Message Passing Interface) to implement Conway's Game of Life by distributing the grid among multiple processors for parallel computation
* Processes exchange boundary information to ensure correct neighbor updates across sub-grid boundaries using MPI communication primitives
* The input is a text file that contains the initial state of the game
* Addition the number of generations, grid size and output file name are passed as parameters
* For visualization we have used OpenGL 

## MPI Execution
* Clone the repo and move into the project directory
* To compile the code run the below command
  ```
    mkdir -p bin
    mpicc -o bin/conways_game src/*/*.c -lm
  ``` 
* To clean run the below command
  ```
    rm -rf bin
    rm -rf "conways_game.out"
  ``` 
* Once compilations is successful and without errors the job can be submitted to the HPC cluster using the below command:
  ```
    sbatch job.sl
  ```
* An output text file will be created
* For debugging errors and failures conways_game.out file can be used

## OpenGL Execution
This documentation includes videos illustrating a variety of patterns in Conway's Game of Life, all rendered with OpenGL to provide enhanced graphical clarity and fluidity. OpenGL, a powerful cross-platform graphics library, ensures high-quality visual representation of the evolving patterns, making it easier to observe intricate dynamics. The showcased patterns are demonstrated using predefined input files, carefully selected to highlight different behaviors and phenomena. These simulations are presented across grids of varying sizes, offering a comprehensive exploration of how the rules of Conway's Game of Life play out in diverse scenarios.



| Nova | Glider Gun | Pulsar Oscillator |
|:-----------:|:-----------:|:----------:|
| ![nova](src/OpenGl/gif/nova.gif) | ![gl](src/OpenGl/gif/glider-gun.gif) | ![po](src/OpenGl/gif/pular_oscillator.gif) |
| Infinite Oscillator | Unbounded Growth | Glider
|:-----------:|:-----------:||:-----------:|
| ![io](src/OpenGl/gif/infinite-osc.gif) | ![ug](src/OpenGl/gif/unbounded-growth.gif) | ![gli](src/OpenGl/gif/glider.gif)

* To compile the code run the below command
```
g++ main.cpp StateReader.cpp -o gof -std=c++11 -I/opt/homebrew/include -L/opt/homebrew/lib -lGL -lglut -framework OpenGL
open -a XQuartz
export DISPLAY=:0

``` 

* To run the code run the below command
``` 
./gof
```

## Why Conway's Game of Life for Scientific Computing Project?
Conway's Game of Life is a great choice for a scientific computing project because it combines simple math with interesting computational challenges. It’s an excellent way to:

* **Explore Parallel Computing:** The Game of Life inherently involves computations that can be distributed across processors, making it a practical example for implementing parallel algorithms. Using MPI, we can efficiently handle large grids and perform high-speed simulations, which is crucial for modern scientific applications.

* **Communication in Distributed Systems:** Implementing the Game of Life in a parallel environment requires efficient communication between processors to update boundary conditions. This illustrates the principles of data sharing and synchronization in distributed systems.

* **Visualization Techniques:** The Game of Life’s dynamic patterns are perfect for graphical representation. Using OpenGL, we make it easy to visualize the results, combining computation with clear, user-friendly displays.

* **Practical Applications:** Beyond its theoretical appeal, the Game of Life has practical implications in fields like biology, physics, and computer science, such as modeling population dynamics and pattern formation.

