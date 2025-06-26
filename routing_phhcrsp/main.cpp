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
	// -o For Cappanera 1 Maxmin 2 MinMax// For our model: 1 Min Total 2 Maxmin 3 Minmax 4 Total+Maxmin 5 Total+Minmax 
	// -c number of threads. If nothing is set, it is left as default.
	// -m Model to solve 0-> Cappanera 1-> Proposed model 2-> All constraints combinations 3-> Proposed model varying lambdas
	//					6-> Hierarchical Optimisation 9-> Goal programming method
	// -t 60 change time limit to 60 seconds or other time limit. If it is not provided, time limit is set to 3600 seconds.

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
		case 't':
			problem.setCPUmax((int)atoi(agv[1]));
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
		case 0:
			problem.MIP_Cappanera();
			break;
		case 1:
			problem.MIP_New3();
			break;
		case 2:
			problem.setr1(1);
			problem.setr3(1);
			problem.setr4(1);
			problem.MIP_New3();
			break;
		case 4:
			for (int r1 = 0; r1 < 2; r1++)
			{
				problem.setr1(r1);
				for (int r2 = 0; r2 < 2; r2++)
				{
					problem.setr2(r2);
					for (int r3 = 0; r3 < 2; r3++)
					{
						problem.setr3(r3);
						for (int r4 = 0; r4 < 2; r4++)
						{
							problem.setr4(r4);
							for (int r5 = 0; r5 < 2; r5++)
							{
								problem.setr5(r5);
								problem.MIP_New3();
							}
						}
					}
				}
			}
			break;
		case 5:
			problem.setr1(1);
			problem.setr3(1);
			problem.setr4(1); 
			
			rl1=0; 
			rl2 = 1;
			while (rl1 <= 1)
			{
				problem.setl1(rl1);
				problem.setl2(rl2);
				if (rl1 != 0.5)
				{
					problem.MIP_New3();
				}
				rl1 = rl1+0.1;
				rl2 = rl2 - 0.1;
			}
			
		
			break;
		case 6:
			problem.setr1(1);
			problem.setr3(1);
			problem.setr4(1);
			problem.MIP_NewRanking();
			break;
		case 9:
			problem.setr1(1);
			problem.setr3(1);
			problem.setr4(1);
			problem.MIP_NewGoal();
			break;

	}

	return 0;
}
































