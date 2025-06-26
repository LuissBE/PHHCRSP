#ifndef DATA_H_
#define DATA_H_
#include <set>
#include <stdio.h>
#include <setjmp.h>
#include <stdlib.h>
#include <climits>   
#include <sstream>
#include <fstream>
#include <string>
#include <math.h>
#include <algorithm>
#include <list>
#include <sys/timeb.h>
#include <exception>
#include <stack>
#include <direct.h>
#include <vector>
#include <iterator>
#include <Windows.h>
#include <sstream>
#include <iostream>
#include <filesystem>
#include <unordered_map>
#include "data.h"
#pragma warning(disable:4786)
#pragma warning(disable:4996)

#include "gurobi_c++.h"
using namespace std;



//typedef IloArray<IloNumVarArray>   NumVarMatrix_2;
//typedef IloArray<NumVarMatrix_2>   NumVarMatrix_3;


//#define REFORCE
#define N_80
#define PRINT
#define OTHER_TREL
#define BOUNDS
#define SINGLE
#define REMOVE_TK
#define LAST_C
#define RL

struct caregiver
{
	int ID;
	int skill;
	int D;
	vector<bool> patient_s;
};
struct patient
{
	int ID;
	vector<int> skills;
	int max_nurses;
	int cluster;
	int time;
};

struct pattern_s
{
	vector<int> day_p; //vector 0 si ese patrón no trabaja día d, en otro caso con qué requerimiento trabaja
	vector<int> day_req; //Vector con los días que se trabaja en ese patrón
	int req; // cardinal de day_req
	vector<int> skill_p; 
	vector<bool> patient_p; //Vector tamaño pacientes con 0 si ese patrón no es apto para el paciente y 1 en otro caso.
};
//#undefine VIEJOS
class Data
{
private:
	int CPUmax; //Maximum CPU time. Default 3600
	int n_jobs; //For us, number of jobs. For Ait and Mk number of customers.
	int n_skills; //NbServices - Number of different skills
	int n_clusters;
	int n_nurses; //NbVehicles - Number of nurses available
	int n_patterns;
	int n_days;
	int max_D;
	int version; //Version. Si es 0 o 1 es la simple normal
	int r1;
	int r2;
	int r3;
	int r4;
	int r5;
	double l1;
	double l2;
	vector<patient> job;
	vector<caregiver> nurse;
	vector<pattern_s> pattern;
	vector<vector<int>> distance;
	
	vector<vector<int>> demandes; //Demandes - Matrix with skills required by the client
	vector<vector<int>> offres; //Offres - For each nurse, what skills they have.Binary: Offres[nurse][skill]

	vector<vector<int>> dureedevisite; //DureeDeVisite - Service time per skill->DureeDeVisite[job][skill]


	int n_dupl; //Jobs duplicados, si un job tiene más de un skill se crea un job gemelo
	

	int preferences; //0 mk, 1 ait

	int skills_nexp;

	vector<vector<double>> pref; 	//CoutPref - Matrix of the preference scores between patients / carers
	vector<double> od_depi;
	vector<double> od_depf;
	vector<vector<double>> od_depi_to_nurse;
	vector<vector<double>> od_depf_to_nurse;
	vector<vector<double>> od; //od - od[i, j] as the distance between two patients.row / col 0 represent the nurses location(same for all of them)


	//TODO M for Ait

	vector<int> experts_rel;
	vector<int> experts;
	vector<int> experts_assig;

	vector<vector<int>> windowsc; //WindowsC - For each job, WindowsC[job][0] is the start of the time window, WindowsC[job][1] the finishing time
	vector<vector<int>> windowsd; //WindowsD - WindowsD[nurse][0] time the nurse starts working, WindowsD[nurse][1] time the nurse finishes
	vector<vector<int>> cxyn;
	vector<vector<int>> cxyc;

	vector<int> gap; //gap - Gap between two services(for those that require two skills only!) if the gap is 0, it means they are simultaneous
	vector<int> gapmin; //gap - Gap between two services(for those that require two skills only!) if the gap is 0, it means they are simultaneous
	vector<vector<int>> rel_jobs;
	vector<vector<int>> gap_jobs;
	vector<vector<int>> gap_jobs_min;
	vector<vector<int>> matrix_skills;
	vector<int> skills;
	
	vector<double> lambda;

	string instance; //Nombre del txt con la instancia
	string data; //Nombre del txt con los datos
	string n_instance; //Nombre folder+instance

	bool export_lp; //Export lp or solve here if 1, export
	bool print; //by default, print true
	bool rl; //rl false by default
	bool html;
	bool v1, v2, v3;
	bool pre; //preproceso
	bool singleskill;
	bool difobj;
	bool flagTmax;
	int algorithm;
	int threads; //número de hilos modelos
	bool cci, ccf;
	double MIPgap = 0.01;
	double relaxation = 0.01;
	double eqgoal;

	bool feas = false;
	int ob;
	int flow;
public:
	Data();
	~Data(void);

	void read_instance();
	void setinstancesnames(char *argv[]);
	void setCPUmax(int);
	void setalgorithm(int);
	void setthreads(int num);
	void setfeas(bool num);
	void setexport_lp(bool);
	void setprint(bool);
	void set_rl(bool);
	void sethtml(bool);
	void setob(int);
	void setflow(int);
	void setversion(int);
	void setflagtmax(bool);
	void setr1(int);
	void setr2(int);
	void setr3(int);
	void setr4(int);
	void setr5(int);
	void setl1(double);
	void setl2(double);
	void setMIPgap(double);
	void setrelaxation(double);
	void setgoal(double);
	int MIP_Cappanera();

	void setjob(int tmax);
	void binary_vars(GRBModel &, GRBVar**, GRBVar**, GRBVar****);
	void integer_vars(GRBModel &, GRBVar***);
	void linear_vars(GRBModel &model, GRBVar** );


	void constraints_1(GRBModel &model, GRBVar****, GRBVar**);
	void constraints_2(GRBModel &model, GRBVar****, GRBVar**);
	void constraints_3(GRBModel &model, GRBVar**);
	void constraints_4(GRBModel &model, GRBVar**);
	void constraints_5(GRBModel &model, GRBVar****, GRBVar**);
	void constraints_6(GRBModel &model, GRBVar****, GRBVar**);
	void constraints_7(GRBModel &model, GRBVar****, GRBVar**);
	void constraints_8(GRBModel &model, GRBVar****);
	void constraints_9(GRBModel &model, GRBVar***, GRBVar**);
	void constraints_10(GRBModel &model, GRBVar***, GRBVar**);
	void constraints_11(GRBModel &model, GRBVar***, GRBVar****);
	void constraints_o1(GRBModel &model, GRBVar**, GRBVar);
	void constraints_o2(GRBModel &model, GRBVar**, GRBVar);

	void constraints_o13(GRBModel &model, GRBVar*, GRBVar);
	void constraints_o23(GRBModel &model, GRBVar*, GRBVar);
	void constraints_o15(GRBModel &model, GRBVar***, GRBVar);
	void constraints_o25(GRBModel &model, GRBVar***, GRBVar);
	void constraints_o14(GRBModel &model, GRBVar***** , GRBVar);
	void constraints_o24(GRBModel &model, GRBVar***** , GRBVar);
	void constraints_zero(GRBModel &model, GRBVar**, GRBVar**** x);
	void constraints_18(GRBModel &model, GRBVar****);
	void constraints_19(GRBModel &model, GRBVar****);
	void constraints_goaldummy(GRBModel& model, GRBVar m, GRBVar M, GRBVar d);

	void write_sol(GRBModel &model, GRBVar** z, GRBVar** u, GRBVar**** x, GRBVar*** y, GRBVar** D);
	void write_solution_csv(GRBModel &, GRBVar** z, GRBVar** u, GRBVar**** x, GRBVar*** y, GRBVar** D);
	void write_sol_n2(GRBModel &model, GRBVar***** x, GRBVar** z, GRBVar**** y, GRBVar** D);
	void write_sol_n24(GRBModel &model, GRBVar***** x, GRBVar** z, GRBVar**** y);
	void write_sol_n23(GRBModel &model, GRBVar***** x, GRBVar** z, GRBVar**** y, GRBVar* D);
	void write_sol_n23_b(GRBModel &model, GRBVar***** x, GRBVar** z, GRBVar**** y, GRBVar* D);
	void write_sol_n25(GRBModel &model, GRBVar***** x, GRBVar** z, GRBVar**** y, GRBVar*** u);
	void write_solution_csv_n5(GRBModel &model, GRBVar***** x, GRBVar** z);
	void write_sol_goal(GRBModel& model, GRBVar***** x, GRBVar** z, GRBVar**** y, GRBVar* D);
	void write_sol_ranking(GRBModel& model, GRBVar***** x, GRBVar** z, GRBVar**** y, GRBVar* D);
	/////////////////////////////////////////

	int MIP_New3();
	int MIP_NewGoal();
	int MIP_NewRanking();
	void binary_vars_n(GRBModel &model, GRBVar***** x, GRBVar** z);
	void linear_vars_n(GRBModel &model, GRBVar** ,GRBVar**);
	void linear_vars_n37(GRBModel &model, GRBVar**, GRBVar*);
	void linear_vars_n3(GRBModel &model, GRBVar**, GRBVar*);
	void linear_vars_n35(GRBModel &model, GRBVar**, GRBVar*);
	void linear_vars_n34(GRBModel &model, GRBVar**, GRBVar*, GRBVar**);
	void linear_vars_n5(GRBModel &model, GRBVar***);
	void linear_vars_n4(GRBModel &model, GRBVar**);
	void integer_vars_n(GRBModel &model, GRBVar**** y);

	void constraints_skills(GRBModel &model, GRBVar***** x);
	void constraints_maxday(GRBModel &model, GRBVar***** x);
	void constraints_symmetry(GRBModel &model, GRBVar***** x);
	void constraints_maxwork(GRBModel &model, GRBVar***** x);
	void constraints_symmetry0(GRBModel &model, GRBVar***** x);
	void constraints_nursesday(GRBModel &model, GRBVar***** x, GRBVar** D);
	void constraints_nursesday3(GRBModel &model, GRBVar***** x, GRBVar* D);
	void constraints_nursesday34(GRBModel &model, GRBVar***** x, GRBVar* D, GRBVar** g);
	void constraints_assign(GRBModel &model, GRBVar***** x, GRBVar** z);
	void constraints_assign5(GRBModel &model, GRBVar*** u, GRBVar** z);
	void constraints_assign_ad(GRBModel &model, GRBVar***** x, GRBVar** z);
	void constraints_assign_ad2(GRBModel &model, GRBVar***** x, GRBVar** z, GRBVar* D);
	void constraints_flow(GRBModel &model, GRBVar***** x, GRBVar** u);
	void constraints_flow_n3(GRBModel &model, GRBVar***** x, GRBVar** u);
	void constraints_flow_n36(GRBModel &model, GRBVar***** x, GRBVar** u);
	void constraints_relate_flow(GRBModel &model, GRBVar***** x, GRBVar** u);
	void constraints_flow_n37(GRBModel &model, GRBVar***** x, GRBVar** u);
	void constraints_flow_n34(GRBModel &model, GRBVar***** x, GRBVar** u);
	void constraints_flow_n5(GRBModel &model, GRBVar***** x, GRBVar*** u);
	void constraints_clusters_x(GRBModel &model, GRBVar***** x);
	void constraints_clusters_x1(GRBModel &model, GRBVar***** x);
	void constraints_clusters_x2(GRBModel &model, GRBVar***** x);
	void constraints_clusters_x_debil(GRBModel &model, GRBVar***** x);
	void constraints_clusters_x_12(GRBModel &model, GRBVar***** x);
	void constraints_paridad(GRBModel &model, GRBVar***** x);
	void constraints_patterns2(GRBModel &model, GRBVar***** x);
	void constraints_patterns(GRBModel &model, GRBVar***** x);
	void constraints_breaksymmetries_day0(GRBModel &model, GRBVar** D);
	void constraints_breaksymmetries_totaldays(GRBModel &model, GRBVar** D);
	void constraints_breaksymmetries_totaldays3(GRBModel &model, GRBVar* D);
	void constraints_flow2(GRBModel &model, GRBVar***** x, GRBVar**** y);

	void write_sol_n(GRBModel &model, GRBVar***** x, GRBVar** z, GRBVar** u, GRBVar** D);
	void write_solution_csv_n(GRBModel &model, GRBVar***** x, GRBVar** z, GRBVar** u, GRBVar** D);
	void write_solution_csv_n4(GRBModel &model, GRBVar***** x, GRBVar** z, GRBVar** u);
	void write_solution_csv_n3(GRBModel &model, GRBVar***** x, GRBVar** z, GRBVar** u, GRBVar* D);
	void write_solution_csv_n2(GRBModel &model, GRBVar***** x, GRBVar** z, GRBVar*** u);

	void write_sol_n22(GRBModel &model, GRBVar***** x, GRBVar** z, GRBVar**** y, GRBVar*** u);

	////////////////////////////////////////////
	int getpreferences();

	//Nuevo
	string get_str_between_two_str(const std::string &s, const std::string &start_delim, const std::string &stop_delim);

	bool endsWithCaseInsensitive(std::string mainStr, std::string toMatch);

	bool startsWithCaseInsensitive(std::string mainStr, std::string toMatch);
	void settimewindows(int);

	
};
#endif






























	////La informacion Load_factor, number y type se requiere para ser almacenada en el arbol, no se utiliza en cada matriz. La paso por referencia y cuando lea la bahía inicial la almaceno donde toque
	////Con esta funcion, además de obtener las "caracteristicas del problem" (disposicion inicial, nombre del fichero, S, H,...), obtengo un vector con el número de contenedores de bloqueo que hay en cada stack y el número total de estos contenedores y un vector con las alturas de los stacks.
	//void LeerDisposicionInicial(char *argv[], int&, int&, int&);
	//
	////Devuelve el nuevo vector con el número de contenedores de bloqueo de cada prioridad y, por referencia prioridad de mayor P de los contenedores de bloqueo.
	//void Matrix::NewBlockingNumber(int s_origen, int s_destino, vector<int> NewBlockingbystack, int& NewTotalBlockingNumber, vector<int> NewBlockingbytype, int& NewMaxBlockingtype);


	//Actualiza el M según el cálculo de Bortfeld de GB or GG
	//void ContarContenedoresMolestos_BortfeldForster();