#include "Data.h"

//ASIGNACION 
vector<vector<vector<vector<bool>>>> Data::assign_sol_data(GRBModel& model, GRBVar**** x)
{
	vector<vector<vector<vector<bool>>>> y(n_patterns, vector<vector<vector<bool>>>(n_days, std::vector<std::vector<bool>>(n_nurses, std::vector<bool>(n_jobs, false))));
	int j, d, w, p;
	int s_p;
	int s_w;
	for (d = 0; d < n_days; d++)
	{
		for (w = 0; w < n_nurses; w++)
		{
			for (j = 1; j < n_jobs; j++)
			{
				for (p = 0; p < n_patterns; p++)
				{
					y[p][d][w][j] = false;
					s_w = nurse[w].skill;
					s_p = pattern[p].day_p[d];
					if (s_p >= 1 && s_w >= s_p && nurse[w].patient_s[j] && pattern[p].patient_p[j])
					{
						if (x[p][d][w][j].get(GRB_DoubleAttr_Xn) > 0.01)
						{
							y[p][d][w][j] = true;
							break;
						}
					}
				}
			}
		}
	}
	return y;
}

vector<vector<vector<bool>>> Data::assignsimplify(vector<vector<vector<vector<bool>>>> patternassign)
{
	vector<vector<vector<bool>>> y(n_days, vector<vector<bool>>(n_nurses, vector<bool>(n_jobs, false)));
	int p, d, w, i, j;
	for (d = 0; d < n_days; d++)
	{
		for (w = 0; w < n_nurses; w++)
		{
			for (i = 0; i < n_jobs; i++)
			{
				for (p = 0; p < n_patterns; p++)
				{
					if (patternassign[p][d][w][i])
					{
						y[d][w][i] = true;
						break;
					}
				}
			}
		}
	}
	return y;
}



vector<vector<vector<vector<vector<bool>>>>> Data::routesolve(GRBModel model, GRBVar**** x, GRBVar*** a, double& total_time)
{
	//cout << "  //////////////////////////////////////" << endl;
	//cout << " ///////  ASIGNACION -> RUTA   ////////" << endl;
	//cout << "//////////////////////////////////////" << endl;
	vector<vector<vector<vector<vector<bool>>>>> route(n_patterns, vector<vector<vector<vector<bool>>>>(n_days, vector<vector<vector<bool>>>(n_nurses, vector<vector<bool>>(n_jobs, vector<bool>(n_jobs, false)))));
	vector<int> cluster_list;
	vector<vector<int>>cluster_list_jobs;
	for (int w = 0; w < n_nurses; w++)
	{
		cout << "nurse " << w << endl;
		for (int d = 0; d < n_days; d++)
		{
			cout << "day " << 0 << endl;
			cluster_list.clear();
			cluster_list.push_back(0);
			cluster_list_jobs = vector<vector<int>>(n_clusters, vector<int>(n_jobs));
			for (int c = 0; c < n_clusters; c++)
			{
				cluster_list_jobs[c].clear();
				if (c == 0)
					cluster_list_jobs[0].push_back(0);
				if (a[d][w][c].get(GRB_DoubleAttr_Xn) > 0.01)
				{
					if (c > 0)
						cluster_list.push_back(c);
					for (int j = 1; j < n_jobs; j++)
					{
						if (job[j].cluster == c)
						{
							for (int p = 0; p < n_patterns; p++)
							{
								if (pattern[p].day_p[d] >= 1 && nurse[w].patient_s[j] && nurse[w].skill >= pattern[p].day_p[d] && pattern[p].patient_p[j])
								{
									if (x[p][d][w][j].get(GRB_DoubleAttr_Xn) > 0.01)
									{
										cout << "cluster " << c << " job " << j << endl;
										cluster_list_jobs[c].push_back(j);
										break;
									}
								}
							}
						}
					}
					for (int k = 0; k < cluster_list_jobs[c].size() - 1; k++)
					{
						for (int p = 0; p < n_patterns; p++)
						{
							if (pattern[p].day_p[d] >= 1 && nurse[w].patient_s[cluster_list_jobs[c][k + 1]] && nurse[w].skill >= pattern[p].day_p[d] && pattern[p].patient_p[cluster_list_jobs[c][k + 1]])
							{
								if (x[p][d][w][cluster_list_jobs[c][k + 1]].get(GRB_DoubleAttr_Xn) > 0.01)
								{
									route[p][d][w][cluster_list_jobs[c][k]][cluster_list_jobs[c][k + 1]] = true;
									cout << "route[" << p << "][" << d << "][" << w << "][" << cluster_list_jobs[c][k] << "][" << cluster_list_jobs[c][k + 1] << "] = true" << endl;
									break;
								}
							}
						}
					}
				}
			}
			struct timeb t_after, t_ini;
			int sumcare = 0;
			int C = cluster_list.size();
			if (C > 3) // For C <= 3 the TSP solution is trivial
			{
				try
				{
					GRBEnv env = GRBEnv();
					GRBModel model = GRBModel(env);

					/* declare variables */
					GRBVar** y = new GRBVar * [C];
					GRBVar* u = new GRBVar[C];
					GRBVar D = model.addVar(0.0, INFINITY, 0.0, GRB_CONTINUOUS, "D");
					binary_vars_r(model, y, C);
					linear_vars_r(model, u, C);
					model.update();

					GRBLinExpr obj = D;
					model.setObjective(obj, GRB_MINIMIZE);
					model.update();

					/* Constraints */
					constraints_nurseday_r(model, y, D, cluster_list);
					constraints_patterns_r(model, y, C);
					constraints_MTZ_r(model, y, u, C);
					model.update();

					// Silence Gurobi output
					model.getEnv().set(GRB_IntParam_OutputFlag, 0);

					// Run the optimization
					model.getEnv().set(GRB_DoubleParam_TimeLimit, CPUmax);
					if (threads > -1)
					{
						model.getEnv().set(GRB_IntParam_Threads, threads);
					}
					ftime(&t_ini);
					model.optimize();
					ftime(&t_after);
					total_time += ((double)((t_after.time - t_ini.time) * 1000 + t_after.millitm - t_ini.millitm)) / 1000;
					for (int i = 0; i < C; i++)
					{
						for (int j = 0; j < C; j++)
						{
							if (i != j)
							{
								if (y[i][j].get(GRB_DoubleAttr_Xn) > 0.01)
								{
									int c0 = cluster_list[i];
									int c1 = cluster_list[j];
									if (cluster_list_jobs[c1].front() == 0)
									{
										route[0][d][w][cluster_list_jobs[c0].back()][0] = true;
										cout << "route[" << 0 << "][" << d << "][" << w << "][" << cluster_list_jobs[c0].back() << "][" << 0 << "] = true" << endl;
									}
									else
									{
										for (int p = 0; p < n_patterns; p++)
										{
											if (pattern[p].day_p[d] >= 1 && nurse[w].patient_s[cluster_list_jobs[c1].front()] && nurse[w].skill >= pattern[p].day_p[d] && pattern[p].patient_p[cluster_list_jobs[c1].front()])
											{
												if (x[p][d][w][cluster_list_jobs[c1].front()].get(GRB_DoubleAttr_Xn) > 0.01)
												{
													route[p][d][w][cluster_list_jobs[c0].back()][cluster_list_jobs[c1].front()] = true;
													cout << "route[" << p << "][" << d << "][" << w << "][" << cluster_list_jobs[c0].back() << "][" << cluster_list_jobs[c1].front() << "] = true" << endl;
													break;
												}
											}
										}
									}
								}
							}
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
			}
			else if (C > 0) // Caso C <= 3
			{
				//for (int L : cluster_list)
				//	cout << L << " ";
				cout << endl;
				for (int k = 0; k < C - 1; k++)
				{
					int c0 = cluster_list[k];
					int c1 = cluster_list[k+1];
					for (int p = 0; p < n_patterns; p++)
					{
						if (pattern[p].day_p[d] >= 1 && nurse[w].patient_s[cluster_list_jobs[c1].front()] && nurse[w].skill >= pattern[p].day_p[d] && pattern[p].patient_p[cluster_list_jobs[c1].front()])
						{
							if (x[p][d][w][cluster_list_jobs[c1].front()].get(GRB_DoubleAttr_Xn) > 0.01)
							{
								route[p][d][w][cluster_list_jobs[c0].back()][cluster_list_jobs[c1].front()] = true;
								cout << "route[" << p << "][" << d << "][" << w << "][" << cluster_list_jobs[c0].back() << "][" << cluster_list_jobs[c1].front() << "] = true" << endl;
								break;
							}
						}
					}
				}
				int c0 = cluster_list.back();
				int c1 = cluster_list.front();
				if (cluster_list_jobs[c0].back() != 0)
				{
					cout << "route[" << 0 << "][" << d << "][" << w << "][" << cluster_list_jobs[c0].back() << "][" << cluster_list_jobs[c1].front() << "] = true [HOSPITAL?]" << endl;
					route[0][d][w][cluster_list_jobs[c0].back()][cluster_list_jobs[c1].front()] = true;
				}
			}
		}
	}
	return route;
}


vector<vector<vector<vector<vector<bool>>>>> Data::routesolve_nurseday(vector<vector<vector<vector<bool>>>> patternassign, vector<vector<vector<vector<vector<bool>>>>>& route, vector<int> days, vector<int> nurses, double& total_time)
{
	struct timeb t_after, t_ini;
	ftime(&t_ini);
	vector<int> cluster_list;
	vector<vector<int>>cluster_list_jobs;
	int n_changes = days.size();
	if (nurses.size() != n_changes)
	{
		std::cout << "Error: nurses and days vectors must be the same size" << endl;
		return route;
	}
	vector<vector<vector<bool>>> a(n_days, vector<vector<bool>>(n_nurses, vector<bool>(n_clusters, false)));
	for (int p = 0; p < n_patterns; p++)
	{
		for (int d = 0; d < n_days; d++)
		{
			for (int w = 0; w < n_nurses; w++)
			{
				for (int j = 0; j < n_jobs; j++)
				{
					if (patternassign[p][d][w][j])
					{
						int s_p = pattern[p].day_p[d];
						int s_w = nurse[w].skill;
						if (s_p >= 1 && s_w >= s_p && nurse[w].patient_s[j] && pattern[p].patient_p[j])
						{
							a[d][w][job[j].cluster] = true;
						}
					}

				}
			}
		}
	}

	for (int l = 0; l < n_changes; l++)
	{
		int w = nurses[l];
		int d = days[l];
		for (int p = 0; p < n_patterns; p++)
		{
			route[p][d][w] = vector<vector<bool>>(n_jobs, vector<bool>(n_jobs, false));
		}

		cluster_list.clear();
		cluster_list.push_back(0);
		cluster_list_jobs = vector<vector<int>>(n_clusters, vector<int>(n_jobs));
		for (int c = 0; c < n_clusters; c++)
		{
			cluster_list_jobs[c].clear();
			if (c == 0)
				cluster_list_jobs[0].push_back(0);
			if (a[d][w][c])
			{
				if (c > 0)
					cluster_list.push_back(c);
				for (int j = 1; j < n_jobs; j++)
				{
					if (job[j].cluster == c)
					{
						for (int p = 0; p < n_patterns; p++)
						{
							if (pattern[p].day_p[d] >= 1 && nurse[w].patient_s[j] && nurse[w].skill >= pattern[p].day_p[d] && pattern[p].patient_p[j])
							{
								if (patternassign[p][d][w][j])
								{
									cluster_list_jobs[c].push_back(j);
									break;
								}
							}
						}
					}
				}
				for (int k = 0; k < cluster_list_jobs[c].size() - 1; k++)
				{
					for (int p = 0; p < n_patterns; p++)
					{
						if (pattern[p].day_p[d] >= 1 && nurse[w].patient_s[cluster_list_jobs[c][k + 1]] && nurse[w].skill >= pattern[p].day_p[d] && pattern[p].patient_p[cluster_list_jobs[c][k + 1]])
						{
							if (patternassign[p][d][w][cluster_list_jobs[c][k + 1]])
							{
								route[p][d][w][cluster_list_jobs[c][k]][cluster_list_jobs[c][k + 1]] = true;
								break;
							}
						}
					}
				}
			}
		}
		int sumcare = 0;
		int C = cluster_list.size();
		if (C > 3) // For C <= 3 the TSP solution is trivial
		{
			try
			{
				GRBEnv env = GRBEnv();
				GRBModel model = GRBModel(env);

				/* declare variables */
				GRBVar** y = new GRBVar * [C];
				GRBVar* u = new GRBVar[C];
				GRBVar D = model.addVar(0.0, INFINITY, 0.0, GRB_CONTINUOUS, "D");
				binary_vars_r(model, y, C);
				linear_vars_r(model, u, C);
				model.update();

				GRBLinExpr obj = D;
				model.setObjective(obj, GRB_MINIMIZE);
				model.update();

				/* Constraints */
				constraints_nurseday_r(model, y, D, cluster_list);
				constraints_patterns_r(model, y, C);
				constraints_MTZ_r(model, y, u, C);
				model.update();

				// Silence Gurobi output
				model.getEnv().set(GRB_IntParam_OutputFlag, 0);

				// Run the optimization
				model.getEnv().set(GRB_DoubleParam_TimeLimit, CPUmax);
				if (threads > -1)
				{
					model.getEnv().set(GRB_IntParam_Threads, threads);
				}
				model.optimize();
				for (int i = 0; i < C; i++)
				{
					for (int j = 0; j < C; j++)
					{
						if (i != j)
						{
							if (y[i][j].get(GRB_DoubleAttr_Xn) > 0.01)
							{
								int c0 = cluster_list[i];
								int c1 = cluster_list[j];
								if (cluster_list_jobs[c1].front() == 0)
								{
									route[0][d][w][cluster_list_jobs[c0].back()][0] = true;
									//cout << "route[" << 0 << "][" << d << "][" << w << "][" << cluster_list_jobs[c0].back() << "][" << 0 << "] = true" << endl;
								}
								else
								{
									for (int p = 0; p < n_patterns; p++)
									{
										if (pattern[p].day_p[d] >= 1 && nurse[w].patient_s[cluster_list_jobs[c1].front()] && nurse[w].skill >= pattern[p].day_p[d] && pattern[p].patient_p[cluster_list_jobs[c1].front()])
										{
											if (patternassign[p][d][w][cluster_list_jobs[c1].front()])
											{
												route[p][d][w][cluster_list_jobs[c0].back()][cluster_list_jobs[c1].front()] = true;
												//cout << "route[" << p << "][" << d << "][" << w << "][" << cluster_list_jobs[c0].back() << "][" << cluster_list_jobs[c1].front() << "] = true" << endl;
												break;
											}
										}
									}
								}
							}
						}
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
		}
		else if (C > 0) // Caso C <= 3
		{
			cout << endl;
			for (int k = 0; k < C - 1; k++)
			{
				int c0 = cluster_list[k];
				int c1 = cluster_list[k + 1];
				for (int p = 0; p < n_patterns; p++)
				{
					if (pattern[p].day_p[d] >= 1 && nurse[w].patient_s[cluster_list_jobs[c1].front()] && nurse[w].skill >= pattern[p].day_p[d] && pattern[p].patient_p[cluster_list_jobs[c1].front()])
					{
						if (patternassign[p][d][w][cluster_list_jobs[c1].front()])
						{
							route[p][d][w][cluster_list_jobs[c0].back()][cluster_list_jobs[c1].front()] = true;
							break;
						}
					}
				}
			}
			int c0 = cluster_list.back();
			int c1 = cluster_list.front();
			if (cluster_list_jobs[c0].back() != 0)
			{
				route[0][d][w][cluster_list_jobs[c0].back()][cluster_list_jobs[c1].front()] = true;
			}
		}
	}
	ftime(&t_after);
	total_time += ((double)((t_after.time - t_ini.time) * 1000 + t_after.millitm - t_ini.millitm)) / 1000;
	return route;
}


vector<vector<vector<vector<bool>>>> Data::routesimplify(vector<vector<vector<vector<vector<bool>>>>> patternroute)
{
	vector<vector<vector<vector<bool>>>> route(n_days, vector<vector<vector<bool>>>(n_nurses, vector<vector<bool>>(n_jobs, vector<bool>(n_jobs, false))));
	int p, d, w, i, j;
	for (d = 0; d < n_days; d++)
	{
		for (w = 0; w < n_nurses; w++)
		{
			for (i = 0; i < n_jobs; i++)
			{
				for (j = 0; j < n_jobs; j++)
				{
					for (p = 0; p < n_patterns; p++)
					{
						if (patternroute[p][d][w][i][j])
						{
							route[d][w][i][j] = true;
							break;
						}
					}
				}
			}
		}
	}
	return route;
}

vector<vector<vector<vector<bool>>>> Data::patternassign_from_patternroute_b(vector<vector<vector<vector<vector<bool>>>>> patternroute)
{
	vector<vector<vector<vector<bool>>>> y(n_patterns, vector<vector<vector<bool>>>(n_days, std::vector<std::vector<bool>>(n_nurses, std::vector<bool>(n_jobs, false))));
	int i, j, d, w, p;
	int s_p;
	int s_w;
	for (d = 0; d < n_days; d++)
	{
		for (w = 0; w < n_nurses; w++)
		{
			for (j = 0; j < n_jobs; j++)
			{
				for (p = 0; p < n_patterns; p++)
				{
					y[p][d][w][j] = false;
					s_w = nurse[w].skill;
					s_p = pattern[p].day_p[d];
					if (s_p >= 1 && s_w >= s_p && nurse[w].patient_s[j] && pattern[p].patient_p[j])
					{
						for (i = 0; i < n_jobs; i++)
						{
							if (patternroute[p][d][w][i][j])
							{
								y[p][d][w][j] = true;
								break;
							}
						}
					}
				}
			}
		}
	}
	return y;
}