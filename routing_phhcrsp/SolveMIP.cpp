#include "Data.h"

int Data::MIP_Cappanera()
{
	struct timeb t_after, t_ini;
	double total_time;
	GRBEnv env;

	try {
		/* declare variables */
		GRBEnv env = GRBEnv();
		GRBModel model = GRBModel(env);

		GRBVar **z = new GRBVar*[n_jobs];
		GRBVar **u = new GRBVar*[n_nurses];
		GRBVar ****x = new GRBVar***[n_days];
		GRBVar ***y = new GRBVar**[n_days];

		GRBVar **D = new GRBVar*[n_nurses];

		GRBVar m;
		string s = "m";
		m = model.addVar(0.0, INFINITY, 0.0, GRB_CONTINUOUS, s);

		// Variables////////////////////////////////////////
		binary_vars(model, z, u, x);
		integer_vars(model, y);
		linear_vars(model, D);
		model.update();

		// Objective Function////////////////////////////////////////
		
		GRBLinExpr obj = m;
		if (ob == 1)
		{
			model.setObjective(obj, GRB_MAXIMIZE);
		}
		else
		{
			model.setObjective(obj, GRB_MINIMIZE);
		}
		model.update();

		//// Constraints///////////////////////////////////////////////
		constraints_1(model, x, z); 
		constraints_2(model, x, z); 
		constraints_3(model, z);
		constraints_4(model, u);
		constraints_5(model, x, u);
		constraints_6(model, x, u); 
		constraints_7(model, x, D);
		constraints_8(model, x);
		constraints_9(model, y, z);
		constraints_10(model, y, z);
		constraints_11(model, y, x);
		
		if (ob == 1)
		{
			constraints_o1(model, D, m);
		}
		else
		{
			constraints_o2(model, D, m);
		}

		constraints_zero(model, z,x);
		constraints_18(model, x);
		constraints_19(model, x);

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
			// Run the optimizion


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
						fprintf(pFile, "\nAlgorithm:\t%d\tObjective:\t%d\tStatus:\t%d\t", algorithm, ob, status);
						fprintf(pFile, "Instance:\t%s\tObjValue:\t%.4f\tLB:\t-\tTime:\t%.2f\tThreads:\t%d\tPreprocess:\t%d", data.c_str(), model.get(GRB_DoubleAttr_ObjVal), total_time, threads, pre);
						fprintf(pFile, "\tBinVar:\t%d\tContVar:\t%d\tTVar:\t%d\tNumConst:\t%d", number_varbin, number_var - number_varbin, number_var, number_const);
						fclose(pFile);
					}
				}

				std::cout << "Obj: " << model.get(GRB_DoubleAttr_ObjVal) << std::endl;

				try
				{
					fprintf(stdout, "Solution:\t%.3f\n", model.get(GRB_DoubleAttr_ObjVal));
					if (print)
					{
						fprintf(stderr, "Algorithm:\t%d\tObjective:\t%d\tStatus:\t%d\t", algorithm, ob, status);

						write_sol(model,z,u,x,y,D);
						fprintf(stderr, "\tTime:\t%.2f\tThreads:\t%d\tPreprocess:\t%d", total_time, threads, pre);
						fprintf(stderr, "\tBinVar:\t%d\tContVar:\t%d\tTVar:\t%d\tNumConst:\t%d\n", number_varbin, number_var - number_varbin, number_var, number_const);


						if (html)
							write_solution_csv(model, z, u, x, y, D);

					}
				}
				catch (GRBException e)
				{
					fprintf(stderr, "Algorithm:\t%d\tObjective:\t%d\tStatus:\t%d\t", algorithm, ob, status);
					fprintf(stderr, "Instance:\t%s\tObjValue:\t-\tLB:\t-\tTime:\t%.2f\tThreads:\t%d\tPreprocess:\t%d", data.c_str(), total_time, threads, pre);
					fprintf(stderr, "\tBinVar:\t%d\tContVar:\t%d\tTVar:\t%d\tNumConst:\t%d\n", number_varbin, number_var - number_varbin, number_var, number_const);
				}
			}
			else if (status == GRB_INFEASIBLE)
			{

				fprintf(stderr, "Algorithm:\t%d\tObjective:\t%d\tStatus:\t%d\t", algorithm, ob, status);
				fprintf(stderr, "Instance:\t%s\tObjValue:\t-\tLB:\t-\tTime:\t%.2f\tThreads:\t%d\tPreprocess:\t%d", data.c_str(), total_time, threads, pre);
				fprintf(stderr, "\tBinVar:\t%d\tContVar:\t%d\tTVar:\t%d\tNumConst:\t%d\n", number_varbin, number_var - number_varbin, number_var, number_const);
			}
			else
			{

				double best_bound = model.get(GRB_DoubleAttr_ObjBound);
				fprintf(stderr, "Algorithm:\t%d\tObjective:\t%d\tStatus:\t%d\t", algorithm, ob, status);
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



int Data::MIP_New3()
{
	struct timeb t_after, t_ini;
	double total_time;
	int sumcare=0;
	try {
		/* declare variables */
		GRBEnv env = GRBEnv();
		GRBModel model = GRBModel(env);


		GRBVar *****x = new GRBVar****[n_patterns];
		GRBVar **u = new GRBVar*[n_days];
		GRBVar **z = new GRBVar*[n_nurses];
		GRBVar *D = new GRBVar[n_nurses];
		GRBVar ****y = new GRBVar***[n_days];

		GRBVar m;
		string s = "m";
		m = model.addVar(0.0, INFINITY, 0.0, GRB_CONTINUOUS, s);

		// Variables////////////////////////////////////////
		binary_vars_n(model, x, z);
		integer_vars_n(model, y);
		linear_vars_n3(model, u, D); //1707-Cambio dominio de D
		model.update();

		// Objective Function////////////////////////////////////////
		GRBLinExpr obj;
		if (ob == 1)
		{
			//Min suma total
			for (int w = 0; w < n_nurses; w++)
			{
				obj += D[w];
			}
			model.setObjective(obj, GRB_MINIMIZE);
		}
		else if (ob == 2)
		{
			//Max min carga
			obj = m;
			model.setObjective(obj, GRB_MAXIMIZE);
		}
		else if (ob == 3)
		{
			//Min max carga
			obj = m;
			model.setObjective(obj, GRB_MINIMIZE);
		}
		else if (ob == 4)
		{
			//Min suma total y max min
			for (int w = 0; w < n_nurses; w++)
			{
				obj += l1 * D[w];
			}

			for (int w = 0; w < n_nurses; w++)
			{
				sumcare= sumcare + nurse[w].D;
			}
			sumcare = sumcare * n_days;
			obj -= l2 * m*sumcare;
			model.setObjective(obj, GRB_MINIMIZE);
		}
		else if (ob == 5)
		{
			//Min suma total y min max
			for (int w = 0; w < n_nurses; w++)
			{
				obj += l1 * D[w];
			}
			for (int w = 0; w < n_nurses; w++)
			{
				sumcare = sumcare + nurse[w].D;
			}
			sumcare = sumcare * n_days;
			obj += l2 * m*sumcare;
			model.setObjective(obj, GRB_MINIMIZE);
		}
		model.update();


		

		// Constraints////////////////////////////////////////////
		if (ob == 2 || ob == 4)
		{
			constraints_o13(model, D, m);
		}
		else if (ob == 3 || ob == 5)
		{
			constraints_o23(model, D, m);
		}

		constraints_patterns(model, x);
		constraints_skills(model, x);
		constraints_maxday(model, x);
		constraints_symmetry(model, x);
		constraints_maxwork(model, x); //Esta estaba dentro de la siguiente
		constraints_nursesday3(model, x, D);
		if (r2 == 1)
			constraints_breaksymmetries_totaldays3(model, D);


		constraints_clusters_x1(model, x);
		if(r4 ==1)
			constraints_clusters_x(model, x);
		if (r5 == 1)
			constraints_paridad(model, x);

		constraints_flow_n36(model, x, u); //Con las variables u de tiempo
		if (r3 == 1)
			constraints_relate_flow(model, x, u);

		if (flagTmax == false || job[0].max_nurses < n_days)
		{
			constraints_assign(model, x, z);
			if (r1 == 1)
				constraints_assign_ad(model, x, z);
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
						fprintf(pFile, "\nAlgorithm:\t%d_v%d_%d%d%d%d%d_%.1f_%.1f\tT:\t%d\tObjective:\t%d\tStatus:\t%d\t", algorithm, version, r1, r2, r3, r4,r5,l1,l2, job[0].max_nurses,ob, status);
						fprintf(pFile, "Instance:\t%s\tObjValue:\t%.4f\tLB:\t-\tTime:\t%.2f\tThreads:\t%d\tPreprocess:\t%d", data.c_str(), model.get(GRB_DoubleAttr_ObjVal), total_time, threads, pre);
						fprintf(pFile, "\tBinVar:\t%d\tContVar:\t%d\tTVar:\t%d\tNumConst:\t%d", number_varbin, number_var - number_varbin, number_var, number_const);
						fclose(pFile);
					}
				}

				std::cout << "Obj: " << model.get(GRB_DoubleAttr_ObjVal) << std::endl;

				try
				{
					fprintf(stdout, "Solution:\t%.3f\n", model.get(GRB_DoubleAttr_ObjVal));
					if (print)
					{

						fprintf(stderr, "\nAlgorithm:\t%d_v%d_%d%d%d%d%d_%.1f_%.1f\tT:\t%d\tObjective:\t%d\tStatus:\t%d\t", algorithm, version, r1, r2, r3, r4, r5, l1, l2, job[0].max_nurses, ob, status);
						//write_sol_n(model, x, z,u, D);

						write_sol_n23(model, x, z, y, D);


						fprintf(stderr, "\tTime:\t%.2f\tThreads:\t%d\tPreprocess:\t%d", total_time, threads, pre);
						fprintf(stderr, "\tBinVar:\t%d\tContVar:\t%d\tTVar:\t%d\tNumConst:\t%d\n", number_varbin, number_var - number_varbin, number_var, number_const);

						write_sol_n23_b(model, x, z, y, D);

						if (html)
							write_solution_csv_n3(model, x, z, u, D);

					}
				}
				catch (GRBException e)
				{

					fprintf(stderr, "\nAlgorithm:\t%d_v%d_%d%d%d%d%d_%.1f_%.1f\tT:\t%d\tObjective:\t%d\tStatus:\t%d\t", algorithm, version, r1, r2, r3, r4, r5, l1, l2, job[0].max_nurses, ob, status);
					fprintf(stderr, "Instance:\t%s\tObjValue:\t-\tLB:\t-\tTime:\t%.2f\tThreads:\t%d\tPreprocess:\t%d", data.c_str(), total_time, threads, pre);
					fprintf(stderr, "\tBinVar:\t%d\tContVar:\t%d\tTVar:\t%d\tNumConst:\t%d\n", number_varbin, number_var - number_varbin, number_var, number_const);
				}
			}
			else if (status == GRB_INFEASIBLE)
			{

				fprintf(stderr, "\nAlgorithm:\t%d_v%d_%d%d%d%d%d_%.1f_%.1f\tT:\t%d\tObjective:\t%d\tStatus:\t%d\t", algorithm, version, r1, r2, r3, r4, r5, l1, l2, job[0].max_nurses, ob, status);
				fprintf(stderr, "Instance:\t%s\tObjValue:\t-\tLB:\t-\tTime:\t%.2f\tThreads:\t%d\tPreprocess:\t%d", data.c_str(), total_time, threads, pre);
				fprintf(stderr, "\tBinVar:\t%d\tContVar:\t%d\tTVar:\t%d\tNumConst:\t%d\n", number_varbin, number_var - number_varbin, number_var, number_const);
			}
			else
			{

				double best_bound = model.get(GRB_DoubleAttr_ObjBound);

				fprintf(stderr, "\nAlgorithm:\t%d_v%d_%d%d%d%d%d_%.1f_%.1f\tT:\t%d\tObjective:\t%d\tStatus:\t%d\t", algorithm, version, r1, r2, r3, r4, r5, l1, l2, job[0].max_nurses, ob, status);
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

int Data::MIP_NewRanking()
{
	struct timeb t_after, t_ini;
	double total_time;
	int sumcare = 0;
	try {
		/* declare variables */
		GRBEnv env = GRBEnv();
		GRBModel model = GRBModel(env);

		GRBVar***** x = new GRBVar * ***[n_patterns];
		GRBVar** u = new GRBVar * [n_days];
		GRBVar** z = new GRBVar * [n_nurses];
		GRBVar* D = new GRBVar[n_nurses];
		GRBVar**** y = new GRBVar * **[n_days];

		// Variables////////////////////////////////////////
		binary_vars_n(model, x, z);
		integer_vars_n(model, y);
		linear_vars_n3(model, u, D); //1707-Cambio dominio de D
		model.update();

		constraints_patterns(model, x);
		constraints_skills(model, x);
		constraints_maxday(model, x);
		constraints_symmetry(model, x);
		constraints_maxwork(model, x); //Esta estaba dentro de la siguiente
		constraints_nursesday3(model, x, D);
		if (r2 == 1)
			constraints_breaksymmetries_totaldays3(model, D);


		constraints_clusters_x1(model, x);
		if (r4 == 1)
			constraints_clusters_x(model, x);
		if (r5 == 1)
			constraints_paridad(model, x);

		constraints_flow_n36(model, x, u); //Con las variables u de tiempo
		if (r3 == 1)
			constraints_relate_flow(model, x, u);

		if (flagTmax == false || job[0].max_nurses < n_days)
		{
			constraints_assign(model, x, z);
			if (r1 == 1)
				constraints_assign_ad(model, x, z);
		}

		// First objective function
		GRBLinExpr obj = 0;
		for (int w = 0; w < n_nurses; w++)
		{
			obj += D[w];
		}
		model.setObjective(obj, GRB_MINIMIZE);
		model.update();

		model.getEnv().set(GRB_DoubleParam_TimeLimit, CPUmax*0.75);
		// Run the first optimization
		model.getEnv().set(GRB_DoubleParam_MIPGap, MIPgap);
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


		GRBVar m = model.addVar(0.0, INFINITY, 0.0, GRB_CONTINUOUS, "m");
		GRBVar M = model.addVar(0.0, INFINITY, 0.0, GRB_CONTINUOUS, "M");
		constraints_o13(model, D, m);
		constraints_o23(model, D, M);

		obj = M - m;
		model.setObjective(obj, GRB_MINIMIZE);
		model.update();

		// Run the second optimization
		model.getEnv().set(GRB_DoubleParam_MIPGap, 0.0001);
		model.getEnv().set(GRB_DoubleParam_TimeLimit, CPUmax - total_time);
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
		total_time += ((double)((t_after.time - t_ini.time) * 1000 + t_after.millitm - t_ini.millitm)) / 1000;

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
				FILE* pFile;
				pFile = fopen("./Results/rl_solutions.txt", "a+");

				if (pFile != NULL)
				{
					fprintf(pFile, "\nAlgorithm:\t%d_v%d_%d%d%d%d%d_%.4f_%.4f\tT:\t%d\tObjective:\t%d\tStatus:\t%d\t", algorithm, version, r1, r2, r3, r4, r5, MIPgap, relaxation, job[0].max_nurses, ob, status);
					fprintf(pFile, "Instance:\t%s\tObjValue:\t%.4f\tLB:\t-\tTime:\t%.2f\tThreads:\t%d\tPreprocess:\t%d", data.c_str(), model.get(GRB_DoubleAttr_ObjVal), total_time, threads, pre);
					fprintf(pFile, "\tBinVar:\t%d\tContVar:\t%d\tTVar:\t%d\tNumConst:\t%d", number_varbin, number_var - number_varbin, number_var, number_const);
					fclose(pFile);
				}
			}

			std::cout << "Obj: " << model.get(GRB_DoubleAttr_ObjVal) << std::endl;

			try
			{
				fprintf(stdout, "Solution:\t%.3f\n", model.get(GRB_DoubleAttr_ObjVal));
				if (print)
				{
					double val_TW = 0;
					for (int w = 0; w < n_nurses; w++)
					{
						val_TW += D[w].get(GRB_DoubleAttr_X);
					}
					double val_minW = m.get(GRB_DoubleAttr_X);
					double val_maxW = M.get(GRB_DoubleAttr_X);

					fprintf(stderr, "\nAlgorithm:\t%d_v%d_%d%d%d%d%d_%.4f_%.4f\tT:\t%d\tObjective:\t%d\tStatus:\t%d\t", algorithm, version, r1, r2, r3, r4, r5, MIPgap, relaxation, job[0].max_nurses, ob, status);

					fprintf(stderr, "\tTime:\t%.2f\tThreads:\t%d\tPreprocess:\t%d", total_time, threads, pre);
					fprintf(stderr, "\tBinVar:\t%d\tContVar:\t%d\tTVar:\t%d\tNumConst:\t%d\n", number_varbin, number_var - number_varbin, number_var, number_const);
					fprintf(stderr, "\tTotalWorkload:\t%.0f\tMinWorkload:\t%.4f\tMaxWorkload:\t%.4f\n", val_TW, val_minW, val_maxW);

					//write_sol_n(model, x, z,u, D);

					write_sol_ranking(model, x, z, y, D);


					if (html)
						write_solution_csv_n3(model, x, z, u, D);

				}
			}
			catch (GRBException e)
			{

				fprintf(stderr, "\nAlgorithm:\t%d_v%d_%d%d%d%d%d_%.4f_%.4f\tT:\t%d\tObjective:\t%d\tStatus:\t%d\t", algorithm, version, r1, r2, r3, r4, r5, MIPgap, relaxation, job[0].max_nurses, ob, status);
				fprintf(stderr, "Instance:\t%s\tObjValue:\t-\tLB:\t-\tTime:\t%.2f\tThreads:\t%d\tPreprocess:\t%d", data.c_str(), total_time, threads, pre);
				fprintf(stderr, "\tBinVar:\t%d\tContVar:\t%d\tTVar:\t%d\tNumConst:\t%d\n", number_varbin, number_var - number_varbin, number_var, number_const);
			}
		}
		else if (status == GRB_INFEASIBLE)
		{

			fprintf(stderr, "\nAlgorithm:\t%d_v%d_%d%d%d%d%d_%.4f_%.4f\tT:\t%d\tObjective:\t%d\tStatus:\t%d\t", algorithm, version, r1, r2, r3, r4, r5, MIPgap, relaxation, job[0].max_nurses, ob, status);
			fprintf(stderr, "Instance:\t%s\tObjValue:\t-\tLB:\t-\tTime:\t%.2f\tThreads:\t%d\tPreprocess:\t%d", data.c_str(), total_time, threads, pre);
			fprintf(stderr, "\tBinVar:\t%d\tContVar:\t%d\tTVar:\t%d\tNumConst:\t%d\n", number_varbin, number_var - number_varbin, number_var, number_const);
		}
		else
		{

			double best_bound = model.get(GRB_DoubleAttr_ObjBound);

			fprintf(stderr, "\nAlgorithm:\t%d_v%d_%d%d%d%d%d_%.4f_%.4f\tT:\t%d\tObjective:\t%d\tStatus:\t%d\t", algorithm, version, r1, r2, r3, r4, r5, MIPgap, relaxation, job[0].max_nurses, ob, status);
			fprintf(stderr, "Instance:\t%s\tObjValue:\t-\tLB:\t%.2f\tTime:\t%.2f\tThreads:\t%d\tPreprocess:\t%d", data.c_str(), best_bound, total_time, threads, pre);
			fprintf(stderr, "\tBinVar:\t%d\tContVar:\t%d\tTVar:\t%d\tNumConst:\t%d\n", number_varbin, number_var - number_varbin, number_var, number_const);
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

int Data::MIP_NewGoal()
{
	struct timeb t_after, t_ini;
	double total_time;
	int sumcare = 0;
	try {
		/* declare variables */
		GRBEnv env = GRBEnv();
		GRBModel model = GRBModel(env);


		GRBVar***** x = new GRBVar * ***[n_patterns];
		GRBVar** u = new GRBVar * [n_days];
		GRBVar** z = new GRBVar * [n_nurses];
		GRBVar* D = new GRBVar[n_nurses];
		GRBVar**** y = new GRBVar * **[n_days];

		// Variables////////////////////////////////////////
		binary_vars_n(model, x, z);
		integer_vars_n(model, y);
		linear_vars_n3(model, u, D); //1707-Cambio dominio de D
		model.update();

		// Objective Function////////////////////////////////////////	
		//min carga total
		GRBLinExpr obj;
		for (int w = 0; w < n_nurses; w++)
		{
			obj += D[w];
		};

		model.setObjective(obj, GRB_MINIMIZE);
		
		model.update();

		// Constraints////////////////////////////////////////////

		constraints_patterns(model, x);
		constraints_skills(model, x);
		constraints_maxday(model, x);
		constraints_symmetry(model, x);
		constraints_maxwork(model, x); //Esta estaba dentro de la siguiente
		constraints_nursesday3(model, x, D);
		if (r2 == 1)
			constraints_breaksymmetries_totaldays3(model, D);


		constraints_clusters_x1(model, x);
		if (r4 == 1)
			constraints_clusters_x(model, x);
		if (r5 == 1)
			constraints_paridad(model, x);

		constraints_flow_n36(model, x, u); //Con las variables u de tiempo
		if (r3 == 1)
			constraints_relate_flow(model, x, u);

		if (flagTmax == false || job[0].max_nurses < n_days)
		{
			constraints_assign(model, x, z);
			if (r1 == 1)
				constraints_assign_ad(model, x, z);
		}
		model.update();

		// Run the optimization for rl

		GRBModel lrmodel = model.relax();
		lrmodel.getEnv().set(GRB_DoubleParam_TimeLimit, CPUmax);
		if (threads > -1)
		{
			lrmodel.getEnv().set(GRB_IntParam_Threads, threads);
		}

		ftime(&t_ini);
		lrmodel.optimize();
		ftime(&t_after);
		total_time = ((double)((t_after.time - t_ini.time) * 1000 + t_after.millitm - t_ini.millitm)) / 1000;

		double TWgoal = lrmodel.get(GRB_DoubleAttr_ObjVal); //total workload goal

		//MAIN OPTIMIZATION
		GRBVar m = model.addVar(0.0, INFINITY, 0.0, GRB_CONTINUOUS, "m");
		GRBVar M = model.addVar(0.0, INFINITY, 0.0, GRB_CONTINUOUS, "M");
		GRBVar d = model.addVar(0.0, INFINITY, 0.0, GRB_CONTINUOUS, "d");
		obj = 0;

		//for(int w = 0; w < n_nurses; w++)
		//{
		//	obj += eqgoal*D[w];
		//}

		//obj -= eqgoal * TWgoal;

		obj += TWgoal*d;
		model.setObjective(obj, GRB_MINIMIZE);
	
		constraints_o13(model, D, m);
		constraints_o23(model, D, M);
		constraints_goaldummy(model, m, M, d);
		model.update();

		model.getEnv().set(GRB_DoubleParam_TimeLimit, CPUmax - total_time);
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
		total_time += ((double)((t_after.time - t_ini.time) * 1000 + t_after.millitm - t_ini.millitm)) / 1000;


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
				FILE* pFile;
				pFile = fopen("./Results/rl_solutions.txt", "a+");

				if (pFile != NULL)
				{
					fprintf(pFile, "\nAlgorithm:\t%d_v%d_%d%d%d%d%d_%.4f\tT:\t%d\tObjective:\t%d\tStatus:\t%d\t", algorithm, version, r1, r2, r3, r4, r5, eqgoal, job[0].max_nurses, ob, status);
					fprintf(pFile, "Instance:\t%s\tObjValue:\t%.4f\tLB:\t-\tTime:\t%.2f\tThreads:\t%d\tPreprocess:\t%d", data.c_str(), model.get(GRB_DoubleAttr_ObjVal), total_time, threads, pre);
					fprintf(pFile, "\tBinVar:\t%d\tContVar:\t%d\tTVar:\t%d\tNumConst:\t%d", number_varbin, number_var - number_varbin, number_var, number_const);
					fclose(pFile);
				}
			}

			std::cout << "Obj: " << model.get(GRB_DoubleAttr_ObjVal) << std::endl;

			try
			{
				fprintf(stdout, "Solution:\t%.3f\n", model.get(GRB_DoubleAttr_ObjVal));
				if (print)
				{
					double val_TW = 0;
					for(int w = 0; w < n_nurses; w++)
					{
						val_TW += D[w].get(GRB_DoubleAttr_X);
					}
					double val_minW = m.get(GRB_DoubleAttr_X);
					double val_maxW = M.get(GRB_DoubleAttr_X);

					fprintf(stderr, "\nAlgorithm:\t%d_v%d_%d%d%d%d%d_%.4f\tT:\t%d\tObjective:\t%d\tStatus:\t%d\t", algorithm, version, r1, r2, r3, r4, r5, eqgoal, job[0].max_nurses, ob, status);
					//write_sol_n(model, x, z,u, D);
					fprintf(stderr, "\tTime:\t%.2f\tThreads:\t%d\tPreprocess:\t%d", total_time, threads, pre);
					fprintf(stderr, "\tBinVar:\t%d\tContVar:\t%d\tTVar:\t%d\tNumConst:\t%d", number_varbin, number_var - number_varbin, number_var, number_const);
					fprintf(stderr, "\tTotalWorkload:\t%.0f\tMinWorkload:\t%.4f\tMaxWorkload:\t%.4f\n", val_TW, val_minW, val_maxW);

					write_sol_goal(model, x, z, y, D);

					if (html)
						write_solution_csv_n3(model, x, z, u, D);

				}
			}
			catch (GRBException e)
			{

				fprintf(stderr, "\nAlgorithm:\t%d_v%d_%d%d%d%d%d_%.4f\tT:\t%d\tObjective:\t%d\tStatus:\t%d\t", algorithm, version, r1, r2, r3, r4, r5, eqgoal, job[0].max_nurses, ob, status);
				fprintf(stderr, "Instance:\t%s\tObjValue:\t-\tLB:\t-\tTime:\t%.2f\tThreads:\t%d\tPreprocess:\t%d", data.c_str(), total_time, threads, pre);
				fprintf(stderr, "\tBinVar:\t%d\tContVar:\t%d\tTVar:\t%d\tNumConst:\t%d\n", number_varbin, number_var - number_varbin, number_var, number_const);
			}
		}
		else if (status == GRB_INFEASIBLE)
		{

			fprintf(stderr, "\nAlgorithm:\t%d_v%d_%d%d%d%d%d_%.4f\tT:\t%d\tObjective:\t%d\tStatus:\t%d\t", algorithm, version, r1, r2, r3, r4, r5, eqgoal, job[0].max_nurses, ob, status);
			fprintf(stderr, "Instance:\t%s\tObjValue:\t-\tLB:\t-\tTime:\t%.2f\tThreads:\t%d\tPreprocess:\t%d", data.c_str(), total_time, threads, pre);
			fprintf(stderr, "\tBinVar:\t%d\tContVar:\t%d\tTVar:\t%d\tNumConst:\t%d\n", number_varbin, number_var - number_varbin, number_var, number_const);
		}
		else
		{

			double best_bound = model.get(GRB_DoubleAttr_ObjBound);

			fprintf(stderr, "\nAlgorithm:\t%d_v%d_%d%d%d%d%d_%.4f\tT:\t%d\tObjective:\t%d\tStatus:\t%d\t", algorithm, version, r1, r2, r3, r4, r5, eqgoal, job[0].max_nurses, ob, status);
			fprintf(stderr, "Instance:\t%s\tObjValue:\t-\tLB:\t%.2f\tTime:\t%.2f\tThreads:\t%d\tPreprocess:\t%d", data.c_str(), best_bound, total_time, threads, pre);
			fprintf(stderr, "\tBinVar:\t%d\tContVar:\t%d\tTVar:\t%d\tNumConst:\t%d\n", number_varbin, number_var - number_varbin, number_var, number_const);
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

