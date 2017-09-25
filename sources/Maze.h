#ifndef MAZE_H
#define MAZE_H

#include <vector>
#include <queue>
#include <stack>

#pragma region Space Struct Definition

/** Space
 * Structure to use in BFS/DFS as nodes. 
 * In hindsight, probably would have been better to name node
 * but it's a tad late for that now
 */
struct Space {
	/*The X and Y co-ordinate in the maze which this structure represents*/
	int x, y;
	/*This is used to back-trace after a solution is found (similar to a stack structure)*/
	Space* parent;

	/*Parameterized constructor for this structure*/
	Space(int pos1, int pos2, Space* prnt=nullptr)
		: x(pos1), y(pos2), parent(prnt) {}
};

#pragma endregion

#pragma region Maze Class Definition

/** Maze
 * A class which encapsulates the logic of structuring and solving mazes
 * dynamically (following the guidelines set forth for 
 */
class Maze 
{
private:
	/**
	 * The beinging point and goal as a pair structure.
	 * The pair was used to provide more intuitive synax for operation
	 * with a co-oridnate plane.
 	 */
	std::pair<int, int> _begin, _dest;
	/*The internal, dynamic representation of a maze*/
	std::vector<std::vector<char>> _mazeMatrix;
	/*Internal queue used in BFS*/
	std::queue<Space*> _bfsQueue;
	/*The pointer to the 'top' node in the BFS path found*/
	Space* _bfsSolution;
	/*Internal stack used in DFS*/
	std::stack<Space*> _dfsStack;
	/*Filepath for output of maze solutions*/
	char* _outFilePathBFS;
	char* _outFilePathDFS;
	/*pointer to the 'top' node in the DFS path found*/
	Space* _dfsSolution;
	/*Integer counters for number of nodes visited*/
	int _bfsTotal;
	int _dfsTotal;
	/**
	 * A 2-D array of bools (pointers to an array of pointers)
	 * This is used in figuring which nodes have been visited
	 * by either DFS or BFS
	 */
	bool** _visited;

	/*Helper method to allocate memory for visited matrix*/
	bool** allocateVisited(const int, const int);

	/*Helper method to reset the visited matrix before performing BFS or DFS*/
	void resetVisited();
	/**
	 * Helper method to reset the attributes associated with the maze 
	 * so we can solve from scratch once more
	 */
	void resetMaze();
	/**
	* internal DFS which performs the actual logic (recursively)
	* This internal DFS is called by the public DFS method to provide
	* the same interface as calling BFS
	*/
	void DFS(Space*);

public:
	/** Default Constructor 
	 * Doesn't do anything!
	 * This is used when declaring variables before they are needed at the
	 * top of a function/method. The default construtor should never be called
	 * directly by the developer!
	 */
	Maze();
	/** Parameterized Constructor
     * This constructor accepts the x and y size of the matrix
	 * as well as the begining and end points of the maze.
	 * These values can be temporary values (-1, -1) and be set later.
	 */
	Maze(int, int, std::pair<int, int>, std::pair<int, int>);
	/** Copy Constructor
	 * Because I use dynamic memory allocated by yours truly,
	 * A copy and move constructor are required to ensure proper
	 * trade-off of the 2D _visited array!
	 */
	Maze(const Maze&);
	/** RValue (Move) Constructor
	 * Because I use dynamic memory allocated by yours truly,
	 * A copy and move constructor are required to ensure proper
	 * trade-off of the 2D _visited array!
	 */
	Maze(Maze&&);
	/** Deconstructor
	 * Cleans up the pointers in use with this class
	 */
	~Maze();
	/** Assignment Operator
	 */
	Maze& operator=(const Maze right);
	#pragma region Search Algorithms
	
	/** Breadth-first Search
	 * Finds the path to the goal using the BFS algortihm.
	 * Returns a pointer to the top of the path "stack"
	 */
	void BFS();

	/** Depth-first Search
	 * Finds the path to the goal using the DFS algortihm.
	 * Returns a pointer to top of the the path "stack"
	 * NOTE: This method is the public interface to the private
	 *       recursive DFS algorithm above.
	 */
	void DFS();

	/** Greedy Search
	 * Uses heuristic knowledge of the maze
	 * to solve the maze in a more efficient way
     */

	#pragma endregion
	
	/** GETTERS FOR PATH SOLUTION **/
	Space* GetBFSResult() { return _bfsSolution; };
	Space* GetDFSResult() { return _dfsSolution; };

	/*PRINTING METHODS*/
	/**Remarks:
	 * These print methods could DEFINITELY be improved,
	 * possibly even combined into one method with a flag for printing the path
	 * of BFS or DFS
	 */
	void PrintMaze();
	void PrintBFSResult();
	void PrintDFSResult();

	void FilePrintBFSResult(char*);
	void FilePrintDFSResult(char*);

	/**SETTERS FOR MAZE CHARACTERISTICS**/
	bool SetStart(std::pair<int, int>);
	bool SetEnd(std::pair<int, int>);
	bool SetWall(std::pair<int, int>);
	bool RemoveWall(std::pair<int, int>);
	bool SetOutputFile(char*);

	/** IsValidPair
	 * Returns whether the given pair is valid
	 * Used to validate user input. Tests if the pair is withing
	 * the bounds of the current matrix, and whether there is already
	 * a 'S', 'G', or 'X' in that position
	 *
	 * Currently, the programmer does not need to substract 1 (for zero indexing)
	 * when passing values to this method. From an API perspective, it is
	 * still unsure if this is useful or confusing!
	 */
	bool IsValidPair(int, int);
	bool IsValidPair(std::pair<int, int>);
};

#pragma endregion

#endif