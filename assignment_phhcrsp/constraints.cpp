#include "Data.h"
void Data::binary_vars_a(GRBModel& model, GRBVar**** x, GRBVar** z, GRBVar*** a)
{
	int c, j, p, w, d;
	int s_p;
	int s_w;
	int s_i0;
	int s_i1;
	int s_j0;
	int s_j1;

	for (p = 0; p < n_patterns; p++)
	{
		x[p] = new GRBVar **[n_days];
		for (d = 0; d < n_days; d++)
		{
			s_p = pattern[p].day_p[d];
			if (s_p >= 1)
			{
				x[p][d] = new GRBVar *[n_nurses];
				for (w = 0; w < n_nurses; w++)
				{
					s_w = nurse[w].skill;
					if (s_w >= s_p)
					{
						x[p][d][w] = new GRBVar [n_jobs];
						for (j = 1; j < n_jobs; j++)
						{
							if (nurse[w].patient_s[j] && pattern[p].patient_p[j])
							{
								if (rl)
								{
									string s = "x(" + to_string(p) + "," + to_string(d) + "," + to_string(w) + "," + to_string(j)  + ")";
									x[p][d][w][j] = model.addVar(0.0, 1.0, 0.0, GRB_CONTINUOUS, s);
								}
								else
								{
									string s = "x(" + to_string(p) + "," + to_string(d) + "," + to_string(w) + "," + to_string(j) + ")";
									x[p][d][w][j] = model.addVar(0.0, 1.0, 0.0, GRB_BINARY, s);
								}
		
							}
						}
					}
				}
			}
		}
	}
	for (d = 0; d < n_days; d++)
	{
		a[d] = new GRBVar * [n_nurses];
		for (w = 0; w < n_nurses; w++)
		{
			a[d][w] = new GRBVar [n_clusters];
			for (c = 0; c < n_clusters; c++)
			{
				if (rl)
				{
					string s = "a(" + to_string(d) + ',' + to_string(w) + "," + to_string(c) + ")";
					a[d][w][c] = model.addVar(0.0, 1.0, 0.0, GRB_CONTINUOUS, s);
				}
				else
				{
					string s = "a(" + to_string(d) + ',' + to_string(w) + "," + to_string(c) + ")";
					a[d][w][c] = model.addVar(0.0, 1.0, 0.0, GRB_BINARY, s);
				}
			}
		}
	}
	for (w = 0; w < n_nurses; w++)
	{
		z[w] = new GRBVar[n_jobs];
		for (j = 1; j < n_jobs; j++)
		{
			if (nurse[w].patient_s[j])
			{
				if (rl)
				{
					string s = "z(" + to_string(w) + "," + to_string(j) + ")";
					z[w][j] = model.addVar(0.0, 1.0, 0.0, GRB_CONTINUOUS, s);
				}
				else
				{
					string s = "z(" + to_string(w) + "," + to_string(j) + ")";
					z[w][j] = model.addVar(0.0, 1.0, 0.0, GRB_BINARY, s);
				}
			}
		}
	}
}

void Data::integer_vars_a(GRBModel& model, GRBVar** y)
{
	int i, j, w, d;

	for (d = 0; d < n_days; d++)
	{
		y[d] = new GRBVar [n_nurses];
		for (w = 0; w < n_nurses; w++)
		{
			if (rl)
			{
				string s = "y(" + to_string(d) + "," + to_string(w) + ")";
				y[d][w] = model.addVar(0.0, n_jobs, 0.0, GRB_CONTINUOUS, s);
			}
			else
			{
				string s = "y(" + to_string(d) + "," + to_string(w) + ")";
				y[d][w] = model.addVar(0.0, n_jobs, 0.0, GRB_INTEGER, s);
			}
		}
	}
}


void Data::constraints_patterns(GRBModel& model, GRBVar**** x)
{
	int p, dp;
	int d0, d1;
	int j, w, s;
	int s_p0;
	int s_p1;
	int s_w;

	for (p = 0; p < n_patterns; p++)
	{
		for (j = 1; j < n_jobs; j++)
		{
			if (pattern[p].patient_p[j])
			{
				for (dp = 0; dp < pattern[p].day_req.size() - 1; dp++)
				{
					d0 = pattern[p].day_req[dp];
					d1 = pattern[p].day_req[dp + 1];
					s_p0 = pattern[p].day_p[d0];
					s_p1 = pattern[p].day_p[d1];
					GRBLinExpr Expr = 0;
					for (w = 0; w < n_nurses; w++)
					{
						s_w = nurse[w].skill;
						if (s_w >= s_p0 && nurse[w].patient_s[j])
						{
							Expr += x[p][d0][w][j];
						}
					}
					for (w = 0; w < n_nurses; w++)
					{
						s_w = nurse[w].skill;
						if (s_w >= s_p1 && nurse[w].patient_s[j])
						{
							Expr -= x[p][d1][w][j];
						}
					}
					model.addConstr(Expr <= 0);
				}
				if (pattern[p].day_req.size() > 1)
				{
					d0 = pattern[p].day_req[pattern[p].day_req.size() - 1];
					d1 = pattern[p].day_req[0];
					s_p0 = pattern[p].day_p[d0];
					s_p1 = pattern[p].day_p[d1];
					GRBLinExpr Expr = 0;
					for (w = 0; w < n_nurses; w++)
					{
						s_w = nurse[w].skill;
						if (s_w >= s_p0 && nurse[w].patient_s[j])
						{
							Expr += x[p][d0][w][j];
						}
					}
					for (w = 0; w < n_nurses; w++)
					{
						s_w = nurse[w].skill;
						if (s_w >= s_p1 && nurse[w].patient_s[j])
						{
							Expr -= x[p][d1][w][j];
						}
					}
					model.addConstr(Expr <= 0);
				}
			}

		}
	}

}

void Data::constraints_skills(GRBModel& model, GRBVar**** x)
{
	int j, d, w, p, s;
	for (j = 1; j < n_jobs; j++)
	{
		for (s = 0; s < n_skills; s++)
		{
			if (job[j].skills[s] >= 1)
			{
				GRBLinExpr Expr = 0;
				for (d = 0; d < n_days; d++)
				{
					for (p = 0; p < n_patterns; p++)
					{
						if (pattern[p].patient_p[j] && pattern[p].day_p[d] == s + 1)
						{
							for (w = 0; w < n_nurses; w++)
							{
								if (nurse[w].skill >= s + 1 && nurse[w].patient_s[j])
								{
									Expr += x[p][d][w][j];
								}
							}
						}
					}
				}
				model.addConstr(Expr == job[j].skills[s]);
			}
		}
	}
}

void Data::constraints_skills2(GRBModel& model, GRBVar**** x)
{

	int j, p, w, d, s;
	int s_p;
	int s_w;
	int s_i0;
	int s_i1;
	int s_j0;
	int s_j1;
	for (j = 1; j < n_jobs; j++)
	{
		for (s = 0; s < n_skills; s++)
		{
			if (job[j].skills[s] >= 1)
			{
				GRBLinExpr Expr = 0;
				for (p = 0; p < n_patterns; p++)
				{
					for (d = 0; d < n_days; d++)
					{
						s_p = pattern[p].day_p[d];
						if (s_p >= 1 && pattern[p].patient_p[j] && pattern[p].day_p[d] == s + 1)
						{
							for (w = 0; w < n_nurses; w++)
							{
								s_w = nurse[w].skill;
								if (s_w >= s_p && nurse[w].patient_s[j])
								{
									if (s_w > s)
									{
										//cout << "x[" + to_string(p) + "][" + to_string(d) + "][" + to_string(w) + "][" + to_string(j) + "]" << endl;
										Expr += x[p][d][w][j];
									}
								}
							}
						}
					}
				}
				//cout << ">= " << job[j].skills[s] << endl;
				model.addConstr(Expr == job[j].skills[s]);
			}
		}
	}
}
void Data::constraints_maxday(GRBModel& model, GRBVar**** x)
{
	int j, d, w, p, s, s_p;
	for (j = 1; j < n_jobs; j++)
	{
		for (d = 0; d < n_days; d++)
		{
			GRBLinExpr Expr = 0;
			for (p = 0; p < n_patterns; p++)
			{
				s_p = pattern[p].day_p[d];
				if (pattern[p].patient_p[j] && s_p >= 1)
				{
					for (w = 0; w < n_nurses; w++)
					{
						if (nurse[w].skill >= s_p && nurse[w].patient_s[j])
						{
							Expr += x[p][d][w][j];
						}
					}
				}
			}
			model.addConstr(Expr <= 1);
		}
	}
}


void Data::constraints_assign(GRBModel& model, GRBVar**** x, GRBVar** z)
{
	int j, d, w, p, s;
	int need;
	for (j = 1; j < n_jobs; j++)
	{
		for (w = 0; w < n_nurses; w++)
		{
			GRBLinExpr Expr = 0;
			if (nurse[w].patient_s[j])
			{
				for (d = 0; d < n_days; d++)
				{
					for (p = 0; p < n_patterns; p++)
					{
						if (pattern[p].day_p[d] >= 1 && pattern[p].patient_p[j] && nurse[w].skill >= pattern[p].day_p[d])
						{
							Expr += x[p][d][w][j];
						}
					}
				}
			}
			need = 0;
			for (s = 0; s < n_skills; s++)
			{
				need += job[j].skills[s];
			}

			if (nurse[w].patient_s[j])
				model.addConstr(Expr <= need * z[w][j]);
		}
	}

	for (j = 1; j < n_jobs; j++)
	{
		GRBLinExpr Expr = 0;
		for (w = 0; w < n_nurses; w++)
		{
			if (nurse[w].patient_s[j])
				Expr += z[w][j];
		}

		if (flagTmax)
		{
			model.addConstr(Expr <= job[0].max_nurses);
		}
		else
		{
			model.addConstr(Expr <= job[j].max_nurses);
		}
	}

}

void Data::constraints_jobcount(GRBModel& model, GRBVar**** x, GRBVar** y) // relaciona variables x con y
{
	int j, d, w, p, s, c;
	int need;
	for (d = 0; d < n_days; d++)
	{
		for (w = 0; w < n_nurses; w++)
		{
			GRBLinExpr Expr = 0;
			for (p = 0; p < n_patterns; p++)
			{
				if (nurse[w].skill >= pattern[p].day_p[d] && pattern[p].day_p[d] >= 1)
				{
					for (j = 1; j < n_jobs; j++)
					{
						if (pattern[p].patient_p[j] && nurse[w].patient_s[j])
						{
							Expr += x[p][d][w][j];
							need++;
						}
					}
				}
			}
			model.addConstr(Expr <= y[d][w]);
		}
	}
}

void Data::constraints_clusters(GRBModel& model, GRBVar**** x, GRBVar*** a) // relaciona variables x con a
{
	int j, d, w, p, s, c;
	int need;
	for (d = 0; d < n_days; d++)
	{
		for (w = 0; w < n_nurses; w++)
		{
			for (c = 0; c < n_clusters; c++)
			{
				GRBLinExpr Expr = 0;
				need = 0;
				for (p = 0; p < n_patterns; p++)
				{
					if (nurse[w].skill >= pattern[p].day_p[d] && pattern[p].day_p[d] >= 1)
					{
						for (j = 1; j < n_jobs; j++)
						{
							if (pattern[p].patient_p[j] && nurse[w].patient_s[j] && job[j].cluster == c)
							{
								Expr += x[p][d][w][j];
								need++;
							}
						}
					}
				}
				model.addConstr(Expr <= need * a[d][w][c]);
				model.addConstr(a[d][w][c] <= Expr);
			}
		}
	}
}

void Data::constraints_softmaxdayworkload(GRBModel& model, GRBVar**** x,GRBVar*** a)
{
	int c1, c2, j, d, w, p, s, s_p;

	double min_intracluster_dist = 1E+30, min_intercluster_dist = 1E+30, min_job_time = 1E+30;
	for (c1 = 0; c1 < n_clusters; c1++)
	{
		min_intracluster_dist = min(min_intracluster_dist, distance[c1][c1]);
		for (c2 = c1 + 1; c2 < n_clusters; c2++)
		{
			min_intercluster_dist = min(min_intercluster_dist, distance[c1][c2]);
		}
	}
	for (j = 1; j < n_jobs; j++)
	{
		min_job_time = min(min_job_time, job[j].time);
	}

	GRBVar** leaves_cluster0_dummy = new GRBVar * [n_days];

	for (d = 0; d < n_days; d++)
	{
		leaves_cluster0_dummy[d] = new GRBVar[n_nurses];
		for (w = 0; w < n_nurses; w++)
		{
			string s = "leaves_cluster0(" + to_string(d) + "," + to_string(w) + ")";
			leaves_cluster0_dummy[d][w] = model.addVar(0.0, 1.0, 0.0, GRB_BINARY, s);

			GRBLinExpr n_jobs_assigned = 0;
			GRBLinExpr n_clusters_visited = 0;
			for (c1 = 1; c1 < n_clusters; c1++)
			{
				n_clusters_visited += a[d][w][c1];
			}
			// determine leave_cluster0_dummy
			model.addConstr(n_clusters_visited <= n_clusters * leaves_cluster0_dummy[d][w]);
			model.addConstr(n_clusters_visited >= leaves_cluster0_dummy[d][w]);

			for (j = 1; j < n_jobs; j++)
			{
				if (nurse[w].patient_s[j])
				{
					for (p = 0; p < n_patterns; p++)
					{
						s_p = pattern[p].day_p[d];
						if (s_p >= 1 && nurse[w].skill >= s_p && pattern[p].patient_p[j])
						{
							n_jobs_assigned += x[p][d][w][j];
						}
					}
				}
			}
			model.addConstr(n_jobs_assigned * min_job_time + (n_jobs_assigned + 1) * min_intracluster_dist + (n_clusters_visited + leaves_cluster0_dummy[d][w]) * (min_intercluster_dist - min_intracluster_dist) <= nurse[w].D);
		}
	}
}

void Data::constraints_hardmaxdayworkload(GRBModel& model, GRBVar**** x, GRBVar*** a)
{
	int c1, c2, j, d, w, p, s, s_p;

	double max_intracluster_dist = 0, max_intercluster_dist = 0, max_job_time = 0;
	for (c1 = 0; c1 < n_clusters; c1++)
	{
		max_intracluster_dist = max(max_intracluster_dist, distance[c1][c1]);
		for (c2 = c1 + 1; c2 < n_clusters; c2++)
		{
			max_intercluster_dist = max(max_intercluster_dist, distance[c1][c2]);
		}
	}
	for (j = 1; j < n_jobs; j++)
	{
		max_job_time = max(max_job_time, job[j].time);
	}

	GRBVar** leaves_cluster0_dummy = new GRBVar * [n_days];

	for (d = 0; d < n_days; d++)
	{
		leaves_cluster0_dummy[d] = new GRBVar[n_nurses];
		for (w = 0; w < n_nurses; w++)
		{
			string s = "leaves_cluster0(" + to_string(d) + "," + to_string(w) + ")";
			leaves_cluster0_dummy[d][w] = model.addVar(0.0, 1.0, 0.0, GRB_BINARY, s);

			GRBLinExpr n_jobs_assigned = 0;
			GRBLinExpr n_clusters_visited = 0;
			for (c1 = 1; c1 < n_clusters; c1++)
			{
				n_clusters_visited += a[d][w][c1];
			}
			// determine leave_cluster0_dummy
			model.addConstr(n_clusters_visited <= n_clusters * leaves_cluster0_dummy[d][w]);
			model.addConstr(n_clusters_visited >= leaves_cluster0_dummy[d][w]);

			for (j = 1; j < n_jobs; j++)
			{
				if (nurse[w].patient_s[j])
				{
					for (p = 0; p < n_patterns; p++)
					{
						s_p = pattern[p].day_p[d];
						if (s_p >= 1 && nurse[w].skill >= s_p && pattern[p].patient_p[j])
						{
							n_jobs_assigned += x[p][d][w][j];
						}
					}
				}
			}
			model.addConstr(n_jobs_assigned * max_job_time + (n_jobs_assigned + 1) * max_intracluster_dist + (n_clusters_visited + leaves_cluster0_dummy[d][w]) * (max_intercluster_dist - max_intracluster_dist) <= nurse[w].D);
		}
	}
}


void Data::constraints_maxjobdummy(GRBModel& model, GRBVar** y, GRBVar dummy)
{
	int d, w;
	GRBLinExpr Expr = 0;
	for (w = 0; w < n_nurses; w++)
	{
		Expr = 0;
		for (d = 0; d < n_days; d++)
		{
			Expr += y[d][w];
		}
		model.addConstr(Expr <= dummy);
	}
}
void Data::constraints_clusterdummy(GRBModel& model, GRBVar*** a, GRBVar dummy)
{
	int w, d, c;
	for (w = 0; w < n_nurses; w++)
	{
		GRBLinExpr Expr = 0;
		for (d = 0; d < n_days; d++)
		{
			for (c = 0; c < n_clusters; c++)
			{
				Expr += a[d][w][c];
			}
		}
		model.addConstr(Expr <= dummy);
	}
}
void Data::constraints_clusterdummy_c(GRBModel& model, GRBVar*** a, GRBVar dummy)
{
	int w, d, c;

	GRBLinExpr Expr = 0;
	for (w = 0; w < n_nurses; w++)
	{
		for (d = 0; d < n_days; d++)
		{
			for (c = 0; c < n_clusters; c++)
			{
				if(c!=0)
					Expr += a[d][w][c];
			}
		}
	}
	model.addConstr(Expr <= dummy);
}
  /////////////////////////////
 /////// RUTA SOLVE //////////
/////////////////////////////

void Data::binary_vars_r(GRBModel& model, GRBVar** y, int C)
{
	int  i, j;

	for (i = 0; i < C; i++)
	{
		y[i] = new GRBVar[C];
		for (j = 0; j < C; j++)
		{
			if(i != j)
			{
				string s = "y(" + to_string(i) + "," + to_string(j) + ")";
				y[i][j] = model.addVar(0.0, 1.0, 0.0, GRB_BINARY, s);
			}
		}
	}
}

void Data::linear_vars_r(GRBModel& model, GRBVar* u, int C)
{
	int  i, j;

	for (i = 0; i < C; i++)
	{
		string s = "u(" + to_string(i) + ")";
		u[i]= model.addVar(0.0, C, 0.0, GRB_CONTINUOUS, s);
	}
	model.addConstr(u[0] == 0);
}

void Data::constraints_nurseday_r(GRBModel& model, GRBVar** y, GRBVar D, vector<int> cluster_list)
{
	int i, j;
	int C = cluster_list.size();
	GRBLinExpr Expr = 0;
	for( i = 0 ; i < C ; i++)
	{
		for (j = 0; j < C; j++)
		{
			if (i != j)
			{
				Expr += y[i][j] * distance[cluster_list[i]][cluster_list[j]];
			}
		}
	}
	Expr -= D;
	model.addConstr(Expr == 0);
}

void Data::constraints_patterns_r(GRBModel& model, GRBVar** y, int C)
{
	int i, j;
	cout << C << endl;

	for (i = 0; i < C; i++)
	{
		GRBLinExpr Expr1 = 0;
		GRBLinExpr Expr2 = 0;

		for (j = 0; j < C; j++)
		{
			if(i != j)
			{
				Expr1 += y[i][j];
				//cout << "+ x[" << i << "][" << j << "]                                    ";
				Expr2 += y[j][i];
				//cout << "+ x2[" << j << "][" << i << "]" << endl;
			}
		}
		model.addConstr(Expr1 == 1);
		model.addConstr(Expr2 == 1);
		//cout << "         = 1                                       = 1" << endl;
	}
}
void Data::constraints_MTZ_r(GRBModel& model, GRBVar** y, GRBVar* u, int C)
{
	int i, j;
	GRBLinExpr Expr;
	for (i = 0; i < C; i++)
	{
		for (j = 1; j < C; j++)
		{
			if (i != j)
			{
				Expr = u[i] - u[j] + 1;
				model.addConstr(Expr <= C * (1 - y[i][j]));
				//cout << "u[" << i << "] - u[" << j << "] + 1 <= " << C << "(1 - x[" << i << "][" << j << "])" <<endl;
			}
		}
	}

}