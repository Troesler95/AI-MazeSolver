#include "Maze.h"
#include <algorithm>
#include <fstream>
#include <iostream>
#include <iomanip>
#include <string.h>
#include <math.h>
using namespace std;

Maze::Maze()
{
	_bfsSolution = nullptr;
	_dfsSolution = nullptr;
	_greedySolution = nullptr;
	_astarSolution = nullptr;
	_bfsTotal = _dfsTotal = _greedyTotal = 0;
	_dest = _begin = make_pair(-1, -1);
	_visited = nullptr;
}

Maze::Maze(int x, int y, pair<int, int> begin, pair<int,int> end)
	: _begin(begin), _dest(end), _visited(allocateVisited(y,x))
{
	_mazeMatrix = vector<vector<char>>(y, vector<char>(x, ' '));
	_bfsQueue = queue<Space*>();
	_dfsStack = stack<Space*>();

	/*comp = function<bool(Space*, Space*)>(bind(&Maze::compDistance, *this, 
		std::placeholders::_1, std::placeholders::_2));
	_greedyPQ = priority_queue<Space*, vector<Space*>,
		function<bool(Space*, Space*)>>(comp);*/

	_bfsSolution = nullptr;
	_dfsSolution = nullptr;
	_greedySolution = nullptr;
	_astarSolution = nullptr;
	_bfsTotal = 0;
	_dfsTotal = 0;
}

Maze::Maze(const Maze& existing)
	: _visited(allocateVisited(existing._mazeMatrix.size(), existing._mazeMatrix[0].size()))
{
	_mazeMatrix = existing._mazeMatrix;
	_bfsQueue = existing._bfsQueue;
	_dfsStack = existing._dfsStack;
	_greedyPQ = existing._greedyPQ;
	_bfsSolution = existing._bfsSolution;
	_dfsSolution = existing._dfsSolution;
	_greedySolution = existing._greedySolution;
	_astarSolution = existing._astarSolution;
	_bfsTotal = existing._bfsTotal;
	_dfsTotal = existing._dfsTotal;
	_greedyTotal = existing._greedyTotal;
	_begin = existing._begin;
	_dest = existing._dest;

	// TODO: Find more concise copy syntax between the old and new object
	/*if (existing._visited)
	{
		bool** temp = existing._visited;
		std::swap(_visited, temp);
	}*/

	if(existing._visited != nullptr)
		for (unsigned i = 0; i < _mazeMatrix.size() - 1; ++i)
			for (unsigned j = 0; j < _mazeMatrix[0].size() - 1; ++j)
				_visited[i][j] = existing._visited[i][j];
}

Maze::Maze(Maze&& existing)
	: _visited(allocateVisited(existing._mazeMatrix.size(), existing._mazeMatrix[0].size()))
{
	_mazeMatrix = existing._mazeMatrix;
	_bfsQueue = existing._bfsQueue;
	_dfsStack = existing._dfsStack;
	_greedyPQ = existing._greedyPQ;
	_bfsSolution = existing._bfsSolution;
	_dfsSolution = existing._dfsSolution;
	_greedySolution = existing._greedySolution;
	_astarSolution = existing._astarSolution;
	_bfsTotal = existing._bfsTotal;
	_dfsTotal = existing._dfsTotal;
	_greedyTotal = existing._greedyTotal;
	_begin = existing._begin;
	_dest = existing._dest;
	
	// Come back to this...
	//std::swap(_visited, existing._visited);

	if(existing._visited != nullptr)
		for (unsigned i = 0; i < _mazeMatrix.size() - 1; ++i)
			for (unsigned j = 0; j < _mazeMatrix[0].size() - 1; ++j)
				_visited[i][j] = existing._visited[i][j];
}

Maze::~Maze()
{
	if (_bfsSolution != nullptr)
		delete _bfsSolution;
	if(_dfsSolution != nullptr)
		delete _dfsSolution;
	if (_greedySolution != nullptr)
		delete _greedySolution;

	if (_visited != nullptr)
	{
		for (unsigned i = 0; i < _mazeMatrix.size(); ++i)
			delete[] _visited[i];
		delete[] _visited;
	}
}

Maze& Maze::operator=(Maze right)
{
	this->~Maze();
	// use rvalue constructor to deep copy object
	new(this)Maze(std::move(right));
	return *this;
}

void Maze::BFS()
{
	/*Reset values*/
	resetMaze();
	resetVisited();

	_bfsQueue.push(new Space(_begin.first, _begin.second));

	while (!_bfsQueue.empty())
	{
		Space* cur = _bfsQueue.front();
		_bfsQueue.pop();
		int n_x = cur->x, n_y = cur->y;

		if (_mazeMatrix[n_y - 1][n_x - 1] == 'G')
		{
			_bfsSolution = cur;
			cout << "BFS: Found goal!" << endl;
			return;
		}
		
		if (IsValidPair(n_x + 1, n_y) && !_visited[n_y - 1][n_x])
			_bfsQueue.push(new Space(n_x + 1, n_y, cur));

		if (IsValidPair(n_x - 1, n_y) && !_visited[n_y - 1][n_x - 2])
			_bfsQueue.push(new Space(n_x - 1, n_y, cur));

		if (IsValidPair(n_x, n_y + 1) && !_visited[n_y][n_x - 1])
			_bfsQueue.push(new Space(n_x, n_y + 1, cur));

		if (IsValidPair(n_x, n_y - 1) && !_visited[n_y - 2][n_x - 1])
			_bfsQueue.push(new Space(n_x, n_y - 1, cur));

		_visited[n_y - 1][n_x - 1] = true;
	}
}

void Maze::DFS()
{
	resetMaze();
	resetVisited();
	DFS(new Space(_begin.first, _begin.second));
}

void Maze::DFS(Space* vertex)
{
	_dfsStack.push(vertex);
	_visited[vertex->y - 1][vertex->x - 1] = true;

	if (_mazeMatrix[vertex->y - 1][vertex->x - 1] == 'G')
	{
		cout << "DFS: Found Goal!" << endl;
		_dfsSolution = vertex;
		return;
	}
	else if (_dfsSolution == nullptr)
	{
		int n_x = vertex->x, n_y = vertex->y;

		if (IsValidPair(n_x + 1, n_y) && !_visited[n_y - 1][n_x])
			DFS(new Space(n_x + 1, n_y, vertex));

		if (IsValidPair(n_x - 1, n_y) && !_visited[n_y - 1][n_x - 2])
			DFS(new Space(n_x - 1, n_y, vertex));

		if (IsValidPair(n_x, n_y + 1) && !_visited[n_y][n_x - 1])
			DFS(new Space(n_x, n_y + 1, vertex));

		if (IsValidPair(n_x, n_y - 1) && !_visited[n_y - 2][n_x - 1])
			DFS(new Space(n_x, n_y - 1, vertex));
	}
	_dfsStack.pop();
}

void Maze::GreedySearch()
{
	resetMaze();
	resetVisited();
	int totalCost = 0;

	/*comp = function<bool(Space*, Space*)>(bind(&Maze::compDistance, *this,
		std::placeholders::_1, std::placeholders::_2));*/

	_greedyPQ = priority_queue<Space*, vector<Space*>,
		function<bool(Space*, Space*)>>([this](Space* a, Space* b) -> bool {
		return (calcEDistanceToGoal(a->x, a->y) > calcEDistanceToGoal(b->x, b->y)); 
	});

	_greedyPQ.push(new Space(_begin.first, _begin.second));
	while (!_greedyPQ.empty())
	{
		Space* cur = _greedyPQ.top();
		_greedyPQ.pop();
		int n_x = cur->x, n_y = cur->y;
		totalCost += cur->cost;

		if (_mazeMatrix[n_y - 1][n_x - 1] == 'G')
		{
			_greedySolution = cur;
			cout << "Greedy: Found goal!" << endl;
			cout << "Total cost: " << totalCost << endl;
			return;
		}

		if (IsValidPair(n_x + 1, n_y) && !_visited[n_y - 1][n_x])
			_greedyPQ.push(new Space(n_x + 1, n_y, cur, 2));

		if (IsValidPair(n_x - 1, n_y) && !_visited[n_y - 1][n_x - 2])
			_greedyPQ.push(new Space(n_x - 1, n_y, cur, 4));

		if (IsValidPair(n_x, n_y + 1) && !_visited[n_y][n_x - 1])
			_greedyPQ.push(new Space(n_x, n_y + 1, cur, 3));

		if (IsValidPair(n_x, n_y - 1) && !_visited[n_y - 2][n_x - 1])
			_greedyPQ.push(new Space(n_x, n_y - 1, cur, 1));

		_visited[n_y - 1][n_x - 1] = true;
	}
}

void Maze::AStarSearch() 
{
	resetMaze();
	resetVisited();
	int fval = 0, totalCost = 0;
	/* This does not work! It sends the SAME fval. It should be different based on the space itself
	 * Add a value in Space for cost of move?
	 */
	_astarPQ = priority_queue<Space*, vector<Space*>,
		function<bool(Space*, Space*)>>([this](Space* a, Space* b)->bool {
		return (calcFullDistanceToGoal(a->x, a->y, a->cost) > calcFullDistanceToGoal(b->x, b->y, b->cost));
	});

	_astarPQ.push(new Space(_begin.first, _begin.second));
	while (!_astarPQ.empty())
	{
		Space* cur = _astarPQ.top();
		_astarPQ.pop();
		int n_x = cur->x, n_y = cur->y;
		totalCost += cur->cost;

		if (_mazeMatrix[n_y - 1][n_x - 1] == 'G')
		{
			_astarSolution = cur;
			cout << "A*: Found goal!" << endl;
			cout << "A* Total cost: " << totalCost << endl;
			return;
		}
		
		// moving right
		if (IsValidPair(n_x + 1, n_y) && !_visited[n_y - 1][n_x])
		{
			fval = 2;
			_astarPQ.push(new Space(n_x + 1, n_y, cur, fval));
		}
		//moving left
		if (IsValidPair(n_x - 1, n_y) && !_visited[n_y - 1][n_x - 2])
		{
			fval = 4;
			_astarPQ.push(new Space(n_x - 1, n_y, cur, fval));
		}
		// moving up
		if (IsValidPair(n_x, n_y + 1) && !_visited[n_y][n_x - 1])
		{
			fval = 3;
			_astarPQ.push(new Space(n_x, n_y + 1, cur, fval));
		}
		//moving down
		if (IsValidPair(n_x, n_y - 1) && !_visited[n_y - 2][n_x - 1])
		{
			fval = 1;
			_astarPQ.push(new Space(n_x, n_y - 1, cur, fval));
		}

		_visited[n_y - 1][n_x - 1] = true;
	}
}

float Maze::calcEDistanceToGoal(int x, int y) const
{
	return sqrt(pow(x - _dest.first, 2) + pow(y - _dest.second, 2));
}

float Maze::calcFullDistanceToGoal(int x, int y, int fval) const
{
	return (sqrt(pow(x - _dest.first, 2) + pow(y - _dest.second, 2))) + fval;
}

/*bool Maze::compDistance(Space* lhs, Space* rhs)
{
return (calcEDistanceToGoal(lhs->x, lhs->y) > calcEDistanceToGoal(rhs->x, rhs->y));
}*/

bool Maze::SetStart(pair<int, int> n_start)
{
	if (IsValidPair(n_start.first, n_start.second)
		&& _mazeMatrix[n_start.second - 1][n_start.first - 1] != 'X'
		&& _mazeMatrix[n_start.second - 1][n_start.first - 1] != 'G'
		&& _mazeMatrix[n_start.second - 1][n_start.first - 1] != 'S')
	{
		pair<int, int> temp = _begin;
		/*If begin has been set before*/
		if(temp != make_pair(-1,-1))
			/*Remove old start*/
			_mazeMatrix[temp.second - 1][temp.first - 1] = ' ';

		/*Set new*/
		_begin = n_start;
		_mazeMatrix[n_start.second - 1][n_start.first - 1] = 'S';
		return true;
	}
	return false;
}

bool Maze::SetEnd(pair<int, int> n_end)
{
	if (IsValidPair(n_end.first, n_end.second)
		&& _mazeMatrix[n_end.second - 1][n_end.first - 1] != 'X' 
		&& _mazeMatrix[n_end.second - 1][n_end.first - 1] != 'G' 
		&& _mazeMatrix[n_end.second - 1][n_end.first - 1] != 'S')
	{
		pair<int, int> temp = this->_dest;
		/*If end was set before*/
		if (temp != make_pair(-1, -1))
			/*Remove old goals*/
			_mazeMatrix[temp.second - 1][temp.first - 1] = ' ';

		this->_dest = n_end;
		_mazeMatrix[n_end.second - 1][n_end.first - 1] = 'G';
		return true;
	}
	return false;
}

bool Maze::SetWall(pair<int, int> wall)
{
	if (IsValidPair(wall.first, wall.second) 
		&& _mazeMatrix[wall.second - 1][wall.first - 1] != 'X'
		&& _mazeMatrix[wall.second - 1][wall.first - 1] != 'G'
		&& _mazeMatrix[wall.second - 1][wall.first - 1] != 'S')
	{
		_mazeMatrix[wall.second-1][wall.first-1] = 'X';
		return true;
	}
	return false;
}

bool Maze::RemoveWall(pair<int, int> wall)
{
	if (_mazeMatrix[wall.second - 1][wall.first - 1] == 'X')
	{
		_mazeMatrix[wall.second - 1][wall.first - 1] = ' ';
		return true;
	}
	return false;
}

bool Maze::SetOutputFile(char* nFilePath)
{
	string nPathString = string(nFilePath);

	if (nPathString.empty())
		throw invalid_argument("MAZE ERROR: Cannot set to null output file path!");

	strcat_s(_outFilePathBFS, sizeof(_outFilePathBFS), (nPathString + "_bfs.out").c_str());
	strcat_s(_outFilePathDFS, sizeof(_outFilePathDFS), (nPathString + "_dfs.out").c_str());
}

bool Maze::IsValidPair(int j, int k)
{
	return (j-1 >= 0 && k-1 >= 0)
		&& (j <= _mazeMatrix[0].size() && k <= _mazeMatrix.size())
		&& (_mazeMatrix[k-1][j-1] == ' ' || _mazeMatrix[k-1][j-1] == 'G');
}

bool Maze::IsValidPair(pair<int, int> p)
{
	return IsValidPair(p.first, p.second);
}

bool** Maze::allocateVisited(const int a, const int b)
{
	bool** temp = nullptr;
	if (a > 0 && b > 0)
	{
		temp = new bool*[a];
		for (unsigned i = 0; i < a; ++i)
			temp[i] = new bool[b] {false};
	}
	return temp;
}

void Maze::resetVisited()
{
	for (int i = 0; i < _mazeMatrix.size(); i++)
		for (int j = 0; j < _mazeMatrix[0].size(); j++)
			_visited[i][j] = false;
}

void Maze::resetMaze()
{
	_bfsQueue = queue<Space*>();
	_dfsStack = stack<Space*>();
	_bfsSolution = _dfsSolution = nullptr;
	_bfsTotal = _dfsTotal = 0;
}

/*PRINT FUNCTIONS*/
void Maze::PrintMaze()
{
	int printedWidth = (_mazeMatrix[0].size() * 2) + 2;
	/*TOP ROW*/
	for (int i = 0; i < printedWidth; ++i)
	{
		if (i == 0)
			cout << ((char)218);
		else if (i == printedWidth - 1)
			cout << ((char)191);
		else
			cout << ((char)196);
	}
	cout << endl;
	/*MATRIX VALUES*/
	for (int i = _mazeMatrix.size() - 1; i >= 0; --i)
	{
		cout << '|';
		for (int j = 0; j < _mazeMatrix[0].size(); ++j)
		{
			cout << _mazeMatrix[i][j];
			
			if (j == _mazeMatrix[0].size() - 1)
				cout << " |";
			else
				cout << '|';
		}
		cout << endl;
		/*ROW SEPARATOR*/
		if (i != 0)
		{
			for (int i = 0; i < printedWidth; ++i)
			{
				if (i == 0)
					cout << '|';
				else if (i == printedWidth - 1)
					cout << '|';
				else
					cout << ((char)196);
			}
			cout << endl;
		}
	}
	/*BOTTOM ROW*/
	for (int i = 0; i < printedWidth; ++i)
	{
		if (i == 0)
			cout << ((char)192);
		else if (i == printedWidth - 1)
			cout << ((char)217);
		else
			cout << ((char)196);
	}
	cout << endl;
}

void Maze::PrintBFSResult()
{
	int printedWidth = (_mazeMatrix[0].size() * 2) + 2;
	int visitedTotal = 0;
	std::vector<std::pair<int, int>> bfsPathVec = std::vector<std::pair<int, int>>();
	Space* temp = _bfsSolution;
	/*FOLLOW PARENTS*/
	while (temp->parent != nullptr)
	{
		bfsPathVec.push_back(std::pair<int, int>(temp->x - 1, temp->y - 1));
		temp = temp->parent;
	}

	/*TOP ROW*/
	for (int i = 0; i < printedWidth; ++i)
	{
		if (i == 0)
			cout << ((char)218);
		else if (i == printedWidth - 1)
			cout << ((char)191);
		else
			cout << ((char)196);
	}
	cout << endl;

	/*MATRIX VALUES*/
	for (int i = _mazeMatrix.size() - 1; i >= 0; --i)
	{
		cout << '|';
		for (int j = 0; j < _mazeMatrix[0].size(); ++j)
		{
			if ((std::find(bfsPathVec.begin(), bfsPathVec.end(), std::pair<int, int>(j, i)) != bfsPathVec.end())
				&& (_mazeMatrix[i][j] != 'G'))
				cout << '*';
			else
				cout << _mazeMatrix[i][j];

			if (j == _mazeMatrix[0].size() - 1)
				cout << " |";
			else
				cout << '|';
			/*Count the number of nodes visited*/
			if (_visited[i][j]) ++visitedTotal;
		}
		cout << endl;
		/*ROW SEPARATOR*/
		if (i != 0)
		{
			for (int i = 0; i < printedWidth; ++i)
			{
				if (i == 0)
					cout << '|';
				else if (i == printedWidth - 1)
					cout << '|';
				else
					cout << ((char)196);
			}
			cout << endl;
		}
	}

	/*BOTTOM ROW*/
	for (int i = 0; i < printedWidth; ++i)
	{
		if (i == 0)
			cout << ((char)192);
		else if (i == printedWidth - 1)
			cout << ((char)217);
		else
			cout << ((char)196);
	}
	cout << endl;

	cout << "Total nodes visited: " << visitedTotal << endl;

	/*PRINT PATH TRAVELED*/
	cout << "BFS Path: " << endl;
	for (int i = bfsPathVec.size() - 1; i >= 0; --i)
	{
		cout << '(' << bfsPathVec[i].first + 1 << ", " 
			 << bfsPathVec[i].second + 1 << ')';
		if (i != 0)
			cout << ", ";
	}
	cout << endl;
	cout << "Press enter to continue!" << endl;
	std::cin.ignore();
}

void Maze::PrintDFSResult()
{
	int printedWidth = (_mazeMatrix[0].size() * 2) + 2;
	int visitedTotal = 0;
	std::vector<std::pair<int, int>> dfsPathVec = std::vector<std::pair<int, int>>();
	Space* temp = _dfsSolution;
	/*FOLLOW PARENTS*/
	while (temp->parent != nullptr)
	{
		dfsPathVec.push_back(std::pair<int, int>(temp->x - 1, temp->y - 1));
		temp = temp->parent;
	}

	/*TOP ROW*/
	for (int i = 0; i < printedWidth; ++i)
	{
		if (i == 0)
			cout << ((char)218);
		else if (i == printedWidth - 1)
			cout << ((char)191);
		else
			cout << ((char)196);
	}
	cout << endl;

	/*MATRIX VALUES*/
	for (int i = _mazeMatrix.size() - 1; i >= 0; --i)
	{
		cout << '|';
		for (int j = 0; j < _mazeMatrix[0].size(); ++j)
		{
			if ((std::find(dfsPathVec.begin(), dfsPathVec.end(), std::pair<int, int>(j, i)) != dfsPathVec.end())
				&& (_mazeMatrix[i][j] != 'G'))
				cout << '*';
			else
				cout << _mazeMatrix[i][j];

			if (j == _mazeMatrix[0].size() - 1)
				cout << " |";
			else
				cout << '|';
			if (_visited[i][j]) ++visitedTotal;
		}
		cout << endl;
		/*ROW SEPARATOR*/
		if (i != 0)
		{
			for (int i = 0; i < printedWidth; ++i)
			{
				if (i == 0)
					cout << '|';
				else if (i == printedWidth - 1)
					cout << '|';
				else
					cout << ((char)196);
			}
			cout << endl;
		}
	}

	/*BOTTOM ROW*/
	for (int i = 0; i < printedWidth; ++i)
	{
		if (i == 0)
			cout << ((char)192);
		else if (i == printedWidth - 1)
			cout << ((char)217);
		else
			cout << ((char)196);
	}
	cout << endl;

	cout << "Total nodes visited: " << visitedTotal << endl;

	/*PRINT PATH*/
	cout << "DFS Path: " << endl;
	for (int i = dfsPathVec.size() - 1; i >= 0; --i)
	{
		cout << '(' << dfsPathVec[i].first + 1 << ", "
			<< dfsPathVec[i].second + 1 << ')';
		if (i != 0)
			cout << ", ";
	}
	cout << endl;
	cout << "Press enter to continue!" << endl;
	std::cin.ignore();
}

void Maze::PrintGreedyResult()
{
	int printedWidth = (_mazeMatrix[0].size() * 2) + 2;
	int visitedTotal = 0;
	std::vector<std::pair<int, int>> greedyPathVec = std::vector<std::pair<int, int>>();
	Space* temp = _greedySolution;
	/*FOLLOW PARENTS*/
	while (temp->parent != nullptr)
	{
		greedyPathVec.push_back(std::pair<int, int>(temp->x - 1, temp->y - 1));
		temp = temp->parent;
	}

	/*TOP ROW*/
	for (int i = 0; i < printedWidth; ++i)
	{
		if (i == 0)
			cout << ((char)218);
		else if (i == printedWidth - 1)
			cout << ((char)191);
		else
			cout << ((char)196);
	}
	cout << endl;

	/*MATRIX VALUES*/
	for (int i = _mazeMatrix.size() - 1; i >= 0; --i)
	{
		cout << '|';
		for (int j = 0; j < _mazeMatrix[0].size(); ++j)
		{
			if ((std::find(greedyPathVec.begin(), greedyPathVec.end(), std::pair<int, int>(j, i)) != greedyPathVec.end())
				&& (_mazeMatrix[i][j] != 'G'))
				cout << '*';
			else
				cout << _mazeMatrix[i][j];

			if (j == _mazeMatrix[0].size() - 1)
				cout << " |";
			else
				cout << '|';
			if (_visited[i][j]) ++visitedTotal;
		}
		cout << endl;
		/*ROW SEPARATOR*/
		if (i != 0)
		{
			for (int i = 0; i < printedWidth; ++i)
			{
				if (i == 0)
					cout << '|';
				else if (i == printedWidth - 1)
					cout << '|';
				else
					cout << ((char)196);
			}
			cout << endl;
		}
	}

	/*BOTTOM ROW*/
	for (int i = 0; i < printedWidth; ++i)
	{
		if (i == 0)
			cout << ((char)192);
		else if (i == printedWidth - 1)
			cout << ((char)217);
		else
			cout << ((char)196);
	}
	cout << endl;

	cout << "Total nodes visited: " << visitedTotal << endl;

	/*PRINT PATH*/
	cout << "Greedy Path: " << endl;
	for (int i = greedyPathVec.size() - 1; i >= 0; --i)
	{
		cout << '(' << greedyPathVec[i].first + 1 << ", "
			<< greedyPathVec[i].second + 1 << ')';
		if (i != 0)
			cout << ", ";
	}
	cout << endl;
	cout << "Press enter to continue!" << endl;
	std::cin.ignore();
}

void Maze::PrintAStarResult()
{
	int printedWidth = (_mazeMatrix[0].size() * 2) + 2;
	int visitedTotal = 0;
	std::vector<std::pair<int, int>> astarPathVec = std::vector<std::pair<int, int>>();
	Space* temp = _astarSolution;
	/*FOLLOW PARENTS*/
	while (temp->parent != nullptr)
	{
		astarPathVec.push_back(std::pair<int, int>(temp->x - 1, temp->y - 1));
		temp = temp->parent;
	}

	/*TOP ROW*/
	for (int i = 0; i < printedWidth; ++i)
	{
		if (i == 0)
			cout << ((char)218);
		else if (i == printedWidth - 1)
			cout << ((char)191);
		else
			cout << ((char)196);
	}
	cout << endl;

	/*MATRIX VALUES*/
	for (int i = _mazeMatrix.size() - 1; i >= 0; --i)
	{
		cout << '|';
		for (int j = 0; j < _mazeMatrix[0].size(); ++j)
		{
			if ((std::find(astarPathVec.begin(), astarPathVec.end(), std::pair<int, int>(j, i)) != astarPathVec.end())
				&& (_mazeMatrix[i][j] != 'G'))
				cout << '*';
			else
				cout << _mazeMatrix[i][j];

			if (j == _mazeMatrix[0].size() - 1)
				cout << " |";
			else
				cout << '|';
			if (_visited[i][j]) ++visitedTotal;
		}
		cout << endl;
		/*ROW SEPARATOR*/
		if (i != 0)
		{
			for (int i = 0; i < printedWidth; ++i)
			{
				if (i == 0)
					cout << '|';
				else if (i == printedWidth - 1)
					cout << '|';
				else
					cout << ((char)196);
			}
			cout << endl;
		}
	}

	/*BOTTOM ROW*/
	for (int i = 0; i < printedWidth; ++i)
	{
		if (i == 0)
			cout << ((char)192);
		else if (i == printedWidth - 1)
			cout << ((char)217);
		else
			cout << ((char)196);
	}
	cout << endl;

	cout << "Total nodes visited: " << visitedTotal << endl;

	/*PRINT PATH*/
	cout << "A* Path: " << endl;
	for (int i = astarPathVec.size() - 1; i >= 0; --i)
	{
		cout << '(' << astarPathVec[i].first + 1 << ", "
			<< astarPathVec[i].second + 1 << ')';
		if (i != 0)
			cout << ", ";
	}
	cout << endl;
	cout << "Press enter to continue!" << endl;
	std::cin.ignore();
}

void Maze::FilePrintBFSResult(char* outPath = "")
{
	ofstream outfile;

	if (string(outPath).empty() && string(_outFilePathBFS).empty())
		throw invalid_argument("MAZE ERROR: No output file has been given!");
	else if (!string(outPath).empty())
		outfile.open(outPath);
	else
		outfile.open(_outFilePathBFS);

	int printedWidth = (_mazeMatrix[0].size() * 2) + 2;
	std::vector<std::pair<int, int>> bfsPathVec = std::vector<std::pair<int, int>>();
	Space* temp = _bfsSolution;
	/*FOLLOW PARENTS*/
	while (temp->parent != nullptr)
	{
		bfsPathVec.push_back(std::pair<int, int>(temp->x - 1, temp->y - 1));
		temp = temp->parent;
	}

	/*TOP ROW*/
	for (int i = 0; i < printedWidth; ++i)
	{
		if (i == 0)
			outfile << ((char)218);
		else if (i == printedWidth - 1)
			outfile << ((char)191);
		else
			outfile << ((char)196);
	}
	outfile << endl;

	/*MATRIX VALUES*/
	for (int i = _mazeMatrix.size() - 1; i >= 0; --i)
	{
		outfile << '|';
		for (int j = 0; j < _mazeMatrix[0].size(); ++j)
		{
			if ((std::find(bfsPathVec.begin(), bfsPathVec.end(), std::pair<int, int>(j, i)) != bfsPathVec.end())
				&& (_mazeMatrix[i][j] != 'G'))
				outfile << '*';
			else
				outfile << _mazeMatrix[i][j];

			if (j == _mazeMatrix[0].size() - 1)
				outfile << " |";
			else
				outfile << '|';
		}
		outfile << endl;
		/*ROW SEPARATOR*/
		if (i != 0)
		{
			for (int i = 0; i < printedWidth; ++i)
			{
				if (i == 0)
					outfile << '|';
				else if (i == printedWidth - 1)
					outfile << '|';
				else
					outfile << ((char)196);
			}
			outfile << endl;
		}
	}

	/*BOTTOM ROW*/
	for (int i = 0; i < printedWidth; ++i)
	{
		if (i == 0)
			outfile << ((char)192);
		else if (i == printedWidth - 1)
			outfile << ((char)217);
		else
			outfile << ((char)196);
	}
	outfile << endl;

	/*PRINT PATH TRAVELED*/
	cout << "BFS Path: " << endl;
	for (int i = bfsPathVec.size() - 1; i >= 0; --i)
	{
		outfile << '(' << bfsPathVec[i].first + 1 << ", "
			<< bfsPathVec[i].second + 1 << ')';
		if (i != 0)
			outfile << ", ";
	}
	outfile << endl;
}

void Maze::FilePrintDFSResult(char* outPath = "")
{
	ofstream outfile;

	if (string(outPath).empty() && string(_outFilePathDFS).empty())
		throw invalid_argument("MAZE ERROR: No output file has been given!");
	else if (!string(outPath).empty())
		outfile.open(outPath);
	else
		outfile.open(_outFilePathDFS);

	int printedWidth = (_mazeMatrix[0].size() * 2) + 2;
	std::vector<std::pair<int, int>> dfsPathVec = std::vector<std::pair<int, int>>();
	Space* temp = _dfsSolution;
	/*FOLLOW PARENTS*/
	while (temp->parent != nullptr)
	{
		dfsPathVec.push_back(std::pair<int, int>(temp->x - 1, temp->y - 1));
		temp = temp->parent;
	}

	/*TOP ROW*/
	for (int i = 0; i < printedWidth; ++i)
	{
		if (i == 0)
			outfile << ((char)218);
		else if (i == printedWidth - 1)
			outfile << ((char)191);
		else
			outfile << ((char)196);
	}
	outfile << endl;

	/*MATRIX VALUES*/
	for (int i = _mazeMatrix.size() - 1; i >= 0; --i)
	{
		outfile << '|';
		for (int j = 0; j < _mazeMatrix[0].size(); ++j)
		{
			if ((std::find(dfsPathVec.begin(), dfsPathVec.end(), std::pair<int, int>(j, i)) != dfsPathVec.end())
				&& (_mazeMatrix[i][j] != 'G'))
				outfile << '*';
			else
				outfile << _mazeMatrix[i][j];

			if (j == _mazeMatrix[0].size() - 1)
				outfile << " |";
			else
				outfile << '|';
		}
		outfile << endl;
		/*ROW SEPARATOR*/
		if (i != 0)
		{
			for (int i = 0; i < printedWidth; ++i)
			{
				if (i == 0)
					outfile << '|';
				else if (i == printedWidth - 1)
					outfile << '|';
				else
					outfile << ((char)196);
			}
			outfile << endl;
		}
	}

	/*BOTTOM ROW*/
	for (int i = 0; i < printedWidth; ++i)
	{
		if (i == 0)
			outfile << ((char)192);
		else if (i == printedWidth - 1)
			outfile << ((char)217);
		else
			outfile << ((char)196);
	}
	outfile << endl;

	/*PRINT PATH*/
	cout << "DFS Path: " << endl;
	for (int i = dfsPathVec.size() - 1; i >= 0; --i)
	{
		cout << '(' << dfsPathVec[i].first + 1 << ", "
			<< dfsPathVec[i].second + 1 << ')';
		if (i != 0)
			cout << ", ";
	}
	outfile << endl;
}