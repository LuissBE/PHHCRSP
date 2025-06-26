#include "Data.h"

#pragma warning(disable:4786)
#pragma warning(disable:4996)
//ILOSTLBEGIN

#include <sys/timeb.h>

int main(int argc, char *argv[])
{
	Data problem;

	char **agv;

	//First, introduce(or specify) the folder.
    //Then, provide the name of the instance
	problem.setinstancesnames(argv);
	problem.read_instance();

	// -e Export lp. No export by default
	// -s Silence. Default is to print the result. 
	// -r Linear relaxation
	// -p Set preprocess to 0. By default it is 1 for all models but both ait and mk
	// -h html solution
	// -v visualize solution
	// -m criterium for local search movement acceptance
	//	  1- only accept dominating solutions, 2- accept solution if better wrt weighted sum (parameter can be tweaked in routeimprove.cpp -> accept_movement_improve)
	// -o 1- min max number of visited clusters in a week 2- min max number of jobs in a week 
	//	  3- min weighted sum of max visited clusters in a week and jobs in a week 4- min weighted sum of max visited clusters in a day and jobs in a day
	//	  6- min max number of visited clusters in a week EXCLUDING hospital cluster
	// -c number of threads. If nothing is set, it is left as default.
	// -m Model to solve 1-> after assigning, solve TSP for clusters per day and nurse
	// -t time limit for initial assignment model. If it is not provided, time limit is set to 3600 seconds.
	// -l time limit for the whole algorithm. If it is not provided, time limit is set to 3600 seconds.

	int algorithm=-2;
	int type_algorithm;
	int twe = 0;
	for (agv = argv + 3, argc--; argc > 2 && agv[0][0] == '-'; --argc, ++agv)
	{
		switch (agv[0][1])
		{
		default:
			break;
		case 'e':
			problem.setexport_lp(true);
			break;
		case 'r': //Linear Relaxation
			problem.set_rl(true);
			break;
		case 's':
			problem.setprint(false);
			break;
		case 'h':
			problem.sethtml(true);
			break;
		case 'v':
			problem.setvisualize(true);
			break;
		case 't':
			problem.setCPUmax((int)atoi(agv[1]));
			++agv;
			--argc;
			break;
		case 'l':
			problem.setTotalmax((int)atoi(agv[1]));
			++agv;
			--argc;
			break;
		case 'w':
			twe = (int)atoi(agv[1]);
			problem.settimewindows(twe);
			++agv;
			--argc;
			break;
		case 'c':
			problem.setthreads((int)atoi(agv[1]));
			++agv;
			--argc;
			break;
		case 'm':
			algorithm = (int)atoi(agv[1]);
			++agv;
			--argc;
			break;
		case 'f':
			problem.setfeas(true);
			++agv;
			--argc;
			break;
		case 'q':
			problem.setflagtmax(true);
			problem.setjob((int)atoi(agv[1]));
			++agv;
			--argc;
			break;
		case 'o':
			problem.setob(true);
			problem.setob((int)atoi(agv[1]));
			++agv;
			--argc;
			break;
		case 'g':
			problem.setMIPgap((double)stod(agv[1]));
			++agv;
			--argc;
			break;
		case 'x':
			problem.setrelaxation((double)stod(agv[1]));
			++agv;
			--argc;
			break;
		case 'T':
			problem.setgoal((double)stod(agv[1]));
			++agv;
			--argc;
			break;
		}
	}

	problem.setalgorithm(algorithm);
	if (algorithm == -2)
	{
		fprintf(stderr, "Se requiere el algoritmo correcto\n");
		return 0;
	}

	int it;
	double rl1;
	double rl2;


	switch (algorithm)
	{
		default:
			break;
		case 1: //LS only accepts dominated solutions
			problem.MIP_asignacion();
			break;
		case 2: //LS uses weighted sum
			problem.MIP_asignacion();
			break;
	}

	//problem.visualize_clusters2();
	return 0;
}
































