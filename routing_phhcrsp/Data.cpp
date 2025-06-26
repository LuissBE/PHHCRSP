#include "Data.h"
#define PRINT

std::string trim(const std::string& str,
	const std::string& whitespace = " \t")
{
	const auto strBegin = str.find_first_not_of(whitespace);
	if (strBegin == std::string::npos)
		return ""; // no content

	const auto strEnd = str.find_last_not_of(whitespace);
	const auto strRange = strEnd - strBegin + 1;

	return str.substr(strBegin, strRange);
}

std::string reduce(const std::string& str,
	const std::string& fill = " ",
	const std::string& whitespace = " \t")
{
	// trim first
	auto result = trim(str, whitespace);

	// replace sub ranges
	auto beginSpace = result.find_first_of(whitespace);
	while (beginSpace != std::string::npos)
	{
		const auto endSpace = result.find_first_not_of(whitespace, beginSpace);
		const auto range = endSpace - beginSpace;

		result.replace(beginSpace, range, fill);

		const auto newStart = beginSpace + fill.length();
		beginSpace = result.find_first_of(whitespace, newStart);
	}

	return result;
}

Data::Data()
{
	CPUmax = 3600;

	export_lp = false;
	print = true;
	n_dupl = 0;
	rl = false;
	html = false;
	threads = -1;
	v1 = false;
	v2 = false;
	v3 = false;
	pre = 1;
	singleskill = true;

	for (int i = 0; i < 3; i++) { lambda.push_back(0); }
	lambda[0] = double(1)/double(3);
	lambda[1] = double(1) / double(3);
	lambda[2] = double(1) / double(3);
	difobj = false;

	cci = true;
	ccf = true;
	feas = false;
	
	flow = 1;
	ob = 2; //1 maxmin 2 minmax
	r1 = 0;
	r2 = 0;
	r3 = 0;
	r4 = 0;
	r5 = 0;
	version = 0;
	l1 = 0.5;
	l2 = 0.5;

	flagTmax = false;
}

Data::~Data(void)
{
}


void Data::setflagtmax(bool tmax)
{
	this->flagTmax = tmax;
}
void Data::setjob(int tmax)
{
	job[0].max_nurses = tmax;
}
int Data::getpreferences() { return preferences; }
//
//void Data::read_instance()
//{
//	const char* ninstance = n_instance.c_str();
//	ifstream ficheroi;
//	ficheroi.open(ninstance);
//
//	string phrase;
//	bool fi = false; //Fin de fichero//
//	std::vector<std::string> elems;
//	std::stringstream s;
//	std::string item;
//	char delim;
//	string Value = "";
//
//	bool flag_ini = true;
//	bool flag_jobs = false;
//	bool flag_nurses = false;
//	bool flag_clusters = false;
//	bool flag_patterns = false;
//	bool flag_ten;
//	bool cubre = false;
//	int d;
//	int i;
//	int c_job;
//	int c_nurse;
//	int c_cluster;
//	int c_pattern;
//
//	if (!ficheroi)
//	{
//		printf("Problemas al leer la instancia, no se encuentra el archivo ");
//		exit(4);
//	}
//	else
//	{
//		delim = '	';
//		while (getline(ficheroi, phrase) && !fi)
//		{
//			elems.clear();
//			item.clear();
//			s.clear();
//			s.str(phrase);
//
//			while (std::getline(s, item, delim))
//			{
//				if (!item.empty())
//				{
//					elems.push_back(item);
//				}
//			}
//
//			//En elems está la frase
//			if (elems.size() > 0)
//			{	
//				if (flag_ini)
//				{
//					n_skills = 2;
//					n_days = 5;
//					n_jobs = atoi(elems[0].c_str());
//					for (i = 0; i < n_jobs; i++)
//					{
//						job.push_back(patient());
//					}
//					job[0].cluster = 0;
//					job[0].time = 0;
//
//					n_nurses = atoi(elems[1].c_str());
//					for (i = 0; i < n_nurses; i++)
//					{
//						nurse.push_back(caregiver());
//					}
//
//					n_clusters = atoi(elems[2].c_str());
//
//					vector<int> temp(n_clusters);
//					for (i = 0; i < n_clusters; i++)
//					{
//						distance.push_back(temp);
//					}
//
//					n_patterns = atoi(elems[3].c_str());
//					vector<int> temp2(n_days);
//					vector<int> temp3(n_jobs);
//					for (i = 0; i < n_patterns; i++)
//					{
//						pattern.push_back(pattern_s());
//						for(int dd=0; dd<n_days; dd++)
//							pattern[i].day_p.push_back(0);
//						for(int k=0; k<n_skills;k++)
//							pattern[i].skill_p.push_back(0);
//						pattern[i].req = 0;
//
//						for (int j = 0; j < n_jobs; j++)
//							pattern[i].patient_p.push_back(false);
//					}
//
//					flag_ini = false;
//					c_job = 1;
//					flag_jobs = true;
//
//				}
//				else if (flag_jobs)
//				{	
//					job[c_job].ID = atoi(elems[0].c_str());
//					job[c_job].max_nurses = atoi(elems[1].c_str());
//					for (i = 2; i < n_skills + 2; i++)
//					{
//						job[c_job].skills.push_back(atoi(elems[i].c_str()));
//					}
//					job[c_job].cluster = atoi(elems[n_skills+2].c_str());
//					job[c_job].time = 30;
//					c_job++;
//					if (c_job == n_jobs)
//					{
//						flag_jobs = false;
//						flag_nurses = true;
//						c_nurse = 0;
//					}
//				}
//				else if (flag_nurses)
//				{	
//					nurse[c_nurse].ID = atoi(elems[0].c_str()); 
//					nurse[c_nurse].skill= atoi(elems[1].c_str());
//					nurse[c_nurse].D = 60*atoi(elems[2].c_str());
//					nurse[c_nurse].patient_s.push_back(true);
//					for (int ii = 1; ii < n_jobs; ii++)
//					{
//						flag_ten = false;
//						for (int s = 0; s < n_nurses; s++)
//						{
//							if (job[ii].skills[s] >= 1 && s + 1 <= nurse[c_nurse].skill)
//								flag_ten = true;
//						}
//						nurse[c_nurse].patient_s.push_back(flag_ten);
//					}
//					if (c_nurse == 0)
//					{
//						max_D = nurse[c_nurse].D;
//					}
//					else if(nurse[c_nurse].D>max_D)
//					{
//						max_D = nurse[c_nurse].D;
//					}
//					c_nurse++;
//					if (c_nurse == n_nurses)
//					{
//						flag_nurses = false;
//						flag_clusters = true;
//						c_cluster = 0;
//					}
//				}
//				else if (flag_clusters)
//				{	
//					for (i = 0; i < elems.size(); i++)
//					{
//						distance[c_cluster][i]= atoi(elems[i].c_str());
//					}
//					c_cluster++;
//					if (c_cluster == n_clusters)
//					{
//						flag_clusters = false;
//						flag_patterns = true;
//						c_pattern = 0;
//					}
//				}
//				else if (flag_patterns)
//				{
//					for (i = 0; i < n_days; i++)
//					{
//						d = atoi(elems[i].c_str());
//						pattern[c_pattern].day_p[i] = d;
//						if (d > 0)
//						{
//							pattern[c_pattern].day_req.push_back(i);
//							pattern[c_pattern].req++;
//							pattern[c_pattern].skill_p[d-1]++;
//						}
//					}
//
//
//					c_pattern++;
//					if (c_pattern == n_patterns)
//					{
//						flag_patterns = false;
//					}
//				}
//			}
//		}
//		
//		for (int p = 0; p < n_patterns; p++)
//		{
//			pattern[p].patient_p[0] = true;
//			for (i = 1; i < n_jobs; i++)
//			{
//				cubre = true;
//				for (int s = 0; s < n_skills; s++)
//				{
//					if (job[i].skills[s] != pattern[p].skill_p[s])
//						cubre = false;
//				}
//				pattern[p].patient_p[i] = cubre;
//			}
//		}
//	}
//}

void Data::read_instance()
{
	//version = -2;
	const char* ninstance = n_instance.c_str();
	ifstream ficheroi;
	ficheroi.open(ninstance);

	string phrase;
	bool fi = false; //Fin de fichero//
	std::vector<std::string> elems;
	std::stringstream s;
	std::string item;
	char delim;
	string Value = "";

	bool flag_ini = true;
	bool flag_jobs = false;
	bool flag_nurses = false;
	bool flag_clusters = false;
	bool flag_patterns = false;
	bool flag_ten;
	bool cubre = false;
	int d;
	int i;
	int c_job;
	int c_nurse;
	int c_cluster;
	int c_pattern;

	if (!ficheroi)
	{
		printf("Problemas al leer la instancia, no se encuentra el archivo ");
		exit(4);
	}
	else
	{
		delim = '	';
		while (getline(ficheroi, phrase) && !fi)
		{
			elems.clear();
			item.clear();
			s.clear();
			s.str(phrase);

			while (std::getline(s, item, delim))
			{
				if (!item.empty())
				{
					elems.push_back(item);
				}
			}

			//En elems está la frase
			if (elems.size() > 0)
			{
				if (flag_ini)
				{
					n_skills = 2;
					n_days = 5;
					n_jobs = atoi(elems[0].c_str());
					for (i = 0; i < n_jobs; i++)
					{
						job.push_back(patient());
					}
					job[0].cluster = 0;
					job[0].time = 0;

					n_nurses = atoi(elems[1].c_str());
					for (i = 0; i < n_nurses; i++)
					{
						nurse.push_back(caregiver());
					}

					n_clusters = atoi(elems[2].c_str());

					vector<int> temp(n_clusters+1);
					for (i = 0; i < n_clusters+1; i++)
					{
						distance.push_back(temp);
					}

					n_patterns = atoi(elems[3].c_str());
					vector<int> temp2(n_days);
					vector<int> temp3(n_jobs);
					for (i = 0; i < n_patterns; i++)
					{
						pattern.push_back(pattern_s());
						for (int dd = 0; dd < n_days; dd++)
							pattern[i].day_p.push_back(0);
						for (int k = 0; k < n_skills; k++)
							pattern[i].skill_p.push_back(0);
						pattern[i].req = 0;

						for (int j = 0; j < n_jobs; j++)
							pattern[i].patient_p.push_back(false);
					}

					flag_ini = false;
					c_job = 1;
					flag_jobs = true;

				}
				else if (flag_jobs)
				{
					job[c_job].ID = atoi(elems[0].c_str());
					job[c_job].max_nurses = atoi(elems[1].c_str());
					for (i = 2; i < n_skills + 2; i++)
					{
						job[c_job].skills.push_back(atoi(elems[i].c_str()));
					}
					job[c_job].cluster = atoi(elems[n_skills + 2].c_str());
					job[c_job].time = 30;
					c_job++;
					if (c_job == n_jobs)
					{
						flag_jobs = false;
						flag_nurses = true;
						c_nurse = 0;
					}
				}
				else if (flag_nurses)
				{
					nurse[c_nurse].ID = atoi(elems[0].c_str());
					nurse[c_nurse].skill = atoi(elems[1].c_str());
					nurse[c_nurse].D = 60 * atoi(elems[2].c_str());
					nurse[c_nurse].patient_s.push_back(true);
					for (int ii = 1; ii < n_jobs; ii++)
					{
						flag_ten = false;
						for (int s = 0; s < n_nurses; s++)
						{
							if (job[ii].skills[s] >= 1 && s + 1 <= nurse[c_nurse].skill)
								flag_ten = true;
						}
						nurse[c_nurse].patient_s.push_back(flag_ten);
					}
					if (c_nurse == 0)
					{
						max_D = nurse[c_nurse].D;
					}
					else if (nurse[c_nurse].D > max_D)
					{
						max_D = nurse[c_nurse].D;
					}
					c_nurse++;
					if (c_nurse == n_nurses)
					{
						flag_nurses = false;
						flag_clusters = true;
						c_cluster = 0;
					}
				}
				else if (flag_clusters)
				{
					for (i = 0; i < elems.size(); i++)
					{
						distance[c_cluster][i] = atoi(elems[i].c_str());
					}
					c_cluster++;
					if (c_cluster == n_clusters)
					{
						flag_clusters = false;
						flag_patterns = true;
						c_pattern = 0;
					}
				}
				else if (flag_patterns)
				{
					for (i = 0; i < n_days; i++)
					{
						d = atoi(elems[i].c_str());
						pattern[c_pattern].day_p[i] = d;
						if (d > 0)
						{
							pattern[c_pattern].day_req.push_back(i);
							pattern[c_pattern].req++;
							pattern[c_pattern].skill_p[d - 1]++;
						}
					}


					c_pattern++;
					if (c_pattern == n_patterns)
					{
						flag_patterns = false;
					}
				}
			}
		}

		for (int p = 0; p < n_patterns; p++)
		{
			pattern[p].patient_p[0] = true;
			for (i = 1; i < n_jobs; i++)
			{
				cubre = true;
				for (int s = 0; s < n_skills; s++)
				{
					if (job[i].skills[s] != pattern[p].skill_p[s])
						cubre = false;
				}
				pattern[p].patient_p[i] = cubre;
			}
		}
	}
	for (int c = 0; c < n_clusters; c++)
	{
		distance[n_clusters][c] = distance[job[0].cluster][c];
		distance[c][n_clusters] = distance[c][job[0].cluster];
	}
	job[0].max_nurses = job[1].max_nurses;
	/*job[0].cluster = n_clusters;
	n_clusters++;*/
	job[0].cluster = 0;
}
void Data::setob(int ob)
{
	this->ob = ob;
}

void Data::setflow(int ob)
{
	this->flow = ob;
}
void Data::setr1(int ob)
{
	this->r1 = ob;
}
void Data::setr2(int ob)
{
	this->r2 = ob;
}
void Data::setr3(int ob)
{
	this->r3 = ob;
}
void Data::setr4(int ob)
{
	this->r4 = ob;
}
void Data::setr5(int ob)
{
	this->r5 = ob;
}
void Data::setversion(int ob)
{
	this->version = ob;
}
void Data::setl1(double ob)
{
	this->l1 = ob;
}
void Data::setl2(double ob)
{
	this->l2 = ob;
}
void Data::setinstancesnames(char *argv[])
{
	string folder = argv[1]; //Folder
	data = argv[2]; //File
	instance = "Result_" + data; //out instance
	n_instance = "./" + folder + "/" + data;

}
void Data::setprint(bool num)
{
	print = num;
}
void Data::sethtml(bool num)
{
	html = num;
}
void Data::setalgorithm(int num)
{
	algorithm = num;
}
void Data::setexport_lp(bool num)
{
	export_lp = num;
}
void Data::set_rl(bool num)
{
	rl = num;
}
void Data::setthreads(int num)
{
	threads = num;
}

void Data::settimewindows(int num)
{
	for (int k = 0; k < n_nurses; k++)
	{
		windowsd[k][1] = num * 60;
	}
}
void Data::setCPUmax(int num)
{
	CPUmax = num;
}
void Data::setfeas(bool num)
{
	feas = num;
}
void Data::setMIPgap(double num)
{
	MIPgap = num;
}
void Data::setrelaxation(double num)
{
	relaxation = num;
}
void Data::setgoal(double num)
{
	eqgoal = num;
}
bool Data::endsWithCaseInsensitive(std::string mainStr, std::string toMatch)
{
	auto it = toMatch.begin();
	return mainStr.size() >= toMatch.size() &&
		std::all_of(std::next(mainStr.begin(), mainStr.size() - toMatch.size()), mainStr.end(), [&it](const char & c) {
		return ::tolower(c) == ::tolower(*(it++));
	});
}
bool Data::startsWithCaseInsensitive(std::string mainStr, std::string toMatch)
{
	// Convert mainStr to lower case
	std::transform(mainStr.begin(), mainStr.end(), mainStr.begin(), ::tolower);
	// Convert toMatch to lower case
	std::transform(toMatch.begin(), toMatch.end(), toMatch.begin(), ::tolower);

	if (mainStr.find(toMatch) == 0)
		return true;
	else
		return false;
}
std::string Data::get_str_between_two_str(const std::string &s, const std::string &start_delim, const std::string &stop_delim)
{
	unsigned first_delim_pos = s.find(start_delim);
	unsigned end_pos_of_first_delim = first_delim_pos + start_delim.length();
	unsigned last_delim_pos = s.find(stop_delim);

	return s.substr(end_pos_of_first_delim,
		last_delim_pos - end_pos_of_first_delim);
}
