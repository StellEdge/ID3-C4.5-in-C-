#include "pch.h"
#include "ID3_decision_tree.h"
#include <iostream>

/*
	Main file for testing 
*/
int main()
{
	/*
	Use ID3 
	ID3_decision_tree ID3;
	ID3.build_tree_from_file("./DATA SET/adult.data");

	ID3.judge_file("./DATA SET/adult.test");
	
	*/

	//use C4.5
	C4_5_decision_tree C4_5;
	C4_5.build_tree_from_file("./DATA SET/adult.data");

	C4_5.judge_file("./DATA SET/adult.test");
	return 0;

}
