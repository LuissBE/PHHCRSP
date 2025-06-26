#include "Data.h"
#include <algorithm>
using namespace std;
vector<vector<vector<vector<bool>>>> Data::patternassign_from_patternroute(vector<vector<vector<vector<vector<bool>>>>> patternroute)
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
						for(i = 0 ; i < n_jobs ; i++)
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

vector<int> Data::daily_wperc_order(vector<vector<double>>& workload, int d)
{
	int p, w;
	vector<double> wperc(n_nurses, 0);
	vector<int> wperc_rank(n_days * n_nurses, 0); // Ranking of the workload percentage, 0 for max wperc
	double max_wperc = 0;

	for (w = 0; w < n_nurses; w++)
	{
		wperc[w] = workload[d][w] / nurse[w].D;
	}

	for (int wi = 0; wi < n_nurses; wi++)
	{
		for (int wj = wi + 1; wj < n_nurses; wj++) 
		{
			if (wperc[wj] > wperc[wi]) {
				wperc_rank[wi]++;
			}
			else {
				wperc_rank[wj]++;
			}
		}
	}
	cout << "NURSE RANKS: ";
	for (w = 0; w < n_nurses; w++)
	{
		cout << wperc_rank[w] << ", ";
	}
	cout << endl;
	return wperc_rank;
}

vector<int> Data::patient_order(vector<vector<vector<bool>>> assign, int d, int w)
{
	vector<int> patient_rank(n_jobs, -1);
	vector<int> cluster_rank(n_clusters, -1); // Ranking of the clusters in terms of jobs done by caregiver w on day d, ascending order. In case of tie, the cluster with the most caregivers is ranked first.
	vector<int> cluster_job_number(n_clusters, 0); // Number of jobs done by caregiver w on day d in cluster c.
	vector<int> cluster_nurse_number(n_clusters, 0); // Number of caregivers in cluster c on day d.
	int i, j, ww, c;
	for (i = 1; i < n_jobs; i++)
	{
		if (assign[d][w][i])
		{
			cluster_job_number[job[i].cluster]++;
			patient_rank[i] = 0;
		}
	}
	bool visitflag = true;
	for (c = 0; c < n_clusters; c++)
	{
		for (ww = 0; ww < n_nurses; ww++)
		{
			if (visitflag)
			{
				for (i = 1; i < n_jobs; i++)
				{
					if (assign[d][ww][i])
					{
						cluster_job_number[job[i].cluster]++;
						visitflag = false;
					}
				}
			}
			visitflag = true;
		}
	}
	//Ordering [procedure: if "j wins over i", i++ ; else j++]
	for (i = 1; i < n_clusters; i++)
	{
		for (j = i + 1; j < n_clusters; j++)
		{
			if ( cluster_job_number[i] > cluster_job_number[j]) {
				cluster_rank[i]++;
			}
			else if(cluster_job_number[i] < cluster_job_number[j])
			{
				cluster_rank[j]++;
			}
			else //tiebreaker
			{
				if (cluster_nurse_number[j] > cluster_nurse_number[i]) {
					cluster_rank[i]++;
				}
				else
				{
					cluster_rank[j]++;
				}
			}
		}
	}
	//ordering patients
	for (i = 1; i < n_jobs; i++)
	{
		if (assign[d][w][i])
		{
			for (j = i + 1; j < n_jobs; j++)
			{
				if (assign[d][w][j])
				{
					if (cluster_rank[job[j].cluster] < cluster_rank[job[i].cluster])
					{
						patient_rank[i]++;
					}
					else
					{
						patient_rank[j]++;
					}
				}
			}
		}
	}
	cout << "PATIENT RANKS" << endl;
	for (int i = 0; i < n_jobs; i++)
	{
		cout << patient_rank[i] << ", ";
	}
	cout << endl;
	return patient_rank;
}

// Decision rules
int rule = 0;
// //  0: always accept; 1: accept if new solution dominates old one; 2: accept if wperc of receiving caregiver does not exceed 100%
bool Data::accept_movement(vector<vector<double>> workload, vector<vector<double>> workload_copy)
{
	if (rule == 0)
	{
		// Always accept the movement
		return true;
	}
	else if (rule == 1)
	{
		// Accept the movement if new solution dominates old one
		double workload_diff = 0;
		double min_wperc = 1, min_wperc_copy = 1, max_wperc = 0, max_wperc_copy = 0, wperc, wperc_copy;
		for (int w = 0; w < n_nurses; w++)
		{
			wperc = 0;
			for (int d = 0; d < n_days; d++)
			{
				workload_diff += workload_copy[d][w] - workload[d][w];
				wperc += workload[d][w] / nurse[w].D;
				wperc_copy += workload_copy[d][w] / nurse[w].D;
				//if (workload_copy[d][w] / nurse[w].D > 1)
				//{
				//	cout << "Workload exceeds maximum for nurse " << w << " on day " << d << endl;
				//	return false; // Reject movement if workload exceeds maximum
				//}
			}
			min_wperc = min(min_wperc, wperc);
			min_wperc_copy = min(min_wperc_copy, wperc_copy);
			max_wperc = max(max_wperc, wperc);
			max_wperc_copy = max(max_wperc_copy, wperc_copy);
		}
		double wperc_diff = max_wperc_copy - min_wperc_copy - (max_wperc - min_wperc);
		cout << "Workload difference: " << workload_diff << endl;
		cout << "Workload disparity difference: " << wperc_diff << endl;

		//if ((workload_diff <= 0 && wperc_diff < 0) || (workload_diff < 0 && wperc_diff <= 0))
		//{
		//	cout << "Accepting movement" << endl;
		//	return true;
		//}
		if (wperc_diff < 0)
		{
			cout << "Accepting movement" << endl;
			return true;
		}
		else
		{
			cout << "Rejecting movement: total workload not reduced" << endl;
			return false;
		}
	}
	else if (rule == 2)
	{
		// Accept the movement
		return true;
	}
	{
		return false;
	}
}

bool Data::attempt_movement_switch_caregiver_c(vector<vector<vector<vector<bool>>>>& patternassign, vector<vector<vector<vector<vector<bool>>>>>& patternroute, vector<vector<double>>& workload, int p1, int p2, vector<int> vec_d1, vector<int> vec_d2, int w1, int w2, int i, double& total_time)
{
	cout << "====================================================================================" << endl;
	cout << "[time = " << total_time << " s]" << endl;
	cout << "Attempting to switch job " << i << " day " << vec_d1[0] << " pattern " << p1 << " from nurse " << w1 << " to day " << vec_d2[0] << " pattern " << p2 <<  " nurse " << w2 << endl;
	// Attempt to switch caregivers for a job on a specific day, to other caregiver that cares for the patient
	vector<vector<vector<vector<bool>>>> patternassign_copy = patternassign;
	vector<vector<vector<vector<vector<bool>>>>> patternroute_copy = patternroute;
	vector<int> vec_d1_d2, vec_w1_w2;
	vec_d1_d2.reserve(vec_d1.size() + vec_d2.size());
	vec_w1_w2.reserve(vec_d1.size() + vec_d2.size());
	vec_d1_d2.insert(vec_d1_d2.end(), vec_d1.begin(), vec_d1.end());
	vec_w1_w2.insert(vec_w1_w2.end(), vec_d1.size(), w1);
	vec_d1_d2.insert(vec_d1_d2.end(), vec_d2.begin(), vec_d2.end());
	vec_w1_w2.insert(vec_w1_w2.end(), vec_d2.size(), w2);

	for (int dd = 0; dd < n_days; dd++)
	{
		if (pattern[p1].day_p[dd])
		{
			//Comprobar si dd está en vec_d1 o vec_d2
			bool dd_in_vec_d1 = false;
			bool dd_in_vec_d2 = false;
			for (int d1 : vec_d1)
			{
				if (dd == d1)
				{
					dd_in_vec_d1 = true;
					break;
				}
			}
			for (int d2 : vec_d2)
			{
				if (dd == d2)
				{
					dd_in_vec_d2 = true;
					break;
				}
			}
			if (dd_in_vec_d1 == false && dd_in_vec_d2 == false) //Se cambia de patrón manteniendo todo lo demás igual. Del resto de dias se encarga routesolve_nurseday
			{
				for (int w = 0; w < n_nurses; w++)
				{
					if (patternassign[p1][dd][w][i])
					{
						patternassign_copy[p1][dd][w][i] = false;
						patternassign_copy[p2][dd][w][i] = true;
					}
					for (int j = 0; j < n_jobs; j++)
					{
						if (patternroute[p1][dd][w][j][i])
						{
							patternroute_copy[p1][dd][w][j][i] = false;
							patternroute_copy[p2][dd][w][j][i] = true;
						}
					}
				}
			}
		}
	}
	for(int d1:vec_d1)
		patternassign_copy[p1][d1][w1][i] = false;
	for (int d2 : vec_d2)
		patternassign_copy[p2][d2][w2][i] = true;
	

	// Update the patternroute with the new assignment
	patternroute_copy = routesolve_nurseday(patternassign_copy, patternroute_copy, vec_d1_d2, vec_w1_w2, total_time);
	/*
	n_visualize_clusters2(assignsimplify(patternassign), routesimplify(patternroute));
	n_visualize_clusters2(assignsimplify(patternassign_copy), routesimplify(patternroute_copy));
	*/
	vector<vector<double>> workload_copy(n_days, vector<double>(n_nurses, 0));
	workload_calc_fix(assignsimplify(patternassign_copy), routesimplify(patternroute_copy), workload_copy, false);

	// Accept/reject movement

	bool accept = true;

	for (int d2 : vec_d2)
	{
		if (workload_copy[d2][w2] > nurse[w2].D)
		{
			accept = false;
			break;
		}
	}
	if (accept)
	{
		// Update the original patternassign and patternroute with the new assignment
		patternassign = patternassign_copy;
		patternroute = patternroute_copy;
		workload = workload_copy;
		workload_calc_fix(assignsimplify(patternassign_copy), routesimplify(patternroute_copy), workload_copy, true);
		return true;

	}
	return false;
}

bool Data::attempt_movement_switch_caregiver_all_cluster_improve(vector<vector<vector<vector<bool>>>>& patternassign, vector<vector<vector<vector<vector<bool>>>>>& patternroute, vector<vector<double>>& workload, vector<int> p1, vector<int> p2, vector<int> vec_d1, vector<int> vec_d2, int w1, int w2, vector<int> vec_i, double& total_time, int& ITER)
{
	// Attempt to switch caregivers for a job on a specific day, to other caregiver that cares for the patient
	cout << "====================================================================================" << endl;
	cout << "[time = " << total_time << " s]" << endl;
	cout << "Attempting to switch all jobs on cluster " << job[vec_i[0]].cluster << " day " << vec_d1[0] << " from nurse " << w1 << " to nurse " << w2 << endl;

	vector<vector<vector<vector<bool>>>> patternassign_copy = patternassign;
	vector<vector<vector<vector<vector<bool>>>>> patternroute_copy = patternroute;
	vector<int> vec_d1_d2, vec_w1_w2;
	vec_d1_d2.reserve(vec_d1.size() + vec_d2.size());
	vec_w1_w2.reserve(vec_d1.size() + vec_d2.size());
	vec_d1_d2.insert(vec_d1_d2.end(), vec_d1.begin(), vec_d1.end());
	vec_w1_w2.insert(vec_w1_w2.end(), vec_d1.size(), w1);
	vec_d1_d2.insert(vec_d1_d2.end(), vec_d2.begin(), vec_d2.end());
	vec_w1_w2.insert(vec_w1_w2.end(), vec_d2.size(), w2);
	/*
	cout << "vec_d1_d2: [";
	for (int d : vec_d1_d2)
		cout << d << " ";
	cout << "]" << endl;
	cout << "vec_w1_w2: [";
	for (int w : vec_w1_w2)
		cout << w << " ";
	cout << "]" << endl;
	*/

	//ESTO SOLO FUNCIONA porque p1[i] = p2[i] para todos los i. En este paso nunca cambiamos de patrón, así que funciona bien
	for (int i = 0; i < vec_i.size(); i++)
	{
		for (int d1 : vec_d1)
			patternassign_copy[p1[i]][d1][w1][vec_i[i]] = false;
		for (int d2 : vec_d2)
			patternassign_copy[p2[i]][d2][w2][vec_i[i]] = true;
	}
	// Update the patternroute with the new assignment
	patternroute_copy = routesolve_nurseday(patternassign_copy, patternroute_copy, vec_d1_d2, vec_w1_w2, total_time);
	/*
	n_visualize_clusters2(assignsimplify(patternassign), routesimplify(patternroute));
	n_visualize_clusters2(assignsimplify(patternassign_copy), routesimplify(patternroute_copy));
	*/
	vector<vector<double>> workload_copy(n_days, vector<double>(n_nurses, 0));
	workload_calc_fix(assignsimplify(patternassign_copy), routesimplify(patternroute_copy), workload_copy, false);

	// Accept/reject movement
	bool accept = true;

	for (int d2 : vec_d2)
	{
		if (workload_copy[d2][w2] > nurse[w2].D)
		{
			accept = false;
			break;
		}
	}
	if (accept)
	{
		// Update the original patternassign and patternroute with the new assignment
		patternassign = patternassign_copy;
		patternroute = patternroute_copy;
		workload = workload_copy;
		workload_calc_fix(assignsimplify(patternassign_copy), routesimplify(patternroute_copy), workload_copy, true);
		cout << "ACCEPTED: workload[" << vec_d1[0] <<"]["<<w1<<"] = " << workload[vec_d1[0]][w1] << ", nurse[" << w1 << "].D = " << nurse[w1].D << endl;
		cout << "          workload[" << vec_d1[0] << "][" << w2 << "] = " << workload[vec_d1[0]][w2] << ", nurse[" << w2 << "].D = " << nurse[w2].D << endl;
		return true;

	}
	cout << "REJECTED: Movement overloaded caregiver " << w2 << " on day " << vec_d2[0] << ": workload[d2][w2] = " << workload[vec_d2[0]][w2] << endl;
	return false;
}

vector<int> Data::patient_order_c(vector<vector<vector<bool>>> assign, int d, int w)
{

	int cluster_j;
	vector<int> patients_ordered; // Patients ordered by non-increasing number of caregivers working in the same cluster
	vector<int> number_caregivers(n_jobs, 0); // Number caregivers working in the same cluster that the corresponding patient


	for (int j = 1; j < n_jobs; j++)
	{
		if (assign[d][w][j])
		{
			cluster_j = job[j].cluster;

			for (int ww = 0; ww < n_nurses; ww++) 
			{
				for (int i = 1; i < n_jobs; i++)
				{
					if (i != j && job[i].cluster == cluster_j && assign[d][ww][i]) //Caregivers visiting cluster_j
					{
						number_caregivers[j]++;
						break;
					}
				}
			}

			//if number_caregivers[j]==0, caregiver w is the only one that enters cluster j and only performs job j. It must go first in the list
			if (number_caregivers[j] == 0)
			{
				int hola;
				hola = 0;
			}
			auto it = patients_ordered.begin();
			while (it != patients_ordered.end() &&
				!(number_caregivers[j] == 0 && number_caregivers[*it] > 0) &&
				(number_caregivers[*it] >= number_caregivers[j])) {
				++it;
			}
			patients_ordered.insert(it, j);

		}
	}

	return patients_ordered;
}

void Data::routefix_c(vector<vector<vector<vector<vector<bool>>>>>& patternroute, vector<vector<double>>& workload, struct timeb t_ini)
{

	struct timeb t_after;
	double total_time;
	//workload[d][w] gives the total worklad of caregiver w day d
	double min_intracluster_dist = 1E+30, min_intercluster_dist = 1E+30, min_job_time = 1E+30;
	for (int c1 = 0; c1 < n_clusters; c1++)
	{
		min_intracluster_dist = min(min_intracluster_dist, distance[c1][c1]);
		for (int c2 = c1 + 1; c2 < n_clusters; c2++)
		{
			min_intercluster_dist = min(min_intercluster_dist, distance[c1][c2]);
		}
	}
	for (int j = 1; j < n_jobs; j++)
	{
		min_job_time = min(min_job_time, job[j].time);
	}

	int cluster_j;
	int n_T;
	int skill_j;
	int pattern_o;
	int day_c;
	vector<vector<vector<vector<bool>>>> patternassign, patternassign_copy;
	vector<vector<vector<bool>>> assign, assign_copy;
	vector<vector<vector<vector<vector<bool>>>>> patternroute_copy;
	vector<int> pattern_compatible;
	vector<int> day_compatible;
	std::vector<std::vector<std::pair<int, bool>>> nurses_ordered_c;
	try
	{
		patternassign = patternassign_from_patternroute(patternroute);
		assign = assignsimplify(patternassign);

		for (int d = 0; d < n_days; d++)
		{
			for (int w = 0; w < n_nurses; w++)
			{
				ftime(&t_after);
				total_time = ((double)((t_after.time - t_ini.time) * 1000 + t_after.millitm - t_ini.millitm)) / 1000;
				if(total_time < Totalmax)
				{

					if (workload[d][w] > nurse[w].D) //Check if there is a caregiver overworking
					{
						cout << endl;
						cout << "Daily workload exceeded: day " << d << " nurse " << w << " total " << workload[d][w] << " max " << nurse[w].D << endl;

						//skill que hace esa enfermera ese día. Voy a organizarlos desde los pacientes que están en clusters "concurridos" (será más fácil cambiarlos) a los que están en clusters "menos concurridos"

						vector<int> patients_ordered; // Patients ordered by non-increasing number of caregivers working in the same cluster
						patients_ordered = patient_order_c(assign, d, w);

						//Start removing those visits that could be done by other caregiver.
						bool repair = false;
						for (int j : patients_ordered)
						{

							nurses_ordered_c.clear();
							pattern_compatible.clear();
							day_compatible.clear();

							cluster_j = job[j].cluster;
							//Si estoy aquí intento quitar el trabajo j del caregiver w el día d
							for (int p = 0; p < n_patterns; p++)
							{
								if (patternassign[p][d][w][j] == true)
								{
									pattern_o = p;
									break;
								}
							}

							vector <int> nurses_dif(n_nurses, 0);
							for (int w2 = 0; w2 < n_nurses; w2++)
							{
								for (int dd = 0; dd < n_days; dd++)
								{

									if (dd != d && pattern[pattern_o].day_p[dd] > 0 && patternassign[pattern_o][dd][w2][j] == true)
									{
										nurses_dif[w2] = 1;
									}
								}
							}
							int sum_nurses = 0;
							for (int w2 = 0; w2 < n_nurses; w2++)
							{
								sum_nurses = sum_nurses + nurses_dif[w2];
							}

							skill_j = pattern[pattern_o].day_p[d];


							bool compatible, first;

							//lista de patrones compatibles. Cambia solamente el día que quiero descargar por otro día.

							for (int p = 0; p < n_patterns; p++)
							{
								compatible = true;
								first = true;
								if (p != pattern_o && pattern[p].patient_p[j] == true)
								{
									if (pattern[p].day_p[d] == 0)
									{
										for (int dd = 0; dd < n_days; dd++)
										{
											if (dd != d && pattern[p].day_p[dd] != pattern[pattern_o].day_p[dd])
											{
												if (first == true && pattern[p].day_p[dd] == pattern[pattern_o].day_p[d])
												{
													first = false;
													day_c = dd;
												}
												else
												{
													compatible = false;
													break;
												}
											}
										}
										if (compatible)
										{


											std::vector<std::pair<int, bool>> enter_cluster_list;
											std::vector<std::pair<int, bool>> no_enter_cluster_list;

											for (int w2 = 0; w2 < n_nurses; w2++)
											{
												if (sum_nurses - nurses_dif[w2] + 1 <= job[j].max_nurses)
												{
													if (nurse[w2].skill >= skill_j && nurse[w2].patient_s[j] && workload[day_c][w2] < nurse[w2].D)
													{
														bool enter_cluster = false;
														bool work = false;

														for (int i = 1; i < n_jobs; i++)
														{
															if (i != j && assign[day_c][w2][i])
															{
																work = true;
																if (job[i].cluster == cluster_j)
																{
																	enter_cluster = true;
																	break;
																}
															}
														}

														if (work == false)
														{
															enter_cluster = true;
														}

														if ((enter_cluster == true && (workload[day_c][w2] + job[j].time + 2 * min_intracluster_dist) < nurse[w2].D) || (enter_cluster == false && (workload[day_c][w2] + job[j].time + 2 * min_intercluster_dist) < nurse[w2].D))
														{
															auto& target_list = enter_cluster ? enter_cluster_list : no_enter_cluster_list;

															auto it = target_list.begin();
															int w2_sem = 0;
															int w_sem = 0;

															if (it != target_list.end())
															{
																for (int ddd = 0; ddd < n_days; ddd++)
																{
																	w_sem = w_sem + workload[ddd][it->first];
																	w2_sem = w2_sem + workload[ddd][w2];

																}
																w_sem = w_sem / (n_days * nurse[it->first].D);
																w2_sem = w2_sem / (n_days * nurse[w2].D);


															}

															while (it != target_list.end() && w_sem < w2_sem)
															{
																++it;
																w_sem = 0;
																for (int ddd = 0; ddd < n_days; ddd++)
																{
																	w_sem = w_sem + workload[ddd][it->first];

																}
																w_sem = w_sem / (n_days * nurse[it->first].D);
															}
															target_list.insert(it, { w2, enter_cluster });
														}
													}
												}
											}

											if (enter_cluster_list.size() > 0 || no_enter_cluster_list.size() > 0)
											{

												pattern_compatible.push_back(p);
												day_compatible.push_back(day_c);
												// Concatenar ambas listas y añadir a nurses_ordered


												// Unimos ambas listas manteniendo la información de entrada al clúster
												std::vector<std::pair<int, bool>> concatenated_list = enter_cluster_list;
												concatenated_list.insert(concatenated_list.end(), no_enter_cluster_list.begin(), no_enter_cluster_list.end());

												// Guardamos el resultado de esta operación
												nurses_ordered_c.push_back(std::move(concatenated_list));

											}
										}
									}
								}
								else if (p == pattern_o)
								{
									std::vector<std::pair<int, bool>> enter_cluster_list;
									std::vector<std::pair<int, bool>> no_enter_cluster_list;
									day_c = d;

									for (int w2 = 0; w2 < n_nurses; w2++)
									{
										if (sum_nurses - nurses_dif[w2] + 1 <= job[j].max_nurses)
										{
											if (w2 != w && nurse[w2].skill >= skill_j && nurse[w2].patient_s[j] && workload[day_c][w2] < nurse[w2].D)
											{
												bool enter_cluster = false;
												bool work = false;

												for (int i = 1; i < n_jobs; i++)
												{
													if (i != j && assign[day_c][w2][i])
													{
														work = true;
														if (job[i].cluster == cluster_j)
														{
															enter_cluster = true;
															break;
														}
													}
												}

												if (work == false)
												{
													enter_cluster = true;
												}
												if ((enter_cluster == true && (workload[day_c][w2] + job[j].time + 2 * min_intracluster_dist) < nurse[w2].D) || (enter_cluster == false && (workload[day_c][w2] + job[j].time + 2 * min_intercluster_dist) < nurse[w2].D))
												{
													auto& target_list = enter_cluster ? enter_cluster_list : no_enter_cluster_list;

													auto it = target_list.begin();


													int w2_sem = 0;
													int w_sem = 0;

													if (it != target_list.end())
													{
														for (int ddd = 0; ddd < n_days; ddd++)
														{
															w_sem = w_sem + workload[ddd][it->first];
															w2_sem = w2_sem + workload[ddd][w2];

														}
														w_sem = w_sem / (n_days * nurse[it->first].D);
														w2_sem = w2_sem / (n_days * nurse[w2].D);

													}

													while (it != target_list.end() && w_sem < w2_sem)
													{
														++it;
														w_sem = 0;
														for (int ddd = 0; ddd < n_days; ddd++)
														{
															w_sem = w_sem + workload[ddd][it->first];

														}
														w_sem = w_sem / (n_days * nurse[it->first].D);
													}
													target_list.insert(it, { w2, enter_cluster });
												}
											}
										}
									}


									if (enter_cluster_list.size() > 0 || no_enter_cluster_list.size() > 0)
									{

										pattern_compatible.push_back(p);
										day_compatible.push_back(day_c);
										// Concatenar ambas listas y añadir a nurses_ordered


										// Unimos ambas listas manteniendo la información de entrada al clúster
										std::vector<std::pair<int, bool>> concatenated_list = enter_cluster_list;
										concatenated_list.insert(concatenated_list.end(), no_enter_cluster_list.begin(), no_enter_cluster_list.end());

										// Guardamos el resultado de esta operación
										nurses_ordered_c.push_back(std::move(concatenated_list));

									}
								}
							}

							std::vector<std::vector<int>> ordered_2;

							for (size_t i = 0; i < nurses_ordered_c.size(); ++i) {
								int pattern = pattern_compatible[i];
								int day = day_compatible[i];

								for (const auto& [nurse_id, enters] : nurses_ordered_c[i]) {
									ordered_2.push_back({ nurse_id, enters ? 1 : 0, pattern, day });
								}
							}

							// Ordenamos: primero los que entran, luego por workload
							std::sort(ordered_2.begin(), ordered_2.end(),
								[&](const std::vector<int>& a, const std::vector<int>& b) {
									if (a[1] != b[1])
										return a[1] > b[1]; // los que entran (1) antes que los que no (0)

									int wa = 0;
									int wb = 0;
									for (int ddd = 0; ddd < n_days; ddd++)
									{
										wa = wa + workload[ddd][a[0]];
										wb = wb + workload[ddd][b[0]];

									}
									wa = wa / (n_days * nurse[a[0]].D);
									wb = wb / (n_days * nurse[b[0]].D);
									return wa < wb;
								});


							for (auto it = ordered_2.begin(); it != ordered_2.end(); ++it)
							{
								int nurse_id = (*it)[0];
								bool enters = (*it)[1];
								int pattern = (*it)[2];
								int day = (*it)[3];

								cout << endl;
								cout << "Caregiver: " << w << " total " << workload[d][w] << " max " << nurse[w].D << endl;
								cout << "Caregiver: " << nurse_id << " total " << workload[day][nurse_id] << " max " << nurse[nurse_id].D << endl;

								ftime(&t_after);
								total_time = ((double)((t_after.time - t_ini.time) * 1000 + t_after.millitm - t_ini.millitm)) / 1000;

								if (attempt_movement_switch_caregiver_c(patternassign, patternroute, workload, pattern_o, pattern, { d }, { day }, w, nurse_id, j, total_time))
								{
									cout << endl;
									cout << "**** Moving visit" << endl;
									cout << "Visit " << j << endl;
									cout << "Moved from:" << endl;
									cout << "Caregiver: " << w << " to " << nurse_id << endl;
									cout << "Pattern: " << pattern_o << " to " << pattern << endl;
									cout << "Day: " << d << " to " << day << endl;
									cout << "Caregiver: " << w << " total " << workload[d][w] << " max " << nurse[w].D << endl;
									cout << "Caregiver: " << nurse_id << " total " << workload[day][nurse_id] << " max " << nurse[nurse_id].D << endl;
									if (workload[d][w] <= nurse[w].D)
									{
										repair = true;
									}
									break;
								}
								else
								{
									cout << "---> No movement" << endl;
									cout << "Visit " << j << endl;
									cout << "Not Moved from:" << endl;
									cout << "Caregiver: " << w << " to " << nurse_id << endl;
									cout << "Pattern: " << pattern_o << " to " << pattern << endl;
									cout << "Day: " << d << " to " << day << endl;
								}
							}
							//1. Intento dárselo a otro caregiver (este y todos los días que diga su patrón).

							vector<vector<int>> vec_days_p(n_nurses);
							vector<int> nurses_ordered; // Caregivers ordered by decreasing number of shared clusters with the patient
							vector<int> number_nurses(n_nurses, 0); // Number caregivers working in the same cluster that the corresponding patient
							bool insert_w2;

							if (repair == false) // Trato de asignar todas las visitas de este caregiver a otro caregiver
							{
								int max_skill = -1;
								for (int dd = 0; dd < n_days; dd++)
								{
									if (pattern[pattern_o].day_p[dd] > 0)
									{
										if (pattern[pattern_o].day_p[dd] >= max_skill)
										{
											max_skill = pattern[pattern_o].day_p[dd];
										}
									}
								}
								for (int w2 = 0; w2 < n_nurses; w2++)
								{
									insert_w2 = false;
									if (w2 != w && nurse[w2].skill >= max_skill && nurse[w2].patient_s[j] == true)
									{
										for (int dd = 0; dd < n_days; dd++)
										{
											if (pattern[pattern_o].day_p[dd] > 0)
											{
												vec_days_p[w2].push_back(dd);
												insert_w2 = true;
												for (int i = 1; i < n_jobs; i++)
												{
													if (job[i].cluster == cluster_j && assign[dd][w2][i])
													{
														number_nurses[w2]++;
														break;
													}
												}
											}
										}
									}

									if (insert_w2)
									{
										int workload_w2 = 0;
										for (int dd = 0; dd < n_days; dd++) {
											workload_w2 += workload[dd][w2];
										}

										auto it = nurses_ordered.begin();
										while (it != nurses_ordered.end()) {
											int w_it = *it;

											// Criterio 1: number_nurses (mayor a menor)
											if (number_nurses[w2] > number_nurses[w_it]) {
												break; // w2 tiene más tareas, va antes
											}
											if (number_nurses[w2] == number_nurses[w_it]) {
												// Criterio 2: workload total (menor a mayor)
												int workload_it = 0;
												for (int dd = 0; dd < n_days; dd++) {
													workload_it += workload[dd][w_it];
												}
												if (workload_w2 < workload_it) {
													break; // w2 tiene menos carga total, va antes
												}
											}

											++it;
										}

										nurses_ordered.insert(it, w2);
									}
								}

								for (int w2 : nurses_ordered)
								{
									ftime(&t_after);
									total_time = ((double)((t_after.time - t_ini.time) * 1000 + t_after.millitm - t_ini.millitm)) / 1000;

									if (attempt_movement_switch_caregiver_c(patternassign, patternroute, workload, pattern_o, pattern_o, vec_days_p[w2], vec_days_p[w2], w, w2, j, total_time))
									{
										cout << endl;
										cout << "Swapping all assigned visits of the patient between caregivers" << endl;
										cout << endl;
										cout << "Patient " << j << " moved from nurse " << w << " to nurse " << w2 << endl;
										cout << "day " << d << " nurse " << w << " total " << workload[d][w] << " max " << nurse[w].D << endl;
										cout << "day " << d << " nurse " << w2 << " total " << workload[d][w2] << " max " << nurse[w2].D << endl;

										if (workload[d][w] <= nurse[w].D)
										{
											repair = true;
										}
										break;
									}
								}
							}

							if (repair == true)
							{
								break;
							}
						}
					}
				}
				else
				{
					cout << "Maximum time limit reached during repair phase" << endl;
					return;
				}

			}
		}
	}
	catch (const std::exception& e)
	{
		std::cerr << "Error: " << e.what() << std::endl;
	}
}

void Data::routefix_cl(vector<vector<vector<vector<vector<bool>>>>>& patternroute, vector<vector<double>>& workload, struct timeb t_ini)
{

	struct timeb t_after;
	double total_time;
	//workload[d][w] gives the total worklad of caregiver w day d
	double min_intracluster_dist = 1E+30, min_intercluster_dist = 1E+30, min_job_time = 1E+30;
	for (int c1 = 0; c1 < n_clusters; c1++)
	{
		min_intracluster_dist = min(min_intracluster_dist, distance[c1][c1]);
		for (int c2 = c1 + 1; c2 < n_clusters; c2++)
		{
			min_intercluster_dist = min(min_intercluster_dist, distance[c1][c2]);
		}
	}
	for (int j = 1; j < n_jobs; j++)
	{
		min_job_time = min(min_job_time, job[j].time);
	}

	int cluster_j;
	int n_T;
	int skill_j;
	int pattern_o;
	int day_c;
	vector<vector<vector<vector<bool>>>> patternassign, patternassign_copy;
	vector<vector<vector<bool>>> assign, assign_copy;
	vector<vector<vector<vector<vector<bool>>>>> patternroute_copy;
	vector<int> pattern_compatible;
	vector<int> day_compatible;
	std::vector<std::vector<std::pair<int, bool>>> nurses_ordered_c;
	vector<int> considered_cluster_visits;
	int current_visit;
	int pattern_i;
	int ITER = 0;
	try
	{
		patternassign = patternassign_from_patternroute(patternroute);
		assign = assignsimplify(patternassign);

		for (int d = 0; d < n_days; d++)
		{
			for (int w = 0; w < n_nurses; w++)
			{
				bool repair_text = true;
				considered_cluster_visits.clear();
				int visit_index = 0;
			restart_search1b:
				ftime(&t_after);
				total_time = ((double)((t_after.time - t_ini.time) * 1000 + t_after.millitm - t_ini.millitm)) / 1000;
				if (total_time < Totalmax)
				{
					if (workload[d][w] > nurse[w].D) //Check if there is a caregiver overworking
					{
						if (repair_text)
						{
							cout << "REPAIR: Attempting to fix caregiver " << w << " day " << d << "(workload[d][w] = " << workload[d][w] << " > nurse[w].D = " << nurse[w].D << endl;
							repair_text = false;
						}
						assign = assignsimplify(patternassign);
						vector<int> patients_ordered; // Patients ordered by non-increasing number of caregivers working in the same cluster
						patients_ordered = patient_order_c(assign, d, w);
						try
						{
							if (visit_index < patients_ordered.size())
							{
								current_visit = patients_ordered[visit_index]; // Visit to consider
								for (int considered_cluster_visit : considered_cluster_visits)
								{
									if (job[current_visit].cluster == considered_cluster_visit)
									{
										visit_index++;
										goto restart_search1b; // Si ya hemos intentado mover ese cluster en este día, lo saltamos
									}
								}
								int c = job[current_visit].cluster; // Paciente a visitar
								considered_cluster_visits.push_back(c);

								//Trato de quitarle todos los trabajos del día d, cluster c
								cout << endl;
								cout << "************************************************************************************" << endl;
								cout << "R1.Trying to free: " << endl;
								cout << "Caregiver " << w << endl;
								cout << "Day " << d << endl;
								cout << "Cluster " << c << endl;

								vector<int> jobs_to_change;
								vector<int> pattern_jobs_to_change;

								for (int i = 1; i < n_jobs; i++)
								{
									if (assign[d][w][i] && job[i].cluster == c)
									{
										jobs_to_change.push_back(i);
										for (int p = 0; p < n_patterns; p++)
										{
											if (patternassign[p][d][w][i] == true)
											{
												pattern_jobs_to_change.push_back(p);
												break;
											}
										}

									}
								}

								if (jobs_to_change.size() > 0)
								{
									// Nos preguntamos si el cambio ha de ser necesariamente a un caregiver concreto por care continuity
									vector<int> w_other; //Si a ese paciente lo sirve otro día w y otro día distinto w_other;
									bool free_w2 = true;

									for (int i_id = 0; i_id < jobs_to_change.size(); i_id++)
									{
										int i = jobs_to_change[i_id];
										pattern_i = pattern_jobs_to_change[i_id];
										free_w2 = true;

										for (int dd = 0; dd < n_days; dd++)
										{
											if (assign[dd][w][i] && dd != d) //w lo sirve otro día?
											{
												free_w2 = false; //w lo sirve otro día
												break;
											}
										}
										if (free_w2 == false)
										{
											//Sabemos que w lo sirve otro día. Miramos si hay otro caregiver que también lo sirva.
											for (int w2 = 0; w2 < n_nurses; w2++)
											{
												if (w2 != w)
												{
													for (int dd = 0; dd < n_days; dd++)
													{
														if (assign[dd][w2][i])
														{
															//Caregiver w2 visita el paciente i otro día.
															bool insert = true;
															for (int ww = 0; ww < w_other.size(); ww++)
															{
																if (w2 == w_other[ww])
																{
																	insert = false; //Es igual que otro que había que visitar.
																	break;
																}
															}
															if (insert == true)
															{
																w_other.push_back(w2); // En caso de cambio, solo se puede cambiar los trabajos a w2
															}
														}
													}
												}
											}
										}
									}

									bool change_possible;

									if (w_other.size() == 1)
									{
										if (workload[d][w_other[0]] > nurse[w_other[0]].D)
										{
											visit_index++;
											goto restart_search1b; //No se lo voy a cambiar a un caregiver que esté sobrecargado
										}

										// nos aseguramos de que w_other tenga habilidad suficiente y compatibilidad con todos los pacientes
										change_possible = true;
										for (int i_id = 0; i_id < jobs_to_change.size(); i_id++)
										{
											int i = jobs_to_change[i_id];
											pattern_i = pattern_jobs_to_change[i_id];

											if (nurse[w_other[0]].patient_s[i] == false || nurse[w_other[0]].skill < pattern[pattern_i].day_p[d])
											{
												change_possible = false;
												break;
											}
										}
										if (change_possible)
										{
											//Intentamos el cambio
											ftime(&t_after);
											total_time = ((double)((t_after.time - t_ini.time) * 1000 + t_after.millitm - t_ini.millitm)) / 1000;
											if (attempt_movement_switch_caregiver_all_cluster_improve(patternassign, patternroute, workload, pattern_jobs_to_change, pattern_jobs_to_change, { d }, { d }, w, w_other[0], jobs_to_change, total_time, ITER))
											{
												ftime(&t_after);
												total_time = ((double)((t_after.time - t_ini.time) * 1000 + t_after.millitm - t_ini.millitm)) / 1000;
												if (total_time > Totalmax)
												{
													return;
												}
												cout << "************************************************************************************" << endl;

												assign = assignsimplify(patternassign);

												goto restart_search1b; //Sin hacer visit_index++ ya que esta visita la hemos quitado. Si ya la hemos arreglado, pasará al siguiente

											}
											else
											{

												ftime(&t_after);
												total_time = ((double)((t_after.time - t_ini.time) * 1000 + t_after.millitm - t_ini.millitm)) / 1000;
												if (total_time > Totalmax)
												{
													return;
												}
												visit_index++;
												goto restart_search1b; // No hemos podido mover el trabajo, pasamos a la siguiente visita del caregiver
											}
										}
									}
									else if (w_other.size() == 0)
									{
										vector<int> wperc_rank = weekly_wperc_order(workload);

										for (int w_idx = n_nurses - 1; w_idx >= 0; w_idx--)
										{
											int w_c = wperc_rank[w_idx]; // Empezamos por los caregivers con menor carga de trabajo diaria
											if (workload[d][w_c] > nurse[w_c].D)
											{
												continue;
											}

											change_possible = true;
											// nos aseguramos de que w_other tenga habilidad suficiente y compatibilidad con todos los pacientes


											for (int i_id = 0; i_id < jobs_to_change.size(); i_id++)
											{
												int i = jobs_to_change[i_id];
												pattern_i = pattern_jobs_to_change[i_id];

												if (nurse[w_c].patient_s[i] == false || nurse[w_c].skill < pattern[pattern_i].day_p[d])
												{
													change_possible = false;
													break;
												}
											}
											if (change_possible)
											{
												//Intentamos el cambio
												ftime(&t_after);
												total_time = ((double)((t_after.time - t_ini.time) * 1000 + t_after.millitm - t_ini.millitm)) / 1000;
												if (attempt_movement_switch_caregiver_all_cluster_improve(patternassign, patternroute, workload, pattern_jobs_to_change, pattern_jobs_to_change, { d }, { d }, w, w_c, jobs_to_change, total_time, ITER))
												{
													ftime(&t_after);
													total_time = ((double)((t_after.time - t_ini.time) * 1000 + t_after.millitm - t_ini.millitm)) / 1000;
													if (total_time > Totalmax)
													{
														return;
													}

													cout << "************************************************************************************" << endl;

													assign = assignsimplify(patternassign);

													goto restart_search1b; //Sin hacer visit_index++ ya que esta visita la hemos quitado. Si ya la hemos arreglado, pasará al siguiente
												}
												else
												{
													ftime(&t_after);
													total_time = ((double)((t_after.time - t_ini.time) * 1000 + t_after.millitm - t_ini.millitm)) / 1000;
													if (total_time > Totalmax)
													{
														return;
													}
												}
											}
										}
									}
								}
								visit_index++;
								goto restart_search1b; // No hemos podido mover el trabajo, pasamos a la siguiente visita del caregiver
							}
						}
						catch (const std::exception& e)
						{
							std::cerr << "Error: " << e.what() << std::endl;
						}

						// R2: Moving single jobs
						patternassign = patternassign_from_patternroute(patternroute);
						assign = assignsimplify(patternassign);


						try
						{
							ftime(&t_after);
							total_time = ((double)((t_after.time - t_ini.time) * 1000 + t_after.millitm - t_ini.millitm)) / 1000;
							if (total_time < Totalmax)
							{

								if (workload[d][w] > nurse[w].D) //Check if there is a caregiver overworking
								{
									cout << endl;
									cout << "Daily workload exceeded: day " << d << " nurse " << w << " total " << workload[d][w] << " max " << nurse[w].D << endl;

									//skill que hace esa enfermera ese día. Voy a organizarlos desde los pacientes que están en clusters "concurridos" (será más fácil cambiarlos) a los que están en clusters "menos concurridos"

									vector<int> patients_ordered; // Patients ordered by non-increasing number of caregivers working in the same cluster
									patients_ordered = patient_order_c(assign, d, w);

									//Start removing those visits that could be done by other caregiver.
									bool repair = false;
									for (int j : patients_ordered)
									{

										nurses_ordered_c.clear();
										pattern_compatible.clear();
										day_compatible.clear();

										cluster_j = job[j].cluster;
										//Si estoy aquí intento quitar el trabajo j del caregiver w el día d
										for (int p = 0; p < n_patterns; p++)
										{
											if (patternassign[p][d][w][j] == true)
											{
												pattern_o = p;
												break;
											}
										}

										vector <int> nurses_dif(n_nurses, 0);
										for (int w2 = 0; w2 < n_nurses; w2++)
										{
											for (int dd = 0; dd < n_days; dd++)
											{

												if (dd != d && pattern[pattern_o].day_p[dd] > 0 && patternassign[pattern_o][dd][w2][j] == true)
												{
													nurses_dif[w2] = 1;
												}
											}
										}
										int sum_nurses = 0;
										for (int w2 = 0; w2 < n_nurses; w2++)
										{
											sum_nurses = sum_nurses + nurses_dif[w2];
										}

										skill_j = pattern[pattern_o].day_p[d];


										bool compatible, first;

										//lista de patrones compatibles. Cambia solamente el día que quiero descargar por otro día.

										for (int p = 0; p < n_patterns; p++)
										{
											compatible = true;
											first = true;
											if (p != pattern_o && pattern[p].patient_p[j] == true)
											{
												if (pattern[p].day_p[d] == 0)
												{
													for (int dd = 0; dd < n_days; dd++)
													{
														if (dd != d && pattern[p].day_p[dd] != pattern[pattern_o].day_p[dd])
														{
															if (first == true && pattern[p].day_p[dd] == pattern[pattern_o].day_p[d])
															{
																first = false;
																day_c = dd;
															}
															else
															{
																compatible = false;
																break;
															}
														}
													}
													if (compatible)
													{


														std::vector<std::pair<int, bool>> enter_cluster_list;
														std::vector<std::pair<int, bool>> no_enter_cluster_list;

														for (int w2 = 0; w2 < n_nurses; w2++)
														{
															if (sum_nurses - nurses_dif[w2] + 1 <= job[j].max_nurses)
															{
																if (nurse[w2].skill >= skill_j && nurse[w2].patient_s[j] && workload[day_c][w2] < nurse[w2].D)
																{
																	bool enter_cluster = false;
																	bool work = false;

																	for (int i = 1; i < n_jobs; i++)
																	{
																		if (i != j && assign[day_c][w2][i])
																		{
																			work = true;
																			if (job[i].cluster == cluster_j)
																			{
																				enter_cluster = true;
																				break;
																			}
																		}
																	}

																	if (work == false)
																	{
																		enter_cluster = true;
																	}

																	if ((enter_cluster == true && (workload[day_c][w2] + job[j].time + 2 * min_intracluster_dist) < nurse[w2].D) || (enter_cluster == false && (workload[day_c][w2] + job[j].time + 2 * min_intercluster_dist) < nurse[w2].D))
																	{
																		auto& target_list = enter_cluster ? enter_cluster_list : no_enter_cluster_list;

																		auto it = target_list.begin();
																		int w2_sem = 0;
																		int w_sem = 0;

																		if (it != target_list.end())
																		{
																			for (int ddd = 0; ddd < n_days; ddd++)
																			{
																				w_sem = w_sem + workload[ddd][it->first];
																				w2_sem = w2_sem + workload[ddd][w2];

																			}
																			w_sem = w_sem / (n_days * nurse[it->first].D);
																			w2_sem = w2_sem / (n_days * nurse[w2].D);


																		}

																		while (it != target_list.end() && w_sem < w2_sem)
																		{
																			++it;
																			w_sem = 0;
																			for (int ddd = 0; ddd < n_days; ddd++)
																			{
																				w_sem = w_sem + workload[ddd][it->first];

																			}
																			w_sem = w_sem / (n_days * nurse[it->first].D);
																		}
																		target_list.insert(it, { w2, enter_cluster });
																	}
																}
															}
														}

														if (enter_cluster_list.size() > 0 || no_enter_cluster_list.size() > 0)
														{

															pattern_compatible.push_back(p);
															day_compatible.push_back(day_c);
															// Concatenar ambas listas y añadir a nurses_ordered


															// Unimos ambas listas manteniendo la información de entrada al clúster
															std::vector<std::pair<int, bool>> concatenated_list = enter_cluster_list;
															concatenated_list.insert(concatenated_list.end(), no_enter_cluster_list.begin(), no_enter_cluster_list.end());

															// Guardamos el resultado de esta operación
															nurses_ordered_c.push_back(std::move(concatenated_list));

														}
													}
												}
											}
											else if (p == pattern_o)
											{
												std::vector<std::pair<int, bool>> enter_cluster_list;
												std::vector<std::pair<int, bool>> no_enter_cluster_list;
												day_c = d;

												for (int w2 = 0; w2 < n_nurses; w2++)
												{
													if (sum_nurses - nurses_dif[w2] + 1 <= job[j].max_nurses)
													{
														if (w2 != w && nurse[w2].skill >= skill_j && nurse[w2].patient_s[j] && workload[day_c][w2] < nurse[w2].D)
														{
															bool enter_cluster = false;
															bool work = false;

															for (int i = 1; i < n_jobs; i++)
															{
																if (i != j && assign[day_c][w2][i])
																{
																	work = true;
																	if (job[i].cluster == cluster_j)
																	{
																		enter_cluster = true;
																		break;
																	}
																}
															}

															if (work == false)
															{
																enter_cluster = true;
															}
															if ((enter_cluster == true && (workload[day_c][w2] + job[j].time + 2 * min_intracluster_dist) < nurse[w2].D) || (enter_cluster == false && (workload[day_c][w2] + job[j].time + 2 * min_intercluster_dist) < nurse[w2].D))
															{
																auto& target_list = enter_cluster ? enter_cluster_list : no_enter_cluster_list;

																auto it = target_list.begin();


																int w2_sem = 0;
																int w_sem = 0;

																if (it != target_list.end())
																{
																	for (int ddd = 0; ddd < n_days; ddd++)
																	{
																		w_sem = w_sem + workload[ddd][it->first];
																		w2_sem = w2_sem + workload[ddd][w2];

																	}
																	w_sem = w_sem / (n_days * nurse[it->first].D);
																	w2_sem = w2_sem / (n_days * nurse[w2].D);

																}

																while (it != target_list.end() && w_sem < w2_sem)
																{
																	++it;
																	w_sem = 0;
																	for (int ddd = 0; ddd < n_days; ddd++)
																	{
																		w_sem = w_sem + workload[ddd][it->first];

																	}
																	w_sem = w_sem / (n_days * nurse[it->first].D);
																}
																target_list.insert(it, { w2, enter_cluster });
															}
														}
													}
												}


												if (enter_cluster_list.size() > 0 || no_enter_cluster_list.size() > 0)
												{

													pattern_compatible.push_back(p);
													day_compatible.push_back(day_c);
													// Concatenar ambas listas y añadir a nurses_ordered


													// Unimos ambas listas manteniendo la información de entrada al clúster
													std::vector<std::pair<int, bool>> concatenated_list = enter_cluster_list;
													concatenated_list.insert(concatenated_list.end(), no_enter_cluster_list.begin(), no_enter_cluster_list.end());

													// Guardamos el resultado de esta operación
													nurses_ordered_c.push_back(std::move(concatenated_list));

												}
											}
										}

										std::vector<std::vector<int>> ordered_2;

										for (size_t i = 0; i < nurses_ordered_c.size(); ++i) {
											int pattern = pattern_compatible[i];
											int day = day_compatible[i];

											for (const auto& [nurse_id, enters] : nurses_ordered_c[i]) {
												ordered_2.push_back({ nurse_id, enters ? 1 : 0, pattern, day });
											}
										}

										// Ordenamos: primero los que entran, luego por workload
										std::sort(ordered_2.begin(), ordered_2.end(),
											[&](const std::vector<int>& a, const std::vector<int>& b) {
												if (a[1] != b[1])
													return a[1] > b[1]; // los que entran (1) antes que los que no (0)

												int wa = 0;
												int wb = 0;
												for (int ddd = 0; ddd < n_days; ddd++)
												{
													wa = wa + workload[ddd][a[0]];
													wb = wb + workload[ddd][b[0]];

												}
												wa = wa / (n_days * nurse[a[0]].D);
												wb = wb / (n_days * nurse[b[0]].D);
												return wa < wb;
											});


										for (auto it = ordered_2.begin(); it != ordered_2.end(); ++it)
										{
											int nurse_id = (*it)[0];
											bool enters = (*it)[1];
											int pattern = (*it)[2];
											int day = (*it)[3];

											cout << endl;
											cout << "Caregiver: " << w << " total " << workload[d][w] << " max " << nurse[w].D << endl;
											cout << "Caregiver: " << nurse_id << " total " << workload[day][nurse_id] << " max " << nurse[nurse_id].D << endl;

											ftime(&t_after);
											total_time = ((double)((t_after.time - t_ini.time) * 1000 + t_after.millitm - t_ini.millitm)) / 1000;

											if (attempt_movement_switch_caregiver_c(patternassign, patternroute, workload, pattern_o, pattern, { d }, { day }, w, nurse_id, j, total_time))
											{
												cout << endl;
												cout << "**** Moving visit" << endl;
												cout << "Visit " << j << endl;
												cout << "Moved from:" << endl;
												cout << "Caregiver: " << w << " to " << nurse_id << endl;
												cout << "Pattern: " << pattern_o << " to " << pattern << endl;
												cout << "Day: " << d << " to " << day << endl;
												cout << "Caregiver: " << w << " total " << workload[d][w] << " max " << nurse[w].D << endl;
												cout << "Caregiver: " << nurse_id << " total " << workload[day][nurse_id] << " max " << nurse[nurse_id].D << endl;
												if (workload[d][w] <= nurse[w].D)
												{
													repair = true;
												}
												break;
											}
											else
											{
												cout << "---> No movement" << endl;
												cout << "Visit " << j << endl;
												cout << "Not Moved from:" << endl;
												cout << "Caregiver: " << w << " to " << nurse_id << endl;
												cout << "Pattern: " << pattern_o << " to " << pattern << endl;
												cout << "Day: " << d << " to " << day << endl;
											}
										}
										//1. Intento dárselo a otro caregiver (este y todos los días que diga su patrón).

										vector<vector<int>> vec_days_p(n_nurses);
										vector<int> nurses_ordered; // Caregivers ordered by decreasing number of shared clusters with the patient
										vector<int> number_nurses(n_nurses, 0); // Number caregivers working in the same cluster that the corresponding patient
										bool insert_w2;

										if (repair == false) // Trato de asignar todas las visitas de este caregiver a otro caregiver
										{
											int max_skill = -1;
											for (int dd = 0; dd < n_days; dd++)
											{
												if (pattern[pattern_o].day_p[dd] > 0)
												{
													if (pattern[pattern_o].day_p[dd] >= max_skill)
													{
														max_skill = pattern[pattern_o].day_p[dd];
													}
												}
											}
											for (int w2 = 0; w2 < n_nurses; w2++)
											{
												insert_w2 = false;
												if (w2 != w && nurse[w2].skill >= max_skill && nurse[w2].patient_s[j] == true)
												{
													for (int dd = 0; dd < n_days; dd++)
													{
														if (pattern[pattern_o].day_p[dd] > 0)
														{
															vec_days_p[w2].push_back(dd);
															insert_w2 = true;
															for (int i = 1; i < n_jobs; i++)
															{
																if (job[i].cluster == cluster_j && assign[dd][w2][i])
																{
																	number_nurses[w2]++;
																	break;
																}
															}
														}
													}
												}

												if (insert_w2)
												{
													int workload_w2 = 0;
													for (int dd = 0; dd < n_days; dd++) {
														workload_w2 += workload[dd][w2];
													}

													auto it = nurses_ordered.begin();
													while (it != nurses_ordered.end()) {
														int w_it = *it;

														// Criterio 1: number_nurses (mayor a menor)
														if (number_nurses[w2] > number_nurses[w_it]) {
															break; // w2 tiene más tareas, va antes
														}
														if (number_nurses[w2] == number_nurses[w_it]) {
															// Criterio 2: workload total (menor a mayor)
															int workload_it = 0;
															for (int dd = 0; dd < n_days; dd++) {
																workload_it += workload[dd][w_it];
															}
															if (workload_w2 < workload_it) {
																break; // w2 tiene menos carga total, va antes
															}
														}

														++it;
													}

													nurses_ordered.insert(it, w2);
												}
											}

											for (int w2 : nurses_ordered)
											{
												ftime(&t_after);
												total_time = ((double)((t_after.time - t_ini.time) * 1000 + t_after.millitm - t_ini.millitm)) / 1000;

												if (attempt_movement_switch_caregiver_c(patternassign, patternroute, workload, pattern_o, pattern_o, vec_days_p[w2], vec_days_p[w2], w, w2, j, total_time))
												{
													cout << endl;
													cout << "Swapping all assigned visits of the patient between caregivers" << endl;
													cout << endl;
													cout << "Patient " << j << " moved from nurse " << w << " to nurse " << w2 << endl;
													cout << "day " << d << " nurse " << w << " total " << workload[d][w] << " max " << nurse[w].D << endl;
													cout << "day " << d << " nurse " << w2 << " total " << workload[d][w2] << " max " << nurse[w2].D << endl;

													if (workload[d][w] <= nurse[w].D)
													{
														repair = true;
													}
													break;
												}
											}
										}

										if (repair == true)
										{
											break;
										}
									}
								}
							}
							else
							{
								cout << "Maximum time limit reached during repair phase" << endl;
								return;
							}
						}
						catch (const std::exception& e)
						{
							std::cerr << "Error: " << e.what() << std::endl;
						}
					}
				}
				else
				{
					cout << "Maximum time limit reached during repair phase" << endl;
					return;
				}
				if (workload[d][w] > nurse[w].D)
				{
					cout << "Route for caregiver " << w << " on day " << d << " was not fixed - exiting...";
					exit(1);
				}
			}
		}
	}
	catch (const std::exception& e)
	{
		std::cerr << "Error: " << e.what() << std::endl;
	}
}


