#include "pch.h"
#include "ID3_decision_tree.h"
#include <fstream>
#include <algorithm>
#include <string>
#include <set>
using namespace std;

/*
	cpp file of class ID3_decision_tree
*/

//initialize tree
ID3_decision_tree::ID3_decision_tree() {
	root = new Node();
	root->index = -1;
	root->value = -1;
	accumulate = new Accumulater();
}

//destory tree
ID3_decision_tree::~ID3_decision_tree() {
	clear(root);
}

//build tree from trainning set.
void ID3_decision_tree::build_tree_from_file(const string fname) {
	cout << "Training with " << fname << endl;
	ifstream in(fname);
	char buf[500];
	vector<string> res;
	while (in.getline(buf, 500)) {
		//ignore blank lines
		if (strlen(buf) == 0)continue;
		//read and split and convert all samples
		res = str_split(buf, ",.");
		if(!check_str_legal(res))continue; //ignore bad lines.
		root->sample.emplace_back(sample_struct_build(res));
	}
	__build(root);
	return;
}

//read and judge a file 
void ID3_decision_tree::judge_file(const string fname) {
	cout << "Judging test set " << fname << endl;
	ifstream in(fname);
	char buf[500];
	vector<string> res;
	Datalist test_set;
	while (in.getline(buf, 500)) {
		//ignore blank lines
		if (strlen(buf) == 0)continue;
		//read and split and convert all samples
		res = str_split(buf, ",");
		if (!check_str_legal(res))continue; //ignore bad lines.
		test_set.emplace_back(sample_struct_build(res));
	}

	int correct_num = 0,error_num=0;
	bool judge_res;
	string ans[2] = { "<=50K",">50K" };
	for (Datalist::const_iterator it = test_set.cbegin(); it != test_set.cend(); it++) {
		judge_res = judge_sample(*it);
		cout << "Result: " << ans[judge_res] << " Answer:" << ans[(*it).res];
		if (judge_res == (*it).res) {
			correct_num++;
			cout << "  Correct.\n";
		}
		else {
			error_num++;
			cout << "  Error.\n";
		}
	}
	cout << "Error rate: " << error_num * 1.0 / test_set.size() << endl;
	cout << "Total entries: " << test_set.size() << endl;
	//judge_sample(const Sample_struct & s)
}

//string split 
vector<string> ID3_decision_tree::str_split(const char *s, const char *splter)
{
	vector<string> result;
	if (s && strlen(s))
	{
		int len = strlen(s);
		char *src = new char[len + 1];
		strcpy(src, s);
		src[len] = '\0';
		char *tokenptr = strtok(src, splter);
		while (tokenptr != NULL)
		{
			string tk = tokenptr;
			//remove space
			while (tk.at(0) == ' ') {
				tk.erase(tk.begin());
			}
			result.emplace_back(tk);	//speed up by emplace_back
			tokenptr = strtok(NULL, splter);
		}
		delete[] src;
	}
	if (result.back().back() == '.') {
		//remove '.'
		result.back().erase(--result.back().end());
	}
	return result;
}

//delete node recursively
void ID3_decision_tree::clear(Node* node) {
	Children &next = node->next;
	for (Children::iterator it = next.begin(); it != next.end(); it++) {
		clear(it->second);
	}
	node->sample.clear();
	node->next.clear();
	delete node;
	return;
}

//build tree recursively
void ID3_decision_tree::__build(Node * node) {
	//if already of same type
	if (is_same_val(node->sample)) {
		node->index = -1;
		node->value = node->sample.front().res;
		node->sample.clear();	//no need to keep these already trained data
		return;
	}

	//not all of the same type
	node->index = get_max_gain(node->sample);
	if (node->index == 0&&check_conflict_data(node->sample)) {
		// may be there are conflict data
		node->index = -1;
		node->value = node->sample.front().res; //GOD
		node->sample.clear();
		return;
	}

	node->value = -1;
	__split(node, node->index);
	//free space for less mem use
	node->sample.clear();
	for (Children::iterator it = (node->next).begin(); it != (node->next).end(); it++) {
		__build((*it).second);
	}
}

void ID3_decision_tree::__split(Node * node, int mg_index) {
	//break into smaller slices
	accumulate->set_accu(mg_index, node->sample);

	Node * tmp;
	for (Accumulate::const_iterator it = accumulate->accu.cbegin(); it != accumulate->accu.cend(); it++) {
		tmp = new Node;
		tmp->sample = *((*it).second);
		node->next.insert(pair<int, Node*>((*it).first, tmp));
	}
	return;
}

//Flit all failed data
bool ID3_decision_tree::check_str_legal(const vector<string> & v) {
	if (v.size() != 15)return false;
	vector<string>::const_iterator it = v.cbegin();
	for (int attr_num = 0; attr_num < 14; attr_num++, it++) {
		if (attr_need_to_conv[attr_num]) {
			if (converter[attr_num].count(*it) == 0) {
				return false;
			}
		}
	}
	//return (converter[14].count(v.back()) > 0);
	return true;
}

//if there are conflict data, program will overflow if left them unhandled.
bool ID3_decision_tree::check_conflict_data(const Datalist & sp) {
	if (sp.size() <= 1)return false;
	int attr_sav[14];
	for (int i = 0; i < 14; i++) {
		attr_sav[i] = sp.front().attr[i];	//init attrs;
	}
	for (Datalist::const_iterator it = ++sp.cbegin(); it != sp.cend(); it++) {
		for (int i = 0; i < 14; i++) {
			if (attr_sav[i] != (*it).attr[i]) {
				return false;	//	has different data;
			}
		}
	}
	return true;	//	has conflict
}
//build Sample_struct with splitted string
Sample_struct ID3_decision_tree::sample_struct_build(const vector<string> & v) {
	Sample_struct result;
	vector<string>::const_iterator it = v.cbegin();
	for (int attr_num = 0; attr_num<14;attr_num++,it++) {
		if (attr_need_to_conv[attr_num]) {
			result.attr[attr_num] = converter[attr_num][*it];	//using converter
		}
		else {
			result.attr[attr_num] = atoi((*it).c_str());	//no need to convert
		}
	}
	result.res = converter[14][v.back()];
	return result;
}

//get the index of the max info gain attribute
int ID3_decision_tree::get_max_gain(const Datalist & sp) {
	int max_gain_index=0;
	double max_gain = 0,tmp_gain;
	for (int i = 0; i < 14; i++) {
		tmp_gain = info_gain(i, sp);
		if (tmp_gain > max_gain) {
			max_gain_index = i;
			max_gain = tmp_gain;
		}
	}
	return max_gain_index;
}

//judge whether all samples are the same--already a leaf node
bool ID3_decision_tree::is_same_val(const Datalist & sp) {
	if (sp.size() <=1)return true;
	bool flag = sp.front().res;
	for (Datalist::const_iterator it = sp.cbegin(); it != sp.cend(); it++) {
		if ((*it).res != flag)return false;
	}
	return true;
}

//calculate endropy
double ID3_decision_tree::entropy(const Datalist & sp) {
	if (sp.size() == 0)return 0;
	int res1 = 0, res2 = 0;
	for (Datalist::const_iterator it = sp.cbegin(); it != sp.cend(); it++) {
		if ((*it).res)res1++;
		else res2++;
	}
	if (res1 == 0 || res2 == 0) {
		return 0;
	}
	double p1, p2;
	p1 = res1 * 1.0 / sp.size();
	p2 = res2 * 1.0 / sp.size();
	return -p1 * log(p1) - p2 * log(p2);
}

//calculate bounded_endropy
double ID3_decision_tree::bounded_entropy(int att, const Datalist & sp) {
	accumulate->set_accu(att,sp);

	double entro = 0;
	for (Accumulate::const_iterator it = accumulate->accu.cbegin(); it != accumulate->accu.cend(); it++) {
		entro += (*it).second->size()*entropy(*((*it).second)) / sp.size();
	}
	
	return entro;
}

//calculate info gain
double ID3_decision_tree::info_gain(int att, const Datalist & sp) {
	return entropy(sp) - bounded_entropy(att, sp);
}

void Accumulater::set_accu(int att, const Datalist & sp) {
	clear_all_child();
	for (Datalist::const_iterator it = sp.cbegin(); it != sp.cend(); it++) {
		if (!(accu.count((*it).attr[att]))) {
			//has no value of this type
			accu.insert(pair<int, Datalist*>((*it).attr[att], new Datalist));
		}
		accu[(*it).attr[att]]->push_back(*it);
	}
	return;
}

void Accumulater::set_accu_continuous(int att, double divide, const Datalist & sp) {
	clear_all_child();
	accu.insert(pair<int, Datalist*>(0, new Datalist));	//smaller
	accu.insert(pair<int, Datalist*>(1, new Datalist));	//greater or equal
	for (Datalist::const_iterator it = sp.cbegin(); it != sp.cend(); it++) {
		if (((*it).attr[att])< divide) 
			accu[0]->push_back(*it);
		else
			accu[1]->push_back(*it);
	}
	return;
}

void Accumulater::clear_all_child() {
	for (Accumulate::iterator it = accu.begin(); it != accu.end(); it++) {
		(*it).second->clear();
		delete (*it).second;
	}
	accu.clear();
}

bool ID3_decision_tree::judge_sample(const Sample_struct & s) {
	Node * cur_node=root;
	while (cur_node->index!=-1) {
		if (cur_node->next.count(s.attr[cur_node->index]) <= 0) {
			return true;
		}
		cur_node = cur_node->next[s.attr[cur_node->index]];	//traversing through the tree.
	}
	return cur_node->value;
}




//here comes the C4.5

void C4_5_decision_tree::__build(Node * node) {
	//if already of same type
	if (is_same_val(node->sample)) {
		node->index = -1;
		node->value = node->sample.front().res;
		node->sample.clear();	//no need to keep these already trained data
		return;
	}

	//not all of the same type
	node->index = get_max_gain(node->sample);

	if (attr_need_to_conv[node->index]) {
		//discrete
		node->value = -1;
		node->divide = 0;
		__split(node, node->index);
		//free space for less mem use
		node->sample.clear();
		for (Children::iterator it = (node->next).begin(); it != (node->next).end(); it++) {
			__build((*it).second);
		}
	}
	else {
		//continuous
		if (node->index == 0 && check_conflict_data(node->sample)) {
			// may be there are conflict data
			node->index = -1;
			node->value = node->sample.front().res; //GOD
			node->sample.clear();
			return;
		}

		node->value = -1;
		node->divide = get_best_con_divide(node->index, node->sample);
		__split_continuous(node, node->divide, node->index);

		node->sample.clear();
		for (Children::iterator it = (node->next).begin(); it != (node->next).end(); it++) {
			__build((*it).second);
		}
		
	}
	return;
}

void C4_5_decision_tree::__split_continuous(Node * node,double divide, int mg_index) {
	accumulate->set_accu_continuous(mg_index, divide, node->sample);
	Node * tmp;
	for (Accumulate::const_iterator it = accumulate->accu.cbegin(); it != accumulate->accu.cend(); it++) {
		tmp = new Node;
		tmp->sample = *((*it).second);
		node->next.insert(pair<int, Node*>((*it).first, tmp));
	}
	return;
}

int comp_attr;
bool comp(const Sample_struct &a, const Sample_struct &b) {
	  return a.attr[comp_attr] < b.attr[comp_attr];
}

double C4_5_decision_tree::get_best_con_divide(int att, const Datalist & sp) {
	vector<Sample_struct> tmp_vec;

	int smaller_num = 0;
	int smaller_res = 0;

	int bigger_num = sp.size();
	int bigger_res = 0;

	for (Datalist::const_iterator it = sp.cbegin(); it != sp.cend(); ++it) {
		tmp_vec.push_back(*it);
		if ((*it).res)bigger_res++;	//cal not nums
	}
	//sort from small to big.
	comp_attr = att;
	sort(tmp_vec.begin(), tmp_vec.end(),comp);
	if (att == 0&&sp.size()==40) {
		comp_attr = att;
	}
	unsigned int pos = 0;
	double divide = 0;
	double bdd_entro_min = INFINITY;
	double bdd_entro;
	double best_divide = tmp_vec[pos].attr[att];
	//init
	for (; pos < tmp_vec.size() - 1; pos++) {
		//WHEN bounded_entropy is the smallest ,info gain is the max;
		divide = (tmp_vec[pos].attr[att] + tmp_vec[pos + 1].attr[att])*1.0 / 2;
		smaller_num++;
		if (tmp_vec[pos].res) {
			smaller_res++;
			bigger_res--;
		}
		bigger_num--;
		if (tmp_vec[pos].attr[att] == tmp_vec[pos + 1].attr[att]) {
			continue;
		}

		bdd_entro = 0;
		if (smaller_res != 0) {
			bdd_entro -= smaller_num * 1.0 / sp.size()*(smaller_res * log(smaller_res*1.0 / smaller_num) / smaller_num);
		}
		if ((smaller_num - smaller_res) != 0) {
			bdd_entro -= smaller_num * 1.0 / sp.size()*((smaller_num - smaller_res) * log((smaller_num - smaller_res)*1.0 / smaller_num) / smaller_num);
		}
		if (bigger_res != 0) {
			bdd_entro -= bigger_num * 1.0 / sp.size()*(bigger_res * log(bigger_res*1.0 / bigger_num) / bigger_num);
		}
		if ((bigger_num - bigger_res) != 0) {
			bdd_entro -= bigger_num * 1.0 / sp.size()*((bigger_num - bigger_res) * log((bigger_num - bigger_res)*1.0 / bigger_num) / bigger_num);
		}
		if (bdd_entro < bdd_entro_min) {
			bdd_entro_min = bdd_entro;
			best_divide = divide;
		}
	}
	return best_divide;
}

double C4_5_decision_tree::info_gain_rate(int att, const Datalist & sp) {
	if(attr_need_to_conv[att]){
		return info_gain(att, sp) / attr_entropy(att, sp);
	}
	else {
		//continous
		double divide = get_best_con_divide(att, sp);
		return info_gain_continuous(att,divide,sp) / attr_entropy_continuous(att,divide, sp);
	}
}

double C4_5_decision_tree::info_gain_continuous(int att,double divide, const Datalist & sp) {
	set<int> tmp;
	for (Datalist::const_iterator it = sp.cbegin(); it != sp.cend(); ++it) {
		tmp.insert((*it).attr[att]);
		//no replica values
	}
	//correction log2(n-1)/D
	return entropy(sp) - bounded_entropy_continuous(att, divide, sp) - log2(tmp.size()-1) / sp.size();
}

double C4_5_decision_tree::attr_entropy(int att, const Datalist & sp) {
	accumulate->set_accu(att, sp);
	double entro = 0;
	double spliter;
	if (accumulate->accu.size() <= 1) {
		return 0;
	}
	for (Accumulate::const_iterator it = accumulate->accu.cbegin(); it != accumulate->accu.cend(); it++) {
		spliter = (*it).second->size()*1.0 / sp.size();
		entro -= spliter*log(spliter);
	}
	return entro;
}

double C4_5_decision_tree::attr_entropy_continuous(int att, double divide, const Datalist & sp) {
	accumulate->set_accu_continuous(att, divide, sp);
	double entro = 0;
	double spliter;
	if (accumulate->accu.size() <= 1) {
		return 0;
	}
	for (Accumulate::const_iterator it = accumulate->accu.cbegin(); it != accumulate->accu.cend(); it++) {
		spliter = (*it).second->size()*1.0 / sp.size();
		entro -= spliter * log(spliter);
	}
	return entro;
}

double C4_5_decision_tree::bounded_entropy_continuous(int att,double divide, const Datalist & sp) {
	accumulate->set_accu_continuous(att, divide,sp);
	double entro = 0;
	for (Accumulate::const_iterator it = accumulate->accu.cbegin(); it != accumulate->accu.cend(); it++) {
		entro += (*it).second->size()*entropy(*((*it).second)) / sp.size();
	}

	return entro;
}

int C4_5_decision_tree::get_max_gain(const Datalist & sp) {
	int max_gain_index = 0;
	double max_gain = -INFINITY, tmp_gain;
	for (int i = 0; i < 14; i++) {
		if (already_divided(i, sp))
			continue;
		//according to rate
		tmp_gain = info_gain_rate(i, sp);
		if (tmp_gain > max_gain) {
			max_gain_index = i;
			max_gain = tmp_gain;
		}
	}
	return max_gain_index;
}

bool C4_5_decision_tree::already_divided(int att, const Datalist & sp) {
	//if (sp.size() == 3) {
	//	int orig = sp.front().attr[att];
	//}
	int orig = sp.front().attr[att];
	for (Datalist::const_iterator it = sp.cbegin(); it != sp.cend(); it++) {
		if ((*it).attr[att] != orig)return false;
	}
	return true;
}

bool C4_5_decision_tree::judge_sample(const Sample_struct & s) {
	Node * cur_node = root;
	while (cur_node->index != -1) {
		//traversing through the tree.
		if (attr_need_to_conv[cur_node->index]|| cur_node->divide==0) {
			if (cur_node->next.count(s.attr[cur_node->index]) <= 0) {
				return true;
			}
			cur_node = cur_node->next[s.attr[cur_node->index]];
		}
		else {
			//continuous
			if (s.attr[cur_node->index] < cur_node->divide) {
				cur_node = cur_node->next[0];
			}
			else
			{
				cur_node = cur_node->next[1];
			}
			
		}
	}
	return cur_node->value;
}