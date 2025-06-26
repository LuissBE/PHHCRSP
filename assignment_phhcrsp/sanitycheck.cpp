#include "Data.h"
#include <algorithm>
using namespace std;

bool Data::sanitycheck(vector<vector<vector<vector<vector<bool>>>>> x) //Checks whether solution is valid for the model, returns false if not
{
	if (!check_constraints_patterns(x))
	{
		cout << "Error in check_constraints_patterns" << endl;
		return false;
	}
	if (!check_constraints_skills(x))
	{
		cout << "Error in check_constraints_skills" << endl;
		return false;
	}
	if (!check_constraints_maxday(x))
	{
		cout << "Error in check_constraints_maxday" << endl;
		return false;
	}
	if (!check_constraints_symmetry(x))
	{
		return false;
	}
	if (!check_constraints_maxwork(x))
	{
		return false;
	}
	return true;
}

bool Data::check_constraints_patterns(vector<vector<vector<vector<vector<bool>>>>> x)
{
	int p, dp;
	int d0, d1;
	int i, j, w, s;
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
					int Expr = 0;
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
					//model.addConstr(Expr <= 0);
					if (Expr > 0)
					{
						cout << "Error in pattern " << p << ", job " << j << ", day " << d0 << " to day " << d1 << endl;
						cout << "pattern[" << p << "].day_req = { ";
						for (int k = 0; k < pattern[p].day_req.size(); k++)
						{
							cout << pattern[p].day_req[k] << ", ";
						}
						cout << "}" << endl;
						int problem_j_cluster_number = 1;
						for(int k = 0 ; k < j ; k++)
						{
							if (job[k].cluster == job[j].cluster)
							{
								problem_j_cluster_number++;
							}
						}
						cout << "job is the " << problem_j_cluster_number << "th in cluster " << job[j].cluster << endl;

						for (w = 0; w < n_nurses; w++)
						{
							s_w = nurse[w].skill;
							if (s_w >= s_p0)
							{
								for (i = 0; i < n_jobs; i++)
								{
									if (i != j && nurse[w].patient_s[i])
									{
										if (x[p][d0][w][i][j])
										{
											cout << "x[" << p << "][" << d0 << "][" << w << "][" << i << "][" << j << "] = 1" << endl;
										}
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
										if (x[p][d1][w][i][j])
										{
											cout << "x[" << p << "][" << d1 << "][" << w << "][" << i << "][" << j << "] = 1" << endl;
										}
									}
								}
							}
						}

						return false;
					}
				}

				if (pattern[p].day_req.size() > 1)
				{
					d0 = pattern[p].day_req[pattern[p].day_req.size() - 1];
					d1 = pattern[p].day_req[0];
					s_p0 = pattern[p].day_p[d0];
					s_p1 = pattern[p].day_p[d1];
					int Expr = 0;
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
					//model.addConstr(Expr <= 0);
					if(Expr > 0)
					{
						return false;
					}
				}
			}

		}
	}
	return true;
}

bool Data::check_constraints_skills(vector<vector<vector<vector<vector<bool>>>>> x)
{
	int i, j, d, w, p, s;
	for (j = 1; j < n_jobs; j++)
	{
		for (s = 0; s < n_skills; s++)
		{
			if (job[j].skills[s] >= 1)
			{
				int Expr = 0;
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
				//model.addConstr(Expr == job[j].skills[s]);
				if (Expr != job[j].skills[s])
				{
					cout << "VIOLATION of constraints_skills! " << endl;
					cout << "Error in job " << j << ", skill " << s << " --> job[j].skill[s] = " << job[j].skills[s] << endl;
					cout << "Expr = " << Expr << endl;
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
												if(x[p][d][w][i][j])
												{
													cout << "x[" << p << "][" << d << "][" << w << "][" << i << "][" << j << "] = " << 1 << endl;
												}
											}
										}
									}
								}
							}
						}
					}
					return false;
				}
			}
		}
	}
	return true;
}
bool Data::check_constraints_maxday(vector<vector<vector<vector<vector<bool>>>>> x)
{
	int i, j, d, w, p, s, s_p;
	for (j = 1; j < n_jobs; j++)
	{
		for (d = 0; d < n_days; d++)
		{
			int Expr = 0;

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
			//model.addConstr(Expr <= 1);
			if (Expr > 1)
			{
				return false;
			}
		}
	}
	return true;
}

bool Data::check_constraints_symmetry(vector<vector<vector<vector<vector<bool>>>>> x)
{
	int i, j, d, w, p, s, s_p;

	for (j = 1; j < n_jobs; j++)
	{
		for (d = 0; d < n_days; d++)
		{
			for (w = 0; w < n_nurses; w++)
			{
				int Expr = 0;
				for (p = 0; p < n_patterns; p++)
				{
					s_p = pattern[p].day_p[d];
					if (nurse[w].skill >= s_p)
					{
						if (pattern[p].patient_p[j])
						{
							for (i = 0; i < n_jobs; i++)
							{
								if (i != j && (nurse[w].patient_s[i] && s_p >= 1 || i == 0))
									Expr += x[p][d][w][i][j];
							}
						}
						if (nurse[w].patient_s[j])
						{
							for (i = 0; i < n_jobs; i++)
							{
								if (i != j && (pattern[p].patient_p[i] && s_p >= 1 || i == 0))
									Expr -= x[p][d][w][j][i];
							}
						}

					}
				}
				//model.addConstr(Expr == 0);
				if (Expr != 0)
				{
					cout << "VIOLATION of constraints_symmetry! " << endl;
					cout << "job " << j << " (cluster " << job[j].cluster  << ") day " << d  << "nurse" << w << endl;
					for (p = 0; p < n_patterns; p++)
					{
						s_p = pattern[p].day_p[d];
						if (nurse[w].skill >= s_p)
						{
							if (pattern[p].patient_p[j])
							{
								for (i = 0; i < n_jobs; i++)
								{
									if (i != j && (nurse[w].patient_s[i] && s_p >= 1 || i == 0))
										if (x[p][d][w][i][j])
											cout << "x[" << p << "][" << d << "][" << w << "][" << i << "][" << j << "] = " << 1 << endl;

								}
							}
							if (nurse[w].patient_s[j])
							{
								for (i = 0; i < n_jobs; i++)
								{
									if (i != j && (pattern[p].patient_p[i] && s_p >= 1 || i == 0))
										if (x[p][d][w][j][i])
											cout << "x[" << p << "][" << d << "][" << w << "][" << j << "][" << i << "] = " << 1 << endl;
								}
							}
						}

					}
					return false;
				}
			}
		}
	}
	return true;
}


bool Data::check_constraints_maxwork(vector<vector<vector<vector<vector<bool>>>>> x)
{
	int i, j, d, w, p, s;
	int num;
	double cost;

	for (w = 0; w < n_nurses; w++)
	{

		for (d = 0; d < n_days; d++)
		{
			int Expr = 0;
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
			//model.addConstr(Expr <= nurse[w].D);
			if (Expr > nurse[w].D)
			{
				cout << "VIOLATION of constraints_maxwork! " << endl;
				cout << "Caregiver: " << w << " day " << d << " works " << Expr << ", max " << nurse[w].D << endl;
				return false;
			}
		}
	}
	return true;
}