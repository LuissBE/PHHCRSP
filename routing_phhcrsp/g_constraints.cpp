#include "Data.h"

void Data::binary_vars(GRBModel &model, GRBVar** z, GRBVar** u, GRBVar**** x)
{
	int i,j, p,w,d;
	for (j = 1; j < n_jobs; j++)
	{
		z[j] = new GRBVar[n_patterns];
		for (p = 0; p < n_patterns; p++)
		{
			if (rl)
			{
				string s = "z(" + to_string(j) + "," + to_string(p) + ")";
				z[j][p] = model.addVar(0.0, 1.0, 0.0, GRB_CONTINUOUS, s);
			}
			else
			{
				string s = "z(" + to_string(j) + "," + to_string(p) + ")";
				z[j][p] = model.addVar(0.0, 1.0, 0.0, GRB_BINARY, s);
			}
		}
	}

	for (w = 0; w < n_nurses; w++)
	{
		u[w] = new GRBVar[n_jobs];
		for (j = 1; j < n_jobs; j++)
		{
			if (rl)
			{
				string s = "u(" + to_string(w) + "," + to_string(j) + ")";
				u[w][j] = model.addVar(0.0, 1.0, 0.0, GRB_CONTINUOUS, s);
			}
			else
			{
				string s = "u(" + to_string(w) + "," + to_string(j) + ")";
				u[w][j] = model.addVar(0.0, 1.0, 0.0, GRB_BINARY, s);
			}
		}
	}

	for (d = 0; d < n_days; d++)
	{
		x[d] = new GRBVar**[n_nurses];
		for (w = 0; w < n_nurses; w++)
		{
			x[d][w] = new GRBVar*[n_jobs];
			for (i = 0; i < n_jobs; i++)
			{
				x[d][w][i] = new GRBVar[n_jobs];
				for (j = 0; j < n_jobs; j++)
				{
					if (rl)
					{
						string s = "x(" + to_string(d) + "," + to_string(w) + "," + to_string(i) + "," + to_string(j) + ")";
						x[d][w][i][j] = model.addVar(0.0, 1.0, 0.0, GRB_CONTINUOUS, s);
					}
					else
					{
						string s = "x(" + to_string(d) + "," + to_string(w) + "," + to_string(i) + "," + to_string(j) + ")";
						x[d][w][i][j] = model.addVar(0.0, 1.0, 0.0, GRB_BINARY, s);
					}
				}
			}
		}
	}
}
void Data::integer_vars(GRBModel &model, GRBVar*** y)
{
	int i, j, d;

	for (d = 0; d < n_days; d++)
	{
		y[d] = new GRBVar*[n_jobs];
		for (i = 0; i < n_jobs; i++)
		{
			y[d][i] = new GRBVar[n_jobs];
			for (j = 0; j < n_jobs; j++)
			{
				if (rl)
				{
					string s = "y(" + to_string(d) + "," + to_string(i) + "," + to_string(j) + ")";
					y[d][i][j] = model.addVar(0.0, n_jobs, 0.0, GRB_CONTINUOUS, s);
				}
				else
				{
					string s = "y(" + to_string(d) +"," + to_string(i) + "," + to_string(j) + ")";
					y[d][i][j] = model.addVar(0.0, n_jobs, 0.0, GRB_INTEGER, s);
				}
			}
		}
	}
	
}
void Data::linear_vars(GRBModel &model, GRBVar** D)
{
	int  w, d;
	for (w = 0; w < n_nurses; w++)
	{
		D[w] = new GRBVar[n_days];
		for (d = 0; d < n_days; d++)
		{
			string s = "D(" + to_string(w) + "," + to_string(d) + ")";
			D[w][d] = model.addVar(0.0,nurse[w].D, 0.0, GRB_CONTINUOUS, s);
		}
	}

}

void Data::constraints_1(GRBModel &model, GRBVar**** x, GRBVar** z) 
{
	//La restricción indica que cada paciente es visitado como máximo una vez por una única enfermera en los días en que se tenga que visitar y cero los días en los que no se visite.
	int i, j, d, w, p;
	for (d = 0; d < n_days; d++)
	{
		for (j = 1; j < n_jobs; j++)
		{
			GRBLinExpr Expr = 0;
			for (i = 0; i < n_jobs; i++)
			{
					for (w = 0; w < n_nurses; w++)
					{
						Expr += x[d][w][i][j];
					}
			}

			for (p = 0; p<n_patterns; p++)
			{
				if (pattern[p].day_p[d] >= 1)
				{
					Expr -= z[j][p];
				}
			}
			model.addConstr(Expr <= 0);
		}
	}
}
void Data::constraints_2(GRBModel &model, GRBVar**** x, GRBVar** z) 
{
	//Si se ha asignado una visita del skill k  el día d para el paciente j, exactamente un operador que pueda realizar la skill k(o alguna superior) será asignado a ese paciente.
	int i, j, d, w,k,sw, p;

	for (d = 0; d < n_days; d++)
	{
		for (j = 1; j < n_jobs; j++)
		{
			for (k = 0; k < n_skills; k++)
			{
				GRBLinExpr Expr = 0;
				for (i = 0; i < n_jobs; i++)
				{
						for (w = 0; w < n_nurses; w++)
						{
							if (nurse[w].skill >= k + 1)
								Expr += x[d][w][i][j];
						}
				}

				for (p = 0; p < n_patterns; p++)
				{
					if (pattern[p].day_p[d] == k+1)
					{
						Expr -= z[j][p];
					}
				}
				model.addConstr(Expr >= 0);
			}
		}
	}
}
void Data::constraints_3(GRBModel &model, GRBVar** z) 
{
	//A cada paciente se le asigna un único patrón.
	int j, p;
	for (j = 1; j < n_jobs; j++)
	{
		GRBLinExpr Expr = 0;
		for (p = 0; p < n_patterns; p++)
			Expr += z[j][p];
		model.addConstr(Expr == 1);
	}
}
void Data::constraints_4(GRBModel &model, GRBVar** u)
{
	//(4)	A cada paciente se le asignan como máximo T enfermeras.
	int w, j;
	for (j = 1; j < n_jobs; j++)
	{
		GRBLinExpr Expr = 0;
		for (w = 0; w < n_nurses; w++)
			Expr += u[w][j];
		model.addConstr(Expr <= job[j].max_nurses);
	}
}
void Data::constraints_5(GRBModel &model, GRBVar**** x, GRBVar** u)
{
	//(5)	Si la enfermera w va el día d del paciente i al paciente j, entonces el paciente j se ha asignado a la enfermera w.
	int i, j, d, w;
	for (i = 0; i < n_jobs; i++)
	{
		for (j = 1; j < n_jobs; j++)
		{
				for (d = 0; d < n_days; d++)
				{
					for (w = 0; w < n_nurses; w++)
					{
						GRBLinExpr Expr = 0;
						Expr += x[d][w][i][j]- u[w][j];
						model.addConstr(Expr <= 0);
					}
				}
		}
	}
}
void Data::constraints_6(GRBModel &model, GRBVar**** x, GRBVar** u)
{
	//(6)	Si la enfermera w no visita al paciente d durante el horizonte, entonces no puede asignarse ese paciente a la enfermera.
	int i, j, d, w;
	
	for (j = 1; j < n_jobs; j++)
	{
		for (w = 0; w < n_nurses; w++)
		{
			GRBLinExpr Expr = 0;
			Expr += u[w][j];
			for (i = 0; i < n_jobs; i++)
			{
				if (i != j)
				{
					for (d = 0; d < n_days; d++)
					{
						Expr -= x[d][w][i][j];
					}
				}
				
			}
			model.addConstr(Expr <= 0);
		}
	}
}
void Data::constraints_7(GRBModel &model, GRBVar**** x, GRBVar** D)
{
	//(7)	La suma de los tiempos de trabajo y de desplazamientos asignados a la enfermera w el día d no puede exceder su jornada laboral.
	int num;
	int i, j, d, w;
	for (d = 0; d < n_days; d++)
	{
		for (w = 0; w < n_nurses; w++)
		{
			GRBLinExpr Expr = 0;
			Expr += D[w][d];
			
			for (i = 0; i < n_jobs; i++)
			{
				for (j = 0; j < n_jobs; j++)
				{
					
					num = 0;
					if (i != j)
					{
						num += distance[job[i].cluster][job[j].cluster];
						if (j != 0)
							num += job[j].time;
					}
					Expr -= num * x[d][w][i][j];
				}
			}
			model.addConstr(Expr == 0);
		}
	}
	for (d = 0; d < n_days; d++)
	{
		for (w = 0; w < n_nurses; w++)
		{
			GRBLinExpr Expr = 0;
			Expr += D[w][d];
			model.addConstr(Expr <= nurse[w].D);
		}
	}
}
void Data::constraints_8(GRBModel &model, GRBVar**** x)
{
	//(8)	El número de arcos que entras es igual al número de arcos que salen para cada enfermera w cada día d.
	int i, j, d, w;
	for (j = 1; j < n_jobs; j++)
	{
		for (d = 0; d < n_days; d++)
		{
			for (w = 0; w < n_nurses; w++)
			{
				GRBLinExpr Expr = 0;
				for (i = 0; i < n_jobs; i++)
				{
					Expr += x[d][w][i][j];
				}
				for (i = 0; i < n_jobs; i++)
				{
					Expr -= x[d][w][j][i];
				}
				model.addConstr(Expr == 0);
			}
		}
	}

		//for (d = 0; d < n_days; d++)
		//{
		//	for (w = 0; w < n_nurses; w++)
		//	{
		//		GRBLinExpr Expr = 0;
		//		for (i = 0; i < n_jobs; i++)
		//		{
		//			Expr += x[d][w][0][i];
		//		}
		//		model.addConstr(Expr == 1);
		//	}
		//}
}
void Data::constraints_9(GRBModel &model, GRBVar*** y, GRBVar** z)
{
	int j, d, p;
	for (d = 0; d < n_days; d++)
	{

		GRBLinExpr Expr = 0;
		for (j = 1; j < n_jobs; j++)
		{
			Expr += y[d][0][j];
		
			for (p = 0; p < n_patterns; p++)
			{
				if (pattern[p].day_p[d] >= 1)
				{
					Expr -= z[j][p];
				}
			}
		}
		model.addConstr(Expr == 0);
	}
}
void Data::constraints_10(GRBModel &model, GRBVar*** y, GRBVar** z)
{

	int i,j, d, p;
	for (j = 1; j < n_jobs; j++)
	{
		for (d = 0; d < n_days; d++)
		{

			GRBLinExpr Expr = 0;
			for (i = 0; i < n_jobs; i++)
			{
				Expr += y[d][i][j] - y[d][j][i];
			}
			for (p = 0; p < n_patterns; p++)
			{
				if (pattern[p].day_p[d] >= 1)
				{
					Expr -= z[j][p];
				}
			}
		
			model.addConstr(Expr == 0);
		}
	}
}
void Data::constraints_11(GRBModel &model, GRBVar*** y, GRBVar**** x)
{
	int i, j, d, w;
	for (i = 0; i < n_jobs; i++)
	{
		for (j = 0; j < n_jobs; j++)
		{
			
				for (d = 0; d < n_days; d++)
				{
					GRBLinExpr Expr = 0;
					Expr += y[d][i][j];
					for (w = 0; w < n_nurses; w++)
					{
						Expr -= (n_jobs-1) * x[d][w][i][j];
					}
					model.addConstr(Expr <= 0);
				}
		}
	}
}
void Data::constraints_o1(GRBModel &model, GRBVar** D, GRBVar m)
{
	int d, w;
	int num;
	for (w = 0; w < n_nurses; w++)
	{
		GRBLinExpr Expr = 0;
		num = n_days * nurse[w].D;
		for (d = 0; d < n_days; d++)
		{
			Expr += D[w][d];
		}
		model.addConstr(Expr >= num*m);
	}
}
void Data::constraints_o2(GRBModel &model, GRBVar** D, GRBVar m)
{
	int d, w;
	int num;
	for (w = 0; w < n_nurses; w++)
	{
		GRBLinExpr Expr = 0;
		for (d = 0; d < n_days; d++)
		{
			Expr += D[w][d];
		}
		num = n_days * nurse[w].D;
		model.addConstr(Expr <= num * m);
	}
}


void Data::constraints_o14(GRBModel &model, GRBVar***** x, GRBVar m)
{
	int d, w,i,j,p;
	int num;
	double cost;
	for (w = 0; w < n_nurses; w++)
	{
		GRBLinExpr Expr = 0;
		num = n_days * nurse[w].D;

		
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
		model.addConstr(Expr >= num * m);
	}
}
void Data::constraints_o24(GRBModel &model, GRBVar***** x, GRBVar m)
{
	int d, w, i, j, p;
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
		num = n_days * nurse[w].D;
		model.addConstr(Expr <= num * m);
	}
}

void Data::constraints_goaldummy(GRBModel& model, GRBVar m,GRBVar M, GRBVar d)
{
	GRBLinExpr Expr = M - m - eqgoal;
	model.addConstr(Expr <= d);
}
void Data::constraints_o13(GRBModel &model, GRBVar* D, GRBVar m)
{
	int d, w;
	int num;
	for (w = 0; w < n_nurses; w++)
	{
		GRBLinExpr Expr = 0;
		num = n_days * nurse[w].D;
		Expr += D[w];
		model.addConstr(Expr >= num * m);
	}
}
void Data::constraints_o23(GRBModel &model, GRBVar* D, GRBVar m)
{
	int d, w;
	int num;
	for (w = 0; w < n_nurses; w++)
	{
		GRBLinExpr Expr = 0;
		Expr += D[w];
		num = n_days * nurse[w].D;
		model.addConstr(Expr <= num * m);
	}
}

void Data::constraints_o15(GRBModel &model, GRBVar*** u, GRBVar m)
{
	int d, w;
	int num;
	for (w = 0; w < n_nurses; w++)
	{
		GRBLinExpr Expr = 0;
		num = n_days * nurse[w].D;
		for (d = 0; d < n_days; d++)
		{
			Expr += u[d][w][0];
		}
		model.addConstr(Expr >= num * m);
	}
}
void Data::constraints_o25(GRBModel &model, GRBVar*** u, GRBVar m)
{
	int d, w;
	int num;
	for (w = 0; w < n_nurses; w++)
	{
		GRBLinExpr Expr = 0;
		for (d = 0; d < n_days; d++)
		{
			Expr += u[d][w][0];
		}
		num = n_days * nurse[w].D;
		model.addConstr(Expr <= num * m);
	}
}


void Data::constraints_zero(GRBModel &model, GRBVar** z, GRBVar**** x)
{
	int i,p,j,k,w,d;
	int cardinal;
	bool flag;
	for (j = 1; j < n_jobs; j++)
	{
		for (p = 0; p < n_patterns; p++)
		{
			flag = true;
			for (k = 0; k < n_skills; k++)
			{
				cardinal = 0;
				for (d = 0; d < n_days; d++)
				{
					if (pattern[p].day_p[d] == k + 1)
						cardinal++;
				}
				if (cardinal != job[j].skills[k])
					flag = false;
			}
			if (flag == false)
			{
				GRBLinExpr Expr = 0;
				Expr += z[j][p];
				model.addConstr(Expr == 0);
			}
		}
	}
	int request_i;
	int request_j;
	int request;

	for (i = 1; i < n_jobs; i++)
	{
		for (j = 1; i < n_jobs; i++)
		{
			request_i=2;
			if (job[i].skills[0] > 0)
				request_i = 1;
			request_j = 2;
			if (job[j].skills[0] > 0)
				request_j = 1;

			request = min(request_i, request_j);

			for (w = 0; w < n_nurses; w++)
			{
				if (nurse[w].skill < request)
				{
					for (d = 0; d < n_days; d++)
					{
						GRBLinExpr Expr = 0;
						Expr += x[d][w][i][j];
						model.addConstr(Expr == 0);
					}
				}

			}
		}
	}
}
void Data::constraints_18(GRBModel &model, GRBVar**** x)
{
	int i, j, d, w,c;

	for (d = 0; d < n_days; d++)
	{
		for (w = 0; w < n_nurses; w++)
		{
			for (c = 0; c < n_clusters; c++)
			{
				for (i = 1; i < n_jobs; i++)
				{
					if (job[i].cluster == c)
					{
						for (j = i-1; j >=1; j--)
						{
							if (job[j].cluster == c)
							{
								GRBLinExpr Expr = 0;
								Expr += x[d][w][i][j];
								model.addConstr(Expr == 0);
							}
						}
					}
				}
			}
		}
	}
}/*
void Data::constraints_19(GRBModel &model, GRBVar**** x)
{
	int i, j, d, w, c;

	for (d = 0; d < n_days; d++)
	{
		for (w = 0; w < n_nurses; w++)
		{
			for (c = 0; c < n_clusters; c++)
			{
				GRBLinExpr Expr = 0;
				for (i = 1; i < n_jobs; i++)
				{
					if (job[i].cluster != c)
					{
						for (j = 1; j <n_jobs; j++)
						{
							if (job[j].cluster == c)
							{
								Expr += x[d][w][i][j];
							}
						}
					}
				}
				model.addConstr(Expr <= 1);
			}
		}
	}
}*/


void Data::constraints_19(GRBModel &model, GRBVar**** x)
{
	int i, j, d, w, c;

	for (d = 0; d < n_days; d++)
	{
		for (w = 0; w < n_nurses; w++)
		{
			for (c = 0; c < n_clusters; c++)
			{
				GRBLinExpr Expr = 0;

				for (i = 0; i < n_jobs; i++)
				{
					if (job[i].cluster != c)
					{
						for (j = 1; j < n_jobs; j++)
						{
							if (job[j].cluster == c)
							{
								Expr += x[d][w][i][j];
							}
						}
					}
				}
				model.addConstr(Expr <= 1);
			}
		}
	}
}

void Data::write_sol(GRBModel &model, GRBVar** z, GRBVar** u, GRBVar**** x, GRBVar*** y, GRBVar** D)
{
	int i, j, p, w, d;

	double value = model.get(GRB_DoubleAttr_ObjVal);
	double best_bound = model.get(GRB_DoubleAttr_ObjBound);
	fprintf(stderr, "Instance:\t%s\tObjValue:\t%.4f\tLB:\t%.4f", data.c_str(), value, best_bound);
	fprintf(stdout, "\n");





	for (j = 1; j < n_jobs; j++)
	{
		for (p = 0; p < n_patterns; p++)
		{
			try
			{
				if (z[j][p].get(GRB_DoubleAttr_X) > 0.001)
				{
					fprintf(stdout, "z(%d,%d)\t%.2f\n", j, p, z[j][p].get(GRB_DoubleAttr_X));
				}
			}
			catch (GRBException e)
			{
			}
		}
	}

	for (w = 0; w < n_nurses; w++)
	{
		for (j = 1; j < n_jobs; j++)
		{
			try
			{
				if (u[w][j].get(GRB_DoubleAttr_X) > 0.001)
				{
					fprintf(stdout, "u(%d,%d)\t%.2f\n", w, j, u[w][j].get(GRB_DoubleAttr_X));
				}
			}
			catch (GRBException e)
			{
			}
		}
	}

	for (d = 0; d < n_days; d++)
	{
		for (w = 0; w < n_nurses; w++)
		{
			for (i = 0; i < n_jobs; i++)
			{
				for (j = 0; j < n_jobs; j++)
				{
					try
					{
						if (x[d][w][i][j].get(GRB_DoubleAttr_X) > 0.001)
						{
							fprintf(stdout, "x(%d,%d,%d,%d)\t%.2f\n", d, w, i, j, x[d][w][i][j].get(GRB_DoubleAttr_X));
						}
					}
					catch (GRBException e)
					{
					}

				}
			}
		}
	}


	for (d = 0; d < n_days; d++)
	{
		for (i = 0; i < n_jobs; i++)
		{
			for (j = 0; j < n_jobs; j++)
			{
				try
				{
					if (y[d][i][j].get(GRB_DoubleAttr_X) > 0.001)
					{
						fprintf(stdout, "y(%d,%d,%d)\t%.2f\n", d, i, j, y[d][i][j].get(GRB_DoubleAttr_X));
					}
				}
				catch (GRBException e)
				{
				}
			}
		}
	}

	double max_carga = 9999999;
	int w_carga; 
	double current;
	for (w = 0; w < n_nurses; w++)
	{
		current = 0;
		for (d = 0; d < n_days; d++)
		{
			try
			{
				if (D[w][d].get(GRB_DoubleAttr_X) > 0.001)
				{
					current += D[w][d].get(GRB_DoubleAttr_X);
					fprintf(stdout, "D(%d,%d)\t%.2f\n", w,d, D[w][d].get(GRB_DoubleAttr_X));
				}
			}
			catch (GRBException e)
			{
			}
		}
		if ((current / (n_days * nurse[w].D)) <= max_carga)
		{
			max_carga = (current / (n_days * nurse[w].D));
			w_carga = w;
		}
	}
	fprintf(stdout, "The caregiver with minimum workload is \t%d\n", w_carga);
}
void Data::write_solution_csv(GRBModel &, GRBVar** z, GRBVar** u, GRBVar**** x, GRBVar*** y, GRBVar** D)
{
	int i, j, k, d,s, w,s_i;

	int ii = 0;
	int jj = 0;
	int size_depot = 10;
	int size_related = 5;
	double color=1;
	int m_dist = 20;
	double increase_color = 1 / double(n_days+n_nurses);
	FILE *fpt;

	data.erase(data.end() - 4, data.end());
	mkdir("Results");
	string algorithmn = to_string(algorithm);
	string s_file = "./Results/Solfgurob_" + algorithmn + "_" + data + ".csv";
	fpt = fopen(s_file.c_str(), "w+");
	fprintf(fpt, "element,id_1,id_2,use,value,NOTAS\n");

	//Shape of the depots
	for (d = 0; d < n_days; d++)
		fprintf(fpt, "node,%d,%d,shape,box,\n", d,d);

	//Draw the depots (Aunque sea el mismo, dibujamos uno por día, en total son n_days depots).

	vector<double> color_v;

	color = increase_color;

	for (d = 0; d < n_days; d++)
	{
			fprintf(fpt, "node,%d,%d,text, Day %d,\n", d,d, d);
			fprintf(fpt, "node,%d,%d,size,%d,\n", d, d,size_depot);
			fprintf(fpt, "node,%d,%d,color_float,%.2f,\n", d,d, color);
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
	for (d = 0; d < n_days; d++)
	{
		for (int ii = 0; ii < n_jobs; ii++)
		{
			i = d;
			if (ii != 0)
				i = d*(n_jobs-1)+(n_days+ii-1);

			for (int jj = 0; jj < n_jobs; jj++)
			{
				j = d;
				if (jj != 0)
					j = d * (n_jobs - 1) + (n_days + jj - 1);

				for (w = 0; w < n_nurses; w++)
				{
					try
					{
						if (x[d][w][ii][jj].get(GRB_DoubleAttr_X) > 0.001)
						{
							num = x[d][w][ii][jj].get(GRB_DoubleAttr_X);
							if (ii != 0 || jj != 0)
							{
								if (exist_node[i] == 0 && i>=n_days)
								{

									fprintf(fpt, "node,%d,%d,text, Node %d,\n", i, job[ii].ID, ii);
									fprintf(fpt, "node,%d,%d,text, Skill_0 %d,\n", i, ii, job[ii].skills[0]);
									fprintf(fpt, "node,%d,%d,text, Skill_1 %d,\n", i, ii, job[ii].skills[1]);
									fprintf(fpt, "node,%d,%d,color_float,%.2f,\n", i, ii, color_v[w]);
									exist_node[i]++;
									for (int p = 0; p < n_patterns; p++)
									{
										try
										{
											if (z[ii][p].get(GRB_DoubleAttr_X) > 0.001)
											{
												for (int dd = 0; dd < n_days; dd++)
												{
													if (pattern[p].day_p[dd] > 0)
														fprintf(fpt, "node,%d,%d,text, Day %d Req. %d,\n", i, ii, dd, pattern[p].day_p[dd]);
												}

											}
										}
										catch (GRBException e) {}
									}
								
								}
								if (exist_node[j] == 0 && j >= n_days)
								{

									fprintf(fpt, "node,%d,%d,text, Node %d,\n", j, job[jj].ID, jj);
									fprintf(fpt, "node,%d,%d,text, Skill_0 %d,\n", j, jj, job[jj].skills[0]);
									fprintf(fpt, "node,%d,%d,text, Skill_1 %d,\n", j, jj, job[jj].skills[1]);
									fprintf(fpt, "node,%d,%d,color_float,%.2f,\n", j, jj, color_v[w]);
									exist_node[j]++;
									
										for (int p = 0; p < n_patterns; p++)
										{
											try
											{
												if (z[jj][p].get(GRB_DoubleAttr_X) > 0.001)
												{
													for (int dd = 0; dd < n_days; dd++)
													{
														if(pattern[p].day_p[dd]>0)
															fprintf(fpt, "node,%d,%d,text, Day %d Req. %d,\n", j, jj, dd, pattern[p].day_p[dd]);
													}

												}
											}
											catch (GRBException e) {}
										}
								}
								
							
								num = x[d][w][ii][jj].get(GRB_DoubleAttr_X);

								fprintf(fpt, "edge,%d,%d,text,x_%d_%d = %.2f,\n", i, j, ii, jj, num);
								fprintf(fpt, "edge,%d,%d,text,Nurse = %d Skill= %d,\n", i, j, w, nurse[w].skill);
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

