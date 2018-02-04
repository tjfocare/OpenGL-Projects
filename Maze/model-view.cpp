#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#include "InputState.h"
#include "Viewer.h"
#include "Maze.h"
#include "Shader.hpp"

const char * usage =
"                                                               \n"
"Basic OpenGL maze game                                         \n"
"                                                               \n"
"To run it type:                                                \n"
"                                                               \n"
"   ./maze <layout.txt>                                         \n"
"                                                               \n"
"Where <layout.txt> is a text file containing the maze layout   \n"
"                                                               \n";

Maze *TheMaze;

int winX = 500;
int winY = 500;

/**
 Cameras
*/
WorldObjectViewer* WorldCam;
ObjectViewer* ObjCam;
Viewer* Camera;

glm::vec3 cameraPos(0.0f, 2.0f, 2.0f);

float mazeSize = 0;
std::vector <std::string> maze;

// Data structure storing mouse input info
InputState Input;

unsigned int programID;

/**
Provides correct directional movement based on
current camera position.
Checks camera values and assigns output according
to rotated angle and direction
**/
int checkDirection(int dir)
{
    glm::mat4 viewM;
    viewM = Camera->getViewMtx();
    float lo = viewM[2][0];
    float hi = viewM[2][1];
    float root2on2 = sqrt(2)/2;

    if (lo >= -root2on2 && lo < root2on2 && hi >= -root2on2 && hi < 0.5) //0 degrees
    {
        return dir;
    }
    else if(lo >= root2on2 && lo < 1.0 && hi >= -0.5 && hi < 0.5)       //90 degrees
    {
        switch(dir){
            case up:
                return right;
            case right:
                return down;
            case down:
                return left;
            case left:
                return up;
        }
    }
    else if(lo >= -root2on2 && lo < root2on2 && hi >= 0.5 && hi < 1.0)  //180 degrees
    {
        switch(dir){
            case up:
                return down;
            case right:
                return left;
            case down:
                return up;
            case left:
                return right;
        }
    }
    else if(lo <= -root2on2 && lo > -1.0 && hi >= -0.5 && hi <= 0.5)    //270 degrees
    {
        switch(dir){
            case up:
                return left;
            case right:
                return up;
            case down:
                return right;
            case left:
                return down;
        }

    }
  return halt;
}

void key_callback(GLFWwindow* window,
                  int key, int scancode, int action, int mods)
{
    if (action == GLFW_PRESS) {
        switch(key)
            {
            case GLFW_KEY_ESCAPE: // escape key pressed
                glfwSetWindowShouldClose(window, GL_TRUE);
                break;
            case '1':
                Camera = ObjCam;
                break;
            case '2':
                Camera = WorldCam;
                break;
            case GLFW_KEY_DOWN:
                TheMaze->direction = checkDirection(down);
                break;
            case GLFW_KEY_UP:
                TheMaze->direction = checkDirection(up);
                break;
            case GLFW_KEY_RIGHT:
                TheMaze->direction = checkDirection(right);
                break;
            case GLFW_KEY_LEFT:
                TheMaze->direction = checkDirection(left);
                break;
            default:
                break;
            }
    }
}

// Set the projection matrix. Takes into account window aspect ratio, so called
// when the window is resized.
void setProjection()
{
    glm::mat4 projection;

    projection = glm::perspective( (float)M_PI/3.0f, (float) winX / winY, 1.0f, 30.0f );

	// Load it to the shader program
	int projHandle = glGetUniformLocation(programID, "projection");
	if (projHandle == -1) {
		std::cout << "Uniform: projection_matrix is not an active uniform label\n";
	}
	glUniformMatrix4fv( projHandle, 1, false, glm::value_ptr(projection) );
}

// Called when the window is resized.
void reshape_callback( GLFWwindow *window, int x, int y )
{
    winX = x;
    winY = y;
    setProjection();
    glViewport( 0, 0, x, y );
}

void mouse_pos_callback(GLFWwindow* window, double x, double y)
{
    // Use a global data structure to store mouse info
    // We can then use it however we want
    Input.update((float)x, (float)y);
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS) {
        Input.rMousePressed = true;
    }
    else if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_RELEASE) {
        Input.rMousePressed = false;
    }
    else if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
        Input.lMousePressed = true;
    }
    else if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE) {
        Input.lMousePressed = false;
    }
}

void render()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    Camera->update(Input);

    glm::mat4 viewMatrix;
    viewMatrix = Camera->getViewMtx();

  	int viewHandle = glGetUniformLocation(programID, "view");
  	if (viewHandle == -1)
      std::cout << "Uniform: view is not an active uniform label\n";

  	glUniformMatrix4fv( viewHandle, 1, false, glm::value_ptr(viewMatrix) );

    TheMaze->render();

    glBindVertexArray(0);
    glFlush();

}

void readMaze(std::string file)
{
    std::ifstream mazeFile(file);
    std::string line;

    while(std::getline(mazeFile, line)) {
        maze.push_back(line);
        printf("%s\n", line.c_str());
    }

    mazeFile.close();

}

/**
 * Error callback for GLFW. Simply prints error message to stderr.
 */
static void error_callback(int error, const char* description)
{
    fputs(description, stderr);
}

int main (int argc, char **argv)
{
    if(argc != 2) {
      fprintf(stderr, "%s", usage);
      exit(-1);
    }

    readMaze(argv[1]);

    GLFWwindow* window;
    glfwSetErrorCallback(error_callback);

    if (!glfwInit())
        exit(1);

    // Specify that we want OpenGL 3.3
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Create the window and OpenGL context
    window = glfwCreateWindow(winX, winY, "Modelling and viewing", NULL, NULL);
    if (!window) {
        glfwTerminate();
        exit(1);
    }

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

  	// Initialize GLEW
  	glewExperimental = true; // Needed for core profile
  	if (glewInit() != GLEW_OK) {
  		fprintf(stderr, "Failed to initialize GLEW\n");
  		exit(1);
  	}

	   // Set up the shaders we are to use. 0 indicates error.
  	programID = LoadShaders("maze.vert", "maze.frag");
  	if (programID == 0)
		    exit(1);

    // Set OpenGL state we need for this application.
    glClearColor(0.5F, 0.5F, 0.5F, 0.0F);
  	glEnable(GL_DEPTH_TEST);
  	glUseProgram(programID);

    // Set up the scene and the cameras
    setProjection();

    TheMaze = new Maze(programID, maze);

    WorldCam = new WorldObjectViewer( cameraPos );
    ObjCam = new ObjectViewer( cameraPos );
    Camera = ObjCam;

    // Define callback functions and start main loop
    glfwSetKeyCallback(window, key_callback);
    glfwSetCursorPosCallback(window, mouse_pos_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetFramebufferSizeCallback(window, reshape_callback);

    while (!glfwWindowShouldClose(window))
    {
        render();

        glfwSwapBuffers(window);
        glfwPollEvents();

        if(TheMaze->gameOver)
            TheMaze->reset();
    }

    // Clean up
    delete TheMaze;

    glfwDestroyWindow(window);
    glfwTerminate();
    exit(0);

    return 0;
}
