#include "Data.h"
#include <algorithm>
using namespace std;

vector<int> Data::weekly_wperc_order(vector<vector<double>>& workload)
{
	int p, d, w, i, j;
	vector<double> wperc(n_nurses, 0);
	vector<int> weekly_wperc_rank_dummy(n_nurses, 0); // element [w] indicates rank of nurse [w]
	vector<int> weekly_wperc_rank(n_nurses, 0); // element [i] indicates nurse with rank [i]
	for (d = 0; d < n_days; d++)
	{
		for (w = 0; w < n_nurses; w++)
		{
			wperc[w] += workload[d][w] / nurse[w].D;
		}
	}

	for (int i = 0; i < n_nurses; i++) {
		for (int j = i + 1; j < n_nurses; j++) {
			if (wperc[j] > wperc[i]) {
				weekly_wperc_rank_dummy[i]++;
			}
			else {
				weekly_wperc_rank_dummy[j]++;
			}
		}
	}
	for (int i = 0; i < n_nurses; i++) {
		weekly_wperc_rank[weekly_wperc_rank_dummy[i]] = i;
	}

	return weekly_wperc_rank;
}

vector<int> Data::weekly_wperc_order_c(vector<double> wperc)
{
	int p, d, w, i, j;
	vector<int> weekly_wperc_rank_dummy(n_nurses, 0); // element [w] indicates rank of nurse [w]
	vector<int> weekly_wperc_rank(n_nurses, 0); // element [i] indicates nurse with rank [i]

	for (int i = 0; i < n_nurses; i++) {
		for (int j = i + 1; j < n_nurses; j++) {
			if (wperc[j] > wperc[i]) {
				weekly_wperc_rank_dummy[i]++;
			}
			else {
				weekly_wperc_rank_dummy[j]++;
			}
		}
	}
	for (int i = 0; i < n_nurses; i++) {
		weekly_wperc_rank[weekly_wperc_rank_dummy[i]] = i;
	}

	return weekly_wperc_rank;
}
vector<pair<int, int>> Data::weekly_patient_order(vector<vector<vector<bool>>> assign, int w)
{
	vector<vector<int>> patient_rank_dummy(n_jobs,vector<int>(n_days, -1));
	vector<vector<int>> cluster_rank(n_clusters, vector<int>(n_days, -1)); // Ranking of the clusters in terms of jobs done by caregiver w on day d, ascending order. In case of tie, the cluster with the most caregivers is ranked first.
	vector<vector<int>> cluster_job_number(n_clusters, vector<int>(n_days, 0)); // Number of jobs done by caregiver w on day d in cluster c.
	vector<vector<int>> cluster_nurse_number_others(n_clusters, vector<int>(n_days, 0)); // Number of jobs done by caregiver w on day d in cluster c.
	int i, j, ww, c, d;
	for (d = 0; d < n_days; d++)
	{
		for (i = 1; i < n_jobs; i++)
		{
			if (assign[d][w][i])
			{
				cluster_job_number[job[i].cluster][d]++;
				patient_rank_dummy[i][d] = 0;
				//cout << "patient_rank_dummy[" << i << "][" << d << "] = " << patient_rank_dummy[i][d] << endl;
			}
		}
	}
	for (d = 0; d < n_days; d++)
	{
		for (c = 0; c < n_clusters; c++)
		{
			for (ww = 0; ww < n_nurses; ww++)
			{
				if (ww != w)
				{
					for (i = 1; i < n_jobs; i++)
					{
						if (assign[d][ww][i] && job[i].cluster == c) 
						{
							cluster_nurse_number_others[c][d]++;
							break;
						}
					}
				}
			}
		}
	}
	//Ordering [procedure: if "j wins over i", i++ ; else j++]
	for (int di = 0; di < n_days; di++)
	{
		for (int dj = di; dj < n_days; dj++)
		{
			for (i = 1; i < n_clusters; i++)
			{
				for (j = 1; j < n_clusters; j++)
				{
					if (dj > di || j > i)
					{
						if (cluster_job_number[i][di] > -1 && cluster_job_number[j][dj] > -1)
						{
							if (cluster_job_number[i][di] > cluster_job_number[j][dj]) {
								cluster_rank[i][di]++;
							}
							else if (cluster_job_number[i][di] < cluster_job_number[j][dj])
							{
								cluster_rank[j][dj]++;
							}
							else //tiebreaker
							{
								if (cluster_nurse_number_others[j][dj] > cluster_nurse_number_others[i][di]) {
									cluster_rank[i][di]++;
								}
								else
								{
									cluster_rank[j][dj]++;
								}
							}
						}
					}
				}
			}
		}
	}
	//ordering patients
	for (int di = 0; di < n_days; di++)
	{
		for (int dj = di; dj < n_days; dj++)
		{
			for (i = 1; i < n_jobs; i++)
			{
				if (assign[di][w][i])
				{
					for (j = 1; j < n_jobs; j++)
					{
						if (assign[dj][w][j])
						{
							if (dj > di || j > i)
							{
								//cout << "comparing [" << i << "][" << di << "] with [" << j << "][" << dj << "]" << endl;
								if (cluster_rank[job[j].cluster][dj] < cluster_rank[job[i].cluster][di])
								{
									patient_rank_dummy[i][di]++;
									//cout << " [" << i << "][" << di << "]++" << endl;
								}
								else
								{
									patient_rank_dummy[j][dj]++;
									//cout << " [" << j << "][" << dj << "]++" << endl;
								}
							}
						}
					}
				}
			}
		}
	}
	//for (d = 0; d < n_days; d++)
	//{
	//	for (i = 0; i < n_jobs; i++)
	//		cout << patient_rank_dummy[i][d] << " ";
	//	cout << endl;
	//}
	vector<pair<int, int>> patient_rank;
	int patient_counter = 0;

	for (int di = 0; di < n_days; di++) {
		for (int i = 1; i < n_jobs; i++) {
			if (patient_rank_dummy[i][di] == patient_counter) {
				patient_rank.emplace_back(i, di);
				patient_counter++;
				di = -1;  // restart outer loop
				break;
			}
		}
	}
	return patient_rank;
}

// Decision rules
bool Data::accept_movement_improve(vector<vector<double>> workload, vector<vector<double>> workload_copy, vector<int> vec_d1, vector<int> vec_d2, int w1, int w2)
{
	// Accept the movement if new solution dominates old one OR if workloads become more balanced without increasing total time.
	double total_workload_diff = 0, total_workload = 0, total_workload_copy = 0;
	double min_wperc = 1, min_wperc_copy = 1, max_wperc = 0, max_wperc_copy = 0, wperc, wperc_copy;
	for (int w = 0; w < n_nurses; w++)
	{
		wperc = 0;
		wperc_copy = 0;
		for (int d = 0; d < n_days; d++)
		{
			total_workload += workload[d][w];
			total_workload_copy += workload_copy[d][w];
			wperc += workload[d][w] / (n_days*nurse[w].D);
			wperc_copy += workload_copy[d][w] / (n_days*nurse[w].D);
			if (workload_copy[d][w] > nurse[w].D)
			{
				cout << "REJECTED: Workload exceeds maximum for nurse " << w << " on day " << d << endl;
				return false; // Reject movement if workload exceeds maximum
			}
		}
		min_wperc = min(min_wperc, wperc);
		min_wperc_copy = min(min_wperc_copy, wperc_copy);
		max_wperc = max(max_wperc, wperc);
		max_wperc_copy = max(max_wperc_copy, wperc_copy);
	}
	double workload_diff = total_workload_copy - total_workload;
	double wperc_diff = max_wperc_copy - min_wperc_copy - (max_wperc - min_wperc);
	cout << "Total Workload: " << total_workload << " ---> " << total_workload_copy << " (" << workload_diff << ")" << endl;
	cout << "Max. Workload difference: " << max_wperc - min_wperc << " ---> " << max_wperc_copy - min_wperc_copy << " (" << wperc_diff << ")" << endl;

	for (int w : {w1, w2})
	{
		for (int d : vec_d1)
		{
			cout << "Nurse " << w << " day " << d << ":" << workload[d][w] << " ---> " << workload_copy[d][w] << endl;
		}
	}
	if (algorithm == 1)
	{
		if ((workload_diff <= 0 && wperc_diff < 0) || (workload_diff < 0 && wperc_diff <= 0))
		{
			cout << "ACCEPTED: New solution dominates old one" << endl;
			return true;
		}
		else
		{
			cout << "REJECTED: New solution does not dominate old one" << endl;
			return false;
		}
	}
	else if (algorithm == 2)
	{
		double lambda = 0.5; // weight for total workload
		int maxL = 0;
		for (int w = 0; w < n_nurses; w++)
		{
			maxL = max(maxL, nurse[w].D);
		}

		if (lambda*workload_diff + (1-lambda) * maxL*n_days * wperc_diff < 0)
		{
			cout << "ACCEPTED: New solution better wrt weighted sum (lambda = " << lambda << ")"<< endl;
			cout << "Weighted sum: " << lambda *total_workload + (1 - lambda) * maxL*n_days * (max_wperc - min_wperc) << " ---> " << lambda * total_workload_copy + (1 - lambda) * maxL*n_days * (max_wperc_copy - min_wperc_copy) <<" (" << lambda * workload_diff + (1 - lambda) * maxL*n_days * wperc_diff << ")" << endl;
			return true;
		}
		else
		{
			cout << "REJECTED: New solution worse wrt weighted sum (lambda = " << lambda << ")" << endl;
			return false;
		}
	}
}

bool Data::attempt_movement_switch_caregiver(vector<vector<vector<vector<bool>>>>& patternassign, vector<vector<vector<vector<vector<bool>>>>>& patternroute, vector<vector<double>>& workload, int p1, int p2, vector<int> vec_d1, vector<int> vec_d2, int w1, int w2, vector<int> vec_i, double& total_time, int& ITER)
{
	// Attempt to switch caregivers for a job on a specific day, to other caregiver that cares for the patient
	cout << "====================================================================================" << endl;
	if (vec_i.size() == 1)
	{
		cout << "[time = " << total_time << " s]" << endl;
		cout << "Trying to free caregiver " << w1 << endl;
		cout << "Moving patient " << vec_i[0] << endl;
		cout << "Pattern " << p1 << " Caregiver " << w1 << " days ";
		cout << "[";
		for (int d : vec_d1)
			cout << d << " ";
		cout << "]" << endl;
		cout << "Pattern " << p2 << " Caregiver " << w2 << " days ";
		cout << "[";
		for (int d : vec_d2)
			cout << d << " ";
		cout << "]" << endl;
	}
	else
	{
		cout << "Attempting to switch all jobs on cluster " << job[vec_i[0]].cluster << " pattern " << p1 << " day " << vec_d1[0] << " from nurse " << w1 << " to nurse " << w2 << endl;
	}
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

	//change ALL assignments

	for (int i : vec_i)
	{
		for (int dd = 0; dd < n_days; dd++)
		{
			if (pattern[p1].day_p[dd] || pattern[p2].day_p[dd])
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
				if (dd_in_vec_d1 == false && dd_in_vec_d2 == false) //Se cambia de patrón manteniendo todo lo demás igual
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
		for (int d1 : vec_d1)
			patternassign_copy[p1][d1][w1][i] = false;
		for (int d2 : vec_d2)
			patternassign_copy[p2][d2][w2][i] = true;
	}

	/*
	for (int i : vec_i)
	{
		for (int dd = 0; dd < n_days; dd++)
		{
			if (pattern[p1].day_p[dd])
			{
				patternassign_copy[p1][dd][w1][i] = false;

				if (pattern[p2].day_p[dd]) // Día en ambos patrones. Si está en vec_d1 o vec_d2 este paso se sobreescribe. Si no, simplemente se cambia de patrón manteniendo todo lo demás igual
				{
					for (int j = 0; j < n_jobs; j++)
					{
						if (patternroute[p1][dd][w1][j][i])
						{
							patternroute_copy[p1][dd][w1][j][i] = false;
							patternroute_copy[p2][dd][w1][j][i] = true;
						}
					}
				}
			}
			if (pattern[p2].day_p[dd])
			{
				bool w1_keeps_visit = true;
				for (int d2 : vec_d2)
				{
					if (dd == d2)
					{
						patternassign_copy[p2][dd][w2][i] = true;
						bool w1_keeps_visit = false;

					}
				}
				if (w1_keeps_visit)
				{
					patternassign_copy[p2][dd][w1][i] = true;
				}
			}
		}
	}
	*/

	// Update the patternroute with the new assignment (only recalculates for days on which change in assignment influences the route - for the days where only change of pattern, no rerouting needed)
	patternroute_copy = routesolve_nurseday(patternassign_copy, patternroute_copy, vec_d1_d2, vec_w1_w2, total_time);

	//
	vector<vector<double>> workload_copy(n_days, vector<double>(n_nurses, 0));
	workload_calc_improvement(assignsimplify(patternassign_copy), routesimplify(patternroute_copy), workload_copy, false, ITER);

	// Accept/reject movement

	if (accept_movement_improve(workload, workload_copy, vec_d1, vec_d2, w1, w2))
	{

		ITER++;
		workload_calc_improvement(assignsimplify(patternassign_copy), routesimplify(patternroute_copy), workload_copy, true, ITER);
		if (sanitycheck(patternroute_copy))
		{
			fprintf(stdout,"\n###########################\n### Sanity check passed ###\n###########################\n");
		}
		else
		{
			fprintf(stdout, "\n###########################\n### SANITY CHECK FAILED ###\n###########################\n");
			vector<vector<vector<bool>>> assign_copy = assignsimplify(patternassign_copy);
			vector<vector<vector<vector<bool>>>> route_copy = routesimplify(patternroute_copy);
			n_visualize_clusters2(assign_copy, route_copy);
			fprintf(stderr, "Sanity check failed. Exiting...\n");
			exit(1);
		}

		// Update the original patternassign and patternroute with the new assignment
		patternassign = patternassign_copy;
		patternroute = patternroute_copy;
		workload = workload_copy;

		return true;
	}
	return false;
}

bool Data::attempt_movement_switch_caregiver_all_cluster_c(vector<vector<vector<vector<bool>>>>& patternassign, vector<vector<vector<vector<vector<bool>>>>>& patternroute, vector<vector<double>>& workload, vector<int> p1, vector<int> p2, vector<int> vec_d1, vector<int> vec_d2, int w1, int w2, vector<int> vec_i, double& total_time, int& ITER)
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
	for (int i=0; i<vec_i.size(); i++)
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
	workload_calc_improvement(assignsimplify(patternassign_copy), routesimplify(patternroute_copy), workload_copy, false, ITER);

	// Accept/reject movement

	if (accept_movement_improve(workload, workload_copy, vec_d1, vec_d2, w1, w2))
	{
		ITER++;
		workload_calc_improvement(assignsimplify(patternassign_copy), routesimplify(patternroute_copy), workload_copy, true, ITER);
		if (sanitycheck(patternroute_copy))
		{
			fprintf(stdout, "\n###########################\n### Sanity check passed ###\n###########################\n");
		}
		else
		{
			fprintf(stdout, "\n###########################\n### SANITY CHECK FAILED ###\n###########################\n");
			vector<vector<vector<bool>>> assign_copy = assignsimplify(patternassign_copy);
			vector<vector<vector<vector<bool>>>> route_copy = routesimplify(patternroute_copy);
			n_visualize_clusters2(assign_copy, route_copy);
			fprintf(stderr, "Sanity check failed. Exiting...\n");
			exit(1);
		}

		// Update the original patternassign and patternroute with the new assignment
		patternassign = patternassign_copy;
		patternroute = patternroute_copy;
		workload = workload_copy;

		cout << endl;
		cout << "====================================================================================" << endl;
		return true;
	}

	cout << "====================================================================================" << endl;
	return false;
}
//
//void Data::routeimprove(vector<vector<vector<vector<vector<bool>>>>>& patternroute, vector<vector<double>>& workload, double& total_time)
//{
//	double min_intracluster_dist = 1E+30, min_intercluster_dist = 1E+30, min_job_time = 1E+30;
//	for (int c1 = 0; c1 < n_clusters; c1++)
//	{
//		min_intracluster_dist = min(min_intracluster_dist, distance[c1][c1]);
//		for (int c2 = c1 + 1; c2 < n_clusters; c2++)
//		{
//			min_intercluster_dist = min(min_intercluster_dist, distance[c1][c2]);
//		}
//	}
//	for (int j = 1; j < n_jobs; j++)
//	{
//		min_job_time = min(min_job_time, job[j].time);
//	}
//
//	int cluster_j;
//	int n_T;
//	int skill_j;
//	int pattern_o;
//	int day_c;
//	vector<vector<vector<vector<bool>>>> patternassign, patternassign_copy;
//	vector<vector<vector<bool>>> assign, assign_copy;
//	vector<vector<vector<vector<vector<bool>>>>> patternroute_copy;
//	vector<int> pattern_compatible;
//	vector<int> day_compatible;
//	std::vector<std::vector<std::pair<int, bool>>> nurses_ordered_c;
//
//	patternassign = patternassign_from_patternroute(patternroute);
//	int n_attempts = -1;
//	int n_successive_rejections = 0;
//	int n_max_rejections = 20; // Maximum number of successive rejections before moving on to next step
//	int n_attempts_max = 500;// Maximum number of attempts before moving on to next step
//	double t_max = 120; // Maximum time to spend on local search
//restart_search:
//	n_successive_rejections = 0;
//	n_attempts++;
//	vector<int> wperc_rank = weekly_wperc_order(workload);
//	assign = assignsimplify(patternassign);
//	try
//	{
//		for (int method = 0; method < 5; method++)
//			{
//				for (int nurse_rank = 0; nurse_rank < n_nurses; nurse_rank++)
//				{
//					int w = wperc_rank[nurse_rank];
//					vector<pair<int, int>> patient_rank = weekly_patient_order(assignsimplify(patternassign), w);
//					for (int patient_day_index = 0; patient_day_index < patient_rank.size(); patient_day_index++)
//					{
//						int j = patient_rank[patient_day_index].first;
//						cout << "patient " << j << endl;
//						int d = patient_rank[patient_day_index].second;
//						cout << "day " << d << endl;
//						nurses_ordered_c.clear();
//						pattern_compatible.clear();
//						day_compatible.clear();
//
//						cluster_j = job[j].cluster;
//						for (int p = 0; p < n_patterns; p++)
//						{
//							if (patternassign[p][d][w][j] == true)
//							{
//								pattern_o = p;
//								break;
//							}
//						}
////**********************// LS1 //************************************************//
//						//Aquí intento mover todos los trabajos del cluster de i a otro caregiver este día
//
//						if (cluster_j != 0) //Si es el cluster 0 no tiene mucho sentido hacer esto
//						{
//							//vector de trabajos que intentamos cambiar
//							vector<int> jobs_to_change;
//							for (int i = 1; i < n_jobs; i++)
//							{
//								if (assign[d][w][i] && job[i].cluster == cluster_j)
//								{
//									jobs_to_change.push_back(i);
//								}
//							}
//
//							// Nos preguntamos si el cambio ha de ser necesariamente a un caregiver concreto por care continuity
//							int w_other = -1;
//							bool free_w2 = true;
//							for (int i : jobs_to_change)
//							{
//								free_w2 = true;
//								for (int dd = 0; dd < n_days; dd++)
//								{
//									if (assign[dd][w][i] && dd != d) //w lo sirve otro día?
//									{
//										free_w2 = false;
//										break;
//									}
//								}
//								if (free_w2 == false)
//								{
//									//buscamos caregiver w2 que lo cuide otro día
//									for (int w2 = 0; w2 < n_nurses; w2++)
//									{
//										if (w2 != w)
//										{
//											for (int dd = 0; dd < n_days; dd++)
//											{
//												if (assign[dd][w2][i])
//												{
//													if (w_other == -1)
//													{
//														w_other = w2;  //si encuentra una w2, solo se puede cambiar los trabajos a w2
//													}
//													else if (w_other != w2)
//													{
//														goto end_search_forced_w; //si encuentra mas de una w2, no se puede hacer el cambio
//													}
//												}
//											}
//										}
//									}
//								}
//							}
//							// Intento hacer el cambio
//							bool change_possible;
//							if (w_other > -1)
//							{
//								change_possible = true;
//								// nos aseguramos de que w_other tenga habilidad suficiente y compatibilidad con todos los pacientes
//								for (int i : jobs_to_change)
//								{
//									if (nurse[w_other].skill < pattern[pattern_o].day_p[d] || nurse[w_other].patient_s[i] == false)
//									{
//										change_possible = false;
//										break;
//									}
//								}
//								if (change_possible)
//								{
//									//Intentamos el cambio
//									if (attempt_movement_switch_caregiver(patternassign, patternroute, workload, pattern_o, pattern_o, { d }, { d }, w, w_other, jobs_to_change, total_time))
//									{
//										cout << "************************************************************************************" << endl;
//
//										goto restart_search;
//									}
//									else
//									{
//										n_attempts++;
//										if (n_attempts >= n_attempts_max)
//										{
//											cout << "No more attempts available" << endl;
//											return;
//										}
//									}
//								}
//							}
//							else
//							{
//								for (int w_idx = 0; w_idx < n_nurses; w_idx++)
//								{
//									w_other = wperc_rank[n_nurses - w_idx - 1]; // Empezamos por los caregivers con menor carga de trabajo semanal
//									change_possible = true;
//									// nos aseguramos de que w_other tenga habilidad suficiente y compatibilidad con todos los pacientes
//									for (int i : jobs_to_change)
//									{
//										if (nurse[w_other].skill < pattern[pattern_o].day_p[d] || nurse[w_other].patient_s[i] == false)
//										{
//											change_possible = false;
//											break;
//										}
//									}
//									if (change_possible)
//									{
//										//Intentamos el cambio
//										if (attempt_movement_switch_caregiver(patternassign, patternroute, workload, pattern_o, pattern_o, { d }, { d }, w, w_other, jobs_to_change, total_time))
//										{
//											cout << "*************************************************************************************************************************************************************************" << endl;
//
//											goto restart_search;
//										}
//										else
//										{
//											n_attempts++;
//											if (n_attempts >= n_attempts_max_1)
//											{
//												cout << "No more attempts available" << endl;
//												return;
//											}
//										}
//									}
//								}
//							}
//						}
//					end_search_forced_w:
////**********************// LS2 //************************************************//
//						//Si estoy aquí intento quitar el trabajo j del caregiver w el día d
//
//						vector <int> nurses_dif(n_nurses, 0);
//						for (int w2 = 0; w2 < n_nurses; w2++)
//						{
//							for (int dd = 0; dd < n_days; dd++)
//							{
//
//								if (dd != d && pattern[pattern_o].day_p[dd] > 0 && patternassign[pattern_o][dd][w2][j] == true)
//								{
//									nurses_dif[w2] = 1; //nurses_dif[w2] = 1 si w2 cuida a j algun dia distinto de d
//								}
//							}
//						}
//						int sum_nurses = 0;
//						for (int w2 = 0; w2 < n_nurses; w2++)
//						{
//							sum_nurses = sum_nurses + nurses_dif[w2];
//						}
//
//						skill_j = pattern[pattern_o].day_p[d];
//
//
//						//lista de patrones compatibles. Cambia solamente el día que quiero descargar por otro día.
//
//						bool compatible, first;
//
//						for (int p = 0; p < n_patterns; p++)
//						{
//							compatible = true;
//							first = true;
//							if (p != pattern_o && pattern[p].patient_p[j] == true)
//							{
//								if (pattern[p].day_p[d] == 0)
//								{
//									for (int dd = 0; dd < n_days; dd++)
//									{
//										if (dd != d && pattern[p].day_p[dd] != pattern[pattern_o].day_p[dd])
//										{
//											if (first == true && pattern[p].day_p[dd] == pattern[pattern_o].day_p[d])
//											{
//												first = false;
//												day_c = dd;
//											}
//											else
//											{
//												compatible = false;
//												break;
//											}
//										}
//									}
//									if (compatible)
//									{
//										// Buscamos enfermeras que puedan entrar en el clúster
//										std::vector<std::pair<int, bool>> enter_cluster_list;
//										std::vector<std::pair<int, bool>> no_enter_cluster_list;
//
//										for (int w2 = 0; w2 < n_nurses; w2++)
//										{
//											if (sum_nurses - nurses_dif[w2] + 1 <= job[j].max_nurses) //Adding this nurse won't exceed the maximum number of nurses for this job
//											{
//												if (nurse[w2].skill >= skill_j && nurse[w2].patient_s[j] && workload[day_c][w2] < nurse[w2].D)
//												{
//													bool enter_cluster = false;
//													bool work = false;
//
//													for (int i = 1; i < n_jobs; i++)
//													{
//														if (i != j && assign[day_c][w2][i])
//														{
//															work = true;
//															if (job[i].cluster == cluster_j)
//															{
//																enter_cluster = true;
//																break;
//															}
//														}
//													}
//
//													if (work == false)
//													{
//														enter_cluster = true;
//													}
//
//													if ((enter_cluster == true && (workload[day_c][w2] + job[j].time + 2 * min_intracluster_dist) < nurse[w2].D) || (enter_cluster == false && (workload[day_c][w2] + job[j].time + 2 * min_intercluster_dist) < nurse[w2].D)) //??? 1
//													{
//														auto& target_list = enter_cluster ? enter_cluster_list : no_enter_cluster_list; // lists sorted by asscending normalized workload.
//
//														auto it = target_list.begin();
//														int w2_sem = 0;
//														int w_sem = 0;
//
//														if (it != target_list.end())
//														{
//															for (int ddd = 0; ddd < n_days; ddd++)
//															{
//																w_sem = w_sem + workload[ddd][it->first];
//																w2_sem = w2_sem + workload[ddd][w2];
//
//															}
//															w_sem = w_sem / (n_days * nurse[it->first].D);
//															w2_sem = w2_sem / (n_days * nurse[w2].D);
//
//
//														}
//
//														while (it != target_list.end() && w_sem < w2_sem)
//														{
//															++it;
//															w_sem = 0;
//															for (int ddd = 0; ddd < n_days; ddd++)
//															{
//																w_sem = w_sem + workload[ddd][it->first];
//
//															}
//															w_sem = w_sem / (n_days * nurse[it->first].D);
//														}
//														target_list.insert(it, { w2, enter_cluster });
//													}
//												}
//											}
//										}
//
//										if (enter_cluster_list.size() > 0 || no_enter_cluster_list.size() > 0)
//										{
//
//											pattern_compatible.push_back(p);
//											day_compatible.push_back(day_c);
//											// Concatenar ambas listas y añadir a nurses_ordered
//
//
//											// Unimos ambas listas manteniendo la información de entrada al clúster
//											std::vector<std::pair<int, bool>> concatenated_list = enter_cluster_list;
//											concatenated_list.insert(concatenated_list.end(), no_enter_cluster_list.begin(), no_enter_cluster_list.end()); //Higher priority nurses place first in this list
//
//											// Guardamos el resultado de esta operación
//											nurses_ordered_c.push_back(std::move(concatenated_list));
//
//										}
//									}
//								}
//							}
//							else if (p == pattern_o)
//							{
//								std::vector<std::pair<int, bool>> enter_cluster_list;
//								std::vector<std::pair<int, bool>> no_enter_cluster_list;
//								day_c = d;
//
//								for (int w2 = 0; w2 < n_nurses; w2++)
//								{
//									if (sum_nurses - nurses_dif[w2] + 1 <= job[j].max_nurses)
//									{
//										if (w2 != w && nurse[w2].skill >= skill_j && nurse[w2].patient_s[j] && workload[day_c][w2] < nurse[w2].D)
//										{
//											bool enter_cluster = false;
//											bool work = false;
//
//											for (int i = 1; i < n_jobs; i++)
//											{
//												if (i != j && assign[day_c][w2][i])
//												{
//													work = true;
//													if (job[i].cluster == cluster_j)
//													{
//														enter_cluster = true;
//														break;
//													}
//												}
//											}
//
//											if (work == false)
//											{
//												enter_cluster = true;
//											}
//											if ((enter_cluster == true && (workload[day_c][w2] + job[j].time + 2 * min_intracluster_dist) < nurse[w2].D) || (enter_cluster == false && (workload[day_c][w2] + job[j].time + 2 * min_intercluster_dist) < nurse[w2].D))
//											{
//												auto& target_list = enter_cluster ? enter_cluster_list : no_enter_cluster_list;
//
//												auto it = target_list.begin();
//
//
//												int w2_sem = 0;
//												int w_sem = 0;
//
//												if (it != target_list.end())
//												{
//													for (int ddd = 0; ddd < n_days; ddd++)
//													{
//														w_sem = w_sem + workload[ddd][it->first];
//														w2_sem = w2_sem + workload[ddd][w2];
//
//													}
//													w_sem = w_sem / (n_days * nurse[it->first].D);
//													w2_sem = w2_sem / (n_days * nurse[w2].D);
//
//												}
//
//												while (it != target_list.end() && w_sem < w2_sem)
//												{
//													++it;
//													w_sem = 0;
//													for (int ddd = 0; ddd < n_days; ddd++)
//													{
//														w_sem = w_sem + workload[ddd][it->first];
//
//													}
//													w_sem = w_sem / (n_days * nurse[it->first].D);
//												}
//												target_list.insert(it, { w2, enter_cluster });
//											}
//										}
//									}
//								}
//
//
//								if (enter_cluster_list.size() > 0 || no_enter_cluster_list.size() > 0)
//								{
//
//									pattern_compatible.push_back(p);
//									day_compatible.push_back(day_c);
//									// Concatenar ambas listas y añadir a nurses_ordered
//
//
//									// Unimos ambas listas manteniendo la información de entrada al clúster
//									std::vector<std::pair<int, bool>> concatenated_list = enter_cluster_list;
//									concatenated_list.insert(concatenated_list.end(), no_enter_cluster_list.begin(), no_enter_cluster_list.end());
//
//									// Guardamos el resultado de esta operación
//									nurses_ordered_c.push_back(std::move(concatenated_list));
//
//								}
//							}
//						}
//
//						std::vector<std::vector<int>> ordered_2;
//
//						for (size_t i = 0; i < nurses_ordered_c.size(); ++i) {
//							int pattern = pattern_compatible[i];
//							int day = day_compatible[i];
//
//							for (const auto& [nurse_id, enters] : nurses_ordered_c[i]) {
//								ordered_2.push_back({ nurse_id, enters ? 1 : 0, pattern, day });
//							}
//						}
//
//						// Ordenamos: primero los que entran, luego por workload
//						std::sort(ordered_2.begin(), ordered_2.end(),
//							[&](const std::vector<int>& a, const std::vector<int>& b) {
//								if (a[1] != b[1])
//									return a[1] > b[1]; // los que entran (1) antes que los que no (0)
//
//								int wa = 0;
//								int wb = 0;
//								for (int ddd = 0; ddd < n_days; ddd++)
//								{
//									wa = wa + workload[ddd][a[0]];
//									wb = wb + workload[ddd][b[0]];
//
//								}
//								wa = wa / (n_days * nurse[a[0]].D);
//								wb = wb / (n_days * nurse[b[0]].D);
//								return wa < wb;
//							});
//
//
//						for (auto it = ordered_2.begin(); it != ordered_2.end(); ++it)
//						{
//							int nurse_id = (*it)[0];
//							bool enters = (*it)[1];
//							int pattern = (*it)[2];
//							int day = (*it)[3];
//
//							cout << endl;/*
//							cout << "Caregiver: " << w << " total " << workload[d][w] << " max " << nurse[w].D << endl;
//							cout << "Caregiver: " << nurse_id << " total " << workload[day][nurse_id] << " max " << nurse[nurse_id].D << endl;
//	*/
//
//							if (attempt_movement_switch_caregiver(patternassign, patternroute, workload, pattern_o, pattern, { d }, { day }, w, nurse_id, { j }, total_time))
//							{
//								//cout << endl;
//								//cout << "**** Moving visit" << endl;
//								//cout << "Visit " << j << endl;
//								//cout << "Moved from:" << endl;
//								//cout << "Caregiver: " << w << " to " << nurse_id << endl;
//								//cout << "Pattern: " << pattern_o << " to " << pattern << endl;
//								//cout << "Day: " << d << " to " << day << endl;
//								//cout << "Caregiver: " << w << " total " << workload[d][w] << " max " << nurse[w].D << endl;
//								//cout << "Caregiver: " << nurse_id << " total " << workload[day][nurse_id] << " max " << nurse[nurse_id].D << endl;
//								
//								goto restart_search;
//							}
//							else
//							{
//								n_attempts++;
//								if (n_attempts >= n_attempts_max)
//								{
//									cout << "No more attempts available" << endl;
//									return;
//								}
//							}
//						}
//
//
//						//1. Intento dárselo a otro caregiver (este y todos los días que diga su patrón).
//
//						vector<vector<int>> vec_days_p(n_nurses);
//						vector<int> nurses_ordered; // Caregivers ordered by decreasing number of shared clusters with the patient
//						vector<int> number_nurses(n_nurses, 0); // Number caregivers working in the same cluster that the corresponding patient
//						bool insert_w2;
//
//						int max_skill = -1;
//						for (int dd = 0; dd < n_days; dd++)
//						{
//							if (pattern[pattern_o].day_p[dd] > 0)
//							{
//								if (pattern[pattern_o].day_p[dd] >= max_skill)
//								{
//									max_skill = pattern[pattern_o].day_p[dd];
//								}
//							}
//						}
//						for (int w2 = 0; w2 < n_nurses; w2++)
//						{
//							insert_w2 = false;
//							if (w2 != w && nurse[w2].skill >= max_skill && nurse[w2].patient_s[j] == true)
//							{
//								for (int dd = 0; dd < n_days; dd++)
//								{
//									if (pattern[pattern_o].day_p[dd] > 0)
//									{
//										vec_days_p[w2].push_back(dd);
//										insert_w2 = true;
//										for (int i = 1; i < n_jobs; i++)
//										{
//											if (job[i].cluster == cluster_j && assign[dd][w2][i])
//											{
//												number_nurses[w2]++;
//												break;
//											}
//										}
//									}
//								}
//							}
//
//							if (insert_w2)
//							{
//								int workload_w2 = 0;
//								for (int dd = 0; dd < n_days; dd++) {
//									workload_w2 += workload[dd][w2];
//								}
//
//								auto it = nurses_ordered.begin();
//								while (it != nurses_ordered.end()) {
//									int w_it = *it;
//
//									// Criterio 1: number_nurses (mayor a menor)
//									if (number_nurses[w2] > number_nurses[w_it]) {
//										break; // w2 tiene más tareas, va antes
//									}
//									if (number_nurses[w2] == number_nurses[w_it]) {
//										// Criterio 2: workload total (menor a mayor)
//										int workload_it = 0;
//										for (int dd = 0; dd < n_days; dd++) {
//											workload_it += workload[dd][w_it];
//										}
//										if (workload_w2 < workload_it) {
//											break; // w2 tiene menos carga total, va antes
//										}
//									}
//
//									++it;
//								}
//
//								nurses_ordered.insert(it, w2);
//							}
//						}
//
//						for (int w2 : nurses_ordered)
//						{
//							if (attempt_movement_switch_caregiver(patternassign, patternroute, workload, pattern_o, pattern_o, vec_days_p[w2], vec_days_p[w2], w, w2, { j }, total_time))
//							{
//								/*cout << endl;
//								cout << "Swapping all assigned visits of the patient between caregivers" << endl;
//								cout << endl;
//								cout << "Patient " << j << " moved from nurse " << w << " to nurse " << w2 << endl;
//								cout << "day " << d << " nurse " << w << " total " << workload[d][w] << " max " << nurse[w].D << endl;
//								cout << "day " << d << " nurse " << w2 << " total " << workload[d][w2] << " max " << nurse[w2].D << endl;*/
//								
//								goto restart_search;
//							}
//							else
//							{
//								n_attempts++;
//								if (n_attempts >= n_attempts_max)
//								{
//									cout << "No more attempts available" << endl;
//									return;
//								}
//							}
//						}
//
//
//					}
//				}
//
//			}
//		
//	}
//	catch (const std::exception& e)
//	{
//		std::cerr << "Error: " << e.what() << std::endl;
//	}
//}


//void Data::routeimprove_c(vector<vector<vector<vector<vector<bool>>>>>& patternroute, vector<vector<double>>& workload, struct timeb t_ini, double& total_time)
//{
//	struct timeb t_after;
//	ftime(&t_after);
//	total_time = ((double)((t_after.time - t_ini.time) * 1000 + t_after.millitm - t_ini.millitm)) / 1000;
//
//	vector<double> wperc(n_nurses, 0);
//	for (int d = 0; d < n_days; d++)
//	{
//		for (int w = 0; w < n_nurses; w++)
//		{
//			wperc[w] += workload[d][w] / nurse[w].D;
//		}
//	}
//
//	double min_intracluster_dist = 1E+30, min_intercluster_dist = 1E+30, min_job_time = 1E+30;
//	for (int c1 = 0; c1 < n_clusters; c1++)
//	{
//		min_intracluster_dist = min(min_intracluster_dist, distance[c1][c1]);
//		for (int c2 = c1 + 1; c2 < n_clusters; c2++)
//		{
//			min_intercluster_dist = min(min_intercluster_dist, distance[c1][c2]);
//		}
//	}
//	for (int j = 1; j < n_jobs; j++)
//	{
//		min_job_time = min(min_job_time, job[j].time);
//	}
//
//	int cluster_j;
//	int n_T;
//	int skill_j;
//	int pattern_o;
//	int day_c;
//	int pattern_i;
//	vector<vector<vector<vector<bool>>>> patternassign, patternassign_copy;
//	vector<vector<vector<bool>>> assign, assign_copy;
//	vector<vector<vector<vector<vector<bool>>>>> patternroute_copy;
//	vector<int> pattern_compatible;
//	vector<int> day_compatible;
//	std::vector<std::vector<std::pair<int, bool>>> nurses_ordered_c;
//
//	patternassign = patternassign_from_patternroute(patternroute);
//	int n_attempts;
//	//
//	int n_successive_rejections = 0;
//	int n_max_succesive_rejections = 20; // Maximum number of successive rejections before moving on to next step
//	int n_attempts_max;
//	int n_attempts_max_fix = 500;// Maximum number of attempts before moving on to next step
//	double t_max = 120; // Maximum time to spend on local search
//
//restart_search1:
//	n_attempts = -1;
//	n_attempts_max = n_attempts_max_fix;
//	n_attempts++;
//	vector<int> wperc_rank = weekly_wperc_order_c(wperc); //Vector con los caregivers ordenados de mayor a menor workload_perc
//	assign = assignsimplify(patternassign);
//	try
//	{
//		for (int nurse_rank = 0; nurse_rank < n_nurses-1; nurse_rank++) //Selecciono la de mayor workload_perc
//		{
//		restart_search1_b:
//			int w = wperc_rank[nurse_rank];
//
//			for (int d = 0; d < n_days; d++)
//			{
//				for (int c = 0; c < n_clusters; c++)
//				{
//					//Trato de quitarle todos los trabajos del día d, cluster c
//					cout << endl;
//					cout << "************************************************************************************" << endl;
//					cout << "lS1.Trying to free: " << endl;
//					cout << "Caregiver " << w << endl;
//					cout << "Day " << d << endl;
//					cout << "Cluster " << c << endl;
//
//					vector<int> jobs_to_change;
//					vector<int> pattern_jobs_to_change;
//
//					for (int i = 1; i < n_jobs; i++)
//					{
//						if (assign[d][w][i] && job[i].cluster == c)
//						{
//							jobs_to_change.push_back(i);
//							for (int p = 0; p < n_patterns; p++)
//							{
//								if (patternassign[p][d][w][i] == true)
//								{
//									pattern_jobs_to_change.push_back(p);
//									break;
//								}
//							}
//
//						}
//					}
//
//					if (jobs_to_change.size() > 0)
//					{
//						// Nos preguntamos si el cambio ha de ser necesariamente a un caregiver concreto por care continuity
//						vector<int> w_other; //Si a ese paciente lo sirve otro día w y otro día distinto w_other;
//						bool free_w2 = true;
//
//						for (int i_id=0; i_id < jobs_to_change.size(); i_id++)
//						{
//							int i = jobs_to_change[i_id];
//							pattern_i = pattern_jobs_to_change[i_id];
//							free_w2 = true; 
//
//							for (int dd = 0; dd < n_days; dd++)
//							{
//								if (assign[dd][w][i] && dd != d) //w lo sirve otro día?
//								{
//									free_w2 = false; //w lo sirve otro día
//									break;
//								}
//							}
//							if (free_w2 ==  false)
//							{
//								//Sabemos que w lo sirve otro día. Miramos si hay otro caregiver que también lo sirva.
//								for (int w2 = 0; w2 < n_nurses; w2++)
//								{
//									if (w2 != w)
//									{
//										for (int dd = 0; dd < n_days; dd++)
//										{
//											if (assign[dd][w2][i])
//											{
//												//Caregiver w2 visita el paciente i otro día.
//												bool insert = true;
//												for (int ww = 0; ww < w_other.size(); ww++)
//												{
//													if(w2 == w_other[ww])
//													{
//														insert = false; //Es igual que otro que había que visitar.
//														break;
//													}
//												} 
//												if (insert == true)
//												{
//													w_other.push_back(w2); // En caso de cambio, solo se puede cambiar los trabajos a w2
//												}
//											}
//										}
//									}
//								}
//							}
//						}
//
//						bool change_possible;
//					
//						if (w_other.size()==1)
//						{
//							if (wperc[w_other[0]] > wperc[w] || (nurse_rank != 0 && w_other[0]!= wperc_rank[n_nurses-1]))
//							{
//								break; //No se lo voy a cambiar a un caregiver que esté más sobrecargado o a uno que no contribuya a que mejore la soluc
//							}
//							
//							// nos aseguramos de que w_other tenga habilidad suficiente y compatibilidad con todos los pacientes
//							change_possible = true; 
//							for (int i_id = 0; i_id < jobs_to_change.size(); i_id++)
//							{
//								int i = jobs_to_change[i_id];
//								pattern_i = pattern_jobs_to_change[i_id];
//								
//								if (nurse[w_other[0]].patient_s[i] == false || nurse[w_other[0]].skill < pattern[pattern_i].day_p[d] )
//								{
//									change_possible = false;
//									break;
//								}
//							}
//							if (change_possible)
//							{
//								//Intentamos el cambio
//								if (attempt_movement_switch_caregiver_all_cluster_c(patternassign, patternroute, workload, pattern_jobs_to_change, pattern_jobs_to_change, { d }, { d }, w, w_other[0], jobs_to_change, total_time))
//								{
//									n_attempts++;
//									if(n_attempts >= n_attempts_max)
//									{
//										cout << "No more attempts available for LS1" << endl;
//										n_successive_rejections = 0;
//										n_attempts = 0;
//										goto restart_search2;
//									}
//									ftime(&t_after);
//									total_time = ((double)((t_after.time - t_ini.time) * 1000 + t_after.millitm - t_ini.millitm)) / 1000;
//									if (total_time > Totalmax)
//									{
//										return;
//									}
//
//									cout << "*************************************************************************************************************************************************************************" << endl;
//
//									
//									for (int w = 0; w < n_nurses; w++)
//									{
//										wperc[w] = 0;
//										for (int d = 0; d < n_days; d++)
//										{
//											wperc[w] += workload[d][w] / nurse[w].D;
//										}
//									}
//
//									n_successive_rejections = 0; // Reiniciamos el contador de rechazos sucesivos
//									n_attempts_max = n_attempts_max_fix;
//									wperc_rank = weekly_wperc_order_c(wperc); //Vector con los caregivers ordenados de mayor a menor workload_perc
//									assign = assignsimplify(patternassign);
//									goto restart_search1_b;
//
//								}
//								else
//								{
//
//									ftime(&t_after);
//									total_time = ((double)((t_after.time - t_ini.time) * 1000 + t_after.millitm - t_ini.millitm)) / 1000;
//									if (total_time > Totalmax)
//									{
//										return;
//									}
//
//									n_attempts++;
//									n_successive_rejections++;
//									if (n_successive_rejections >= n_max_succesive_rejections)
//									{
//										cout << "Maximum number of successive rejections reached for LS1" << endl;
//										n_successive_rejections = 0;
//										n_attempts = 0;
//										goto restart_search2;
//									}
//									if (n_attempts >= n_attempts_max)
//									{
//										cout <<"No more attempts available for LS1" << endl;
//										n_successive_rejections = 0;
//										n_attempts = 0;
//										goto restart_search2;
//									}
//								}
//							}
//						}
//						else if (w_other.size() == 0)
//						{
//							int max_rank;
//							if (nurse_rank == 0)
//							{
//								max_rank = nurse_rank + 1;
//							}
//							else
//							{
//								max_rank = n_nurses - 1; // Solo consideramos cambios que le añadan trabajo al caregiver con menor carga
//							}
//							for (int w_idx = n_nurses - 1; w_idx >= max_rank; w_idx--)
//							{
//								int w_c = wperc_rank[w_idx]; // Empezamos por los caregivers con menor carga de trabajo semanal
//								
//								change_possible = true;
//								// nos aseguramos de que w_other tenga habilidad suficiente y compatibilidad con todos los pacientes
//
//
//								for (int i_id = 0; i_id < jobs_to_change.size(); i_id++)
//								{
//									int i = jobs_to_change[i_id];
//									pattern_i = pattern_jobs_to_change[i_id];
//
//									if (nurse[w_c].patient_s[i] == false || nurse[w_c].skill < pattern[pattern_i].day_p[d])
//									{
//										change_possible = false;
//										break;
//									}
//								}
//								if (change_possible)
//								{
//									//Intentamos el cambio
//									if (attempt_movement_switch_caregiver_all_cluster_c(patternassign, patternroute, workload, pattern_jobs_to_change, pattern_jobs_to_change, { d }, { d }, w, w_c, jobs_to_change, total_time))
//									{
//										cout << "************************************************************************************" << endl;
//										n_attempts++;
//										if (n_attempts >= n_attempts_max)
//										{
//											cout << "No more attempts available for LS1" << endl;
//											n_attempts = 0;
//											n_max_succesive_rejections = 0;
//											goto restart_search2;
//										}
//
//										ftime(&t_after);
//										total_time = ((double)((t_after.time - t_ini.time) * 1000 + t_after.millitm - t_ini.millitm)) / 1000;
//										if (total_time > Totalmax)
//										{
//											return;
//										}
//
//										
//										for (int w = 0; w < n_nurses; w++)
//										{
//											wperc[w] = 0;
//											for (int d = 0; d < n_days; d++)
//											{
//												wperc[w] += workload[d][w] / nurse[w].D;
//											}
//										}
//										if (nurse_rank == 0)
//										{
//											n_successive_rejections = 0;
//											goto restart_search1;
//										}
//										else
//										{
//											n_successive_rejections = 0;
//											n_attempts_max = n_attempts_max_fix;
//											wperc_rank = weekly_wperc_order_c(wperc); //Vector con los caregivers ordenados de mayor a menor workload_perc
//											assign = assignsimplify(patternassign);
//											nurse_rank = 1;
//											goto restart_search1_b;
//										}
//									}
//									else
//									{
//
//
//										ftime(&t_after);
//										total_time = ((double)((t_after.time - t_ini.time) * 1000 + t_after.millitm - t_ini.millitm)) / 1000;
//										if (total_time > Totalmax)
//										{
//											return;
//										}
//
//										n_attempts++;
//										if (n_attempts >= n_attempts_max)
//										{
//											cout << "No more attempts available" << endl;
//											goto restart_search2;
//										}
//									}
//								}
//							}
//						}
//					}
//					
//				}
//			}
//		}
//	}
//	catch (const std::exception& e)
//	{
//		std::cerr << "Error: " << e.what() << std::endl;
//	}
//
//restart_search2:
//	n_successive_rejections = 0;
//	n_attempts_max = n_attempts_max_fix;
//	n_attempts++;
//	wperc_rank = weekly_wperc_order(workload);
//	assign = assignsimplify(patternassign);
//
//	try
//	{
//		for (int nurse_rank = 0; nurse_rank < n_nurses; nurse_rank++)
//		{
//			int w = wperc_rank[nurse_rank];
//			vector<pair<int, int>> patient_rank = weekly_patient_order(assignsimplify(patternassign), w);
//
//			for (int patient_day_index = 0; patient_day_index < patient_rank.size(); patient_day_index++)
//			{
//				int j = patient_rank[patient_day_index].first;
//				int d = patient_rank[patient_day_index].second;
//
//				//Trato de quitar el trabajo j el día d a un caregivers.
//				cout << endl;
//				cout << "*************************************************************************************************************************************************************************" << endl;
//				cout << "lS2.Trying to remove: " << endl;
//				cout << "Caregiver " << w << endl;
//				cout << "Patient " << j << endl;
//				cout << "Day " << d << endl;
//
//
//				
//				nurses_ordered_c.clear();
//				pattern_compatible.clear();
//				day_compatible.clear();
//				cluster_j = job[j].cluster;
//
//				for (int p = 0; p < n_patterns; p++)
//				{
//					if (patternassign[p][d][w][j] == true)
//					{
//						pattern_o = p;
//						break;
//					}
//				}
//
//				//Si estoy aquí intento quitar el trabajo j del caregiver w el día d
//				vector <int> nurses_dif(n_nurses, 0);
//				for (int w2 = 0; w2 < n_nurses; w2++)
//				{
//					for (int dd = 0; dd < n_days; dd++)
//					{
//						if (dd != d && pattern[pattern_o].day_p[dd] > 0 && patternassign[pattern_o][dd][w2][j] == true)
//						{
//							nurses_dif[w2] = 1;
//						}
//					}
//				}
//				int sum_nurses = 0;
//				for (int w2 = 0; w2 < n_nurses; w2++)
//				{
//					sum_nurses = sum_nurses + nurses_dif[w2];
//				}
//
//				skill_j = pattern[pattern_o].day_p[d];
//
//
//				//lista de patrones compatibles. Cambia solamente el día que quiero descargar por otro día.
//
//				bool compatible, first;
//
//				for (int p = 0; p < n_patterns; p++)
//				{
//					compatible = true;
//					first = true;
//					if (p != pattern_o && pattern[p].patient_p[j] == true)
//					{
//						if (pattern[p].day_p[d] == 0)
//						{
//							for (int dd = 0; dd < n_days; dd++)
//							{
//								if (dd != d && pattern[p].day_p[dd] != pattern[pattern_o].day_p[dd])
//								{
//									if (first == true && pattern[p].day_p[dd] == pattern[pattern_o].day_p[d])
//									{
//										first = false;
//										day_c = dd;
//									}
//									else
//									{
//										compatible = false;
//										break;
//									}
//								}
//							}
//							if (compatible)
//							{
//
//								std::vector<std::pair<int, bool>> enter_cluster_list;
//								std::vector<std::pair<int, bool>> no_enter_cluster_list;
//
//								for (int w2 = 0; w2 < n_nurses; w2++)
//								{
//									if (sum_nurses - nurses_dif[w2] + 1 <= job[j].max_nurses)
//									{
//										if (nurse[w2].skill >= skill_j && nurse[w2].patient_s[j] && workload[day_c][w2] < nurse[w2].D)
//										{
//											bool enter_cluster = false;
//											bool work = false;
//
//											for (int i = 1; i < n_jobs; i++)
//											{
//												if (i != j && assign[day_c][w2][i])
//												{
//													work = true;
//													if (job[i].cluster == cluster_j)
//													{
//														enter_cluster = true;
//														break;
//													}
//												}
//											}
//
//											if (work == false)
//											{
//												enter_cluster = true;
//											}
//
//											if ((nurse_rank==0 || w2== wperc_rank[n_nurses-1]) && ((enter_cluster == true && (workload[day_c][w2] + job[j].time + 2 * min_intracluster_dist) < nurse[w2].D) || (enter_cluster == false && (workload[day_c][w2] + job[j].time + 2 * min_intercluster_dist) < nurse[w2].D)))
//											{
//												auto& target_list = enter_cluster ? enter_cluster_list : no_enter_cluster_list;
//
//												auto it = target_list.begin();
//												int w2_sem = 0;
//												int w_sem = 0;
//
//												if (it != target_list.end())
//												{
//													for (int ddd = 0; ddd < n_days; ddd++)
//													{
//														w_sem = w_sem + workload[ddd][it->first];
//														w2_sem = w2_sem + workload[ddd][w2];
//
//													}
//													w_sem = w_sem / (n_days * nurse[it->first].D);
//													w2_sem = w2_sem / (n_days * nurse[w2].D);
//
//
//												}
//
//												while (it != target_list.end() && w_sem < w2_sem)
//												{
//													++it;
//													w_sem = 0;
//													for (int ddd = 0; ddd < n_days; ddd++)
//													{
//														w_sem = w_sem + workload[ddd][it->first];
//
//													}
//													w_sem = w_sem / (n_days * nurse[it->first].D);
//												}
//												target_list.insert(it, { w2, enter_cluster });
//											}
//										}
//									}
//								}
//
//								if (enter_cluster_list.size() > 0 || no_enter_cluster_list.size() > 0)
//								{
//
//									pattern_compatible.push_back(p);
//									day_compatible.push_back(day_c);
//									// Concatenar ambas listas y añadir a nurses_ordered
//
//
//									// Unimos ambas listas manteniendo la información de entrada al clúster
//									std::vector<std::pair<int, bool>> concatenated_list = enter_cluster_list;
//									concatenated_list.insert(concatenated_list.end(), no_enter_cluster_list.begin(), no_enter_cluster_list.end());
//
//									// Guardamos el resultado de esta operación
//									nurses_ordered_c.push_back(std::move(concatenated_list));
//
//								}
//							}
//						}
//					}
//					else if (p == pattern_o)
//					{
//						std::vector<std::pair<int, bool>> enter_cluster_list;
//						std::vector<std::pair<int, bool>> no_enter_cluster_list;
//						day_c = d;
//
//						for (int w2 = 0; w2 < n_nurses; w2++)
//						{
//							if (sum_nurses - nurses_dif[w2] + 1 <= job[j].max_nurses)
//							{
//								if (w2 != w && nurse[w2].skill >= skill_j && nurse[w2].patient_s[j] && workload[day_c][w2] < nurse[w2].D)
//								{
//									bool enter_cluster = false;
//									bool work = false;
//
//									for (int i = 1; i < n_jobs; i++)
//									{
//										if (i != j && assign[day_c][w2][i])
//										{
//											work = true;
//											if (job[i].cluster == cluster_j)
//											{
//												enter_cluster = true;
//												break;
//											}
//										}
//									}
//
//									if (work == false)
//									{
//										enter_cluster = true;
//									}
//									if ((nurse_rank == 0 || w2 == wperc_rank[n_nurses - 1]) && ((enter_cluster == true && (workload[day_c][w2] + job[j].time + 2 * min_intracluster_dist) < nurse[w2].D) || (enter_cluster == false && (workload[day_c][w2] + job[j].time + 2 * min_intercluster_dist) < nurse[w2].D)))
//									{
//										auto& target_list = enter_cluster ? enter_cluster_list : no_enter_cluster_list;
//
//										auto it = target_list.begin();
//
//
//										int w2_sem = 0;
//										int w_sem = 0;
//
//										if (it != target_list.end())
//										{
//											for (int ddd = 0; ddd < n_days; ddd++)
//											{
//												w_sem = w_sem + workload[ddd][it->first];
//												w2_sem = w2_sem + workload[ddd][w2];
//
//											}
//											w_sem = w_sem / (n_days * nurse[it->first].D);
//											w2_sem = w2_sem / (n_days * nurse[w2].D);
//
//										}
//
//										while (it != target_list.end() && w_sem < w2_sem)
//										{
//											++it;
//											w_sem = 0;
//											for (int ddd = 0; ddd < n_days; ddd++)
//											{
//												w_sem = w_sem + workload[ddd][it->first];
//
//											}
//											w_sem = w_sem / (n_days * nurse[it->first].D);
//										}
//										target_list.insert(it, { w2, enter_cluster });
//									}
//								}
//							}
//						}
//
//
//						if (enter_cluster_list.size() > 0 || no_enter_cluster_list.size() > 0)
//						{
//
//							pattern_compatible.push_back(p);
//							day_compatible.push_back(day_c);
//							// Concatenar ambas listas y añadir a nurses_ordered
//
//
//							// Unimos ambas listas manteniendo la información de entrada al clúster
//							std::vector<std::pair<int, bool>> concatenated_list = enter_cluster_list;
//							concatenated_list.insert(concatenated_list.end(), no_enter_cluster_list.begin(), no_enter_cluster_list.end());
//
//							// Guardamos el resultado de esta operación
//							nurses_ordered_c.push_back(std::move(concatenated_list));
//
//						}
//					}
//				}
//
//				std::vector<std::vector<int>> ordered_2;
//
//				for (size_t i = 0; i < nurses_ordered_c.size(); ++i) {
//					int pattern = pattern_compatible[i];
//					int day = day_compatible[i];
//
//					for (const auto& [nurse_id, enters] : nurses_ordered_c[i]) {
//						ordered_2.push_back({ nurse_id, enters ? 1 : 0, pattern, day });
//					}
//				}
//
//				// Ordenamos: primero los que entran, luego por workload
//				std::sort(ordered_2.begin(), ordered_2.end(),
//					[&](const std::vector<int>& a, const std::vector<int>& b) {
//						if (a[1] != b[1])
//							return a[1] > b[1]; // los que entran (1) antes que los que no (0)
//
//						int wa = 0;
//						int wb = 0;
//						for (int ddd = 0; ddd < n_days; ddd++)
//						{
//							wa = wa + workload[ddd][a[0]];
//							wb = wb + workload[ddd][b[0]];
//
//						}
//						wa = wa / (n_days * nurse[a[0]].D);
//						wb = wb / (n_days * nurse[b[0]].D);
//						return wa < wb;
//					});
//
//
//				for (auto it = ordered_2.begin(); it != ordered_2.end(); ++it)
//				{
//					int nurse_id = (*it)[0];
//					bool enters = (*it)[1];
//					int pattern = (*it)[2];
//					int day = (*it)[3];
//
//					cout << endl;/*
//					cout << "Caregiver: " << w << " total " << workload[d][w] << " max " << nurse[w].D << endl;
//					cout << "Caregiver: " << nurse_id << " total " << workload[day][nurse_id] << " max " << nurse[nurse_id].D << endl;
//*/
//
//					if (attempt_movement_switch_caregiver(patternassign, patternroute, workload, pattern_o, pattern, { d }, { day }, w, nurse_id, { j }, total_time))
//					{
//						//cout << endl;
//						//cout << "**** Moving visit" << endl;
//						//cout << "Visit " << j << endl;
//						//cout << "Moved from:" << endl;
//						//cout << "Caregiver: " << w << " to " << nurse_id << endl;
//						//cout << "Pattern: " << pattern_o << " to " << pattern << endl;
//						//cout << "Day: " << d << " to " << day << endl;
//						//cout << "Caregiver: " << w << " total " << workload[d][w] << " max " << nurse[w].D << endl;
//						//cout << "Caregiver: " << nurse_id << " total " << workload[day][nurse_id] << " max " << nurse[nurse_id].D << endl;
//						for (int w = 0; w < n_nurses; w++)
//						{
//							wperc[w] = 0;
//							for (int d = 0; d < n_days; d++)
//							{
//								wperc[w] += workload[d][w] / nurse[w].D;
//							}
//						}
//						if (total_time > Totalmax)
//						{
//							return;
//						}
//						
//						goto restart_search2;
//					}
//					else
//					{
//						if (total_time > Totalmax)
//						{
//							return;
//						}
//						n_attempts++;
//						if (n_attempts >= n_attempts_max)
//						{
//							cout << "No more attempts available ls 2" << endl;
//							return;
//							//goto restart_search3;
//						}
//					}
//				}
//			}
//		}
//
//	}
//	catch (const std::exception& e)
//	{
//		std::cerr << "Error: " << e.what() << std::endl;
//	}
////
////restart_search3:
////
////	n_attempts = -1;
////	n_attempts_max = n_attempts_max_fix;
////	n_attempts++;
////	wperc_rank = weekly_wperc_order(workload);
////	assign = assignsimplify(patternassign);
////	try
////	{
////		for (int nurse_rank = 0; nurse_rank < n_nurses; nurse_rank++)
////		{
////		restart_search3_b:
////			int w = wperc_rank[nurse_rank];
////			vector<pair<int, int>> patient_rank = weekly_patient_order(assignsimplify(patternassign), w);
////			for (int patient_day_index = 0; patient_day_index < patient_rank.size(); patient_day_index++)
////			{
////				int j = patient_rank[patient_day_index].first;
////				cout << "patient " << j << endl;
////				int d = patient_rank[patient_day_index].second;
////				cout << "day " << d << endl;
////				nurses_ordered_c.clear();
////				pattern_compatible.clear();
////				day_compatible.clear();
////
////				cluster_j = job[j].cluster;
////				for (int p = 0; p < n_patterns; p++)
////				{
////					if (patternassign[p][d][w][j] == true)
////					{
////						pattern_o = p;
////						break;
////					}
////				}
////
////
////				//Si estoy aquí intento quitar el trabajo j del caregiver w el día d
////				vector <int> nurses_dif(n_nurses, 0);
////				for (int w2 = 0; w2 < n_nurses; w2++)
////				{
////					for (int dd = 0; dd < n_days; dd++)
////					{
////
////						if (dd != d && pattern[pattern_o].day_p[dd] > 0 && patternassign[pattern_o][dd][w2][j] == true)
////						{
////							nurses_dif[w2] = 1;
////						}
////					}
////				}
////				int sum_nurses = 0;
////				for (int w2 = 0; w2 < n_nurses; w2++)
////				{
////					sum_nurses = sum_nurses + nurses_dif[w2];
////				}
////
////				skill_j = pattern[pattern_o].day_p[d];
////
////
////				//1. Intento dárselo a otro caregiver (este y todos los días que diga su patrón).
////
////				cout << endl;
////				cout << "************************************************************************************" << endl;
////				cout << "LS3.Trying to remove all days: " << endl;
////				cout << "Caregiver " << w << endl;
////				cout << "Patient " << j << endl;
////
////				vector<vector<int>> vec_days_p(n_nurses);
////				vector<int> nurses_ordered; // Caregivers ordered by decreasing number of shared clusters with the patient
////				vector<int> number_nurses(n_nurses, 0); // Number caregivers working in the same cluster that the corresponding patient
////				bool insert_w2;
////
////				int max_skill = -1;
////				for (int dd = 0; dd < n_days; dd++)
////				{
////					if (pattern[pattern_o].day_p[dd] > 0)
////					{
////						if (pattern[pattern_o].day_p[dd] >= max_skill)
////						{
////							max_skill = pattern[pattern_o].day_p[dd];
////						}
////					}
////				}
////				for (int w2 = 0; w2 < n_nurses; w2++)
////				{
////					insert_w2 = false;
////					if ((nurse_rank == 0 || w2 == wperc_rank[n_nurses - 1]) && w2 != w && nurse[w2].skill >= max_skill && nurse[w2].patient_s[j] == true && wperc[w2] < wperc[w])
////					{
////						for (int dd = 0; dd < n_days; dd++)
////						{
////							if (pattern[pattern_o].day_p[dd] > 0)
////							{
////								vec_days_p[w2].push_back(dd);
////								insert_w2 = true;
////								for (int i = 1; i < n_jobs; i++)
////								{
////									if (job[i].cluster == cluster_j && assign[dd][w2][i])
////									{
////										number_nurses[w2]++;
////										break;
////									}
////								}
////							}
////						}
////					}
////
////					if (insert_w2)
////					{
////						int workload_w2 = 0;
////						for (int dd = 0; dd < n_days; dd++) {
////							workload_w2 += workload[dd][w2];
////						}
////
////						auto it = nurses_ordered.begin();
////						while (it != nurses_ordered.end()) {
////							int w_it = *it;
////
////							// Criterio 1: number_nurses (mayor a menor)
////							if (number_nurses[w2] > number_nurses[w_it]) {
////								break; // w2 tiene más tareas, va antes
////							}
////							if (number_nurses[w2] == number_nurses[w_it]) {
////								// Criterio 2: workload total (menor a mayor)
////								int workload_it = 0;
////								for (int dd = 0; dd < n_days; dd++) {
////									workload_it += workload[dd][w_it];
////								}
////								if (workload_w2 < workload_it) {
////									break; // w2 tiene menos carga total, va antes
////								}
////							}
////
////							++it;
////						}
////
////						nurses_ordered.insert(it, w2);
////					}
////				}
////
////				for (int w2 : nurses_ordered)
////				{
////					if (attempt_movement_switch_caregiver(patternassign, patternroute, workload, pattern_o, pattern_o, vec_days_p[w2], vec_days_p[w2], w, w2, { j }, total_time))
////					{
////						for (int w = 0; w < n_nurses; w++)
////						{
////							wperc[w] = 0;
////							for (int d = 0; d < n_days; d++)
////							{
////								wperc[w] += workload[d][w] / nurse[w].D;
////							}
////						}
////
////
////						ftime(&t_after);
////						total_time = ((double)((t_after.time - t_ini.time) * 1000 + t_after.millitm - t_ini.millitm)) / 1000;
////						if (total_time > Totalmax)
////						{
////							return;
////						}
////
////						if (nurse_rank == 0)
////						{
////							goto restart_search3;
////						}
////						else
////						{
////							n_attempts = -1;
////							n_attempts_max = n_attempts_max_fix;
////							n_attempts++;
////							wperc_rank = weekly_wperc_order_c(wperc); //Vector con los caregivers ordenados de mayor a menor workload_perc
////							assign = assignsimplify(patternassign);
////							nurse_rank = 1;
////							goto restart_search3_b;
////						}
////					}
////					else
////					{
////
////
////						ftime(&t_after);
////						total_time = ((double)((t_after.time - t_ini.time) * 1000 + t_after.millitm - t_ini.millitm)) / 1000;
////						if (total_time > Totalmax)
////						{
////							return;
////						}
////
////						n_attempts++;
////						if (n_attempts >= n_attempts_max)
////						{
////							cout << "No more attempts available ls 3" << endl;
////							return;
////						}
////					}
////				}
////			}
////		}
////
////	}
////	catch (const std::exception& e)
////	{
////		std::cerr << "Error: " << e.what() << std::endl;
////	}
//}

void Data::routeimprove_cl(vector<vector<vector<vector<vector<bool>>>>>& patternroute, vector<vector<double>>& workload, struct timeb t_ini, double& total_time)
{
	struct timeb t_after;
	ftime(&t_after);
	total_time = ((double)((t_after.time - t_ini.time) * 1000 + t_after.millitm - t_ini.millitm)) / 1000;

	vector<double> wperc(n_nurses, 0);
	for (int d = 0; d < n_days; d++)
	{
		for (int w = 0; w < n_nurses; w++)
		{
			wperc[w] += workload[d][w] / nurse[w].D;
		}
	}

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
	int pattern_i;
	int ITER = 0;
	vector<vector<vector<vector<bool>>>> patternassign, patternassign_copy;
	vector<vector<vector<bool>>> assign, assign_copy;
	vector<vector<vector<vector<vector<bool>>>>> patternroute_copy;
	vector<int> pattern_compatible;
	vector<int> day_compatible;
	std::vector<std::vector<std::pair<int, bool>>> nurses_ordered_c;
	patternassign = patternassign_from_patternroute(patternroute);
	assign = assignsimplify(patternassign);
	while (total_time <= Totalmax)
	{
	restart_search1A:
		int n_attempts = 0;
		int n_successive_rejections = 0;
		int n_max_succesive_rejections = 50; // Maximum number of successive rejections before moving on to next step
		int n_attempts_max = 500;// Maximum number of attempts before moving on to next step
		vector<pair<int, int>> considered_cluster_visits; // clusters and days we have failed to move for the considered caregiver
		int nurse_rank = 0;
	restart_search1:
		n_successive_rejections = 0;
		vector<int> wperc_rank = weekly_wperc_order_c(wperc); //Vector con los caregivers ordenados de mayor a menor workload_perc
		int w = wperc_rank[nurse_rank];
		vector<pair<int, int>> patient_rank = weekly_patient_order(assign, w);
		considered_cluster_visits.clear();
		int visit_index = 0;
		pair<int, int> current_visit;
		try
		{
		restart_search1b:
			if (visit_index >= patient_rank.size())
			{
				nurse_rank++;
				if (nurse_rank >= n_nurses)
				{
					cout << "No more switches to consider for LS1" << endl;
					n_successive_rejections = 0;
					n_attempts = 0;
					goto restart_search2;
				}
				goto restart_search1;
			}
			current_visit = patient_rank[visit_index]; // Visit to consider
			for (pair<int, int> considered_cluster_visit : considered_cluster_visits)
			{
				if (job[current_visit.first].cluster == considered_cluster_visit.first && current_visit.second == considered_cluster_visit.second)
				{
					visit_index++;
					goto restart_search1b; // Si ya hemos intentado mover ese cluster en este día, lo saltamos
				}
			}
			int c = job[current_visit.first].cluster; // Paciente a visitar
			int d = current_visit.second; // Día de la visita
			considered_cluster_visits.emplace_back(c, d);

			//Trato de quitarle todos los trabajos del día d, cluster c
			cout << endl;
			cout << "************************************************************************************" << endl;
			cout << "lS1.Trying to free: " << endl;
			cout << "Caregiver " << w << " (nurse_rank = " << nurse_rank << ")" << endl;
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
					if (wperc[w_other[0]] > wperc[w] || (nurse_rank != 0 && w_other[0] != wperc_rank[n_nurses - 1]))
					{
						visit_index++;
						goto restart_search1b; //No se lo voy a cambiar a un caregiver que esté más sobrecargado o a uno que no contribuya a que mejore la soluc
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
						if (attempt_movement_switch_caregiver_all_cluster_c(patternassign, patternroute, workload, pattern_jobs_to_change, pattern_jobs_to_change, { d }, { d }, w, w_other[0], jobs_to_change, total_time, ITER))
						{
							n_attempts++;
							n_successive_rejections = 0; // Reiniciamos el contador de rechazos sucesivos
							if (n_attempts >= n_attempts_max)
							{
								cout << "No more attempts available for LS1" << endl;
								n_successive_rejections = 0;
								n_attempts = 0;
								goto restart_search2;
							}
							ftime(&t_after);
							total_time = ((double)((t_after.time - t_ini.time) * 1000 + t_after.millitm - t_ini.millitm)) / 1000;
							if (total_time > Totalmax)
							{
								return;
							}
							cout << "************************************************************************************" << endl;

							for (int w = 0; w < n_nurses; w++)
							{
								wperc[w] = 0;
								for (int d = 0; d < n_days; d++)
								{
									wperc[w] += workload[d][w] / nurse[w].D;
								}
							}

							wperc_rank = weekly_wperc_order_c(wperc); //Vector con los caregivers ordenados de mayor a menor workload_perc
							assign = assignsimplify(patternassign);
							if (w = wperc_rank[nurse_rank])
							{
								patient_rank = weekly_patient_order(assign, w); //Actualizamos lista de pacientes del caregiver.
								goto restart_search1b; //Sin hacer visit_index++ ya que esta visita la hemos quitado
							}
							else
							{
								goto restart_search1;
							}

						}
						else
						{

							ftime(&t_after);
							total_time = ((double)((t_after.time - t_ini.time) * 1000 + t_after.millitm - t_ini.millitm)) / 1000;
							if (total_time > Totalmax)
							{
								return;
							}

							n_attempts++;
							n_successive_rejections++;
							if (n_successive_rejections >= n_max_succesive_rejections)
							{
								cout << "Maximum number of successive rejections reached for LS1" << endl;
								n_successive_rejections = 0;
								n_attempts = 0;
								goto restart_search2;
							}
							if (n_attempts >= n_attempts_max)
							{
								cout << "No more attempts available for LS1" << endl;
								n_successive_rejections = 0;
								n_attempts = 0;
								goto restart_search2;
							}

							visit_index++;
							goto restart_search1b; // No hemos podido mover el trabajo, pasamos a la siguiente visita del caregiver
						}
					}
				}
				else if (w_other.size() == 0)
				{
					int max_rank;
					if (nurse_rank == 0)
					{
						max_rank = nurse_rank + 1;
					}
					else
					{
						max_rank = n_nurses - 1; // Solo consideramos cambios que le añadan trabajo al caregiver con menor carga
					}
					for (int w_idx = n_nurses - 1; w_idx >= max_rank; w_idx--)
					{
						int w_c = wperc_rank[w_idx]; // Empezamos por los caregivers con menor carga de trabajo semanal

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
							if (attempt_movement_switch_caregiver_all_cluster_c(patternassign, patternroute, workload, pattern_jobs_to_change, pattern_jobs_to_change, { d }, { d }, w, w_c, jobs_to_change, total_time, ITER))
							{
								n_attempts++;
								n_successive_rejections = 0; // Reiniciamos el contador de rechazos sucesivos
								if (n_attempts >= n_attempts_max)
								{
									cout << "No more attempts available for LS1" << endl;
									n_successive_rejections = 0;
									n_attempts = 0;
									goto restart_search2;
								}
								ftime(&t_after);
								total_time = ((double)((t_after.time - t_ini.time) * 1000 + t_after.millitm - t_ini.millitm)) / 1000;
								if (total_time > Totalmax)
								{
									return;
								}

								cout << "************************************************************************************" << endl;


								for (int w = 0; w < n_nurses; w++)
								{
									wperc[w] = 0;
									for (int d = 0; d < n_days; d++)
									{
										wperc[w] += workload[d][w] / nurse[w].D;
									}
								}

								wperc_rank = weekly_wperc_order_c(wperc); //Vector con los caregivers ordenados de mayor a menor workload_perc
								assign = assignsimplify(patternassign);
								if (w = wperc_rank[nurse_rank])
								{
									patient_rank = weekly_patient_order(assign, w); //Actualizamos lista de pacientes del caregiver.
									goto restart_search1b; //Sin hacer visit_index++ ya que esta visita la hemos quitado
								}
								else
								{
									goto restart_search1;
								}
							}
							else
							{
								ftime(&t_after);
								total_time = ((double)((t_after.time - t_ini.time) * 1000 + t_after.millitm - t_ini.millitm)) / 1000;
								if (total_time > Totalmax)
								{
									return;
								}

								n_attempts++;
								n_successive_rejections++;
								if (n_successive_rejections >= n_max_succesive_rejections)
								{
									cout << "Maximum number of successive rejections reached for LS1" << endl;
									n_successive_rejections = 0;
									n_attempts = 0;
									goto restart_search2;
								}
								if (n_attempts >= n_attempts_max)
								{
									cout << "No more attempts available for LS1" << endl;
									n_successive_rejections = 0;
									n_attempts = 0;
									goto restart_search2;
								}
							}
						}
					}
				}
			}
			// Si hemos llegado aquí es que no hemos podido mover el trabajo a ningún otro caregiver
			visit_index++;
			goto restart_search1b; // No hemos podido mover el trabajo, pasamos a la siguiente visita del caregiver
		}
		catch (const std::exception& e)
		{
			std::cerr << "Error: " << e.what() << std::endl;
		}
	restart_search2:
		n_successive_rejections = 0;
		n_attempts++;
		wperc_rank = weekly_wperc_order(workload);
		assign = assignsimplify(patternassign);

		try
		{
			for (int nurse_rank = 0; nurse_rank < n_nurses; nurse_rank++)
			{
				int w = wperc_rank[nurse_rank];
				vector<pair<int, int>> patient_rank = weekly_patient_order(assignsimplify(patternassign), w);

				for (int patient_day_index = 0; patient_day_index < patient_rank.size(); patient_day_index++)
				{
					int j = patient_rank[patient_day_index].first;
					int d = patient_rank[patient_day_index].second;

					//Trato de quitar el trabajo j el día d a un caregivers.
					cout << endl;
					cout << "************************************************************************************" << endl;
					cout << "lS2.Trying to remove: " << endl;
					cout << "Caregiver " << w << endl;
					cout << "Patient " << j << endl;
					cout << "Day " << d << endl;



					nurses_ordered_c.clear();
					pattern_compatible.clear();
					day_compatible.clear();
					cluster_j = job[j].cluster;

					for (int p = 0; p < n_patterns; p++)
					{
						if (patternassign[p][d][w][j] == true)
						{
							pattern_o = p;
							break;
						}
					}

					//Si estoy aquí intento quitar el trabajo j del caregiver w el día d
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


					//lista de patrones compatibles. Cambia solamente el día que quiero descargar por otro día.

					bool compatible, first;

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

												if ((nurse_rank == 0 || w2 == wperc_rank[n_nurses - 1]) && ((enter_cluster == true && (workload[day_c][w2] + job[j].time + 2 * min_intracluster_dist) < nurse[w2].D) || (enter_cluster == false && (workload[day_c][w2] + job[j].time + 2 * min_intercluster_dist) < nurse[w2].D)))
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
										if ((nurse_rank == 0 || w2 == wperc_rank[n_nurses - 1]) && ((enter_cluster == true && (workload[day_c][w2] + job[j].time + 2 * min_intracluster_dist) < nurse[w2].D) || (enter_cluster == false && (workload[day_c][w2] + job[j].time + 2 * min_intercluster_dist) < nurse[w2].D)))
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

						cout << endl;/*
						cout << "Caregiver: " << w << " total " << workload[d][w] << " max " << nurse[w].D << endl;
						cout << "Caregiver: " << nurse_id << " total " << workload[day][nurse_id] << " max " << nurse[nurse_id].D << endl;
	*/
						ftime(&t_after);
						total_time = ((double)((t_after.time - t_ini.time) * 1000 + t_after.millitm - t_ini.millitm)) / 1000;
						if (attempt_movement_switch_caregiver(patternassign, patternroute, workload, pattern_o, pattern, { d }, { day }, w, nurse_id, { j }, total_time, ITER))
						{
							//cout << endl;
							//cout << "**** Moving visit" << endl;
							//cout << "Visit " << j << endl;
							//cout << "Moved from:" << endl;
							//cout << "Caregiver: " << w << " to " << nurse_id << endl;
							//cout << "Pattern: " << pattern_o << " to " << pattern << endl;
							//cout << "Day: " << d << " to " << day << endl;
							//cout << "Caregiver: " << w << " total " << workload[d][w] << " max " << nurse[w].D << endl;
							//cout << "Caregiver: " << nurse_id << " total " << workload[day][nurse_id] << " max " << nurse[nurse_id].D << endl;
							n_attempts++;
							n_successive_rejections = 0;
							for (int w = 0; w < n_nurses; w++)
							{
								wperc[w] = 0;
								for (int d = 0; d < n_days; d++)
								{
									wperc[w] += workload[d][w] / nurse[w].D;
								}
							}
							if (total_time > Totalmax)
							{
								return;
							}
							if (n_successive_rejections >= n_max_succesive_rejections)
							{
								cout << "Maximum number of successive rejections reached for LS2" << endl;
								n_successive_rejections = 0;
								n_attempts = 0;
								goto restart_search1A;
							}
							if (n_attempts >= n_attempts_max)
							{
								cout << "No more attempts available for LS2" << endl;
								n_successive_rejections = 0;
								n_attempts = 0;
								goto restart_search1A;
								//return;
							}

							goto restart_search2;
						}
						else
						{
							if (total_time > Totalmax)
							{
								return;
							}
							n_attempts++;
							n_successive_rejections++;
							if (n_attempts >= n_attempts_max)
							{
								cout << "No more attempts available for LS2" << endl;
								n_successive_rejections = 0;
								n_attempts = 0;
								goto restart_search1A;
								//return;
							}
						}
					}
				}
			}

		}
		catch (const std::exception& e)
		{
			std::cerr << "Error: " << e.what() << std::endl;
		}
		ftime(&t_after);
		total_time = ((double)((t_after.time - t_ini.time) * 1000 + t_after.millitm - t_ini.millitm)) / 1000;
	}
}