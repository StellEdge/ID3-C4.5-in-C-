#pragma once
#ifndef _ID3
#define _ID3

#include <iostream>
#include <utility>  
#include <vector>  
#include <list>  
#include <map> 
using namespace std;

#define Datalist list<Sample_struct>
#define Children map<int,Node*>
#define Accumulate map<int, Datalist*>
/*
	head file of class ID3_decision_tree
*/

static bool attr_need_to_conv[14] = {0,1,0,1,0,1,1,1,1,1,0,0,0,1};
static map<string, int> converter[15] = {
	//age:
	{},
	//workclass:
	{
		/*
		Private, Self - emp - not- inc, Self - emp - inc, Federal - gov,
		Local - gov, State - gov, Without - pay, Never - worked
		*/
		{"Private",0},
		{"Self-emp-not-inc",1},
		{"Self-emp-inc",2},
		{"Federal-gov",3},
		{"Local-gov",4},
		{"State-gov",5},
		{"Without-pay",6},
		{"Never-worked",7}
	},
	//fnlwgt:
	{},
	//education:
	{
		/*
		Bachelors, Some-college, 11th, HS-grad, Prof-school, Assoc-acdm, 
		Assoc-voc, 9th, 7th-8th, 12th, Masters, 1st-4th, 10th, Doctorate,
		5th-6th, Preschool
		*/
		{"Bachelors",0},
		{"Some-college",1},
		{"11th",2},
		{"HS-grad",3},
		{"Prof-school",4},
		{"Assoc-acdm",5},
		{"Assoc-voc",6},
		{"9th",7},
		{"7th-8th",8},
		{"12th",9},
		{"Masters",10},
		{"1st-4th",11},
		{"10th",12},
		{"Doctorate",13},
		{"5th-6th",14},
		{"Preschool",15}
	},
	//education-num:
	{},
	//marital-status:
	{
		/*
		Married-civ-spouse, Divorced, Never-married, Separated, Widowed,
		Married-spouse-absent, Married-AF-spouse.
		*/
		{"Married-civ-spouse",0},
		{"Divorced",1},
		{"Never-married",2},
		{"Separated",3},
		{"Widowed",4},
		{"Married-spouse-absent",5},
		{"Married-AF-spouse",6}
	},
	//occupation:
	{
		/*
		Tech-support, Craft-repair, Other-service, Sales, Exec-managerial,
		Prof-specialty, Handlers-cleaners, Machine-op-inspct, Adm-clerical,
		Farming-fishing, Transport-moving, Priv-house-serv, Protective-serv,
		Armed-Forces. 
		*/
		{"Tech-support",0},
		{"Craft-repair",1},
		{"Other-service",2},
		{"Sales",3},
		{"Exec-managerial",4},
		{"Prof-specialty",5},
		{"Handlers-cleaners",6},
		{"Machine-op-inspct",7},
		{"Adm-clerical",8},
		{"Farming-fishing",9},
		{"Transport-moving",10},
		{"Priv-house-serv",11},
		{"Protective-serv",12},
		{"Armed-Forces",13}
	},
	//relationship: 
	{
		/*
		Wife, Own-child, Husband, Not-in-family, Other-relative, Unmarried
		*/
		{"Wife",0},
		{"Own-child",1},
		{"Husband",2},
		{"Not-in-family",3},
		{"Other-relative",4},
		{"Unmarried",5}
	},
	//race: 
	{
		/*
		White, Asian-Pac-Islander, Amer-Indian-Eskimo, Other, Black.
		*/
		{"White",0},
		{"Asian-Pac-Islander",1},
		{"Amer-Indian-Eskimo",2},
		{"Other",3},
		{"Black",4}
	},
	//sex:
	{
		/*
		Female, Male. 
		*/
		{"Female",0},
		{"Male",1}
	},
	//capital - gain:
	{},
	//capital-loss:
	{},
	//hours-per-week:
	{},
	//native-country:
	{
		/*
		United-States, Cambodia, England, Puerto-Rico, Canada,
		Germany, Outlying-US(Guam-USVI-etc), India, Japan, 
		Greece, South, China, Cuba, Iran, Honduras, Philippines,
		Italy, Poland, Jamaica, Vietnam, Mexico, Portugal, Ireland,
		France, Dominican-Republic, Laos, Ecuador, Taiwan, Haiti,
		Columbia, Hungary, Guatemala, Nicaragua, Scotland,
		Thailand, Yugoslavia, El-Salvador, Trinadad&Tobago,
		Peru, Hong, Holand-Netherlands.
		*/
		{"United-States",0},{"Cambodia",1},
		{"England",2},{"Puerto-Rico",3},
		{"Canada",4},{"Germany",5},
		{"Outlying-US(Guam-USVI-etc)",6},{"India",7},
		{"Japan",8},{"Greece",9},
		{"South",10},{"China",11},
		{"Cuba",12},{"Iran",13},
		{"Honduras",14},{"Philippines",15},
		{"Italy",16},{"Poland",17},
		{"Jamaica",18},{"Vietnam",19},
		{"Mexico",20},{"Portugal",21},
		{"Ireland",22},{"France",23},
		{"Dominican-Republic",24},{"Laos",25},
		{"Ecuador",26},{"Taiwan",27},
		{"Haiti",28},{"Columbia",29},
		{"Hungary",30},{"Guatemala",31},
		{"Nicaragua",32},{"Scotland",33},
		{"Thailand",34},{"Yugoslavia",35},
		{"El-Salvador",36},{"Trinadad&Tobago",37},
		{"Peru",38},{"Hong",39},
		{"Holand-Netherlands",40}
	},
	//res
	{
		{ ">50K",1},
		{ "<=50K",0}
	}
};
/*
	Sample_struct:数据样本结构，有14个属性
	例子：52, Self-emp-not-inc, 209642, HS-grad,9, Married-civ-spouse, Exec-managerial, 
	Husband, White, Male, 0, 0, 45, United-States, >50K
*/
struct Sample_struct{
	int attr[14];
	bool res;	//0:<=50K 1>50K
};

struct Node{
	int index;					//classify by index'th attribute
	int value;					//this node's value
	double divide;
	Children next;				//all children
	Datalist sample;			//unclassified samples
};


class Accumulater {
	//for memory management
public:
	Accumulate accu;

	//divide with what？？
	void set_accu(int att, const Datalist & sp);
	void set_accu_continuous(int att,double divide, const Datalist & sp);
	//

	void clear_all_child();
};

class ID3_decision_tree {
public:

	ID3_decision_tree();
	~ID3_decision_tree();
	void build_tree_from_file(const string fname);
	void judge_file(const string fname);
	vector<string> str_split(const char *s, const char *splter);
protected:
	Node *root;
	Accumulater * accumulate;

	void clear(Node *);
	virtual void __build(Node * node);
	void __split(Node * node,int mg_index);


	bool check_str_legal(const vector<string> &);
	bool check_conflict_data(const Datalist &);
	Sample_struct sample_struct_build(const vector<string> & v);
	virtual int get_max_gain(const Datalist &);
	bool is_same_val(const Datalist &);
	double entropy(const Datalist &);
	double bounded_entropy(int att, const Datalist &);
	double info_gain(int att, const Datalist &);

	virtual bool judge_sample(const Sample_struct & s);
};

class C4_5_decision_tree:public ID3_decision_tree {
protected:
	void __build(Node * node);
	void __split_continuous(Node * node, double divide,int mg_index);

	int get_max_gain(const Datalist &);
	double get_best_con_divide(int att, const Datalist &);
	double info_gain_rate(int att, const Datalist &);
	double info_gain_continuous(int att,double divide, const Datalist &);
	double attr_entropy(int att, const Datalist &);
	double attr_entropy_continuous(int att, double divide, const Datalist &);
	double bounded_entropy_continuous(int att, double divide, const Datalist & sp);
	bool already_divided(int att, const Datalist &);

	bool judge_sample(const Sample_struct & s);
};

#endif