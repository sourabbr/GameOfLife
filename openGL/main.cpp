#include <GL/glut.h>
#include "StateReader.h"
#include <vector>

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 800

std::vector<State> states;
int currentState = 0;

void init() {
    glClearColor(1.0, 1.0, 1.0, 1.0); // Set background color to white
    glMatrixMode(GL_PROJECTION);
    gluOrtho2D(0, GRID_SIZE, 0, GRID_SIZE); // Set coordinate system
}

void drawGrid() {
    glColor3f(0.0, 0.0, 0.0); // Set grid color to black
    glLineWidth(1.0); // Set line width for the grid

    for (int i = 0; i <= GRID_SIZE; i++) {
        // Vertical lines
        glBegin(GL_LINES);
        glVertex2f(i, 0);
        glVertex2f(i, GRID_SIZE);
        glEnd();

        // Horizontal lines
        glBegin(GL_LINES);
        glVertex2f(0, i);
        glVertex2f(GRID_SIZE, i);
        glEnd();
    }
}

void drawCell(int x, int y) {
    float cellSize = 1.0f; // Set the size of each cell

    // Draw filled cell
    glColor3f(0.0f, 0.0f, 0.0f); // Set cell color to black
    glBegin(GL_QUADS);
    glVertex2f(x, y);
    glVertex2f(x + cellSize, y);
    glVertex2f(x + cellSize, y + cellSize);
    glVertex2f(x, y + cellSize);
    glEnd();
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT);
    drawGrid(); // Draw the grid

    State& current = states[currentState];
    for (int i = 0; i < GRID_SIZE; ++i) {
        for (int j = 0; j < GRID_SIZE; ++j) {
            if (current.grid[i][j] == 1) {
                drawCell(j, i); // Draw cell
            }
        }
    }

    glFlush();
    glutSwapBuffers();
}

void timer(int) {
    currentState = (currentState + 1) % states.size();
    glutPostRedisplay();
    glutTimerFunc(250, timer, 0); // Set time for 250 ms
}

int main(int argc, char** argv) {
    states = StateReader::readGameOfLifeStates("output/out2.txt");

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
    glutCreateWindow("Game of Life Simulation");

    init();
    glutDisplayFunc(display);
    glutTimerFunc(0, timer, 0);

    glutMainLoop();
    return 0;
}
