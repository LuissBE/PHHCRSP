#include "Data.h"

void Data::binary_vars_n(GRBModel &model, GRBVar***** x, GRBVar** z)
{
	int i, j, p, w, d;
	int s_p;
	int s_w;
	int s_i0;
	int s_i1;
	int s_j0;
	int s_j1;

	for (p = 0; p < n_patterns; p++)
	{
		x[p] = new GRBVar***[n_days];
		for (d = 0; d < n_days; d++)
		{
			s_p = pattern[p].day_p[d]; 
			if (s_p >= 1) 
			{
				x[p][d] = new GRBVar**[n_nurses];
				for (w = 0; w < n_nurses; w++)
				{
					s_w = nurse[w].skill;
					if (s_w >= s_p) 
					{
						x[p][d][w] = new GRBVar*[n_jobs];
						for (i = 0; i < n_jobs; i++)
						{
							if (nurse[w].patient_s[i]) 
							{
								x[p][d][w][i] = new GRBVar[n_jobs];
								for (j = 0; j < n_jobs; j++)
								{
									if (i == 0 || j != i)
									{
										if (pattern[p].patient_p[j]) 
										{
											if (rl)
											{
												string s = "x(" + to_string(p) + "," + to_string(d) + "," + to_string(w) + "," + to_string(i) + "," + to_string(j) + ")";
												x[p][d][w][i][j] = model.addVar(0.0, 1.0, 0.0, GRB_CONTINUOUS, s);
											}
											else
											{
												string s = "x(" + to_string(p) + "," + to_string(d) + "," + to_string(w) + "," + to_string(i) + "," + to_string(j) + ")";
												x[p][d][w][i][j] = model.addVar(0.0, 1.0, 0.0, GRB_BINARY, s);
											}
										}
									}
								}
							}
						}
					}
				}
			}
		}
	}

	for (w = 0; w < n_nurses; w++)
	{
		z[w] = new GRBVar[n_jobs];
		for (j = 0; j < n_jobs; j++)
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

void Data::integer_vars_n(GRBModel &model, GRBVar**** y)
{
	int i, j, w, d;

	for (d = 0; d < n_days; d++)
	{
		y[d] = new GRBVar**[n_nurses];
		for (w = 0; w < n_nurses; w++)
		{
			y[d][w] = new GRBVar*[n_jobs];
			for (i = 0; i < n_jobs; i++)
			{
				if (nurse[w].patient_s[i]) 
				{
					y[d][w][i] = new GRBVar[n_jobs];
					for (j = 0; j < n_jobs; j++)
					{
						if (nurse[w].patient_s[j]) 
						{
							if (rl)
							{
								string s = "y(" + to_string(d) + "," + to_string(w) + "," + to_string(i) + "," + to_string(j) + ")";
								y[d][w][i][j] = model.addVar(0.0, n_jobs, 0.0, GRB_CONTINUOUS, s);
							}
							else
							{
								string s = "y(" + to_string(d) + "," + to_string(w) + "," + to_string(i) + "," + to_string(j) + ")";
								y[d][w][i][j] = model.addVar(0.0, n_jobs, 0.0, GRB_INTEGER, s);
							}
						}
					}
				}
			}
		}
	}
}
void Data::linear_vars_n(GRBModel &model, GRBVar** u, GRBVar** D)
{
	int  w, i, d;

	for (w = 0; w < n_nurses; w++)
	{
		D[w] = new GRBVar[n_days];
		for (d = 0; d < n_days; d++)
		{
			string s = "D(" + to_string(w) + "," + to_string(d) + ")";
			D[w][d] = model.addVar(0.0, nurse[w].D, 0.0, GRB_CONTINUOUS, s);
		}
	}

	for (d = 0; d < n_days; d++)
	{
		u[d] = new GRBVar[n_jobs];
		for (i = 0; i < n_jobs; i++)
		{
			string s = "u(" + to_string(d) + "," + to_string(i) + ")";
			u[d][i] = model.addVar(0.0, max_D, 0.0, GRB_CONTINUOUS, s);
		}
	}
}

void Data::linear_vars_n3(GRBModel &model, GRBVar** u, GRBVar* D)
{
	int  w, i, d;

	for (w = 0; w < n_nurses; w++)
	{
		string s = "D(" + to_string(w) + ")";
		D[w] = model.addVar(0.0, nurse[w].D*n_days, 0.0, GRB_CONTINUOUS, s);
	}

	for (d = 0; d < n_days; d++)
	{
		u[d] = new GRBVar[n_jobs];
		for (i = 0; i < n_jobs; i++)
		{
			string s = "u(" + to_string(d) + "," + to_string(i) + ")";
			u[d][i] = model.addVar(0.0, max_D, 0.0, GRB_CONTINUOUS, s);
		}
	}
}
void Data::linear_vars_n37(GRBModel &model, GRBVar** u, GRBVar* D)
{
	int  w, i, d;

	for (w = 0; w < n_nurses; w++)
	{
		string s = "D(" + to_string(w) + ")";
		D[w] = model.addVar(0.0, nurse[w].D*n_days, 0.0, GRB_CONTINUOUS, s);
	}

	for (d = 0; d < n_days; d++)
	{
		u[d] = new GRBVar[n_jobs];
		for (i = 0; i < n_jobs; i++)
		{
			string s = "u(" + to_string(d) + "," + to_string(i) + ")";
			u[d][i] = model.addVar(0.0, GRB_INFINITY, 0.0, GRB_CONTINUOUS, s);
		}
	}
}

void Data::linear_vars_n35(GRBModel &model, GRBVar** u, GRBVar* D)
{
	int  w, i, d;

	for (w = 0; w < n_nurses; w++)
	{
		string s = "D(" + to_string(w) + ")";
		D[w] = model.addVar(0.0, nurse[w].D*n_days, 0.0, GRB_CONTINUOUS, s);
	}

	for (d = 0; d < n_days; d++)
	{
		u[d] = new GRBVar[n_jobs];
		for (i = 0; i < n_jobs; i++)
		{
			string s = "u(" + to_string(d) + "," + to_string(i) + ")";
			u[d][i] = model.addVar(0.0, GRB_INFINITY, 0.0, GRB_CONTINUOUS, s);
		}
	}
}

void Data::linear_vars_n34(GRBModel &model, GRBVar** u, GRBVar* D, GRBVar** g)
{
	int  w, i, d;

	for (w = 0; w < n_nurses; w++)
	{
		string s = "D(" + to_string(w) + ")";
		D[w] = model.addVar(0.0, nurse[w].D*n_days, 0.0, GRB_CONTINUOUS, s);
	}

	for (w = 0; w < n_nurses; w++)
	{
		g[w] = new GRBVar[n_days];
		for (d = 0; d < n_days; d++)
		{
			string s = "g(" + to_string(w) + "," + to_string(d) + ")";
			g[w][d] = model.addVar(0.0, nurse[w].D, 0.0, GRB_CONTINUOUS, s);
		}
	}

	for (d = 0; d < n_days; d++)
	{
		u[d] = new GRBVar[n_jobs];
		for (i = 0; i < n_jobs; i++)
		{
			string s = "u(" + to_string(d) + "," + to_string(i) + ")";
			u[d][i] = model.addVar(0.0, max_D, 0.0, GRB_CONTINUOUS, s);
		}
	}
}

void Data::linear_vars_n5(GRBModel &model, GRBVar*** u)
{
	int  w, i, d;

	for (d = 0; d < n_days; d++)
	{
		u[d] = new GRBVar*[n_nurses];
		for (w = 0; w < n_nurses; w++)
		{
			u[d][w] = new GRBVar[n_jobs];
			for (i = 0; i < n_jobs; i++)
			{
				string s = "u(" + to_string(d) + "," + to_string(w) + "," + to_string(i) + ")";
				u[d][w][i] = model.addVar(0.0, nurse[w].D, 0.0, GRB_CONTINUOUS, s);
			}
		}
	}
}
void Data::linear_vars_n4(GRBModel &model, GRBVar** u)
{
	int  w, i, d;

	for (d = 0; d < n_days; d++)
	{
		u[d] = new GRBVar[n_jobs];
		for (i = 0; i < n_jobs; i++)
		{
			string s = "u(" + to_string(d) + "," + to_string(i) + ")";
			u[d][i] = model.addVar(0.0, max_D, 0.0, GRB_CONTINUOUS, s);
		}
	}
}


void Data::constraints_patterns2(GRBModel &model, GRBVar***** x)
{
	int p, dp;
	int d0, d1;
	int i, j, w, s;
	int s_p0;
	int s_p1;
	int s_w;

	for (p = 0; p < n_patterns; p++)
	{
		for (j = 0; j < n_jobs; j++)
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
						if (s_w >= s_p0)
						{
							for (i = 0; i < n_jobs; i++)
							{
								if (i != j && nurse[w].patient_s[i])
								{
									Expr += x[p][d0][w][i][j];
								}
							}
						}
					}
					for (w = 0; w < n_nurses; w++)
					{
						s_w = nurse[w].skill;
						if (s_w >= s_p1)
						{
							for (i = 0; i < n_jobs; i++)
							{
								if (i != j && nurse[w].patient_s[i])
								{
									Expr -= x[p][d1][w][i][j];
								}
							}
						}
					}
					model.addConstr(Expr == 0);
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
						if (s_w >= s_p0)
						{
							for (i = 0; i < n_jobs; i++)
							{
								if (i != j && nurse[w].patient_s[i])
								{
									Expr += x[p][d0][w][i][j];
								}
							}
						}
					}
					for (w = 0; w < n_nurses; w++)
					{
						s_w = nurse[w].skill;
						if (s_w >= s_p1)
						{
							for (i = 0; i < n_jobs; i++)
							{
								if (i != j && nurse[w].patient_s[i])
								{
									Expr -= x[p][d1][w][i][j];
								}
							}
						}
					}
					model.addConstr(Expr == 0);
				}
			}

		}
	}

}
void Data::constraints_patterns(GRBModel &model, GRBVar***** x)
{
	int p, dp;
	int d0, d1;
	int i, j, w, s;
	int s_p0;
	int s_p1;
	int s_w;

	for (p = 0; p < n_patterns; p++)
	{
		for (j = 0; j < n_jobs; j++)
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
						if (s_w >= s_p0)
						{
							for (i = 0; i < n_jobs; i++)
							{
								if (i != j && nurse[w].patient_s[i])
								{
									Expr += x[p][d0][w][i][j];
								}
							}
						}
					}
					for (w = 0; w < n_nurses; w++)
					{
						s_w = nurse[w].skill;
						if (s_w >= s_p1)
						{
							for (i = 0; i < n_jobs; i++)
							{
								if (i != j && nurse[w].patient_s[i])
								{
									Expr -= x[p][d1][w][i][j];
								}
							}
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
						if (s_w >= s_p0)
						{
							for (i = 0; i < n_jobs; i++)
							{
								if (i != j && nurse[w].patient_s[i])
								{
									Expr += x[p][d0][w][i][j];
								}
							}
						}
					}
					for (w = 0; w < n_nurses; w++)
					{
						s_w = nurse[w].skill;
						if (s_w >= s_p1)
						{
							for (i = 0; i < n_jobs; i++)
							{
								if (i != j && nurse[w].patient_s[i])
								{
									Expr -= x[p][d1][w][i][j];
								}
							}
						}
					}
					model.addConstr(Expr <= 0);
				}
			}

		}
	}

}

void Data::constraints_clusters_x_debil(GRBModel &model, GRBVar***** x)
{
	int i, j, d, w, p, s, c, s_w;


	for (w = 0; w < n_nurses; w++)
	{
		s_w = nurse[w].skill;
		for (d = 0; d < n_days; d++)
		{
			for (c = 0; c < n_clusters; c++)
			{
				GRBLinExpr Expr = 0;

				for (p = 0; p < n_patterns; p++)
				{
					if (pattern[p].day_p[d] >= 1 && pattern[p].day_p[d] <= s_w) 
					{
						for (j = 1; j < n_jobs; j++)
						{
							if (pattern[p].patient_p[j] && job[j].cluster == c)
							{
								for (i = j + 1; i < n_jobs; i++)
								{
									if (nurse[w].patient_s[i] && job[i].cluster == c)
										Expr += x[p][d][w][i][j];
								}
							}

						}
					}
				}

				model.addConstr(Expr == 0);
			}
		}
	}

}

void Data::constraints_clusters_x2(GRBModel &model, GRBVar***** x)
{
	int i, j, d, w, p, s, c, s_w;



	for (w = 0; w < n_nurses; w++)
	{
		s_w = nurse[w].skill;
		for (d = 0; d < n_days; d++)
		{
			for (c = 0; c < n_clusters; c++)
			{
				GRBLinExpr Expr = 0;

				for (p = 0; p < n_patterns; p++)
				{
					if (pattern[p].day_p[d] >= 1 && pattern[p].day_p[d] <= s_w) 
					{
						for (j = 1; j < n_jobs; j++)
						{
							if (pattern[p].patient_p[j] && job[j].cluster == c)
							{
								for (i = j + 1; i < n_jobs; i++)
								{
									if (nurse[w].patient_s[i] && job[i].cluster == c)
										Expr += x[p][d][w][i][j];
								}
							}

						}
					}
				}

				model.addConstr(Expr == 0);
			}
		}
	}



	for (w = 0; w < n_nurses; w++)
	{
		s_w = nurse[w].skill;
		for (d = 0; d < n_days; d++)
		{
			for (c = 0; c < n_clusters; c++)
			{
				GRBLinExpr Expr = 0;

				for (p = 0; p < n_patterns; p++)
				{
					if (pattern[p].day_p[d] >= 1 && pattern[p].day_p[d] <= s_w) 
					{
						for (j = 1; j < n_jobs; j++)
						{
							if (pattern[p].patient_p[j] && job[j].cluster == c)
							{
								for (i = 0; i < n_jobs; i++)
								{
									if (nurse[w].patient_s[i] && job[i].cluster != c)
										Expr += x[p][d][w][i][j];
								}
							}

						}
					}
				}

				model.addConstr(Expr <= 1);
			}
		}
	}


	for (w = 0; w < n_nurses; w++)
	{
		s_w = nurse[w].skill;
		for (d = 0; d < n_days; d++)
		{
			for (c = 0; c < n_clusters; c++)
			{
				GRBLinExpr Expr = 0;

				for (p = 0; p < n_patterns; p++)
				{
					if (pattern[p].day_p[d] >= 1 && pattern[p].day_p[d] <= s_w) 
					{
						for (j = 1; j < n_jobs; j++)
						{
							if (nurse[w].patient_s[j] && job[j].cluster == c)
							{
								for (i = 0; i < n_jobs; i++)
								{
									if (pattern[p].patient_p[i] && job[i].cluster != c)
										Expr += x[p][d][w][j][i];
								}
							}

						}
					}
				}

				model.addConstr(Expr <= 1);
			}
		}
	}
}
void Data::constraints_clusters_x1(GRBModel &model, GRBVar***** x)
{
	int i, j, d, w, p, s, c, s_w;

	int inicio = 1;

	for (w = 0; w < n_nurses; w++)
	{
		s_w = nurse[w].skill;
		for (d = 0; d < n_days; d++)
		{
			for (c = 0; c < n_clusters; c++)
			{
				GRBLinExpr Expr = 0;

				for (p = 0; p < n_patterns; p++)
				{
					if (pattern[p].day_p[d] >= 1 && pattern[p].day_p[d] <= s_w)
					{
						if (version == 4)
							inicio = 0;
						for (j = inicio; j < n_jobs; j++)
						{
							if (pattern[p].patient_p[j] && job[j].cluster == c)
							{
								for (i = 0; i < n_jobs; i++)
								{
									if (nurse[w].patient_s[i] && job[i].cluster != c)
										Expr += x[p][d][w][i][j];
								}
							}

						}
					}
				}

				model.addConstr(Expr <= 1);
			}
		}
	}
}

void Data::constraints_clusters_x(GRBModel &model, GRBVar***** x)
{
	int i, j, d, w, p, s, c, s_w;


	if (version != -1)
	{
		for (w = 0; w < n_nurses; w++)
		{
			s_w = nurse[w].skill;
			for (d = 0; d < n_days; d++)
			{
				for (c = 0; c < n_clusters; c++)
				{
					GRBLinExpr Expr = 0;

					for (p = 0; p < n_patterns; p++)
					{
						if (pattern[p].day_p[d] >= 1 && pattern[p].day_p[d] <= s_w) 
						{
							for (j = 1; j < n_jobs; j++)
							{
								if (pattern[p].patient_p[j] && job[j].cluster == c)
								{
									for (i = j + 1; i < n_jobs; i++)
									{
										if (nurse[w].patient_s[i] && job[i].cluster == c)
											Expr += x[p][d][w][i][j];
									}
								}

							}
						}
					}

					model.addConstr(Expr == 0);
				}
			}
		}
	}
}

void Data::constraints_paridad(GRBModel &model, GRBVar***** x)
{
	int i, j, d, w, p, s, c, s_w;

	for (w = 0; w < n_nurses; w++)
	{
		s_w = nurse[w].skill;
		for (d = 0; d < n_days; d++)
		{
			for (c = 0; c < n_clusters; c++)
			{
				GRBLinExpr Expr = 0;

				for (p = 0; p < n_patterns; p++)
				{
					if (pattern[p].day_p[d] >= 1 && pattern[p].day_p[d] <= s_w) 
					{
						for (j = 1; j < n_jobs; j++)
						{
							if (pattern[p].patient_p[j] && job[j].cluster == c)
							{
								for (i = 0; i < n_jobs; i++)
								{
									if (nurse[w].patient_s[i] && job[i].cluster != c)
										Expr += x[p][d][w][i][j];
								}
							}

						}
					}
				}
				for (p = 0; p < n_patterns; p++)
				{
					if (pattern[p].day_p[d] >= 1 && pattern[p].day_p[d] <= s_w) 
					{
						for (j = 1; j < n_jobs; j++)
						{
							if (nurse[w].patient_s[j] && job[j].cluster == c)
							{
								for (i = 0; i < n_jobs; i++)
								{
									if (pattern[p].patient_p[i] && job[i].cluster != c)
										Expr -= x[p][d][w][j][i];
								}
							}

						}
					}
				}
				model.addConstr(Expr == 0);
			}
		}
	}
}
void Data::constraints_skills(GRBModel &model, GRBVar***** x)
{
	int i, j, d, w, p, s;
	for (j = 1; j < n_jobs; j++)
	{
		for (s = 0; s < n_skills; s++)
		{
			if (job[j].skills[s] >= 1)
			{
				GRBLinExpr Expr = 0;
				for (i = 0; i < n_jobs; i++)
				{
					if (i != j)
					{
						for (d = 0; d < n_days; d++)
						{
							for (p = 0; p < n_patterns; p++)
							{
								if (pattern[p].patient_p[j] && pattern[p].day_p[d] == s + 1)
								{
									for (w = 0; w < n_nurses; w++)
									{
										if (nurse[w].skill >= s + 1 && nurse[w].patient_s[i])
										{
											Expr += x[p][d][w][i][j];
										}
									}
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
void Data::constraints_maxday(GRBModel &model, GRBVar***** x)
{
	int i, j, d, w, p, s, s_p;
	for (j = 1; j < n_jobs; j++)
	{
		for (d = 0; d < n_days; d++)
		{
			GRBLinExpr Expr = 0;

			for (i = 0; i < n_jobs; i++)
			{
				if (i != j)
				{
					for (p = 0; p < n_patterns; p++)
					{
						s_p = pattern[p].day_p[d];
						if (pattern[p].patient_p[j] && s_p >= 1)
						{
							for (w = 0; w < n_nurses; w++)
							{
								if (nurse[w].skill >= s_p && nurse[w].patient_s[i])
								{
									Expr += x[p][d][w][i][j];
								}
							}
						}
					}
				}
			}
			model.addConstr(Expr <= 1);
		}
	}
}
void Data::constraints_symmetry(GRBModel &model, GRBVar***** x)
{
	int i, j, d, w, p, s, s_p;

	for (j = 1; j < n_jobs; j++)
	{
		for (d = 0; d < n_days; d++)
		{
			for (w = 0; w < n_nurses; w++)
			{
				GRBLinExpr Expr = 0;
				for (p = 0; p < n_patterns; p++)
				{
					s_p = pattern[p].day_p[d];
					if (s_p >= 1 && nurse[w].skill >= s_p)
					{
						if (pattern[p].patient_p[j])
						{
							for (i = 0; i < n_jobs; i++)
							{
								if (i != j && nurse[w].patient_s[i])
									Expr += x[p][d][w][i][j];
							}
						}
						if (nurse[w].patient_s[j])
						{
							for (i = 0; i < n_jobs; i++)
							{
								if (i != j && pattern[p].patient_p[i])
									Expr -= x[p][d][w][j][i];
							}
						}

					}
				}
				model.addConstr(Expr == 0);
			}
		}
	}

}

void Data::constraints_symmetry0(GRBModel &model, GRBVar***** x)
{
	int i, j, d, w, p, s, s_p;

	for (w = 0; w < n_nurses; w++)
	{
		for (d = 0; d < n_days; d++)
		{
			GRBLinExpr Expr = 0;
			i = 0;
			if (nurse[w].patient_s[i])
			{
				for (j = 0; j < n_jobs; j++)
				{
					if (i != j)
					{
						for (p = 0; p < n_patterns; p++)
						{
							if (pattern[p].day_p[d] >= 1 && pattern[p].patient_p[j] && nurse[w].skill >= pattern[p].day_p[d])
							{
								Expr += x[p][d][w][i][j];
							}
						}
					}
				}
			}
			model.addConstr(Expr <= 1);
		}
	}
}
void Data::constraints_nursesday(GRBModel &model, GRBVar***** x, GRBVar** D)
{
	int i, j, d, w, p, s;
	int num;
	double cost;

	for (w = 0; w < n_nurses; w++)
	{
		for (d = 0; d < n_days; d++)
		{
			GRBLinExpr Expr = 0;

			for (i = 0; i < n_jobs; i++)
			{
				if (nurse[w].patient_s[i])
				{
					for (j = 0; j < n_jobs; j++)
					{
						if (i != j)
						{
							for (p = 0; p < n_patterns; p++)
							{
								if (pattern[p].day_p[d] >= 1 && pattern[p].patient_p[j] && nurse[w].skill >= pattern[p].day_p[d])
								{
									cost = 0;
									if (j != 0)
										cost = cost + job[j].time;
									cost = cost + distance[job[i].cluster][job[j].cluster];
									Expr += cost * x[p][d][w][i][j];
								}
							}
						}
					}
				}
			}

			Expr -= D[w][d];
			model.addConstr(Expr == 0);
		}
	}
}


void Data::constraints_nursesday3(GRBModel &model, GRBVar***** x, GRBVar* D)
{
	int i, j, d, w, p, s;
	int num;
	double cost;

	for (w = 0; w < n_nurses; w++)
	{
		GRBLinExpr Expr = 0;
		for (d = 0; d < n_days; d++)
		{

			for (i = 0; i < n_jobs; i++)
			{
				if (nurse[w].patient_s[i])
				{
					for (j = 0; j < n_jobs; j++)
					{
						if (i != j)
						{
							for (p = 0; p < n_patterns; p++)
							{
								if (pattern[p].day_p[d] >= 1 && pattern[p].patient_p[j] && nurse[w].skill >= pattern[p].day_p[d])
								{
									cost = 0;
									if (j != 0)
										cost = cost + job[j].time;
									cost = cost + distance[job[i].cluster][job[j].cluster];
									Expr += cost * x[p][d][w][i][j];
								}
							}
						}
					}
				}
			}

		}

		Expr -= D[w];
		model.addConstr(Expr == 0);
	}
}

void Data::constraints_maxwork(GRBModel &model, GRBVar***** x)
{
	int i, j, d, w, p, s;
	int num;
	double cost;

	for (w = 0; w < n_nurses; w++)
	{

		for (d = 0; d < n_days; d++)
		{
			GRBLinExpr Expr = 0;
			for (i = 0; i < n_jobs; i++)
			{
				if (nurse[w].patient_s[i])
				{
					for (j = 0; j < n_jobs; j++)
					{
						if (i != j)
						{
							for (p = 0; p < n_patterns; p++)
							{
								if (pattern[p].day_p[d] >= 1 && pattern[p].patient_p[j] && nurse[w].skill >= pattern[p].day_p[d])
								{
									cost = 0;
									if (j != 0)
										cost = cost + job[j].time;
									cost = cost + distance[job[i].cluster][job[j].cluster];
									Expr += cost * x[p][d][w][i][j];
								}
							}
						}
					}
				}
			}
			model.addConstr(Expr <= nurse[w].D);
		}

	}
}


void Data::constraints_nursesday34(GRBModel &model, GRBVar***** x, GRBVar* D, GRBVar** g)
{
	int i, j, d, w, p, s;
	int num;
	double cost;
	for (w = 0; w < n_nurses; w++)
	{

		for (d = 0; d < n_days; d++)
		{
			GRBLinExpr Expr = 0;
			for (i = 0; i < n_jobs; i++)
			{
				if (nurse[w].patient_s[i])
				{
					for (j = 0; j < n_jobs; j++)
					{
						if (i != j)
						{
							for (p = 0; p < n_patterns; p++)
							{
								if (pattern[p].day_p[d] >= 1 && pattern[p].patient_p[j] && nurse[w].skill >= pattern[p].day_p[d])
								{
									cost = 0;
									if (j != 0)
										cost = cost + job[j].time;
									cost = cost + distance[job[i].cluster][job[j].cluster];
									Expr += cost * x[p][d][w][i][j];
								}
							}
						}
					}
				}
			}
			Expr -= g[w][d];
			model.addConstr(Expr == 0);
		}

	}

	for (w = 0; w < n_nurses; w++)
	{

		GRBLinExpr Expr = 0;
		for (d = 0; d < n_days; d++)
		{
			Expr += g[w][d];
		}
		Expr -= D[w];
		model.addConstr(Expr == 0);

	}
}

void Data::constraints_assign(GRBModel &model, GRBVar***** x, GRBVar** z)
{
	int i, j, d, w, p, s;
	int need;
	for (j = 1; j < n_jobs; j++)
	{
		for (w = 0; w < n_nurses; w++)
		{
			GRBLinExpr Expr = 0;
			for (i = 0; i < n_jobs; i++)
			{
				if (i != j)
				{
					if (nurse[w].patient_s[i])
					{
						for (d = 0; d < n_days; d++)
						{
							for (p = 0; p < n_patterns; p++)
							{
								if (pattern[p].day_p[d] >= 1 && pattern[p].patient_p[j] && nurse[w].skill >= pattern[p].day_p[d])
								{
									Expr += x[p][d][w][i][j];
								}
							}
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

void Data::constraints_assign5(GRBModel &model, GRBVar*** u, GRBVar** z)
{
	int i, j, d, w, p, s;
	int need;
	for (j = 1; j < n_jobs; j++)
	{
		for (w = 0; w < n_nurses; w++)
		{
			if (nurse[w].patient_s[j])
			{
				GRBLinExpr Expr = 0;
				for (d = 0; d < n_days; d++)
				{
					Expr += u[d][w][j];
				}

				need = nurse[w].D*n_days;
				model.addConstr(Expr <= need * z[w][j]);
			}
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
		model.addConstr(Expr <= job[j].max_nurses);
	}

}
void Data::constraints_assign_ad(GRBModel &model, GRBVar***** x, GRBVar** z)
{
	int i, j, d, w, p, s;

	for (j = 1; j < n_jobs; j++)
	{
		GRBLinExpr Expr = 0;
		for (w = 0; w < n_nurses; w++)
		{
			if (nurse[w].patient_s[j])
				Expr += z[w][j];

			for (i = 0; i < n_jobs; i++)
			{
				if (i != j)
				{

					if (nurse[w].patient_s[i])
					{
						for (d = 0; d < n_days; d++)
						{
							for (p = 0; p < n_patterns; p++)
							{
								if (pattern[p].day_p[d] >= 1 && nurse[w].skill >= pattern[p].day_p[d] && pattern[p].patient_p[j])
								{
									Expr -= x[p][d][w][i][j];
								}
							}
						}
					}
				}

			}
		}
		model.addConstr(Expr <= 0);
	}
}

void Data::constraints_assign_ad2(GRBModel &model, GRBVar***** x, GRBVar** z, GRBVar* D)
{
	double num;
	int i, j, d, w, p, s;
	for (w = 0; w < n_nurses; w++)
	{
		GRBLinExpr Expr = 0;
		for (j = 1; j < n_jobs; j++)
		{
			num = (job[j].time + distance[job[j].cluster][job[j].cluster]);
			if (nurse[w].patient_s[j])
				Expr += num * z[w][j];
		}
		model.addConstr(Expr <= D[w]);
	}
}
void Data::constraints_flow(GRBModel &model, GRBVar***** x, GRBVar** u)
{
	int i, j, d, w, p, s;

	double num;
	for (i = 0; i < n_jobs; i++)
	{
		for (j = 0; j < n_jobs; j++)
		{
			if (i != j)
			{
				for (d = 0; d < n_days; d++)
				{
					GRBLinExpr Expr = 0;

					if (i > 0)
						Expr += u[d][i];

					num = (job[i].time + distance[job[i].cluster][job[j].cluster]);
					for (w = 0; w < n_nurses; w++)
					{
						if (nurse[w].patient_s[i])
						{
							for (p = 0; p < n_patterns; p++)
							{
								if (pattern[p].patient_p[j] && pattern[p].day_p[d] >= 1 && nurse[w].skill >= pattern[p].day_p[d])
									Expr += num * x[p][d][w][i][j];
							}
						}
					}
					Expr -= u[d][j];
					for (w = 0; w < n_nurses; w++)
					{
						if (nurse[w].patient_s[i])
						{
							for (p = 0; p < n_patterns; p++)
							{
								if (pattern[p].patient_p[j] && pattern[p].day_p[d] >= 1 && nurse[w].skill >= pattern[p].day_p[d])
									Expr += max_D * x[p][d][w][i][j];
							}
						}
					}
					model.addConstr(Expr <= max_D);
				}
			}
		}
	}
}

void Data::constraints_relate_flow(GRBModel &model, GRBVar***** x, GRBVar** u)
{
	int i, j, d, w, p, s;

	double num;
	for (d = 0; d < n_days; d++)
	{
		for (j = 1; j < n_jobs; j++)
		{
			GRBLinExpr Expr = 0;
			Expr += u[d][j];
			for (i = 0; i < n_jobs; i++)
			{
				if (i != j)
				{
					num = distance[job[j].cluster][job[0].cluster];
					for (w = 0; w < n_nurses; w++)
					{
						if (nurse[w].patient_s[i])
						{
							for (p = 0; p < n_patterns; p++)
							{
								if (pattern[p].patient_p[j] && pattern[p].day_p[d] >= 1 && nurse[w].skill >= pattern[p].day_p[d])
								{
									Expr -= (nurse[w].D-num) * x[p][d][w][i][j];
								}
							}
						}
					}
				}
			}
			model.addConstr(Expr <= 0);
		}
	}
}
void Data::constraints_flow_n3(GRBModel &model, GRBVar***** x, GRBVar** u)
{
	int i, j, d, w, p, s;

	double num;
	for (i = 0; i < n_jobs; i++)
	{
		for (j = 0; j < n_jobs; j++)
		{
			if (i != j)
			{
				for (d = 0; d < n_days; d++)
				{
					GRBLinExpr Expr = 0;

					if (i > 0)
						Expr += u[d][i];

					num = (job[i].time + distance[job[i].cluster][job[j].cluster]);
					for (w = 0; w < n_nurses; w++)
					{
						if (nurse[w].patient_s[i])
						{
							for (p = 0; p < n_patterns; p++)
							{
								if (pattern[p].patient_p[j] && pattern[p].day_p[d] >= 1 && nurse[w].skill >= pattern[p].day_p[d])
								{
									Expr += num * x[p][d][w][i][j];
									if (j == 0)
									{
										Expr -= (nurse[w].D)*x[p][d][w][i][j];
									}
								}
							}
						}
					}
					if (j > 0)
						Expr -= u[d][j];

					for (w = 0; w < n_nurses; w++)
					{
						if (nurse[w].patient_s[i])
						{
							for (p = 0; p < n_patterns; p++)
							{
								if (pattern[p].patient_p[j] && pattern[p].day_p[d] >= 1 && nurse[w].skill >= pattern[p].day_p[d])
									Expr += max_D * x[p][d][w][i][j];
							}
						}
					}
					model.addConstr(Expr <= max_D);
				}
			}
		}
	}
}

void Data::constraints_flow_n36(GRBModel &model, GRBVar***** x, GRBVar** u)
{
	int i, j, d, w, p, s;

	double num;
	for (i = 0; i < n_jobs; i++)
	{
		for (j = 0; j < n_jobs; j++)
		{
			if (i != j)
			{
				for (d = 0; d < n_days; d++)
				{
					GRBLinExpr Expr = 0;

					if (i > 0)
						Expr += u[d][i];

					num = (job[i].time + distance[job[i].cluster][job[j].cluster]);
					for (w = 0; w < n_nurses; w++)
					{
						if (nurse[w].patient_s[i])
						{
							for (p = 0; p < n_patterns; p++)
							{
								if (pattern[p].patient_p[j] && pattern[p].day_p[d] >= 1 && nurse[w].skill >= pattern[p].day_p[d])
								{
									Expr += num * x[p][d][w][i][j];
									if (j == 0)
									{
										Expr -= (nurse[w].D)*x[p][d][w][i][j];
									}
								}
							}
						}
					}
					if (j > 0)
						Expr -= u[d][j];

					if (i > 0)
					{
						for (w = 0; w < n_nurses; w++)
						{
							if (nurse[w].patient_s[i])
							{
								for (p = 0; p < n_patterns; p++)
								{
									if (pattern[p].patient_p[j] && pattern[p].day_p[d] >= 1 && nurse[w].skill >= pattern[p].day_p[d])
										Expr += max_D * x[p][d][w][i][j];
								}
							}
						}
					}
					model.addConstr(Expr <= max_D);
				}
			}
		}
	}
}
void Data::constraints_flow_n37(GRBModel &model, GRBVar***** x, GRBVar** u)
{
	int i, j, d, w, p, s;

	double num;
	for (i = 1; i < n_jobs; i++)
	{
		for (j = 1; j < n_jobs; j++)
		{
			if (i != j)
			{
				for (d = 0; d < n_days; d++)
				{
					GRBLinExpr Expr = 0;

					if (i > 0)
						Expr += u[d][i];

					num = (job[i].time + distance[job[i].cluster][job[j].cluster]);
					for (w = 0; w < n_nurses; w++)
					{
						if (nurse[w].patient_s[i])
						{
							for (p = 0; p < n_patterns; p++)
							{
								if (pattern[p].patient_p[j] && pattern[p].day_p[d] >= 1 && nurse[w].skill >= pattern[p].day_p[d])
								{
									Expr += num * x[p][d][w][i][j];
									if (j == 0)
									{
										Expr -= (nurse[w].D)*x[p][d][w][i][j];
									}
								}
							}
						}
					}
					if (j > 0)
						Expr -= u[d][j];

					if (i > 0)
					{
						for (w = 0; w < n_nurses; w++)
						{
							if (nurse[w].patient_s[i])
							{
								for (p = 0; p < n_patterns; p++)
								{
									if (pattern[p].patient_p[j] && pattern[p].day_p[d] >= 1 && nurse[w].skill >= pattern[p].day_p[d])
										Expr += max_D * x[p][d][w][i][j];
								}
							}
						}
					}
					model.addConstr(Expr <= max_D);
				}
			}
		}
	}
}
void Data::constraints_flow_n34(GRBModel &model, GRBVar***** x, GRBVar** u)
{
	int i, j, d, w, p, s;

	double num;
	for (i = 0; i < n_jobs; i++)
	{
		for (j = 0; j < n_jobs; j++)
		{
			if (i != j)
			{
				for (d = 0; d < n_days; d++)
				{
					GRBLinExpr Expr = 0;

					if (i > 0)
						Expr += u[d][i];

					num = (job[i].time + distance[job[i].cluster][job[j].cluster]);
					for (w = 0; w < n_nurses; w++)
					{
						if (nurse[w].patient_s[i])
						{
							for (p = 0; p < n_patterns; p++)
							{
								if (pattern[p].patient_p[j] && pattern[p].day_p[d] >= 1 && nurse[w].skill >= pattern[p].day_p[d])
								{
									Expr += num * x[p][d][w][i][j];
									if (j == 0)
									{
										Expr -= (nurse[w].D)*x[p][d][w][i][j];
									}
								}
							}
						}
					}
					if (j > 0)
						Expr -= u[d][j];
					if (i > 0)
					{
						for (w = 0; w < n_nurses; w++)
						{
							if (nurse[w].patient_s[i])
							{
								for (p = 0; p < n_patterns; p++)
								{
									if (pattern[p].patient_p[j] && pattern[p].day_p[d] >= 1 && nurse[w].skill >= pattern[p].day_p[d])
										Expr += max_D * x[p][d][w][i][j];
								}
							}
						}
						model.addConstr(Expr <= max_D);
					}
					else
					{
						model.addConstr(Expr <= 0);
					}
				}
			}
		}
	}
}

void Data::constraints_flow_n5(GRBModel &model, GRBVar***** x, GRBVar*** u)
{
	int i, j, d, w, p, s;

	double num;
	for (i = 0; i < n_jobs; i++)
	{
		for (j = 0; j < n_jobs; j++)
		{
			if (i != j)
			{
				for (d = 0; d < n_days; d++)
				{
					for (w = 0; w < n_nurses; w++)
					{
						if (nurse[w].patient_s[i])
						{
							GRBLinExpr Expr = 0;

							if (i > 0)
								Expr += u[d][w][i];

							num = (job[i].time + distance[job[i].cluster][job[j].cluster]);

							for (p = 0; p < n_patterns; p++)
							{
								if (pattern[p].patient_p[j] && pattern[p].day_p[d] >= 1 && nurse[w].skill >= pattern[p].day_p[d])
								{
									Expr += num * x[p][d][w][i][j];
								}
							}

							Expr -= u[d][w][j];

							for (p = 0; p < n_patterns; p++)
							{
								if (pattern[p].patient_p[j] && pattern[p].day_p[d] >= 1 && nurse[w].skill >= pattern[p].day_p[d])
									Expr += nurse[w].D* x[p][d][w][i][j];
							}

							model.addConstr(Expr <= nurse[w].D);
						}
					}
				}
			}
		}
	}


	double cost;
	for (w = 0; w < n_nurses; w++)
	{

		for (d = 0; d < n_days; d++)
		{
			GRBLinExpr Expr = 0;
			for (i = 0; i < n_jobs; i++)
			{
				if (nurse[w].patient_s[i])
				{
					for (j = 0; j < n_jobs; j++)
					{
						if (i != j)
						{
							for (p = 0; p < n_patterns; p++)
							{
								if (pattern[p].day_p[d] >= 1 && pattern[p].patient_p[j] && nurse[w].skill >= pattern[p].day_p[d])
								{
									cost = 0;
									if (j != 0)
										cost = cost + job[j].time;
									cost = cost + distance[job[i].cluster][job[j].cluster];
									Expr += cost * x[p][d][w][i][j];
								}
							}
						}
					}
				}
			}
			Expr -= u[d][w][0];
			model.addConstr(Expr == 0);
		}

	}
}

void Data::constraints_breaksymmetries_day0(GRBModel &model, GRBVar** D)
{
	int s, w1, w2;
	for (s = 0; s < n_skills; s++)
	{
		for (w1 = 0; w1 < n_nurses; w1++)
		{
			if (nurse[w1].skill == s + 1)
			{
				for (w2 = w1 + 1; w2 < n_nurses; w2++)
				{
					if (nurse[w2].skill == s + 1)
					{

						GRBLinExpr Expr = 0;
						Expr += D[w1][0] - D[w2][0];
						model.addConstr(Expr >= 0);
					}
				}
			}
		}
	}

}

void Data::constraints_flow2(GRBModel &model, GRBVar***** x, GRBVar**** y)
{
	int i, j, d, w, p, s;
	int num;
	double cost;

	for (w = 0; w < n_nurses; w++)
	{
		for (d = 0; d < n_days; d++)
		{
			GRBLinExpr Expr = 0;

			for (i = 0; i < n_jobs; i++)
			{
				if (nurse[w].patient_s[i])
				{
					for (j = 1; j < n_jobs; j++)
					{
						if (i != j)
						{
							for (p = 0; p < n_patterns; p++)
							{
								if (pattern[p].day_p[d] >= 1 && pattern[p].patient_p[j] && nurse[w].skill >= pattern[p].day_p[d])
								{
									Expr += x[p][d][w][i][j];
								}
							}
						}

					}
				}
			}
			for (j = 1; j < n_jobs; j++)
			{
				if (nurse[w].patient_s[j])
				{
					Expr -= y[d][w][0][j];
				}
			}
			model.addConstr(Expr == 0);
		}
	}



	for (w = 0; w < n_nurses; w++)
	{
		for (d = 0; d < n_days; d++)
		{
			for (j = 1; j < n_jobs; j++)
			{
				if (nurse[w].patient_s[j])
				{
					GRBLinExpr Expr = 0;

					for (i = 0; i < n_jobs; i++)
					{

						if (i != j && nurse[w].patient_s[i])
						{
							Expr += y[d][w][i][j] - y[d][w][j][i];
							for (p = 0; p < n_patterns; p++)
							{
								if (pattern[p].day_p[d] >= 1 && pattern[p].patient_p[j] && nurse[w].skill >= pattern[p].day_p[d])
								{
									Expr -= x[p][d][w][i][j];
								}
							}
						}
					}
					model.addConstr(Expr == 0);
				}
			}
		}
	}

	for (w = 0; w < n_nurses; w++)
	{
		for (d = 0; d < n_days; d++)
		{
			for (i = 0; i < n_jobs; i++)
			{
				if (nurse[w].patient_s[i])
				{
					for (j = 0; j < n_jobs; j++)
					{
						if (i != j && nurse[w].patient_s[j])
						{
							GRBLinExpr Expr = 0;
							Expr += y[d][w][i][j];
							if (j != 0)
							{
								for (p = 0; p < n_patterns; p++)
								{
									if (pattern[p].day_p[d] >= 1 && pattern[p].patient_p[j] && nurse[w].skill >= pattern[p].day_p[d])
									{
										Expr -= n_jobs * x[p][d][w][i][j];
									}
								}
							}

							model.addConstr(Expr <= 0);
						}
					}
				}
			}
		}
	}


}
void Data::constraints_breaksymmetries_totaldays(GRBModel &model, GRBVar** D)
{
	int s, w1, w2, d;
	for (s = 0; s < n_skills; s++)
	{
		for (w1 = 0; w1 < n_nurses; w1++)
		{
			if (nurse[w1].skill == s + 1)
			{
				for (w2 = w1 + 1; w2 < n_nurses; w2++)
				{
					if (nurse[w2].skill == s + 1)
					{

						GRBLinExpr Expr = 0;
						for (d = 0; d < n_days; d++)
						{
							Expr += D[w1][d] - D[w2][d];
						}
						model.addConstr(Expr >= 0);
					}
				}
			}
		}
	}

}

void Data::constraints_breaksymmetries_totaldays3(GRBModel &model, GRBVar* D)
{
	int s, w1, w2, d;
	for (s = 0; s < n_skills; s++)
	{
		for (w1 = 0; w1 < n_nurses; w1++)
		{
			if (nurse[w1].skill == s + 1)
			{
				for (w2 = w1 + 1; w2 < n_nurses; w2++)
				{
					if (nurse[w2].skill == s + 1)
					{

						GRBLinExpr Expr = 0;
						Expr += D[w1] - D[w2];
						model.addConstr(Expr >= 0);
					}
				}
			}
		}
	}

}
void Data::write_sol_n(GRBModel &model, GRBVar***** x, GRBVar** z, GRBVar** u, GRBVar** D)
{
	int i, j, p, w, d;

	double value = model.get(GRB_DoubleAttr_ObjVal);
	double best_bound = model.get(GRB_DoubleAttr_ObjBound);
	fprintf(stderr, "Instance:\t%s\tObjValue:\t%.4f\tLB:\t%.4f", data.c_str(), value, best_bound);
	fprintf(stdout, "\n");


	for (w = 0; w < n_nurses; w++)
	{
		for (j = 1; j < n_jobs; j++)
		{
			try
			{
				if (z[w][j].get(GRB_DoubleAttr_X) > 0.001)
				{
					fprintf(stdout, "z(%d,%d)\t%.2f\n", w, j, z[w][j].get(GRB_DoubleAttr_X));
				}
			}
			catch (GRBException e)
			{
			}
		}
	}



	int s_p, s_w;

	for (d = 0; d < n_days; d++)
	{
		for (w = 0; w < n_nurses; w++)
		{
			for (p = 0; p < n_patterns; p++)
			{
				s_p = pattern[p].day_p[d]; //Si en el patrón se considera el día d
				if (s_p >= 1) //Se crea la variable x[p,d,w,i,j] si el patrón cubre el día d
				{

					s_w = nurse[w].skill;
					if (s_w >= s_p) //Se crea la variable x[p,d,w,i,j] si la enfermera tiene la skill requerida ese día o una superior
					{
						for (i = 0; i < n_jobs; i++)
						{
							if (nurse[w].patient_s[i]) //Se crea la variable x[p,d,w,i,j] si la enfermera puede hacer i
							{
								for (j = 0; j < n_jobs; j++)
								{
									if (pattern[p].patient_p[j]) //Se crea x[p,d,w,i,j] si j es compatible con el patrón
									{
										try
										{
											if (x[p][d][w][i][j].get(GRB_DoubleAttr_X) > 0.001)
											{
												fprintf(stdout, "x(%d,%d,%d,%d,%d)\t%.2f\n", p, d, w, i, j, x[p][d][w][i][j].get(GRB_DoubleAttr_X));
											}
										}
										catch (GRBException e)
										{
										}
									}
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
		for (i = 1; i < n_jobs; i++)
		{
			try
			{
				if (u[d][i].get(GRB_DoubleAttr_X) > 0.001)
				{
					fprintf(stdout, "u(%d,%d)\t%.2f\n", d, i, u[d][i].get(GRB_DoubleAttr_X));
				}
			}
			catch (GRBException e)
			{
			}
		}
	}
	for (w = 0; w < n_nurses; w++)
	{
		for (d = 0; d < n_days; d++)
		{
			try
			{
				if (D[w][d].get(GRB_DoubleAttr_X) > 0.001)
				{
					fprintf(stdout, "D(%d,%d)\t%.2f\n", w, d, D[w][d].get(GRB_DoubleAttr_X));
				}
			}
			catch (GRBException e)
			{
			}
		}
	}
}

void Data::write_sol_n2(GRBModel &model, GRBVar***** x, GRBVar** z, GRBVar**** y, GRBVar** D)
{
	int i, j, p, w, d;

	double value = model.get(GRB_DoubleAttr_ObjVal);
	double best_bound = model.get(GRB_DoubleAttr_ObjBound);
	fprintf(stderr, "Instance:\t%s\tObjValue:\t%.4f\tLB:\t%.4f", data.c_str(), value, best_bound);
	fprintf(stdout, "\n");


	for (w = 0; w < n_nurses; w++)
	{
		for (j = 1; j < n_jobs; j++)
		{
			try
			{
				if (z[w][j].get(GRB_DoubleAttr_X) > 0.001)
				{
					fprintf(stdout, "z(%d,%d)\t%.2f\n", w, j, z[w][j].get(GRB_DoubleAttr_X));
				}
			}
			catch (GRBException e)
			{
			}
		}
	}



	int s_p, s_w;

	for (d = 0; d < n_days; d++)
	{
		for (w = 0; w < n_nurses; w++)
		{
			for (p = 0; p < n_patterns; p++)
			{
				s_p = pattern[p].day_p[d]; //Si en el patrón se considera el día d
				if (s_p >= 1) //Se crea la variable x[p,d,w,i,j] si el patrón cubre el día d
				{

					s_w = nurse[w].skill;
					if (s_w >= s_p) //Se crea la variable x[p,d,w,i,j] si la enfermera tiene la skill requerida ese día o una superior
					{
						for (i = 0; i < n_jobs; i++)
						{
							if (nurse[w].patient_s[i]) //Se crea la variable x[p,d,w,i,j] si la enfermera puede hacer i
							{
								for (j = 0; j < n_jobs; j++)
								{
									if (pattern[p].patient_p[j]) //Se crea x[p,d,w,i,j] si j es compatible con el patrón
									{
										try
										{
											if (x[p][d][w][i][j].get(GRB_DoubleAttr_X) > 0.001)
											{
												fprintf(stdout, "x(%d,%d,%d,%d,%d)\t%.2f\n", p, d, w, i, j, x[p][d][w][i][j].get(GRB_DoubleAttr_X));
											}
										}
										catch (GRBException e)
										{
										}
									}
								}
							}
						}
					}
				}
			}
		}
	}

	for (int d = 0; d < n_days; d++)
	{
		for (int w = 0; w < n_nurses; w++)
		{
			for (int i = 0; i < n_jobs; i++)
			{
				for (int j = 0; j < n_jobs; j++)
				{

					if (nurse[w].patient_s[i] && nurse[w].patient_s[j])
					{
						try
						{
							if (y[d][w][i][j].get(GRB_DoubleAttr_X) > 0.001)
							{
								fprintf(stdout, "y(%d,%d,%d,%d)\t%.2f\n", d, w, i, j, y[d][w][i][j].get(GRB_DoubleAttr_X));
							}
						}
						catch (GRBException e)
						{
						}
					}

				}

			}



		}
	}

	for (w = 0; w < n_nurses; w++)
	{
		for (d = 0; d < n_days; d++)
		{
			try
			{
				if (D[w][d].get(GRB_DoubleAttr_X) > 0.001)
				{
					fprintf(stdout, "D(%d,%d)\t%.2f\n", w, d, D[w][d].get(GRB_DoubleAttr_X));
				}
			}
			catch (GRBException e)
			{
			}
		}
	}
}
void Data::write_sol_n23(GRBModel &model, GRBVar***** x, GRBVar** z, GRBVar**** y, GRBVar* D)
{
	int i, j, p, w, d;

	double value = model.get(GRB_DoubleAttr_ObjVal);
	double best_bound = model.get(GRB_DoubleAttr_ObjBound);
	fprintf(stderr, "Instance:\t%s\tObjValue:\t%.4f\tLB:\t%.4f", data.c_str(), value, best_bound);
	fprintf(stdout, "\n");


	for (w = 0; w < n_nurses; w++)
	{
		for (j = 1; j < n_jobs; j++)
		{
			try
			{
				if (z[w][j].get(GRB_DoubleAttr_X) > 0.001)
				{
					fprintf(stdout, "z(%d,%d)\t%.2f\n", w, j, z[w][j].get(GRB_DoubleAttr_X));
				}
			}
			catch (GRBException e)
			{
			}
		}
	}



	int s_p, s_w;

	for (d = 0; d < n_days; d++)
	{
		for (w = 0; w < n_nurses; w++)
		{
			for (p = 0; p < n_patterns; p++)
			{
				s_p = pattern[p].day_p[d]; //Si en el patrón se considera el día d
				if (s_p >= 1) //Se crea la variable x[p,d,w,i,j] si el patrón cubre el día d
				{

					s_w = nurse[w].skill;
					if (s_w >= s_p) //Se crea la variable x[p,d,w,i,j] si la enfermera tiene la skill requerida ese día o una superior
					{
						for (i = 0; i < n_jobs; i++)
						{
							if (nurse[w].patient_s[i]) //Se crea la variable x[p,d,w,i,j] si la enfermera puede hacer i
							{
								for (j = 0; j < n_jobs; j++)
								{
									if (pattern[p].patient_p[j]) //Se crea x[p,d,w,i,j] si j es compatible con el patrón
									{
										try
										{
											if (x[p][d][w][i][j].get(GRB_DoubleAttr_X) > 0.001)
											{
												fprintf(stdout, "x(%d,%d,%d,%d,%d)\t%.2f\n", p, d, w, i, j, x[p][d][w][i][j].get(GRB_DoubleAttr_X));
											}
										}
										catch (GRBException e)
										{
										}
									}
								}
							}
						}
					}
				}
			}
		}
	}

	for (int d = 0; d < n_days; d++)
	{
		for (int w = 0; w < n_nurses; w++)
		{
			for (int i = 0; i < n_jobs; i++)
			{
				for (int j = 0; j < n_jobs; j++)
				{

					if (nurse[w].patient_s[i] && nurse[w].patient_s[j])
					{
						try
						{
							if (y[d][w][i][j].get(GRB_DoubleAttr_X) > 0.001)
							{
								fprintf(stdout, "y(%d,%d,%d,%d)\t%.2f\n", d, w, i, j, y[d][w][i][j].get(GRB_DoubleAttr_X));
							}
						}
						catch (GRBException e)
						{
						}
					}

				}

			}



		}
	}

	for (w = 0; w < n_nurses; w++)
	{

		try
		{
			if (D[w].get(GRB_DoubleAttr_X) > 0.001)
			{
				fprintf(stdout, "D(%d)\t%.2f\n", w, D[w].get(GRB_DoubleAttr_X));
			}
		}
		catch (GRBException e)
		{
		}
	}
}
void Data::write_sol_n23_b(GRBModel &model, GRBVar***** x, GRBVar** z, GRBVar**** y, GRBVar* D)
{
	int i, j, p, w, d;

	double value = model.get(GRB_DoubleAttr_ObjVal);
	double best_bound = model.get(GRB_DoubleAttr_ObjBound);

	double day_work;
	int s_day_work;
	double cost;
	double total_work;
	for (w = 0; w < n_nurses; w++)
	{
		total_work = 0;

		fprintf(stderr, "\nAlgorithm:\t%d_v%d_%d%d%d%d%d_%.1f_%.1f\tT:\t%d\tObjective:\t%d\tInstance:\t%s\tCaregiver_%d:\tMax_day:\t%d\t", algorithm, version, r1, r2, r3, r4, r5, l1, l2, job[0].max_nurses, ob, data.c_str(), w, nurse[w].D);
		for (d = 0; d < n_days; d++)
		{
			fprintf(stderr, "day_%d:\t", d);
			day_work = 0;
			for (i = 0; i < n_jobs; i++)
			{
				if (nurse[w].patient_s[i])
				{
					for (j = 0; j < n_jobs; j++)
					{
						if (i != j)
						{
							for (p = 0; p < n_patterns; p++)
							{
								if (pattern[p].day_p[d] >= 1 && pattern[p].patient_p[j] && nurse[w].skill >= pattern[p].day_p[d])
								{
									cost = 0;
									if (j != 0)
										cost = cost + job[j].time;
									cost = cost + distance[job[i].cluster][job[j].cluster];

									try
									{
										if (x[p][d][w][i][j].get(GRB_DoubleAttr_X) > 0.001)
										{
											total_work = total_work + cost;
											day_work = day_work + cost;
										}
									}
									catch (GRBException e)
									{
									}
								}
							}
						}
					}
				}
			}

			fprintf(stderr, "%.2f\t", day_work);
		}
		fprintf(stderr, "Total:\t%.2f", total_work);

		fprintf(stderr, "\t");
		for (d = 0; d < n_days; d++)
		{
			fprintf(stderr, "s_day_%d:\t", d);
			s_day_work = 0;
			i = 0;
					for (j = 0; j < n_jobs; j++)
					{
						if (i != j)
						{
							for (p = 0; p < n_patterns; p++)
							{
								if (pattern[p].day_p[d] >= 1 && pattern[p].patient_p[j] && nurse[w].skill >= pattern[p].day_p[d])
								{

									try
									{
										if (x[p][d][w][i][j].get(GRB_DoubleAttr_X) > 0.001)
										{
											s_day_work++;
										}
									}
									catch (GRBException e)
									{
									}
								}
							}
						}
					}
				
			

			fprintf(stderr, "%d\t", s_day_work);
		}

	}
}
void Data::write_sol_n25(GRBModel &model, GRBVar***** x, GRBVar** z, GRBVar**** y, GRBVar*** u)
{
	int i, j, p, w, d;

	double value = model.get(GRB_DoubleAttr_ObjVal);
	double best_bound = model.get(GRB_DoubleAttr_ObjBound);
	fprintf(stderr, "Instance:\t%s\tObjValue:\t%.4f\tLB:\t%.4f", data.c_str(), value, best_bound);
	fprintf(stdout, "\n");


	for (w = 0; w < n_nurses; w++)
	{
		for (j = 1; j < n_jobs; j++)
		{
			try
			{
				if (z[w][j].get(GRB_DoubleAttr_X) > 0.001)
				{
					fprintf(stdout, "z(%d,%d)\t%.2f\n", w, j, z[w][j].get(GRB_DoubleAttr_X));
				}
			}
			catch (GRBException e)
			{
			}
		}
	}



	int s_p, s_w;

	for (d = 0; d < n_days; d++)
	{
		for (w = 0; w < n_nurses; w++)
		{
			for (p = 0; p < n_patterns; p++)
			{
				s_p = pattern[p].day_p[d]; //Si en el patrón se considera el día d
				if (s_p >= 1) //Se crea la variable x[p,d,w,i,j] si el patrón cubre el día d
				{

					s_w = nurse[w].skill;
					if (s_w >= s_p) //Se crea la variable x[p,d,w,i,j] si la enfermera tiene la skill requerida ese día o una superior
					{
						for (i = 0; i < n_jobs; i++)
						{
							if (nurse[w].patient_s[i]) //Se crea la variable x[p,d,w,i,j] si la enfermera puede hacer i
							{
								for (j = 0; j < n_jobs; j++)
								{
									if (pattern[p].patient_p[j]) //Se crea x[p,d,w,i,j] si j es compatible con el patrón
									{
										try
										{
											if (x[p][d][w][i][j].get(GRB_DoubleAttr_X) > 0.001)
											{
												fprintf(stdout, "x(%d,%d,%d,%d,%d)\t%.2f\n", p, d, w, i, j, x[p][d][w][i][j].get(GRB_DoubleAttr_X));
											}
										}
										catch (GRBException e)
										{
										}
									}
								}
							}
						}
					}
				}
			}
		}
	}

	for (int d = 0; d < n_days; d++)
	{
		for (int w = 0; w < n_nurses; w++)
		{
			for (int i = 0; i < n_jobs; i++)
			{
				for (int j = 0; j < n_jobs; j++)
				{

					if (nurse[w].patient_s[i] && nurse[w].patient_s[j])
					{
						try
						{
							if (y[d][w][i][j].get(GRB_DoubleAttr_X) > 0.001)
							{
								fprintf(stdout, "y(%d,%d,%d,%d)\t%.2f\n", d, w, i, j, y[d][w][i][j].get(GRB_DoubleAttr_X));
							}
						}
						catch (GRBException e)
						{
						}
					}

				}

			}



		}
	}

	for (w = 0; w < n_nurses; w++)
	{
		for (d = 0; d < n_days; d++)
		{

			try
			{
				if (u[d][w][0].get(GRB_DoubleAttr_X) > 0.001)
				{
					fprintf(stdout, "u(%d,%d,%d)\t%.2f\n", d, w, 0, u[d][w][0].get(GRB_DoubleAttr_X));
				}
			}
			catch (GRBException e)
			{
			}
		}
	}
}
//RANKING
void Data::write_sol_ranking(GRBModel& model, GRBVar***** x, GRBVar** z, GRBVar**** y, GRBVar* D)
{
	int i, j, p, w, d;

	double value = model.get(GRB_DoubleAttr_ObjVal);
	double best_bound = model.get(GRB_DoubleAttr_ObjBound);

	double day_work;
	int s_day_work;
	double cost;
	double total_work;
	for (w = 0; w < n_nurses; w++)
	{
		total_work = 0;

		fprintf(stderr, "\nAlgorithm:\t%d_v%d_%d%d%d%d%d_%.4f_%.4f\tT:\t%d\tObjective:\t%d\tInstance:\t%s\tCaregiver_%d:\tMax_day:\t%d\t", algorithm, version, r1, r2, r3, r4, r5, MIPgap, relaxation, job[0].max_nurses, ob, data.c_str(), w, nurse[w].D);
		for (d = 0; d < n_days; d++)
		{
			fprintf(stderr, "day_%d:\t", d);
			day_work = 0;
			for (i = 0; i < n_jobs; i++)
			{
				if (nurse[w].patient_s[i])
				{
					for (j = 0; j < n_jobs; j++)
					{
						if (i != j)
						{
							for (p = 0; p < n_patterns; p++)
							{
								if (pattern[p].day_p[d] >= 1 && pattern[p].patient_p[j] && nurse[w].skill >= pattern[p].day_p[d])
								{
									cost = 0;
									if (j != 0)
										cost = cost + job[j].time;
									cost = cost + distance[job[i].cluster][job[j].cluster];

									try
									{
										if (x[p][d][w][i][j].get(GRB_DoubleAttr_X) > 0.001)
										{
											total_work = total_work + cost;
											day_work = day_work + cost;
										}
									}
									catch (GRBException e)
									{
									}
								}
							}
						}
					}
				}
			}

			fprintf(stderr, "%.2f\t", day_work);
		}
		fprintf(stderr, "Total:\t%.2f", total_work);

		fprintf(stderr, "\t");
		for (d = 0; d < n_days; d++)
		{
			fprintf(stderr, "s_day_%d:\t", d);
			s_day_work = 0;
			i = 0;
			for (j = 0; j < n_jobs; j++)
			{
				if (i != j)
				{
					for (p = 0; p < n_patterns; p++)
					{
						if (pattern[p].day_p[d] >= 1 && pattern[p].patient_p[j] && nurse[w].skill >= pattern[p].day_p[d])
						{

							try
							{
								if (x[p][d][w][i][j].get(GRB_DoubleAttr_X) > 0.001)
								{
									s_day_work++;
								}
							}
							catch (GRBException e)
							{
							}
						}
					}
				}
			}



			fprintf(stderr, "%d\t", s_day_work);
		}

	}
}

//GOAL
void Data::write_sol_goal(GRBModel& model, GRBVar***** x, GRBVar** z, GRBVar**** y, GRBVar* D)
{
	int i, j, p, w, d;

	double value = model.get(GRB_DoubleAttr_ObjVal);
	double best_bound = model.get(GRB_DoubleAttr_ObjBound);

	double day_work;
	int s_day_work;
	double cost;
	double total_work;
	for (w = 0; w < n_nurses; w++)
	{
		total_work = 0;

		fprintf(stderr, "\nAlgorithm:\t%d_v%d_%d%d%d%d%d_%.4f\tT:\t%d\tObjective:\t%d\tInstance:\t%s\tCaregiver_%d:\tMax_day:\t%d\t", algorithm, version, r1, r2, r3, r4, r5, eqgoal, job[0].max_nurses, ob, data.c_str(), w, nurse[w].D);
		for (d = 0; d < n_days; d++)
		{
			fprintf(stderr, "day_%d:\t", d);
			day_work = 0;
			for (i = 0; i < n_jobs; i++)
			{
				if (nurse[w].patient_s[i])
				{
					for (j = 0; j < n_jobs; j++)
					{
						if (i != j)
						{
							for (p = 0; p < n_patterns; p++)
							{
								if (pattern[p].day_p[d] >= 1 && pattern[p].patient_p[j] && nurse[w].skill >= pattern[p].day_p[d])
								{
									cost = 0;
									if (j != 0)
										cost = cost + job[j].time;
									cost = cost + distance[job[i].cluster][job[j].cluster];

									try
									{
										if (x[p][d][w][i][j].get(GRB_DoubleAttr_X) > 0.001)
										{
											total_work = total_work + cost;
											day_work = day_work + cost;
										}
									}
									catch (GRBException e)
									{
									}
								}
							}
						}
					}
				}
			}

			fprintf(stderr, "%.2f\t", day_work);
		}
		fprintf(stderr, "Total:\t%.2f", total_work);

		fprintf(stderr, "\t");
		for (d = 0; d < n_days; d++)
		{
			fprintf(stderr, "s_day_%d:\t", d);
			s_day_work = 0;
			i = 0;
			for (j = 0; j < n_jobs; j++)
			{
				if (i != j)
				{
					for (p = 0; p < n_patterns; p++)
					{
						if (pattern[p].day_p[d] >= 1 && pattern[p].patient_p[j] && nurse[w].skill >= pattern[p].day_p[d])
						{

							try
							{
								if (x[p][d][w][i][j].get(GRB_DoubleAttr_X) > 0.001)
								{
									s_day_work++;
								}
							}
							catch (GRBException e)
							{
							}
						}
					}
				}
			}



			fprintf(stderr, "%d\t", s_day_work);
		}

	}
}

void Data::write_sol_n24(GRBModel &model, GRBVar***** x, GRBVar** z, GRBVar**** y)
{
	int i, j, p, w, d;

	double value = model.get(GRB_DoubleAttr_ObjVal);
	double best_bound = model.get(GRB_DoubleAttr_ObjBound);
	fprintf(stderr, "Instance:\t%s\tObjValue:\t%.4f\tLB:\t%.4f", data.c_str(), value, best_bound);
	fprintf(stdout, "\n");


	for (w = 0; w < n_nurses; w++)
	{
		for (j = 1; j < n_jobs; j++)
		{
			try
			{
				if (z[w][j].get(GRB_DoubleAttr_X) > 0.001)
				{
					fprintf(stdout, "z(%d,%d)\t%.2f\n", w, j, z[w][j].get(GRB_DoubleAttr_X));
				}
			}
			catch (GRBException e)
			{
			}
		}
	}



	int s_p, s_w;

	for (d = 0; d < n_days; d++)
	{
		for (w = 0; w < n_nurses; w++)
		{
			for (p = 0; p < n_patterns; p++)
			{
				s_p = pattern[p].day_p[d]; //Si en el patrón se considera el día d
				if (s_p >= 1) //Se crea la variable x[p,d,w,i,j] si el patrón cubre el día d
				{

					s_w = nurse[w].skill;
					if (s_w >= s_p) //Se crea la variable x[p,d,w,i,j] si la enfermera tiene la skill requerida ese día o una superior
					{
						for (i = 0; i < n_jobs; i++)
						{
							if (nurse[w].patient_s[i]) //Se crea la variable x[p,d,w,i,j] si la enfermera puede hacer i
							{
								for (j = 0; j < n_jobs; j++)
								{
									if (pattern[p].patient_p[j]) //Se crea x[p,d,w,i,j] si j es compatible con el patrón
									{
										try
										{
											if (x[p][d][w][i][j].get(GRB_DoubleAttr_X) > 0.001)
											{
												fprintf(stdout, "x(%d,%d,%d,%d,%d)\t%.2f\n", p, d, w, i, j, x[p][d][w][i][j].get(GRB_DoubleAttr_X));
											}
										}
										catch (GRBException e)
										{
										}
									}
								}
							}
						}
					}
				}
			}
		}
	}

	for (int d = 0; d < n_days; d++)
	{
		for (int w = 0; w < n_nurses; w++)
		{
			for (int i = 0; i < n_jobs; i++)
			{
				for (int j = 0; j < n_jobs; j++)
				{

					if (nurse[w].patient_s[i] && nurse[w].patient_s[j])
					{
						try
						{
							if (y[d][w][i][j].get(GRB_DoubleAttr_X) > 0.001)
							{
								fprintf(stdout, "y(%d,%d,%d,%d)\t%.2f\n", d, w, i, j, y[d][w][i][j].get(GRB_DoubleAttr_X));
							}
						}
						catch (GRBException e)
						{
						}
					}

				}

			}



		}
	}

}
void Data::write_sol_n22(GRBModel &model, GRBVar***** x, GRBVar** z, GRBVar**** y, GRBVar*** u)
{
	int i, j, p, w, d;

	double value = model.get(GRB_DoubleAttr_ObjVal);
	double best_bound = model.get(GRB_DoubleAttr_ObjBound);
	fprintf(stderr, "Instance:\t%s\tObjValue:\t%.4f\tLB:\t%.4f", data.c_str(), value, best_bound);
	fprintf(stdout, "\n");


	for (w = 0; w < n_nurses; w++)
	{
		for (j = 1; j < n_jobs; j++)
		{
			try
			{
				if (z[w][j].get(GRB_DoubleAttr_X) > 0.001)
				{
					fprintf(stdout, "z(%d,%d)\t%.2f\n", w, j, z[w][j].get(GRB_DoubleAttr_X));
				}
			}
			catch (GRBException e)
			{
			}
		}
	}



	int s_p, s_w;

	for (d = 0; d < n_days; d++)
	{
		for (w = 0; w < n_nurses; w++)
		{
			for (p = 0; p < n_patterns; p++)
			{
				s_p = pattern[p].day_p[d]; //Si en el patrón se considera el día d
				if (s_p >= 1) //Se crea la variable x[p,d,w,i,j] si el patrón cubre el día d
				{

					s_w = nurse[w].skill;
					if (s_w >= s_p) //Se crea la variable x[p,d,w,i,j] si la enfermera tiene la skill requerida ese día o una superior
					{
						for (i = 0; i < n_jobs; i++)
						{
							if (nurse[w].patient_s[i]) //Se crea la variable x[p,d,w,i,j] si la enfermera puede hacer i
							{
								for (j = 0; j < n_jobs; j++)
								{
									if (pattern[p].patient_p[j]) //Se crea x[p,d,w,i,j] si j es compatible con el patrón
									{
										try
										{
											if (x[p][d][w][i][j].get(GRB_DoubleAttr_X) > 0.001)
											{
												fprintf(stdout, "x(%d,%d,%d,%d,%d)\t%.2f\n", p, d, w, i, j, x[p][d][w][i][j].get(GRB_DoubleAttr_X));
											}
										}
										catch (GRBException e)
										{
										}
									}
								}
							}
						}
					}
				}
			}
		}
	}

	for (int d = 0; d < n_days; d++)
	{
		for (int w = 0; w < n_nurses; w++)
		{
			for (int i = 0; i < n_jobs; i++)
			{
				for (int j = 0; j < n_jobs; j++)
				{

					if (nurse[w].patient_s[i] && nurse[w].patient_s[j])
					{
						try
						{
							if (y[d][w][i][j].get(GRB_DoubleAttr_X) > 0.001)
							{
								fprintf(stdout, "y(%d,%d,%d,%d)\t%.2f\n", d, w, i, j, y[d][w][i][j].get(GRB_DoubleAttr_X));
							}
						}
						catch (GRBException e)
						{
						}
					}

				}

			}



		}
	}

	for (w = 0; w < n_nurses; w++)
	{
		for (d = 0; d < n_days; d++)
		{
			try
			{
				if (u[d][w][0].get(GRB_DoubleAttr_X) > 0.001)
				{
					fprintf(stdout, "u(%d,%d,0)\t%.2f\n", w, d, u[d][w][0].get(GRB_DoubleAttr_X));
				}
			}
			catch (GRBException e)
			{
			}
		}
	}
}
void Data::write_solution_csv_n(GRBModel &model, GRBVar***** x, GRBVar** z, GRBVar** u, GRBVar** D)
{
	int i, j, k, d, s, w, p, s_i;

	int ii = 0;
	int jj = 0;
	int size_depot = 10;
	int size_related = 5;
	double color = 1;
	int m_dist = 20;
	double increase_color = 1 / double(n_days + n_nurses);
	FILE *fpt;

	data.erase(data.end() - 4, data.end());
	mkdir("Results");
	string algorithmn = to_string(algorithm);
	string s_file = "./Results/Solfgurob_" + algorithmn + "_" + data + ".csv";
	fpt = fopen(s_file.c_str(), "w+");
	fprintf(fpt, "element,id_1,id_2,use,value,NOTAS\n");

	//Shape of the depots
	for (d = 0; d < n_days; d++)
		fprintf(fpt, "node,%d,%d,shape,box,\n", d, d);

	//Draw the depots (Aunque sea el mismo, dibujamos uno por día, en total son n_days depots).

	vector<double> color_v;

	color = increase_color;

	for (d = 0; d < n_days; d++)
	{
		fprintf(fpt, "node,%d,%d,text, Day %d,\n", d, d, d);
		fprintf(fpt, "node,%d,%d,size,%d,\n", d, d, size_depot);
		fprintf(fpt, "node,%d,%d,color_float,%.2f,\n", d, d, color);
		color = color + increase_color;
	}

	for (w = 0; w < n_nurses; w++)
	{
		color_v.push_back(color);
		color = color + increase_color;
	}
	int shape = 0;
	//Star 0
	//Triangle 1
	//Square
	int count;
	vector<int> exist_node;
	for (d = 0; d < (n_jobs)*n_days; d++)
	{
		exist_node.push_back(0);
	}



	//Nodes y variables
	double num;
	for (p = 0; p < n_patterns; p++)
	{
		for (d = 0; d < n_days; d++)
		{
			if (pattern[p].day_p[d] >= 1)
			{
				for (int ii = 0; ii < n_jobs; ii++)
				{
					i = d;
					if (ii != 0)
						i = d * (n_jobs - 1) + (n_days + ii - 1);

					for (int jj = 0; jj < n_jobs; jj++)
					{
						if (pattern[p].patient_p[jj])
						{
							j = d;
							if (jj != 0)
								j = d * (n_jobs - 1) + (n_days + jj - 1);

							for (w = 0; w < n_nurses; w++)
							{
								if (nurse[w].skill >= pattern[p].day_p[d] && nurse[w].patient_s[ii])
								{
									try
									{
										if (x[p][d][w][ii][jj].get(GRB_DoubleAttr_X) > 0.001)
										{
											num = x[p][d][w][ii][jj].get(GRB_DoubleAttr_X);
											if (ii != 0 || jj != 0)
											{
												if (exist_node[i] == 0 && i >= n_days)
												{
													fprintf(fpt, "node,%d,%d,text, Node %d,\n", i, job[ii].ID, ii);
													fprintf(fpt, "node,%d,%d,text, Skill_0 %d,\n", i, job[ii].ID, job[ii].skills[0]);
													fprintf(fpt, "node,%d,%d,text, Skill_1 %d,\n", i, job[ii].ID, job[ii].skills[1]);
													fprintf(fpt, "node,%d,%d,color_float,%.2f,\n", i, job[ii].ID, color_v[w]);
													exist_node[i]++;
													for (int dd = 0; dd < n_days; dd++)
													{
														if (pattern[p].day_p[dd] > 0)
															fprintf(fpt, "node,%d,%d,text, Day %d Req. %d,\n", i, job[ii].ID, dd, pattern[p].day_p[dd]);
													}

												}
												if (exist_node[j] == 0 && j >= n_days)
												{

													fprintf(fpt, "node,%d,%d,text, Node %d,\n", j, job[jj].ID, jj);
													fprintf(fpt, "node,%d,%d,text, Skill_0 %d,\n", j, job[jj].ID, job[jj].skills[0]);
													fprintf(fpt, "node,%d,%d,text, Skill_1 %d,\n", j, job[jj].ID, job[jj].skills[1]);
													fprintf(fpt, "node,%d,%d,color_float,%.2f,\n", j, job[jj].ID, color_v[w]);
													exist_node[j]++;
													for (int dd = 0; dd < n_days; dd++)
													{
														if (pattern[p].day_p[dd] > 0)
															fprintf(fpt, "node,%d,%d,text, Day %d Req. %d,\n", j, job[jj].ID, dd, pattern[p].day_p[dd]);
													}

												}


												num = x[p][d][w][ii][jj].get(GRB_DoubleAttr_X);

												fprintf(fpt, "edge,%d,%d,text,x_%d_%d = %.2f,\n", i, j, ii, jj, num);
												fprintf(fpt, "edge,%d,%d,text,Pattern = %d Nurse = %d Skill= %d,\n", i, j, p, w, nurse[w].skill);
												fprintf(fpt, "edge,%d,%d,width,%.2f,\n", i, j, 3 * num);
												if (num < 0.9999)
													fprintf(fpt, "edge,%d,%d,dashes,1,\n", i, j);

											}


										}
									}
									catch (GRBException e)
									{
									}
								}
							}
						}
					}
				}
			}
		}
	}


	fclose(fpt);



	//std::string pandas = "pip install pandas";
	//system(pandas.c_str());

	//std::string maplotlib = "pip install matplotlib";
	//system(maplotlib.c_str());

	//std::string pyvis = "pip install pyvis";
	//system(pyvis.c_str());



	string s_html = "./Results/Solfgurob_" + algorithmn + "_" + data + ".html";
	std::string filename;

	if (algorithm == -1 || algorithm > 10)
	{
		filename = "create_graphb.py " + s_file + " --out " + s_html;
	}
	else
	{
		filename = "create_graph.py " + s_file + " --out " + s_html;
	}



	//std::string filename = "create_graph.py -h";
	std::string command = "python ";
	command += filename;
	system(command.c_str());


	//Py_Initialize();
	//char ArchivoPython[] = "create_graph.py" + s_file;
	//FILE* file;
	////Leemos archivo
	//file = _Py_fopen(ArchivoPython, "r");

	////Ejecutamos
	//PyRun_SimpleFile(file, ArchivoPython);

	//PyRun_SimpleString("from time import time,ctime\n"
	//	"print 'Today is',ctime(time())\n");
	//system(“python create_graph.py test_data.csv”);
	//Py_Finalize();
}
void Data::write_solution_csv_n3(GRBModel &model, GRBVar***** x, GRBVar** z, GRBVar** u, GRBVar* D)
{
	int i, j, k, d, s, w, p, s_i;

	int ii = 0;
	int jj = 0;
	int size_depot = 10;
	int size_related = 5;
	double color = 1;
	int m_dist = 20;
	double increase_color = 1 / double(n_days + n_nurses);
	FILE *fpt;

	data.erase(data.end() - 4, data.end());
	mkdir("Results");
	string algorithmn = to_string(algorithm);
	string s_file = "./Results/Solfgurob_" + algorithmn + "_" + data + ".csv";
	fpt = fopen(s_file.c_str(), "w+");
	fprintf(fpt, "element,id_1,id_2,use,value,NOTAS\n");

	//Shape of the depots
	for (d = 0; d < n_days; d++)
		fprintf(fpt, "node,%d,%d,shape,box,\n", d, d);

	//Draw the depots (Aunque sea el mismo, dibujamos uno por día, en total son n_days depots).

	vector<double> color_v;

	color = increase_color;

	for (d = 0; d < n_days; d++)
	{
		fprintf(fpt, "node,%d,%d,text, Day %d,\n", d, d, d);
		fprintf(fpt, "node,%d,%d,size,%d,\n", d, d, size_depot);
		fprintf(fpt, "node,%d,%d,color_float,%.2f,\n", d, d, color);
		color = color + increase_color;
	}

	for (w = 0; w < n_nurses; w++)
	{
		color_v.push_back(color);
		color = color + increase_color;
	}
	int shape = 0;
	//Star 0
	//Triangle 1
	//Square
	int count;
	vector<int> exist_node;
	for (d = 0; d < (n_jobs)*n_days; d++)
	{
		exist_node.push_back(0);
	}



	//Nodes y variables
	double num;
	for (p = 0; p < n_patterns; p++)
	{
		for (d = 0; d < n_days; d++)
		{
			if (pattern[p].day_p[d] >= 1)
			{
				for (int ii = 0; ii < n_jobs; ii++)
				{
					i = d;
					if (ii != 0)
						i = d * (n_jobs - 1) + (n_days + ii - 1);

					for (int jj = 0; jj < n_jobs; jj++)
					{
						if (pattern[p].patient_p[jj])
						{
							j = d;
							if (jj != 0)
								j = d * (n_jobs - 1) + (n_days + jj - 1);

							for (w = 0; w < n_nurses; w++)
							{
								if (nurse[w].skill >= pattern[p].day_p[d] && nurse[w].patient_s[ii])
								{
									try
									{
										if (x[p][d][w][ii][jj].get(GRB_DoubleAttr_X) > 0.001)
										{
											num = x[p][d][w][ii][jj].get(GRB_DoubleAttr_X);
											if (ii != 0 || jj != 0)
											{
												if (exist_node[i] == 0 && i >= n_days)
												{
													fprintf(fpt, "node,%d,%d,text, Node %d,\n", i, job[ii].ID, ii);
													fprintf(fpt, "node,%d,%d,text, Cluster %d,\n", i, job[ii].ID, job[ii].cluster);
													fprintf(fpt, "node,%d,%d,text, Skill_0 %d,\n", i, job[ii].ID, job[ii].skills[0]);
													fprintf(fpt, "node,%d,%d,text, Skill_1 %d,\n", i, job[ii].ID, job[ii].skills[1]);
													fprintf(fpt, "node,%d,%d,color_float,%.2f,\n", i, job[ii].ID, color_v[w]);
													exist_node[i]++;
													for (int dd = 0; dd < n_days; dd++)
													{
														if (pattern[p].day_p[dd] > 0)
															fprintf(fpt, "node,%d,%d,text, Day %d Req. %d,\n", i, job[ii].ID, dd, pattern[p].day_p[dd]);
													}

												}
												if (exist_node[j] == 0 && j >= n_days)
												{

													fprintf(fpt, "node,%d,%d,text, Node %d,\n", j, job[jj].ID, jj);
													fprintf(fpt, "node,%d,%d,text, Cluster %d,\n", j, job[jj].ID, job[jj].cluster);
													fprintf(fpt, "node,%d,%d,text, Skill_0 %d,\n", j, job[jj].ID, job[jj].skills[0]);
													fprintf(fpt, "node,%d,%d,text, Skill_1 %d,\n", j, job[jj].ID, job[jj].skills[1]);
													fprintf(fpt, "node,%d,%d,color_float,%.2f,\n", j, job[jj].ID, color_v[w]);
													exist_node[j]++;
													for (int dd = 0; dd < n_days; dd++)
													{
														if (pattern[p].day_p[dd] > 0)
															fprintf(fpt, "node,%d,%d,text, Day %d Req. %d,\n", j, job[jj].ID, dd, pattern[p].day_p[dd]);
													}

												}


												num = x[p][d][w][ii][jj].get(GRB_DoubleAttr_X);

												fprintf(fpt, "edge,%d,%d,text,x_%d_%d = %.2f,\n", i, j, ii, jj, num);
												fprintf(fpt, "edge,%d,%d,text,Pattern = %d Nurse = %d Skill= %d,\n", i, j, p, w, nurse[w].skill);
												fprintf(fpt, "edge,%d,%d,width,%.2f,\n", i, j, 3 * num);
												if (num < 0.9999)
													fprintf(fpt, "edge,%d,%d,dashes,1,\n", i, j);

											}


										}
									}
									catch (GRBException e)
									{
									}
								}
							}
						}
					}
				}
			}
		}
	}


	fclose(fpt);



	//std::string pandas = "pip install pandas";
	//system(pandas.c_str());

	//std::string maplotlib = "pip install matplotlib";
	//system(maplotlib.c_str());

	//std::string pyvis = "pip install pyvis";
	//system(pyvis.c_str());



	string s_html = "./Results/Solfgurob_" + algorithmn + "_" + data + ".html";
	std::string filename;

	if (algorithm == -1 || algorithm > 10)
	{
		filename = "create_graphb.py " + s_file + " --out " + s_html;
	}
	else
	{
		filename = "create_graph.py " + s_file + " --out " + s_html;
	}



	//std::string filename = "create_graph.py -h";
	std::string command = "python ";
	command += filename;
	system(command.c_str());


	//Py_Initialize();
	//char ArchivoPython[] = "create_graph.py" + s_file;
	//FILE* file;
	////Leemos archivo
	//file = _Py_fopen(ArchivoPython, "r");

	////Ejecutamos
	//PyRun_SimpleFile(file, ArchivoPython);

	//PyRun_SimpleString("from time import time,ctime\n"
	//	"print 'Today is',ctime(time())\n");
	//system(“python create_graph.py test_data.csv”);
	//Py_Finalize();
}
void Data::write_solution_csv_n4(GRBModel &model, GRBVar***** x, GRBVar** z, GRBVar** u)
{
	int i, j, k, d, s, w, p, s_i;

	int ii = 0;
	int jj = 0;
	int size_depot = 10;
	int size_related = 5;
	double color = 1;
	int m_dist = 20;
	double increase_color = 1 / double(n_days + n_nurses);
	FILE *fpt;

	data.erase(data.end() - 4, data.end());
	mkdir("Results");
	string algorithmn = to_string(algorithm);
	string s_file = "./Results/Solfgurob_" + algorithmn + "_" + data + ".csv";
	fpt = fopen(s_file.c_str(), "w+");
	fprintf(fpt, "element,id_1,id_2,use,value,NOTAS\n");

	//Shape of the depots
	for (d = 0; d < n_days; d++)
		fprintf(fpt, "node,%d,%d,shape,box,\n", d, d);

	//Draw the depots (Aunque sea el mismo, dibujamos uno por día, en total son n_days depots).

	vector<double> color_v;

	color = increase_color;

	for (d = 0; d < n_days; d++)
	{
		fprintf(fpt, "node,%d,%d,text, Day %d,\n", d, d, d);
		fprintf(fpt, "node,%d,%d,size,%d,\n", d, d, size_depot);
		fprintf(fpt, "node,%d,%d,color_float,%.2f,\n", d, d, color);
		color = color + increase_color;
	}

	for (w = 0; w < n_nurses; w++)
	{
		color_v.push_back(color);
		color = color + increase_color;
	}
	int shape = 0;
	//Star 0
	//Triangle 1
	//Square
	int count;
	vector<int> exist_node;
	for (d = 0; d < (n_jobs)*n_days; d++)
	{
		exist_node.push_back(0);
	}



	//Nodes y variables
	double num;
	for (p = 0; p < n_patterns; p++)
	{
		for (d = 0; d < n_days; d++)
		{
			if (pattern[p].day_p[d] >= 1)
			{
				for (int ii = 0; ii < n_jobs; ii++)
				{
					i = d;
					if (ii != 0)
						i = d * (n_jobs - 1) + (n_days + ii - 1);

					for (int jj = 0; jj < n_jobs; jj++)
					{
						if (pattern[p].patient_p[jj])
						{
							j = d;
							if (jj != 0)
								j = d * (n_jobs - 1) + (n_days + jj - 1);

							for (w = 0; w < n_nurses; w++)
							{
								if (nurse[w].skill >= pattern[p].day_p[d] && nurse[w].patient_s[ii])
								{
									try
									{
										if (x[p][d][w][ii][jj].get(GRB_DoubleAttr_X) > 0.001)
										{
											num = x[p][d][w][ii][jj].get(GRB_DoubleAttr_X);
											if (ii != 0 || jj != 0)
											{
												if (exist_node[i] == 0 && i >= n_days)
												{
													fprintf(fpt, "node,%d,%d,text, Node %d,\n", i, job[ii].ID, ii);
													fprintf(fpt, "node,%d,%d,text, Skill_0 %d,\n", i, job[ii].ID, job[ii].skills[0]);
													fprintf(fpt, "node,%d,%d,text, Skill_1 %d,\n", i, job[ii].ID, job[ii].skills[1]);
													fprintf(fpt, "node,%d,%d,color_float,%.2f,\n", i, job[ii].ID, color_v[w]);
													exist_node[i]++;
													for (int dd = 0; dd < n_days; dd++)
													{
														if (pattern[p].day_p[dd] > 0)
															fprintf(fpt, "node,%d,%d,text, Day %d Req. %d,\n", i, job[ii].ID, dd, pattern[p].day_p[dd]);
													}

												}
												if (exist_node[j] == 0 && j >= n_days)
												{

													fprintf(fpt, "node,%d,%d,text, Node %d,\n", j, job[jj].ID, jj);
													fprintf(fpt, "node,%d,%d,text, Skill_0 %d,\n", j, job[jj].ID, job[jj].skills[0]);
													fprintf(fpt, "node,%d,%d,text, Skill_1 %d,\n", j, job[jj].ID, job[jj].skills[1]);
													fprintf(fpt, "node,%d,%d,color_float,%.2f,\n", j, job[jj].ID, color_v[w]);
													exist_node[j]++;
													for (int dd = 0; dd < n_days; dd++)
													{
														if (pattern[p].day_p[dd] > 0)
															fprintf(fpt, "node,%d,%d,text, Day %d Req. %d,\n", j, job[jj].ID, dd, pattern[p].day_p[dd]);
													}

												}


												num = x[p][d][w][ii][jj].get(GRB_DoubleAttr_X);

												fprintf(fpt, "edge,%d,%d,text,x_%d_%d = %.2f,\n", i, j, ii, jj, num);
												fprintf(fpt, "edge,%d,%d,text,Pattern = %d Nurse = %d Skill= %d,\n", i, j, p, w, nurse[w].skill);
												fprintf(fpt, "edge,%d,%d,width,%.2f,\n", i, j, 3 * num);
												if (num < 0.9999)
													fprintf(fpt, "edge,%d,%d,dashes,1,\n", i, j);

											}


										}
									}
									catch (GRBException e)
									{
									}
								}
							}
						}
					}
				}
			}
		}
	}


	fclose(fpt);



	//std::string pandas = "pip install pandas";
	//system(pandas.c_str());

	//std::string maplotlib = "pip install matplotlib";
	//system(maplotlib.c_str());

	//std::string pyvis = "pip install pyvis";
	//system(pyvis.c_str());



	string s_html = "./Results/Solfgurob_" + algorithmn + "_" + data + ".html";
	std::string filename;

	if (algorithm == -1 || algorithm > 10)
	{
		filename = "create_graphb.py " + s_file + " --out " + s_html;
	}
	else
	{
		filename = "create_graph.py " + s_file + " --out " + s_html;
	}



	//std::string filename = "create_graph.py -h";
	std::string command = "python ";
	command += filename;
	system(command.c_str());


	//Py_Initialize();
	//char ArchivoPython[] = "create_graph.py" + s_file;
	//FILE* file;
	////Leemos archivo
	//file = _Py_fopen(ArchivoPython, "r");

	////Ejecutamos
	//PyRun_SimpleFile(file, ArchivoPython);

	//PyRun_SimpleString("from time import time,ctime\n"
	//	"print 'Today is',ctime(time())\n");
	//system(“python create_graph.py test_data.csv”);
	//Py_Finalize();
}
void Data::write_solution_csv_n2(GRBModel &model, GRBVar***** x, GRBVar** z, GRBVar*** u)
{
	int i, j, k, d, s, w, p, s_i;

	int ii = 0;
	int jj = 0;
	int size_depot = 10;
	int size_related = 5;
	double color = 1;
	int m_dist = 20;
	double increase_color = 1 / double(n_days + n_nurses);
	FILE *fpt;

	data.erase(data.end() - 4, data.end());
	mkdir("Results");
	string algorithmn = to_string(algorithm);
	string s_file = "./Results/Solfgurob_" + algorithmn + "_" + data + ".csv";
	fpt = fopen(s_file.c_str(), "w+");
	fprintf(fpt, "element,id_1,id_2,use,value,NOTAS\n");

	//Shape of the depots
	for (d = 0; d < n_days; d++)
		fprintf(fpt, "node,%d,%d,shape,box,\n", d, d);

	//Draw the depots (Aunque sea el mismo, dibujamos uno por día, en total son n_days depots).

	vector<double> color_v;

	color = increase_color;

	for (d = 0; d < n_days; d++)
	{
		fprintf(fpt, "node,%d,%d,text, Day %d,\n", d, d, d);
		fprintf(fpt, "node,%d,%d,size,%d,\n", d, d, size_depot);
		fprintf(fpt, "node,%d,%d,color_float,%.2f,\n", d, d, color);
		color = color + increase_color;
	}

	for (w = 0; w < n_nurses; w++)
	{
		color_v.push_back(color);
		color = color + increase_color;
	}
	int shape = 0;
	//Star 0
	//Triangle 1
	//Square
	int count;
	vector<int> exist_node;
	for (d = 0; d < (n_jobs)*n_days; d++)
	{
		exist_node.push_back(0);
	}



	//Nodes y variables
	double num;
	for (p = 0; p < n_patterns; p++)
	{
		for (d = 0; d < n_days; d++)
		{
			if (pattern[p].day_p[d] >= 1)
			{
				for (int ii = 0; ii < n_jobs; ii++)
				{
					i = d;
					if (ii != 0)
						i = d * (n_jobs - 1) + (n_days + ii - 1);

					for (int jj = 0; jj < n_jobs; jj++)
					{
						if (pattern[p].patient_p[jj])
						{
							j = d;
							if (jj != 0)
								j = d * (n_jobs - 1) + (n_days + jj - 1);

							for (w = 0; w < n_nurses; w++)
							{
								if (nurse[w].skill >= pattern[p].day_p[d] && nurse[w].patient_s[ii])
								{
									try
									{
										if (x[p][d][w][ii][jj].get(GRB_DoubleAttr_X) > 0.001)
										{
											num = x[p][d][w][ii][jj].get(GRB_DoubleAttr_X);
											if (ii != 0 || jj != 0)
											{
												if (exist_node[i] == 0 && i >= n_days)
												{
													fprintf(fpt, "node,%d,%d,text, Node %d,\n", i, job[ii].ID, ii);
													fprintf(fpt, "node,%d,%d,text, Skill_0 %d,\n", i, job[ii].ID, job[ii].skills[0]);
													fprintf(fpt, "node,%d,%d,text, Skill_1 %d,\n", i, job[ii].ID, job[ii].skills[1]);
													fprintf(fpt, "node,%d,%d,color_float,%.2f,\n", i, job[ii].ID, color_v[w]);
													exist_node[i]++;
													for (int dd = 0; dd < n_days; dd++)
													{
														if (pattern[p].day_p[dd] > 0)
															fprintf(fpt, "node,%d,%d,text, Day %d Req. %d,\n", i, job[ii].ID, dd, pattern[p].day_p[dd]);
													}

												}
												if (exist_node[j] == 0 && j >= n_days)
												{

													fprintf(fpt, "node,%d,%d,text, Node %d,\n", j, job[jj].ID, jj);
													fprintf(fpt, "node,%d,%d,text, Skill_0 %d,\n", j, job[jj].ID, job[jj].skills[0]);
													fprintf(fpt, "node,%d,%d,text, Skill_1 %d,\n", j, job[jj].ID, job[jj].skills[1]);
													fprintf(fpt, "node,%d,%d,color_float,%.2f,\n", j, job[jj].ID, color_v[w]);
													exist_node[j]++;
													for (int dd = 0; dd < n_days; dd++)
													{
														if (pattern[p].day_p[dd] > 0)
															fprintf(fpt, "node,%d,%d,text, Day %d Req. %d,\n", j, job[jj].ID, dd, pattern[p].day_p[dd]);
													}

												}


												num = x[p][d][w][ii][jj].get(GRB_DoubleAttr_X);

												fprintf(fpt, "edge,%d,%d,text,x_%d_%d = %.2f,\n", i, j, ii, jj, num);
												fprintf(fpt, "edge,%d,%d,text,Pattern = %d Nurse = %d Skill= %d,\n", i, j, p, w, nurse[w].skill);
												fprintf(fpt, "edge,%d,%d,width,%.2f,\n", i, j, 3 * num);
												if (num < 0.9999)
													fprintf(fpt, "edge,%d,%d,dashes,1,\n", i, j);

											}


										}
									}
									catch (GRBException e)
									{
									}
								}
							}
						}
					}
				}
			}
		}
	}


	fclose(fpt);



	//std::string pandas = "pip install pandas";
	//system(pandas.c_str());

	//std::string maplotlib = "pip install matplotlib";
	//system(maplotlib.c_str());

	//std::string pyvis = "pip install pyvis";
	//system(pyvis.c_str());



	string s_html = "./Results/Solfgurob_" + algorithmn + "_" + data + ".html";
	std::string filename;

	if (algorithm == -1 || algorithm > 10)
	{
		filename = "create_graphb.py " + s_file + " --out " + s_html;
	}
	else
	{
		filename = "create_graph.py " + s_file + " --out " + s_html;
	}



	//std::string filename = "create_graph.py -h";
	std::string command = "python ";
	command += filename;
	system(command.c_str());


	//Py_Initialize();
	//char ArchivoPython[] = "create_graph.py" + s_file;
	//FILE* file;
	////Leemos archivo
	//file = _Py_fopen(ArchivoPython, "r");

	////Ejecutamos
	//PyRun_SimpleFile(file, ArchivoPython);

	//PyRun_SimpleString("from time import time,ctime\n"
	//	"print 'Today is',ctime(time())\n");
	//system(“python create_graph.py test_data.csv”);
	//Py_Finalize();
}

void Data::write_solution_csv_n5(GRBModel &model, GRBVar***** x, GRBVar** z)
{
	int i, j, k, d, s, w, p, s_i;

	int ii = 0;
	int jj = 0;
	int size_depot = 10;
	int size_related = 5;
	double color = 1;
	int m_dist = 20;
	double increase_color = 1 / double(n_days + n_nurses);
	FILE *fpt;

	data.erase(data.end() - 4, data.end());
	mkdir("Results");
	string algorithmn = to_string(algorithm);
	string s_file = "./Results/Solfgurob_" + algorithmn + "_" + data + ".csv";
	fpt = fopen(s_file.c_str(), "w+");
	fprintf(fpt, "element,id_1,id_2,use,value,NOTAS\n");

	//Shape of the depots
	for (d = 0; d < n_days; d++)
		fprintf(fpt, "node,%d,%d,shape,box,\n", d, d);

	//Draw the depots (Aunque sea el mismo, dibujamos uno por día, en total son n_days depots).

	vector<double> color_v;

	color = increase_color;

	for (d = 0; d < n_days; d++)
	{
		fprintf(fpt, "node,%d,%d,text, Day %d,\n", d, d, d);
		fprintf(fpt, "node,%d,%d,size,%d,\n", d, d, size_depot);
		fprintf(fpt, "node,%d,%d,color_float,%.2f,\n", d, d, color);
		color = color + increase_color;
	}

	for (w = 0; w < n_nurses; w++)
	{
		color_v.push_back(color);
		color = color + increase_color;
	}
	int shape = 0;
	//Star 0
	//Triangle 1
	//Square
	int count;
	vector<int> exist_node;
	for (d = 0; d < (n_jobs)*n_days; d++)
	{
		exist_node.push_back(0);
	}



	//Nodes y variables
	double num;
	for (p = 0; p < n_patterns; p++)
	{
		for (d = 0; d < n_days; d++)
		{
			if (pattern[p].day_p[d] >= 1)
			{
				for (int ii = 0; ii < n_jobs; ii++)
				{
					i = d;
					if (ii != 0)
						i = d * (n_jobs - 1) + (n_days + ii - 1);

					for (int jj = 0; jj < n_jobs; jj++)
					{
						if (pattern[p].patient_p[jj])
						{
							j = d;
							if (jj != 0)
								j = d * (n_jobs - 1) + (n_days + jj - 1);

							for (w = 0; w < n_nurses; w++)
							{
								if (nurse[w].skill >= pattern[p].day_p[d] && nurse[w].patient_s[ii])
								{
									try
									{
										if (x[p][d][w][ii][jj].get(GRB_DoubleAttr_X) > 0.001)
										{
											num = x[p][d][w][ii][jj].get(GRB_DoubleAttr_X);
											if (ii != 0 || jj != 0)
											{
												if (exist_node[i] == 0 && i >= n_days)
												{
													fprintf(fpt, "node,%d,%d,text, Node %d,\n", i, job[ii].ID, ii);
													fprintf(fpt, "node,%d,%d,text, Skill_0 %d,\n", i, job[ii].ID, job[ii].skills[0]);
													fprintf(fpt, "node,%d,%d,text, Skill_1 %d,\n", i, job[ii].ID, job[ii].skills[1]);
													fprintf(fpt, "node,%d,%d,color_float,%.2f,\n", i, job[ii].ID, color_v[w]);
													exist_node[i]++;
													for (int dd = 0; dd < n_days; dd++)
													{
														if (pattern[p].day_p[dd] > 0)
															fprintf(fpt, "node,%d,%d,text, Day %d Req. %d,\n", i, job[ii].ID, dd, pattern[p].day_p[dd]);
													}

												}
												if (exist_node[j] == 0 && j >= n_days)
												{

													fprintf(fpt, "node,%d,%d,text, Node %d,\n", j, job[jj].ID, jj);
													fprintf(fpt, "node,%d,%d,text, Skill_0 %d,\n", j, job[jj].ID, job[jj].skills[0]);
													fprintf(fpt, "node,%d,%d,text, Skill_1 %d,\n", j, job[jj].ID, job[jj].skills[1]);
													fprintf(fpt, "node,%d,%d,color_float,%.2f,\n", j, job[jj].ID, color_v[w]);
													exist_node[j]++;
													for (int dd = 0; dd < n_days; dd++)
													{
														if (pattern[p].day_p[dd] > 0)
															fprintf(fpt, "node,%d,%d,text, Day %d Req. %d,\n", j, job[jj].ID, dd, pattern[p].day_p[dd]);
													}

												}


												num = x[p][d][w][ii][jj].get(GRB_DoubleAttr_X);

												fprintf(fpt, "edge,%d,%d,text,x_%d_%d = %.2f,\n", i, j, ii, jj, num);
												fprintf(fpt, "edge,%d,%d,text,Pattern = %d Nurse = %d Skill= %d,\n", i, j, p, w, nurse[w].skill);
												fprintf(fpt, "edge,%d,%d,width,%.2f,\n", i, j, 3 * num);
												if (num < 0.9999)
													fprintf(fpt, "edge,%d,%d,dashes,1,\n", i, j);

											}


										}
									}
									catch (GRBException e)
									{
									}
								}
							}
						}
					}
				}
			}
		}
	}


	fclose(fpt);



	//std::string pandas = "pip install pandas";
	//system(pandas.c_str());

	//std::string maplotlib = "pip install matplotlib";
	//system(maplotlib.c_str());

	//std::string pyvis = "pip install pyvis";
	//system(pyvis.c_str());



	string s_html = "./Results/Solfgurob_" + algorithmn + "_" + data + ".html";
	std::string filename;

	if (algorithm == -1 || algorithm > 10)
	{
		filename = "create_graphb.py " + s_file + " --out " + s_html;
	}
	else
	{
		filename = "create_graph.py " + s_file + " --out " + s_html;
	}



	//std::string filename = "create_graph.py -h";
	std::string command = "python ";
	command += filename;
	system(command.c_str());


	//Py_Initialize();
	//char ArchivoPython[] = "create_graph.py" + s_file;
	//FILE* file;
	////Leemos archivo
	//file = _Py_fopen(ArchivoPython, "r");

	////Ejecutamos
	//PyRun_SimpleFile(file, ArchivoPython);

	//PyRun_SimpleString("from time import time,ctime\n"
	//	"print 'Today is',ctime(time())\n");
	//system(“python create_graph.py test_data.csv”);
	//Py_Finalize();
}