#include <iostream>

#define MAX_SIZE 1024
using namespace std;

typedef struct
{
	int x;
	int y;
} Coordinate;

typedef struct
{
	Coordinate neighbours[4];
} Neighbours;

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
	float theta;
	int distance; // Distance measured in Encoder Clicks
} Goto;

class FloodFill
{
public:
	FloodFill()
	{
		for (int i = 0; i < 625; i++)
		{
			m_visited[i] = false;
		}
	}

	bool m_visited[625];
	Stack &m_stack = Stack::getInstance();

	Neighbours getNeighbours(Coordinate c);
	bool validateCoordinate(Coordinate c); // This is going to validate the height, width, visited
	void addToStack(Coordinate c);
	Coordinate getCoordinate();
	Goto rotateTo(Coordinate src, Coordinate dst);
	bool visited(Coordinate c); // Use Hash Function: index = 25x+y
	void addToVisited(Coordinate c); // Use Hash Function: index = 25x+y
	bool isEmpty(); // Check if underlying stack is empty
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

bool FloodFill::validateCoordinate(Coordinate c)
{
	if (c.x > 25 || c.x < 0 || c.y > 25 || c.y < 0)
	{
		return false;
	}
	return !visited(c);
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
	return m_visited[25 * c.x + c.y];
}

void FloodFill::addToVisited(Coordinate c)
{
	m_visited[25 * c.x + c.y] = true;
}

bool FloodFill::isEmpty()
{
	return m_stack.isEmpty();
}

int main()
{
	//Stack my_stack = Stack::getInstance();
	FloodFill ff;
	ff.addToVisited(Coordinate{ 0, 0 });
	ff.addToVisited(Coordinate{ 1, 1 });
	ff.addToVisited(Coordinate{ 5, 5 });
	ff.addToVisited(Coordinate{ 7, 3 });
	ff.addToVisited(Coordinate{ 24, 24 });
	ff.addToVisited(Coordinate{ 10, 11 });
	for (int i = 0; i < 625; i++)
	{
		if (ff.m_visited[i])
		{
			cout << i << " has been visited\n";
		}
	}
	Neighbours n = ff.getNeighbours(Coordinate{ 10, 11 });
	for (auto i : n.neighbours)
	{
		cout << i.x << ", " << i.y << endl;
	}
	return 0;
}
