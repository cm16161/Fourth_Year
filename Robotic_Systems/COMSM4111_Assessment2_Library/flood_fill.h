#include "stack.h"

typedef struct
{
  Coordinate neighbours[4];
} Neighbours;

typedef struct {
  float theta;
  int distance; // Distance measured in Encoder Clicks
} Goto;

class FloodFill {
  public:
    FloodFill() {
      for (int i = 0; i < MAX_SIZE; i++) {
        m_visited[i] = -1;
        m_added[i] = false;
      }
    }

    int m_visited[MAX_SIZE];
    bool m_added[MAX_SIZE];
    Stack& m_stack = Stack::getInstance();

    Neighbours getNeighbours(Coordinate c);
    bool validateCoordinate(Coordinate c); // This is going to validate the height, width, visited
    bool validateStack(Coordinate c);
    void addToStack(Coordinate c);
    Coordinate getCoordinate();
    Goto rotateTo(Coordinate src, Coordinate dst);
    bool visited(Coordinate c); // Use Hash Function: index = 25x+y
    bool onStack(Coordinate c);
    void addToVisited(Coordinate c, int index); // Use Hash Function: index = 25x+y
    void addToAdded(Coordinate c);
    bool isEmpty(); // Check if underlying stack is empty
    int getIndex(Coordinate c);
};

Neighbours FloodFill::getNeighbours(Coordinate c) {
  Neighbours ret;
  ret.neighbours[0] = Coordinate{c.x - 1, c.y};
  ret.neighbours[1] = Coordinate{c.x, c.y - 1};
  ret.neighbours[2] = Coordinate{c.x + 1, c.y};
  ret.neighbours[3] = Coordinate{c.x, c.y + 1};
  return ret;
}

int FloodFill::getIndex(Coordinate c){
  return m_visited[ROOT_MAX*c.x + c.y];
}

bool FloodFill::validateCoordinate(Coordinate c) {
  if (c.x >= ROOT_MAX || c.x < 0 || c.y >= ROOT_MAX || c.y < 0) {
    return false;
  }
  return !visited(c);
}

bool FloodFill::validateStack(Coordinate c) {
  return !onStack(c);
}

void FloodFill::addToStack(Coordinate c) {
  m_stack.push(c);
}

Coordinate FloodFill::getCoordinate() {
  return m_stack.pop();
}

Goto FloodFill::rotateTo(Coordinate src, Coordinate dst) {
  //TODO
  return Goto{0, 0};
}

bool FloodFill::visited(Coordinate c) {
  if(m_visited[ROOT_MAX*c.x + c.y] != -1){
    return true;
  }
  else{
    return false;
  }
}

bool FloodFill::onStack(Coordinate c) {
  return m_added[ROOT_MAX * c.x + c.y];
}

void FloodFill::addToVisited(Coordinate c, int index) {
  m_visited[ROOT_MAX * c.x + c.y] = index;
}

void FloodFill::addToAdded(Coordinate c) {
  m_added[ROOT_MAX * c.x + c.y] = true;
}

bool FloodFill::isEmpty() {
  return m_stack.isEmpty();
}
