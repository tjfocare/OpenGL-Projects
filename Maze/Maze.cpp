#include "Maze.h"
#include <cstdio>
#include <iostream>
#include <GL/glew.h>

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#include "Sphere.hpp"

#define CUBE_NUM_VERTICES 8
#define CUBE_NUM_TRIS 12
#define VALS_PER_VERT 3
#define VALS_PER_NORMAL 3

unsigned int vaoHandleCube;
unsigned int vaoHandleSphere;
Sphere* ball;

/**
 * Creates a new vertex array object for a cube
 * and loads in data into a vertex attribute buffer
 */
int Maze::createVAOCube()
{
    // Cube vertex data
    float Vertices[CUBE_NUM_VERTICES * VALS_PER_VERT] = {
        -1.0, -1.0,  1.0,
        1.0, -1.0,  1.0,
        1.0,  1.0,  1.0,
        -1.0,  1.0,  1.0,
        -1.0, -1.0, -1.0,
        1.0, -1.0, -1.0,
        1.0,  1.0, -1.0,
        -1.0,  1.0, -1.0 };

    // 12 triangles - 2 per face of the cube
    unsigned int Indices[CUBE_NUM_TRIS*3] = {
        0,1,2, 2,3,0,
        1,5,6, 6,2,1,
        5,4,7, 7,6,5,
        4,0,3, 3,7,4,
        3,2,6, 6,7,3,
        4,5,1, 1,0,4
    };

	glGenVertexArrays(1, &vaoHandleCube);
	glBindVertexArray(vaoHandleCube);

	unsigned int buffer[2];
	glGenBuffers(2, buffer);

	// Set vertex attribute
	glBindBuffer(GL_ARRAY_BUFFER, buffer[0]);
	glBufferData(GL_ARRAY_BUFFER,
                 sizeof(Vertices), Vertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, VALS_PER_VERT, GL_FLOAT, GL_FALSE, 0, 0);

 	// Set element attributes. Notice the change to using GL_ELEMENT_ARRAY_BUFFER
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer[1]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                 sizeof(Indices), Indices, GL_STATIC_DRAW);

    // Un-bind
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	return vaoHandleCube;
}

int Maze::createVAOSphere()
{
    ball = new Sphere();

    glGenVertexArrays(1, &vaoHandleSphere);
    glBindVertexArray(vaoHandleSphere);

    unsigned int buffer[3];
    glGenBuffers(3, buffer);

    glBindBuffer(GL_ARRAY_BUFFER, buffer[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * ball->vertCount, ball->vertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, VALS_PER_VERT, GL_FLOAT, GL_FALSE, 0, 0);

    glBindBuffer(GL_ARRAY_BUFFER, buffer[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * ball->normCount, ball->normals, GL_STATIC_DRAW);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, VALS_PER_NORMAL, GL_FLOAT, GL_FALSE, 0, 0);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer[2]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * ball->indCount, ball->indices, GL_STATIC_DRAW);

    return vaoHandleSphere;
}

/**
 Constructor - calculates the Maze's dimensions based on the
 desired height and width.
*/
Maze::Maze(int id, std::vector<std::string> file)
{
    programID = id;

    vaoHandleCube = createVAOCube();
    vaoHandleSphere = createVAOSphere();

    mazeFile = file;
    mazeSize = atoi(mazeFile.front().c_str());
    mazeHeight = 0.01f;

    cubeWidth = 2/float(mazeSize);

    //stores locations of cubes for collision detection
    cubeLoc = new bool*[int(mazeSize)];
    for (int i = 0; i < mazeSize; i++)
    {
        cubeLoc[i] = new bool[int(mazeSize)];
        for(int j=0; j<mazeSize; j++){
            cubeLoc[i][j] = false;  //initialise
        }
    }

    //default starting location (0,0)
    sphere_x = 0;
    sphere_z = 0;

    moveCnt = 0;
    direction = halt;
    gameOver = false;

}

/**
 Draw the Maze as a set of scaled blocks.
*/
void Maze::render()
{
	glUseProgram(programID);

  drawFloor();

  int final_x, final_z; //coordinates of final position

  drawMaze(final_x, final_z);

  moveSphere(direction);

  //**Game over**//
  if(sphere_x == final_x && sphere_z == final_z){
      printf("/****** \n\n GAME OVER!\n Number of moves: %d \n\n", moveCnt);
      printf("******/\n");
      gameOver = true;
  }

}

// draw size*size cubes to make up floor grid
void Maze::drawFloor() {

  for(float z = 0; z < mazeSize; z++) {
      for(float x = 0; x < mazeSize; x++)
          drawCube(-1 + cubeWidth/2 + x*cubeWidth,
             -1 + cubeWidth/2 + z*cubeWidth, true);
  }
}

void Maze::drawCube(float x, float z, bool isFloor) {

  glBindVertexArray(vaoHandleCube);

  //handles to colour surfaces correctly
  int surfaceUniformHandle = glGetUniformLocation(programID, "surf");
  int sphereUniformHandle = glGetUniformLocation(programID, "sphere");
  int modelUniformHandle = glGetUniformLocation(programID, "model");
  if (modelUniformHandle == -1 || surfaceUniformHandle == -1 ||
     sphereUniformHandle == -1)
    exit(1);

  glm::mat4 model;
  model = glm::translate(model, glm::vec3(x, mazeHeight, z));
  model = glm::scale(model, glm::vec3(cubeWidth/2,
     isFloor ? mazeHeight : cubeWidth/2, cubeWidth/2));
	glUniformMatrix4fv(modelUniformHandle, 1, false, glm::value_ptr(model));
  glUniform1i(surfaceUniformHandle, isFloor);
  glUniform1i(sphereUniformHandle, false);
  glDrawElements(GL_TRIANGLES, CUBE_NUM_TRIS*3, GL_UNSIGNED_INT, 0);

}

void Maze::drawSphere(float x, float z) {

    glUseProgram(programID);

    int sphereUniformHandle = glGetUniformLocation(programID, "sphere");
    int modelUniformHandle = glGetUniformLocation(programID, "model");
    if (modelUniformHandle == -1 || sphereUniformHandle == -1)
        exit(1);

    glBindVertexArray(vaoHandleSphere);

    glm::mat4 model;

    model = glm::translate(model, glm::vec3(x, mazeHeight + cubeWidth/2, z));
    model = glm::scale(model, glm::vec3(cubeWidth/2, cubeWidth/2, cubeWidth/2));
    glUniformMatrix4fv(modelUniformHandle, 1, false, glm::value_ptr(model));
    glUniform1i(sphereUniformHandle, true);
    glDrawElements(GL_TRIANGLES, ball->indCount, GL_UNSIGNED_INT, 0);

}

/**
Reads text line by line from vector, and draws box or sphere at
corresponding locations.
X and Z are coordinates of goal, so program knows when game is won
**/
void Maze::drawMaze(int &X, int &Z)
{
    std::string temp;
    float cube_z = -1 + cubeWidth/2;
    glm::mat4 model;

    for (int z = 1; z <= mazeSize; z++) {
        temp = mazeFile.at(z);  //read in lines from vector

        for(int x = 0; x < mazeSize; x++) {

            if(temp.compare(x,1,"*") == 0) {
                drawCube(-1 + cubeWidth/2 + x*cubeWidth, cube_z, false);
                cubeLoc[z-1][x] = true;

            } else if(temp.compare(x,1,"X") == 0) {
                glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
                drawSphere(-1 + cubeWidth/2 + x*cubeWidth, cube_z);
                glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
                X = x;
                Z = z-1;

            } else if(x == mazeSize-1) {
                break; //move onto next line
            }

        }
        cube_z += cubeWidth; //shift cubes to next row
    }
}


/**
0 = up, 1 = right, 2 = down, 3 = left, 4 = not moving
**/
void Maze::moveSphere(int dir)
{

    if(dir == up && !checkCollision(dir)) {
        sphere_z -= calcCollision(up);
        drawSphere(-1 + cubeWidth/2 + sphere_x*cubeWidth, -1 + cubeWidth/2 + sphere_z*cubeWidth);
    }
    else if(dir == right && !checkCollision(dir)) {
        sphere_x += calcCollision(right);
        drawSphere(-1 + cubeWidth/2 + sphere_x*cubeWidth, -1 + cubeWidth/2 + sphere_z*cubeWidth);
    }
    else if(dir == down && !checkCollision(dir)) {
        sphere_z += calcCollision(down);
        drawSphere(-1 + cubeWidth/2 + sphere_x*cubeWidth, -1 + cubeWidth/2 + sphere_z*cubeWidth);
    }
    else if(dir == left && !checkCollision(dir)) {
        sphere_x -= calcCollision(left);
        drawSphere(-1 + cubeWidth/2 + sphere_x*cubeWidth, -1 + cubeWidth/2 + sphere_z*cubeWidth);
    }
    else {
        drawSphere(-1 + cubeWidth/2 + sphere_x*cubeWidth, -1 + cubeWidth/2 + sphere_z*cubeWidth);
        moveCnt--;

    }

    direction = halt;    //reset direction
    moveCnt++;

}

/**
    true = collision, can't move
    false = safe, can move
    Checks if ball tries to leave map or hit cube based on direction
**/
bool Maze::checkCollision(int dir)
{

    if( (dir == up && (sphere_z-1 < 0             || cubeLoc[sphere_z-1][sphere_x])) ||
        (dir == right && (sphere_x+1 > mazeSize-1 || cubeLoc[sphere_z][sphere_x+1])) ||
        (dir == down && (sphere_z+1 > mazeSize-1  || cubeLoc[sphere_z+1][sphere_x])) ||
        (dir == left && (sphere_x-1 < 0           || cubeLoc[sphere_z][sphere_x-1])) )
        return true;

    return false;

}

/**
  Calculates distance to either next cube or barrier based
  on the direction and returns this value.
**/
int Maze::calcCollision(int dir)
{
    if(dir == up)
    {
        for(int i = sphere_z; i >= 0; i--)
        {
            if(cubeLoc[i][sphere_x])
                return(abs(i - sphere_z)-1);
            else if(i-1 < 0)
                return(abs(i-sphere_z));
        }
    }
    else if(dir == right)
    {
        for(int i = sphere_x; i < mazeSize; i++)
        {
            if(cubeLoc[sphere_z][i])
                return(abs(i - sphere_x)-1);
            else if(i+1 > mazeSize-1)
                return(abs(i - sphere_x));
        }
    }
    else if(dir == down)
    {
        for(int i = sphere_z; i < mazeSize; i++)
        {
            if(cubeLoc[i][sphere_x])
                return(abs(i - sphere_z)-1);
            else if(i+1 > mazeSize-1)
                return(abs(i-sphere_z));
        }
    }
    else if(dir == left)
    {
        for(int i = sphere_x; i >= 0; i--)
        {
            if(cubeLoc[sphere_z][i])
                return(abs(i - sphere_x)-1);
            else if(i-1 < 0)
                return(abs(i - sphere_x));
        }
    }

return 0;
}

/**
Called when game is over, resets relevant members
**/
void Maze::reset()
{
    sphere_x = 0;
    sphere_z = 0;

    moveCnt = 0;
    direction = halt;
    gameOver = false;
}
