#include "Data.h"

int Data::MIP_asignacion()
{
	struct timeb t_after, t_ini;
	double total_time;
	int sumcare=0;
	bool hardmaxday = true;
soft_solve:
	try {
		/* declare variables */
		GRBEnv env = GRBEnv();
		GRBModel model = GRBModel(env);


		GRBVar ****x = new GRBVar***[n_patterns]; // x[p][d][w][i] = 1 sii enfermera w visita i el dia d segun patron p
		//GRBVar **u = new GRBVar*[n_days];	
		GRBVar*** a = new GRBVar * *[n_days]; // a[d][w][c] = 1 sii enfermera w visita cluster c el dia d
		GRBVar **z = new GRBVar*[n_nurses]; // z[w][i] = 1 si enfermera w visita paciente i 
		//GRBVar *D = new GRBVar[n_nurses];
		GRBVar** y = new GRBVar * [n_days]; // y[d][w] = numero de trabajos de enfermera w en dia d

		GRBVar m;
		string s = "m";
		m = model.addVar(0.0, INFINITY, 0.0, GRB_CONTINUOUS, s);

		// Variables////////////////////////////////////////
		binary_vars_a(model, x, z, a);
		//linear_vars_a(model, u);
		integer_vars_a(model, y);
		//linear_vars_n3(model, u, D); 
		model.update();

		// Objective Function////////////////////////////////////////
		GRBLinExpr obj = 0;
		if (ob == 1)
		{
			GRBVar dummy = model.addVar(0.0, INFINITY, 0.0, GRB_CONTINUOUS, "dummy");
			constraints_clusterdummy(model, a, dummy);
			obj = dummy;
		}
		else if (ob == 2)
		{
			GRBVar dummy = model.addVar(0.0, INFINITY, 0.0, GRB_CONTINUOUS, "dummy");
			constraints_maxjobdummy(model, y, dummy);
			obj = dummy;
		}
		else if (ob == 3)
		{
			GRBVar dummy1 = model.addVar(0.0, INFINITY, 0.0, GRB_CONTINUOUS, "dummy1");
			GRBVar dummy2 = model.addVar(0.0, INFINITY, 0.0, GRB_CONTINUOUS, "dummy2");
			constraints_clusterdummy(model, a, dummy1);
			constraints_maxjobdummy(model, y, dummy2);
			obj = n_jobs*dummy1 + n_clusters*dummy2;
		}
		else if (ob == 4)
		{
			GRBVar dummy1 = model.addVar(0.0, INFINITY, 0.0, GRB_CONTINUOUS, "dummy1");
			GRBVar dummy2 = model.addVar(0.0, INFINITY, 0.0, GRB_CONTINUOUS, "dummy2");
			GRBLinExpr ExprJ = 0;
			GRBLinExpr ExprC = 0;
			for (int w = 0; w < n_nurses; w++)
			{
				for (int d = 0; d < n_days; d++)
				{
					ExprJ = y[d][w];
					model.addConstr(ExprJ <= dummy2);
					ExprC = 0;
					for (int c = 0; c < n_clusters; c++)
					{
						ExprC += a[d][w][c];
					}
					model.addConstr(ExprC <= dummy1);
				}
			}
			obj = n_jobs * dummy1 + n_clusters * dummy2;
		}
		else if (ob == 5)
		{
			GRBVar dummy1 = model.addVar(0.0, INFINITY, 0.0, GRB_CONTINUOUS, "dummy1");
			GRBVar dummy2 = model.addVar(0.0, INFINITY, 0.0, GRB_CONTINUOUS, "dummy2");
			constraints_clusterdummy(model, a, dummy1);
			GRBLinExpr ExprJ = 0;
			GRBLinExpr ExprC = 0;
			for (int w = 0; w < n_nurses; w++)
			{
				for (int d = 0; d < n_days; d++)
				{
					ExprJ = y[d][w];
				}
			}
			obj = n_jobs*dummy1 + ExprJ;
		}
		else if (ob == 6)
		{
			//lOS CLUSTERS
			GRBVar dummy = model.addVar(0.0, INFINITY, 0.0, GRB_CONTINUOUS, "dummy");
			constraints_clusterdummy_c(model, a, dummy);
			obj = dummy;
		}
		model.setObjective(obj, GRB_MINIMIZE);
		model.update();

		// Constraints////////////////////////////////////////////
		constraints_patterns(model, x);
		constraints_skills2(model, x);
		constraints_maxday(model, x);
		constraints_assign(model, x, z);
		constraints_clusters(model, x, a);
		constraints_jobcount(model, x, y);
		if (hardmaxday)
		{
			constraints_hardmaxdayworkload(model, x, a); //Hard maximum workload constraint (eliminates some feasible solutions)
			cout << "constraints_hardmaxdayworkload active!" << endl;
		}
		else
		{
			constraints_softmaxdayworkload(model, x, a); //Soft maximum workload constraint (only eliminates infeasible solutions)
			cout << "constraints_softmaxdayworkload active!" << endl;
		}
		model.update();

		if (export_lp)
		{
			mkdir("Results");
			string lp_name;
			lp_name = data.substr(0, data.find(".txt"));
			lp_name = lp_name + ".lp";
			lp_name = "./Results/lpfgurob__" + lp_name;
			model.write(lp_name.c_str());
		}
		else
		{
			model.getEnv().set(GRB_DoubleParam_TimeLimit, CPUmax); 
			model.getEnv().set(GRB_IntParam_PoolSearchMode, 2); // búsqueda activa de soluciones óptimas diversas
			model.getEnv().set(GRB_IntParam_PoolSolutions, 10); // hasta 10 soluciones
			// Run the optimization
			if (feas)
			{
				model.getEnv().set(GRB_IntParam_SolutionLimit, 1);
			}
			if (rl)
			{
				model.getEnv().set(GRB_IntParam_Presolve, 0);
				model.getEnv().set(GRB_DoubleParam_NodeLimit, 0);
			}

			if (threads > -1)
			{
				model.getEnv().set(GRB_IntParam_Threads, threads);
			}
			ftime(&t_ini);
			model.optimize();
			ftime(&t_after);
			total_time = ((double)((t_after.time - t_ini.time) * 1000 + t_after.millitm - t_ini.millitm)) / 1000;


			int status = model.get(GRB_IntAttr_Status);
			int num_sol = model.get(GRB_IntAttr_SolCount);
			int number_var = model.get(GRB_IntAttr_NumVars);
			int number_varbin = model.get(GRB_IntAttr_NumBinVars);
			int number_const = model.get(GRB_IntAttr_NumConstrs);

			fprintf(stdout, "Status:\t%d\n", status);
			if (status == GRB_OPTIMAL || (status == GRB_TIME_LIMIT && num_sol > 0) || status == GRB_SOLUTION_LIMIT)
			{
				if (rl)
				{
					mkdir("Results");
					FILE * pFile;
					pFile = fopen("./Results/rl_solutions.txt", "a+");

					if (pFile != NULL)
					{
						fprintf(pFile, "\nAlgorithm:\tah%d_i0\tT:\t%d\tObjective:\t%d\tStatus:\t%d\t", algorithm, job[0].max_nurses, ob, status);
						fprintf(pFile, "Instance:\t%s\tObjValue:\t%.4f\tLB:\t-\tTime:\t%.2f\tThreads:\t%d\tPreprocess:\t%d", data.c_str(), model.get(GRB_DoubleAttr_ObjVal), total_time, threads, pre);
						fprintf(pFile, "\tBinVar:\t%d\tContVar:\t%d\tTVar:\t%d\tNumConst:\t%d", number_varbin, number_var - number_varbin, number_var, number_const);
						fclose(pFile);
					}
				}

				std::cout << "Obj: " << model.get(GRB_DoubleAttr_ObjVal) << std::endl;

				try
				{
					int solCount = model.get(GRB_IntAttr_SolCount);
					fprintf(stdout, "Se han encontrado %d soluciones\n", solCount);
					solCount = 1;
					for (int s = 0; s < solCount; ++s) {
						model.set(GRB_IntParam_SolutionNumber, s); // selecciona la s-ésima solución

						fprintf(stdout, "\n>> Analizando solución %d con objetivo %.3f\n", s, model.get(GRB_DoubleAttr_PoolObjVal));

						if (print)
						{
							//write_sol_n(model, x, z,u, D);

							write_sol_n23(model, x, z, a, y);
							vector<vector<vector<vector<bool>>>> patternassign = assign_sol_data(model, x);
							vector<vector<vector<bool>>> assign = assignsimplify(patternassign);
							vector<vector<vector<vector<vector<bool>>>>> patternroute(n_patterns, vector<vector<vector<vector<bool>>>>(n_days, vector<vector<vector<bool>>>(n_nurses, vector<vector<bool>>(n_jobs, vector<bool>(n_jobs, false)))));
							vector<vector<vector<vector<bool>>>> route(n_days, vector<vector<vector<bool>>>(n_nurses, vector<vector<bool>>(n_jobs, vector<bool>(n_jobs, false))));
							patternroute = routesolve(model, x, a, total_time);
							route = routesimplify(patternroute);
							vector<vector<double>> workload(n_days, vector<double>(n_nurses, 0));
							workload_calc(assign, route, workload, false);

							if (!hardmaxday)
							{
								cout << "Solution found using softmaxday constraints - testing for infeasibilities" << endl;
								if (!sanitycheck(patternroute))
								{
									cout << "Solution was INFEASIBLE - attempting to fix the solution" << endl;
									routefix_cl(patternroute, workload, t_ini);

									if (!sanitycheck(patternroute))
									{
										cout << "Solution not fixed within alloted time - exiting..." << endl;
										exit(1);
									}
									else
									{
										cout << "Solution fixed successfully - advancing to repair phase" << endl;
										route = routesimplify(patternroute);
										patternassign = patternassign_from_patternroute(patternroute);
										assign = assignsimplify(patternassign);
									}
								}
								else
								{
									cout << "Solution was FEASIBLE - advancing to repair phase" << endl;
								}
							}
							workload_calc_improvement(assign, route, workload, true, 0);

							fprintf(stderr, "\nAlgorithm:\tah%d\tT:\t%d\tObjective:\t%d\tStatus:\t%d\t", algorithm, job[0].max_nurses, ob, status);
							fprintf(stderr, "\tTime:\t%.2f\tThreads:\t%d\tPreprocess:\t%d", total_time, threads, pre);
							fprintf(stderr, "\tBinVar:\t%d\tContVar:\t%d\tTVar:\t%d\tNumConst:\t%d\n", number_varbin, number_var - number_varbin, number_var, number_const);

							if (visualize)
							{
								n_visualize_clusters2(assign, route);
							}

							routeimprove_cl(patternroute, workload, t_ini, total_time);
							route = routesimplify(patternroute);
							patternassign = patternassign_from_patternroute(patternroute);
							assign = assignsimplify(patternassign);
							//Output final solution
							fprintf(stderr, "\n");
							fprintf(stderr, "\nAlgorithm:\tah%d\tT:\t%d\tObjective:\t%d\tStatus:\t%d\t", algorithm, job[0].max_nurses, ob, status);
							fprintf(stderr, "\tTime:\t%.2f\tThreads:\t%d\tPreprocess:\t%d", total_time, threads, pre);
							fprintf(stderr, "\tBinVar:\t%d\tContVar:\t%d\tTVar:\t%d\tNumConst:\t%d\n", number_varbin, number_var - number_varbin, number_var, number_const);

							ftime(&t_after);
							total_time = ((double)((t_after.time - t_ini.time) * 1000 + t_after.millitm - t_ini.millitm)) / 1000;
							workload_calc(assign, route, workload, true);
							if (visualize)
							{

								n_visualize_clusters2(assign, route);
							}
							/*
							if (html)
								write_solution_csv_n3(model, x, z, u, D);*/
						}
					}
				}
				catch (GRBException e)
				{

					fprintf(stderr, "\nAlgorithm:\tah%d\tT:\t%d\tObjective:\t%d\tStatus:\t%d\t", algorithm, job[0].max_nurses, ob, status);
					fprintf(stderr, "Instance:\t%s\tObjValue:\t-\tLB:\t-\tTime:\t%.2f\tThreads:\t%d\tPreprocess:\t%d", data.c_str(), total_time, threads, pre);
					fprintf(stderr, "\tBinVar:\t%d\tContVar:\t%d\tTVar:\t%d\tNumConst:\t%d\n", number_varbin, number_var - number_varbin, number_var, number_const);
				}
			}
			else if (status == GRB_INFEASIBLE)
			{
				if (hardmaxday)
				{
					hardmaxday = false;
					cout << "############## LP with constraints_hardmaxday INFEASIBLE. Solving with constraints_softmaxday ##############" << endl;
					goto soft_solve;
				}
				else
				{
					fprintf(stderr, "\nAlgorithm:\tah%d\tT:\t%d\tObjective:\t%d\tStatus:\t%d\t", algorithm, job[0].max_nurses, ob, status);
					fprintf(stderr, "Instance:\t%s\tObjValue:\t-\tLB:\t-\tTime:\t%.2f\tThreads:\t%d\tPreprocess:\t%d", data.c_str(), total_time, threads, pre);
					fprintf(stderr, "\tBinVar:\t%d\tContVar:\t%d\tTVar:\t%d\tNumConst:\t%d\n", number_varbin, number_var - number_varbin, number_var, number_const);
					model.computeIIS();
					model.write("model.ilp");  // Optional: write to file for inspection
				}
			}
			else
			{

				double best_bound = model.get(GRB_DoubleAttr_ObjBound);

				fprintf(stderr, "\nAlgorithm:\tah%d\tT:\t%d\tObjective:\t%d\tStatus:\t%d\t", algorithm, job[0].max_nurses, ob, status);
				fprintf(stderr, "Instance:\t%s\tObjValue:\t-\tLB:\t%.2f\tTime:\t%.2f\tThreads:\t%d\tPreprocess:\t%d", data.c_str(), best_bound, total_time, threads, pre);
				fprintf(stderr, "\tBinVar:\t%d\tContVar:\t%d\tTVar:\t%d\tNumConst:\t%d\n", number_varbin, number_var - number_varbin, number_var, number_const);
			}
		}
	}
	catch (GRBException e)
	{
		std::cout << "Error code = "
			<< e.getErrorCode()
			<< std::endl;
		std::cout << e.getMessage() << std::endl;
	}
	catch (...)
	{
		std::cout << "Exception during optimization"
			<< std::endl;
	}
	return 1;
}
