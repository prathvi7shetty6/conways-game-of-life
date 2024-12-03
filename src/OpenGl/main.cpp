#include <GL/glut.h>
#include "GridStateReader.h"
#include <vector>

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 800

std::vector<GridState> lifeGrids;
int currentGridIndex = 0;
int gridSize;

// Function to render the current grid and active cells
void renderScene() {
    glClear(GL_COLOR_BUFFER_BIT); // Clear the color buffer to prepare for drawing

    // Draw grid lines
    glColor3f(0.2, 0.2, 0.2); // Set grid color to dark gray
    glLineWidth(1.0); // Set line thickness

    for (int i = 0; i <= gridSize; ++i) {
        glBegin(GL_LINES);
        glVertex2f(i, 0); // Vertical grid line start
        glVertex2f(i, gridSize); // Vertical grid line end
        glEnd();

        glBegin(GL_LINES);
        glVertex2f(0, i); // Horizontal grid line start
        glVertex2f(gridSize, i); // Horizontal grid line end
        glEnd();
    }

    // Draw active cells in the current grid
    GridState& activeGrid = lifeGrids[currentGridIndex]; // Access the current grid
    for (int row = 0; row < gridSize; ++row) {
        for (int col = 0; col < gridSize; ++col) {
            if (activeGrid.grid[row][col]) { // Check if the cell is active
                float cellDimension = 1.0f; // Define cell size
                glColor3f(0.0f, 0.0f, 0.0f); // Set active cell color to black
                glBegin(GL_QUADS);
                glVertex2f(row, col); // Bottom-left corner of the cell
                glVertex2f(row + cellDimension, col); // Bottom-right corner of the cell
                glVertex2f(row + cellDimension, col + cellDimension); // Top-right corner of the cell
                glVertex2f(row, col + cellDimension); // Top-left corner of the cell
                glEnd();
            }
        }
    }

    glFlush(); // Ensure all OpenGL commands are executed
    glutSwapBuffers(); // Swap buffers for double buffering
}

// Function to update the grid and re-render periodically
void updateGrid(int) {
    currentGridIndex = (currentGridIndex + 1) % lifeGrids.size(); // Move to the next grid cyclically
    glutPostRedisplay(); // Request a re-render of the display
    glutTimerFunc(250, updateGrid, 0); // Schedule the next update in 250 ms
}

int main(int argc, char** argv) {
    // Load grids from the file
    lifeGrids = GridStateReader::readGridStates("output/100_nova.txt");
    if (!lifeGrids.empty()) {
        gridSize = lifeGrids[0].grid.size(); // Determine grid size from the first grid
    }

    // Initialize GLUT and configure the window
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB); // Enable double buffering and RGB color mode
    glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT); // Set initial window size
    glutCreateWindow("Grid Viewer"); // Create window with title

    // Set up OpenGL display properties
    glClearColor(1.0, 1.0, 0.0, 1.0); // Set background color to yellow
    glMatrixMode(GL_PROJECTION); // Set matrix mode to projection for 2D rendering
    gluOrtho2D(0, gridSize, 0, gridSize); // Define a 2D orthogonal projection

    // Register display and timer callback functions
    glutDisplayFunc(renderScene); // Set the display function to render the scene
    glutTimerFunc(0, updateGrid, 0); // Start the periodic update timer

    // Enter the main loop
    glutMainLoop(); // Run the GLUT main loop to process events and render
    return 0;
}
