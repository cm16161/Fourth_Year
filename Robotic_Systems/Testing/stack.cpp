#include <iostream>

#define MAX_SIZE 25
#define ROOT_MAX 5
using namespace std;

typedef struct
{
	int x;
	int y;
} Coordinate;

class Stack
{
private:
	Stack(){};

public:
	static Stack &getInstance()
	{
		static Stack stack;
		return stack;
	}
	void push(Coordinate c);
	Coordinate pop();
	Coordinate peak();
	bool isEmpty();

	Coordinate m_stack[MAX_SIZE];
	int m_index = 0;
};

void Stack::push(Coordinate c)
{
	if (m_index == MAX_SIZE)
	{
		return;
	}
	m_stack[m_index] = c;
	m_index++;
}

Coordinate Stack::pop()
{
	m_index--;
	if (m_index == -1)
	{
		m_index = 0;
		return Coordinate{ -1, -1 };
	}
	return m_stack[m_index];
}

Coordinate Stack::peak()
{
	if (m_index <= 0)
	{
		return Coordinate{ -1, -1 };
	}
	return m_stack[m_index - 1];
}

bool Stack::isEmpty()
{
	return (m_index == 0);
}

typedef struct
{
	Coordinate neighbours[4];
} Neighbours;

typedef struct
{
	float theta;
	int distance; // Distance measured in Encoder Clicks
} Goto;

class FloodFill
{
public:
	FloodFill()
	{
		for (int i = 0; i < MAX_SIZE; i++)
		{
			m_visited[i] = -1;
			m_added[i] = false;
		}
	}

	int m_visited[MAX_SIZE];
	bool m_added[MAX_SIZE];
	Stack &m_stack = Stack::getInstance();

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

Neighbours FloodFill::getNeighbours(Coordinate c)
{
	Neighbours ret;
	ret.neighbours[0] = Coordinate{ c.x - 1, c.y };
	ret.neighbours[1] = Coordinate{ c.x, c.y - 1 };
	ret.neighbours[2] = Coordinate{ c.x + 1, c.y };
	ret.neighbours[3] = Coordinate{ c.x, c.y + 1 };
	return ret;
}

int FloodFill::getIndex(Coordinate c)
{
	return m_visited[ROOT_MAX * c.x + c.y];
}

bool FloodFill::validateCoordinate(Coordinate c)
{
	if (c.x >= ROOT_MAX || c.x < 0 || c.y >= ROOT_MAX || c.y < 0)
	{
		return false;
	}
	return !visited(c);
}

bool FloodFill::validateStack(Coordinate c)
{
	return !onStack(c);
}

void FloodFill::addToStack(Coordinate c)
{
	m_stack.push(c);
}

Coordinate FloodFill::getCoordinate()
{
	return m_stack.pop();
}

Goto FloodFill::rotateTo(Coordinate src, Coordinate dst)
{
	//TODO
	return Goto{ 0, 0 };
}

bool FloodFill::visited(Coordinate c)
{
	if (m_visited[ROOT_MAX * c.x + c.y] != -1)
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool FloodFill::onStack(Coordinate c)
{
	return m_added[ROOT_MAX * c.x + c.y];
}

void FloodFill::addToVisited(Coordinate c, int index)
{
	m_visited[ROOT_MAX * c.x + c.y] = index;
}

void FloodFill::addToAdded(Coordinate c)
{
	m_added[ROOT_MAX * c.x + c.y] = true;
}

bool FloodFill::isEmpty()
{
	return m_stack.isEmpty();
}

FloodFill ff;
Coordinate curr = { 0, 0 };

void rotateTo(Coordinate tgt);
void silentGoTo(Coordinate tgt)
{
	cout << "(" << curr.x << ", " << curr.y << ") -> (" << tgt.x << ", " << tgt.y << ")" << endl;
	curr = tgt;
}

void backTrack(Coordinate tgt)
{
	cout << "##############################\n";
	cout << "(" << curr.x << ", " << curr.y << ") -> (" << tgt.x << ", " << tgt.y << ")" << endl;
	cout << "##############################\n";
	Coordinate c = curr;
	cout << "(" << curr.x << ", " << curr.y << ") = " << ff.getIndex(curr) << endl;
	while (c.x != tgt.x || c.y != tgt.y)
	{
		Neighbours n = ff.getNeighbours(c);
		cout << "##############################\n";
		cout << "NEIGHBOUR to go to: \n";

		for (auto i : n.neighbours)
		{
			if (i.x == tgt.x && i.y == tgt.y)
			{
				cout << "adjacent\n";
				cout << "(" << curr.x << ", " << curr.y << ") -> (" << i.x << ", " << i.y << ")" << endl;
				c.x = i.x;
				c.y = i.y;
				break;
			}
			else if (ff.getIndex(i) + 1 == ff.getIndex(c))
			{
				cout << "backtrack\n";
				cout << "(" << c.x << ", " << c.y << ") -> (" << i.x << ", " << i.y << ")" << endl;
				c.x = i.x;
				c.y = i.y;
			}
		}
		cout << "(" << c.x << ", " << c.y << ") = " << ff.getIndex(c) << endl;
		cout << "##############################\n";
		if (c.x != tgt.x || c.y != tgt.y)
		{
			cout << "backtracking location\n";
			rotateTo(c);
			silentGoTo(c);
			//curr = c; // Rotate and go to C
		}
	}
	cout << "Adjacent to target\n";
	rotateTo(c);
}

void rotateTo(Coordinate tgt)
{
	int diff_x = curr.x - tgt.x;
	int diff_y = curr.y - tgt.y;
	float err = 0.1;
	if (abs(diff_x) > 1 || abs(diff_y) > 1 || abs(diff_y) + abs(diff_x) > 1)
	{
		// Implement BackTrack
		//backTrack(curr);
		cout << ("Error: Abs diff to large\n");
		cout << "Backtrack\n";
		backTrack(tgt);

		return;
	}

	if (diff_x < 0)
	{
		//Rotate to PI/2
		cout << ("Rotate to PI/2\n");
		return;
		//turnToTheta(PI/2);
	}
	else if (diff_x > 0)
	{
		//Rotate to 3PI/2
		cout << ("Rotate to 3PI/2\n");
		return;
	}

	else if (diff_y < 0)
	{
		//Rotate to PI
		cout << ("Rotate to PI\n");
		return;
	}
	else if (diff_y > 0)
	{
		//Rotate to 0
		cout << ("Rotate to 0\n");
		return;
	}
}

void goTo(Coordinate tgt)
{
	//  Go forwards 1 unit in straight line
	//   Write power to motors.
	//static int index = 1;
	cout << "(" << curr.x << ", " << curr.y << ") -> (" << tgt.x << ", " << tgt.y << ")" << endl;
	curr = tgt;
	//ff.addToVisited(curr, index);
	//index++;
	//Map.updateMapFeature( ' ' , RomiPose.x, RomiPose.y );
}

void mapCoordinate()
{
	static int index = 1;
	//Read Sensor Value
	if (sensor_val > threshold)
	{
		ff.addToVisited(curr, -5);
                
		//Map.updateMapFeature( 'W' , RomiPose.x, RomiPose.y );
	}
	else
	{
		ff.addToVisited(curr, index);
		index++;
	}
}

int main()
{
	ff.addToStack(Coordinate{ 0, 0 });
	bool init = false;
	while (!ff.isEmpty())
	{
		Coordinate tgt = ff.getCoordinate();
		if (!ff.visited(tgt))
		{
			if (!init)
			{
				init = true;
				curr = tgt;
				ff.addToVisited(curr, 0);
				Neighbours n = ff.getNeighbours(tgt);
				for (int i = 0; i < 4; i++)
				{
					if (ff.validateCoordinate(n.neighbours[i]) && ff.validateStack(n.neighbours[i]))
					{
						ff.addToStack(n.neighbours[i]);
						ff.addToAdded(n.neighbours[i]);
					}
				}
			}
			rotateTo(tgt);
			goTo(tgt);
			mapCoordinate();
			Neighbours n = ff.getNeighbours(tgt);
			for (int i = 0; i < 4; i++)
			{
				if (ff.validateCoordinate(n.neighbours[i]) && ff.validateStack(n.neighbours[i]))
				{
					ff.addToStack(n.neighbours[i]);
					ff.addToAdded(n.neighbours[i]);
				}
			}
		}
	}
	for (int i = 0; i < ROOT_MAX; i++)
	{
		for (int j = 0; j < ROOT_MAX; j++)
		{
			cout << ff.m_visited[ROOT_MAX * j + i] << "  ";
		}
		cout << endl;
	}
	return 0;
}
