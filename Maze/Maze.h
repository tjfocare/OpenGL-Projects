#include <vector>
#include <string>

/**
 This class draws an interactive maze with cubes and spheres.
*/

enum direction{up, right, down, left, halt};

class Maze
{
  private:

    float mazeHeight, cubeWidth;
    int mazeSize, programID, moveCnt, sphere_x, sphere_z;
    bool **cubeLoc;
    std::vector<std::string> mazeFile;

    int createVAOCube();
    int createVAOSphere();

    void drawCube(float, float, bool);
    void drawMaze(int&, int&);
    void drawSphere(float, float);
    void drawFloor();

    void moveSphere(int);
  	bool checkCollision(int);
  	int calcCollision(int);


  public:

    Maze(int, std::vector<std::string>);
    int direction;
    bool gameOver;

    void render();
    void reset();
};
