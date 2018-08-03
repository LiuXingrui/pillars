// pillars.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <iostream> 
#include <stdlib.h> 
#include <time.h>  
#include <fstream>
#include<string>
#include <random>
#include "ArgParse.hpp"
#include<algorithm>
#include <vector>
#include<math.h>


using std::cout;
using std::endl;
using std::string;
using std::min;
using std::max;
using std::vector;
using std::ofstream;

std::default_random_engine s((unsigned)time(NULL));


void initialize(int number_of_rows, int number_of_columns, int *top, int *bottom) {

	for (int i = 0; i < number_of_columns; i++) {
		
		top[i] = number_of_rows;
		bottom [i] = 0;
		
	}
	
}

//overlap[0] and overlap[number_of_columns] are just the 
//number of blocks in column[0] and column[number_of_columns-1]
void get_overlap_beteen_two_columns(int number_of_columns, int *top, int *bottom, int *overlap) {

	auto clamp = [](int x) { return x>0 ? x : 0; };
	for (int i = 1; i < number_of_columns; i++) {

		overlap[i] = clamp(min(top[i-1], top[i]) - max(bottom[i-1], bottom[i]));
	}
	overlap[0] = clamp(top[0] - bottom[0]);
	overlap[number_of_columns] = clamp(top[number_of_columns - 1] - bottom[number_of_columns - 1]);
	}


//get the possibility of removing
void get_p(int number_of_columns, double *possibility,int *top,int *bottom) {
	auto clamp = [](int x) { return x>0 ? x : 0; };
	for (int i = 0; i < number_of_columns-1; i++) {
		possibility[i] = 1.0/ clamp(min(top[i], top[i + 1]) - max(bottom[i], bottom[i + 1]));
	
	}
	possibility[number_of_columns - 1] = top[number_of_columns - 1] - bottom[number_of_columns - 1];
}

bool if_failed(int number_of_columns, int *top, int *bottom) {
	
	// Sets negative numbers to 0
	auto clamp = [](int x) { return x>0 ? x : 0; };
	int overlap=0;
	
	for (int i = 0; i < number_of_columns-1; i++) {
		// Compute the overlap between the [i] and [i+1] columns
		overlap = clamp(min(top[i], top[i+1]) - max(bottom[i], bottom[i+1]));
		if (overlap == 0) {
			return true;
		}
	}


	return false;
}

//remove the blocks and give the number of the removed blocks 
int remove(int number_of_columns, double *possibility, int *top, int *bottom) {
	std::uniform_int_distribution<unsigned> random_number_for_top_or_bottom(0, 1);
	std::uniform_real_distribution<> if_remove(0, 1);
	
	
	int number_of_removed_blocks = 0;
	
	//get the possibility of if move a certern column
	get_p(number_of_columns, possibility, top, bottom);
	
	for (int c = 0; c < number_of_columns; c++) {//select a column
		
		//if move a block from the column
		if (if_remove(s) <= *(possibility+c) ){
			//how many blocks need to move
			number_of_removed_blocks++;

			//remove from top or bottom
			if (random_number_for_top_or_bottom(s)==0) {//remove from top
				top[c] = top[c] - 1;

			}
			else {//remove from bottom
				bottom[c] = bottom[c] + 1;

			}
		}
	}
	return number_of_removed_blocks;


}
void place_back(int number_of_columns,int number_of_removed_blocks, int *top, int *bottom){
	std::uniform_int_distribution<unsigned> random_number_for_top_or_bottom(0, 1);
	std::uniform_int_distribution<unsigned> random_number_for_choosing_a_column(0, number_of_columns - 1);
	
	int which_row_to_place = 0;
	for (int i = 1; i <= number_of_removed_blocks; i++) {
		//choose a column to place

		which_row_to_place = random_number_for_choosing_a_column(s);
		//place the block on the top or on the bottom?

		if (random_number_for_top_or_bottom(s) == 0) {//place the block on the top
			top[ which_row_to_place] =top[which_row_to_place] + 1;

		}
		else {//place the block on the bottom
			bottom[which_row_to_place] = bottom[which_row_to_place] - 1;

		}
	}

}


double average_square(int number_of_columns, int *top, int *bottom) {
	double radius_square_sum = 0.0;
	for (int i = 0; i < number_of_columns; i++) {
		radius_square_sum = radius_square_sum + pow(top[i] - bottom[i],2);

	}
	return radius_square_sum / number_of_columns;
}

double get_variance(int number_of_rows,int number_of_columns, int *top, int *bottom) {
	

	return average_square(number_of_columns, top, bottom) - pow(number_of_rows,2);
}


int main(int argc, char* argv[])
{
	ArgParse parser(argc,argv);
	
	
	//input parameters
	
	int number_of_rows=15;
	parser.get("rows", number_of_rows);
	
	int number_of_columns = 7;
	parser.get("columns", number_of_columns);
	
	int number_of_systems= 100;
	parser.get("systems", number_of_systems);

	int number_of_steps = 1000;
	parser.get("steps", number_of_steps);
		
	
	
	
	//creat arrays storing data
	double *possibility = new double[number_of_columns];
	int *top = new int[number_of_columns];
	int *bottom = new int[number_of_columns]();
	int number_of_removed_blocks = 0;
	double *variance = new double[number_of_systems+1];
	vector<int> fail_times;
	
	ofstream file_for_variance("variance.txt");

	for (int n = 1; n <= number_of_systems; n++) {

		//initialize a new system
		initialize(number_of_rows,number_of_columns,top,bottom);
		
		

		//when will this system fail?
		for (int t = 1; t <= number_of_steps; t++) {
			if (if_failed(number_of_columns, top, bottom)) {
				//*(how_many_systems_failed + t)= *(how_many_systems_failed + t)+1;
				fail_times.push_back(t);
				variance[n] = get_variance(number_of_rows,number_of_columns, top, bottom);
				file_for_variance << variance[n] << endl;
				break;
			}
			else {
				number_of_removed_blocks = remove(number_of_columns, possibility, top, bottom);
				
				place_back(number_of_columns,number_of_removed_blocks, top, bottom);
			}


		}
	}
	//cout << "steps   number of failed systems" << endl;
	//for (int t = 1; t <= number_of_steps; t++) {
	
	//	cout << t << "       " << *(how_many_systems_failed+t) << endl;
	//}
	ofstream file_for_top("top.txt");
	ofstream file_for_bottom("bottom.txt");
	ofstream file_for_fail_time("fail_time.txt");
	for (auto t : fail_times)
		file_for_fail_time << t << endl;
	for (int i = 0; i < number_of_columns; i++) {
		file_for_top << top[i] << endl;
		file_for_bottom << bottom[i] << endl;
	
	}

	delete[] possibility;
	delete [] top;
	delete[] bottom;
	delete[] variance;
	//delete[] how_many_systems_failed;
	file_for_fail_time.close();
	file_for_variance.close();
	file_for_top.close();
	file_for_bottom.close();
	return 0;

	
}