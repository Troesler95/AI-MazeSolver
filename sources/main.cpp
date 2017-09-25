#include <fstream>
#include <iomanip>
#include <iostream>
#include <map>
#include <signal.h>
#include <Shlwapi.h>
#include <stdio.h>
#include <string>
#include <sstream>
#include <queue>
#include <tchar.h>
#include <utility>
#include <windows.h>
#include "Maze.h"
#include "Shlwapi.h"
#pragma comment(lib, "Shlwapi.lib")
#define MAX_BUF 256
using namespace std;

static std::string PROG_NAME = "";

/*FUNCTION HEADERS*/
void ShowUsage();
void ParseCommandLine(int, char**, std::string&, std::string&, std::pair<int, int>&, std::pair<int,int>&);
std::pair<int, int>* GetCoOrds(std::queue<char*>&);
void HandleException(exception);
BOOL InterruptHandler(DWORD);

Maze InitializeMazeFromFile(std::string iniFilePath)
{
	std::vector<std::pair<int, int>> walls;
	DWORD errorMsg;
	ifstream iniFile;

	try
	{
		if (PathFileExistsA((LPCSTR)iniFilePath.c_str()))
			iniFile.open(iniFilePath);
		else
		{
			errorMsg = GetLastError();
			throw invalid_argument(
				PROG_NAME + " ERROR: Unable to find file '" + iniFilePath + "'."
				+ ((errorMsg != 0) ? "\n PathFileExists Error: " + errorMsg : '\0')
			);
		}
		if (iniFile.is_open())
		{
			std::string line;
			std::getline(iniFile, line);
			std::stringstream linestream(line);
			int x1, y1, x2, y2;
			x1 = y1 = x2 = y2 = -1;
			Maze maze;

			// Operate under the assumption that the first two values are the
			// X and Y size of the matrix
			std::getline(linestream, line, ',');
			x1 = std::stoi(line);
			std::getline(linestream, line, ',');
			y1 = std::stoi(line);

			if (x1 != -1 && y1 != -1)
				// Initialize with placeholder values to be replaced later
				maze = Maze(x1, y1, std::pair<int, int>(-1, -1), std::pair<int, int>(-1, -1));
			else
				throw runtime_error(
					PROG_NAME + " ERROR: Initialization file is not in the correct CSV format. \
					Please ensure the first two comma-separated values are the size (X and Y) of the overall matrix \
					and the remaining values are x,y pairs which specify walls within the maze.\n"
				);

			// assume the next values are x and y for wall coordinates
			while (std::getline(linestream, line, ','))
			{
				x2 = std::stoi(line);
				if (!std::getline(linestream, line, ','))
					throw runtime_error(PROG_NAME + " ERROR: uneven amount of co-ordinate pairs available in initialization file.");
				y2 = std::stoi(line);
				if (x2 != -1 && y2 != -1)
					maze.SetWall(std::pair<int, int>(x2, y2));
				else
					throw runtime_error(PROG_NAME + " ERROR: invalid co-ordinate in initialization file.");
			}
			return maze;
		}
		else
			throw runtime_error(PROG_NAME + " ERROR: Unable to open file '" + iniFilePath + "'!");
	}
	catch (exception ex)
	{
		HandleException(ex);
		cout << "Press ENTER to continue..." << endl;
		cin.get();
	}
}

std::pair<int, int> GetCoOrdsFromUser(std::string message)
{
	char* input = new char[255];
	int x = -1, y = -1;

	try 
	{
		cout << message << endl;
		cout << "X: ";
		cin.getline(input, sizeof(input));
		x = std::stoi(input);
		std::cout << endl;
		std::cout << "Y: ";
		std::cin.getline(input, sizeof(input));
		y = std::stoi(input);
	}
	catch (exception ex)
	{
		std::cout << "GetCoOrdsFromUser ERROR: " << ex.what() << endl;
	}
	return std::pair<int, int>(x, y);
}

Maze GetOperationalMazeState(Maze curMaze, std::pair<int, int> startPair, std::pair<int, int> goalPair)
{
	char choiceTemp;
	int userChoice = 0;
	/*auto CheckPairValid = [=](std::pair<int, int> p) -> bool {
		return !((p.first == -1 || p.second == -1)
			&& ();
	};*/
	bool validState = false, startValid = false, goalValid = false;

	do
	{
		if (!curMaze.IsValidPair(startPair.first, startPair.second))
		{
			curMaze.PrintMaze();
			startPair = GetCoOrdsFromUser("Please enter the x and y for the starting position in the maze: ");
		}
		if (!curMaze.IsValidPair(startPair.first, startPair.second))
		{
			std::cout << "Unable to read x, y values for start position or values are too large, try again!" << endl;
			continue;
		}
		else
			startValid = curMaze.SetStart(startPair);

		if (!curMaze.IsValidPair(goalPair.first, goalPair.second))
		{
			curMaze.PrintMaze();
			goalPair = GetCoOrdsFromUser("Please enter the x and y for the goal in the maze: ");
		}
		if (!curMaze.IsValidPair(goalPair.first, goalPair.second))
		{
			std::cout << "Unable to read x, y values for goal position or values are too large, try again!" << endl;
			continue;
		}
		else
			goalValid = curMaze.SetEnd(goalPair);

		validState = startValid && goalValid;

	} while (!validState);

	return curMaze;
}

void Menu(std::string iniFilePath, bool output, std::pair<int, int> startPair, std::pair<int, int> goalPair)
{
	/*if (!SetConsoleCtrlHandler((PHANDLER_ROUTINE)InterruptHandler, 1))
		throw runtime_error("ERROR: Unable to setup InterruptHandler throught Windows API!");*/
	
	std::cout << "----------------------------------------------------------" << endl;
	std::cout << "---  CSIT 431: Intro to AI/Knowledge Engineering       ---" << endl;
	std::cout << "---  Assignment 1: Naieve Path finding (BFS and DFS)   ---" << endl;
	std::cout << "---  Published: 09/10/2017                             ---" << endl;
	std::cout << "---  Author: Tyler Roesler                             ---" << endl;
	std::cout << "----------------------------------------------------------" << endl << endl;
	std::cout << "Press enter to continue to program!" << endl << endl;
	std::cin.get();
	
	Maze maze;
	maze = InitializeMazeFromFile(iniFilePath);

	maze = GetOperationalMazeState(maze, startPair, goalPair);

	char choiceTemp;
	char* fileBuf = new char[255];
	int userChoice;
	std::pair<int, int> temp;
	
	char userIn;
	bool cont = true, inputValid = false, 
		setSuccessful = false, success = false;

	/*LAMBDAS*/
	auto getContinue = [=](char &userIn) -> void {
		std::cout << "Would you like to add another wall?" << endl;
		std::cout << "(y/n): ";
		std::cin.getline(&userIn, 2);
	};

	while (true)
	{
		try 
		{
			userChoice = 0;
			userIn = choiceTemp = '\0';
			cont = true; inputValid = setSuccessful = false;
			fileBuf[0] = '\0';
			
			temp = std::pair<int, int>(-1, -1);

			maze.PrintMaze();

			/*TODO: Print current maze*/
			std::cout << "Menu:" << endl;
			std::cout << "\t1. Solve maze with Breadth-First Search Algorithm" << endl;
			std::cout << "\t2. Solve maze with Depth-First Search Algorithm" << endl;
			std::cout << "\t3. Change starting point" << endl;
			std::cout << "\t4. Change ending point" << endl;
			std::cout << "\t5. Add walls" << endl;
			std::cout << "\t6. Remove walls" << endl;
			std::cout << "\t7. Load from new CSV file" << endl;
			std::cout << "\t8. Exit program" << endl;
			std::cout << "\nChoice: ";
			std::cin.getline(&choiceTemp, 2);
			userChoice = std::stoi(&choiceTemp);
			/*std::cin.ignore();
			choiceTemp;*/

			switch (userChoice)
			{
			case 1:
				maze.BFS();
				maze.PrintBFSResult();
				if (output)
				{
					do
					{
						std::cout << "Would you like to output to file? (Y/N)" << endl;
						std::cin.getline(&userIn, 2);
						//std::cin.ignore();
					
						switch (userIn)
						{
						case 'y':
						case 'Y':
							inputValid = true;
							break;
						case 'n':
						case 'N':
							cont = false;
							inputValid = true;
							break;
						default:
							std::cout << "Cannot understand input!" << endl;
							break;
						}
					} while (!inputValid);
					if (userIn == 'Y' || userIn == 'y')
						maze.FilePrintBFSResult("");
				}
				break;
			case 2:
				maze.DFS();
				maze.PrintDFSResult();
				if (output)
				{
					do
					{
						std::cout << "Would you like to output to file? (Y/N)" << endl;
						std::cin.getline(&userIn, 2);
						//std::cin.ignore();

						switch (userIn)
						{
						case 'y':
						case 'Y':
							inputValid = true;
							break;
						case 'n':
						case 'N':
							cont = false;
							inputValid = true;
							break;
						default:
							std::cout << "Cannot understand input!" << endl;
							break;
						}
					} while (!inputValid);
					if (userIn == 'Y' || userIn == 'y')
						maze.FilePrintDFSResult("");
				}
				break;
			case 3:
				temp = GetCoOrdsFromUser("Please enter the X,Y co-ordinates for the new start position: ");
				setSuccessful = maze.SetStart(temp);
				if (!setSuccessful)
					std::cout << "Unable to set the new start position! Try again." << endl;
				break;
			case 4:
				temp = GetCoOrdsFromUser("Please enter the X,Y co-ordinates for the new end goal: ");
				setSuccessful = maze.SetEnd(temp);
				if (!setSuccessful)
					std::cout << "Unable to set the new goal position! Try again." << endl;
				break;
			case 5:
				while (cont)
				{
					inputValid = false;
					temp = GetCoOrdsFromUser("Please enter the X,Y co-ordinates for the new wall: ");
					success = maze.SetWall(temp);
					if (!success)
					{
						std::cout << "Unable to set a wall at position (" << temp.first << ", " << temp.second << ")!" << endl;
						break;
					}
					getContinue(userIn);
					do
					{
						switch (userIn)
						{
						case 'y':
						case 'Y':
							inputValid = true;
							break;
						case 'n':
						case 'N':
							cont = false;
							inputValid = true;
							break;
						default:
							std::cout << "Cannot understand input!" << endl;
							getContinue(userIn);
							break;
						}
					} while (!inputValid);
				}
				break;
			case 6:
				while (cont)
				{
					inputValid = false;
					temp = GetCoOrdsFromUser("Please enter the X,Y co-ordinates for the wall to remove: ");
					success = maze.RemoveWall(temp);
					if (!success)
					{
						std::cout << "Unable to remove a wall at position (" 
							<< temp.first << ", " << temp.second << ")!" << endl;
						break;
					}
					getContinue(userIn);

					do
					{
						switch (userIn)
						{
						case 'y':
						case 'Y':
							inputValid = true;
							break;
						case 'n':
						case 'N':
							cont = false;
							inputValid = true;
							break;
						default:
							std::cout << "Cannot understand input!" << endl;
							getContinue(userIn);
							break;
						}
					} while (!inputValid);
				}
				break;
			case 7:
				std::cout << "Please enter the filepath to the new .csv file: " << endl;
				std::cin.getline(fileBuf, 255);
				if (!string(fileBuf).empty())
					iniFilePath = string(fileBuf);
				else
				{
					std::cout << "Unable to read in file path! Try again!" << endl;
					continue;
				}
				maze = InitializeMazeFromFile(iniFilePath);
				maze = GetOperationalMazeState(maze, startPair, goalPair);
				break;
			case 8:
				std::cout << "Exiting program..." << endl;
				std::cin.get();
				exit(0);
				break;
			default:
				std::cout << "Unable to interperet answer! Please try again with a valid integer (1-7)" << endl;
				break;
			}
		}
		catch (exception e)
		{
			std::cout << PROG_NAME << " ERROR: " << e.what() << "." << endl;
			std::cout << "Please review your input and try again!" << endl;
			std::cout << "Press enter to continue..." << endl;
			std::cin.get();
		}
	}
}

/*MAIN*/
int main(int argc, char* argv[], char* envp[])
{
	std::string iniFile, outFile;
	std::pair<int, int> startPair, goalPair;
	bool output = false;
	char pwd_buf[MAX_BUF], *iniFilePath;

	PROG_NAME = "Maze Finder";

	// If used with command line arguments
	if (argc > 1)
	{
		ParseCommandLine(argc, argv, iniFile, outFile, startPair, goalPair);
		/*HOTFIX CODE*/
		try 
		{
			if (!outFile.empty())
				output = std::stoi(outFile.c_str());
			else
				output = false;
		}
		catch (exception e)
		{
			std::cout << PROG_NAME << " ERROR: Unable to determine commandline option 'output'!" << endl;
			std::cout << "Exiting program..." << endl;
			std::cin.get();
			exit(1);
		}
	}
	// else assign default values
	else
	{
		//pwd_buf = new char[MAX_BUF];
		iniFilePath = new char[MAX_BUF] {'\0'};

		GetCurrentDirectoryA(MAX_BUF, (LPSTR)pwd_buf);
		cout << pwd_buf << endl;
		strcat_s(iniFilePath, (size_t)MAX_BUF, pwd_buf);
		strcat_s(iniFilePath, (size_t)MAX_BUF, "\\maze.csv");
		iniFile = std::string(iniFilePath);
		outFile = "";
		startPair = goalPair = std::pair<int, int>(-1, -1);
	}
	Menu(iniFile, output, startPair, goalPair);
	system("pause");
	return 0;
}

#pragma region Command Line Parsing

/*COMMAND LINE PARSING LOGIC*/
// Enumeration corresponding to the possible command line arguments
// If additional functionalityis added, an enumeration value should be 
// inserted below as well as the string equivolent associated in the CLMap
static enum CLOptions {
	InitPath,
	StartCoOrds,
	GoalCoOrds,
	OutputPath,
	DispHelp
};

// Maps string CL arguments to enumeration values above for use in a switch case
static std::map<std::string, CLOptions> CLMap = std::map<string, CLOptions>{
	{ "-i", InitPath },{ "--ini", InitPath },
	{ "-o", OutputPath },{ "--output", OutputPath },
	{ "-s", StartCoOrds },{ "--start", StartCoOrds },
	{ "-g", GoalCoOrds },{ "--goal", GoalCoOrds },
	{ "-h", DispHelp },{ "--help", DispHelp }
};

// Parse command line for valid argument, raises exception if invalid argument is passed
void ParseCommandLine(int argc, char** argv, std::string &iniFilePath, std::string &outFilePath, std::pair<int, int> &startPair, std::pair<int,int> &goalPair)
{
	std::queue<char*> argQ = std::queue<char*>();
	// push all the arguments passed into a queue for processing
	// start at one to skip the first argument (the name of the command that called this program)
	for (unsigned i = 1; i < argc; ++i)
		argQ.push(argv[i]);
	try
	{
		while (!argQ.empty())
		{
			std::string s_option = argQ.front();
			argQ.pop();
			CLOptions option = CLMap.at(s_option);

			switch (option)
			{
			case InitPath:
				iniFilePath = argQ.front();
				if (iniFilePath.find(".csv") == std::string::npos)
					throw invalid_argument("Invalid file name for initialize file! Must have the .ini suffix!\n");
				argQ.pop();
				break;
			case StartCoOrds:
				startPair = *GetCoOrds(argQ);
				break;
			case GoalCoOrds:
				goalPair = *GetCoOrds(argQ);
				break;
			case OutputPath:
				outFilePath = argQ.front();
				argQ.pop();
				break;
			case DispHelp:
				ShowUsage();
				break;
			default:
				throw invalid_argument("Cannot determine argument from value " + s_option);
			}
		}
	}
	catch (invalid_argument ia)
	{
		std::cout << PROG_NAME << " ERROR: Invalid argument passed to " << PROG_NAME << "!" << endl;
		std::cout << "\t" << ia.what() << endl;
		ShowUsage();
	}
	catch (exception ex)
	{
		std::cout << PROG_NAME << " ERROR: " << ex.what() << endl;
		ShowUsage();
	}
}

std::pair<int, int>* GetCoOrds(std::queue<char*> &q)
{
	int tempx = -1, tempy = -1;
	try 
	{
		tempx = std::stoi(q.front());
		q.pop();
		tempy = std::stoi(q.front());
		q.pop();

		if (tempx == -1 || tempy == -1)
			throw invalid_argument("Cannot interperet values for start coordinate. X: "
				+ std::to_string(tempx) + " Y: " + std::to_string(tempy));
		return new std::pair<int, int>(tempx, tempy);
	}
	catch (exception ex)
	{
		std::cout << "Parse Error: unable to determine values for co-ordinate pair!" << endl;
		throw ex;
	}
}

void ShowUsage()
{
	std::cout << "Usage: " << PROG_NAME << "<option> SOURCE(s) <option> SOURCE(s) etc..\n"
		<< "Options: \n"
		<< "\t-i, --ini\t\tInitialization file for this program. Must have the .csv exstention!\n"
		<< "\t         \t\tIf initialization file is in another directory, put the SOURCE in quotes (\"C:\\Example\\Directory\\init.csv\")\n"
		<< "\t-s, --start\t\tStarting coordinates for the search algorithms. This should be two integer values (X and Y) separated by a space (i.e. 3 5)\n"
		<< "\t-g, --goal\t\tThe coordinate position of the goal for the algorithm to find. This should be two integer values separated by a space (i.e. 4 6)\n"
		<< "\t-o, --output\t\tA boolean (0 or 1) value to output the BFS and DFS results to a file.\n"
		<< "\t-h, --help\t\tDisplay this message" << endl;
	exit(0);
}
/*END COMMAND LINE PARSING*/

#pragma endregion

// Display error information to console
void HandleException(exception ex)
{
	std::cout << PROG_NAME << "ERROR: " << ex.what() << endl;
}

BOOL InterruptHandler(DWORD sig)
{
	if (sig == CTRL_C_EVENT)
	{
		std::cout << "Stopping proess and exiting program..." << endl;
		std::cin.get();
		exit(0);
	}
}