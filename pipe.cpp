// pipe.cpp : Defines the entry point for the console application.
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

		top[i] = number_of_rows + 1;
		bottom[i] = 0;
	
		/*like this
		when number of rows=3
		4 x x x x x x x x x
		3
		2
		1
		0 x x x x x x x x x

		*/

	}

}

bool if_failed(int number_of_columns, int *top, int *bottom) {

	// Sets negative numbers to 0
	int temp;

	for (int i = 0; i < number_of_columns ; i++) {
		temp = top[i] - bottom[i];
		if (temp ==1) {
			return true;
		}
	}


	return false;
}

void clot(int number_of_rows,int number_of_columns, int *top, int *bottom, double *possibility, double initial_velocity, double density_of_dirt) {
	
	double coefficient = 1;//for different situation
	std::uniform_real_distribution<> if_deposit(0, 1);
	//clot
	for (int i = 0; i < number_of_columns; ++i) {
		coefficient = 1;
		possibility[i] = 1 - exp(-density_of_dirt);
		//cout <<"clot possibility  "<< possibility[i] << endl;
		//for top
		//when there is no dirt 
		if (top[i] == number_of_rows+1) {
			
			coefficient = coefficient -0.5;

		}
		/*x x x
		    x 
		*/
		if (top[i + 1] < top[i]) {
		
			coefficient = coefficient +0.5;
		}

		if (if_deposit(s) <coefficient* possibility[i]) {
			top[i] = top[i] - 1;
		}
		//for bottom
		coefficient = 1;
		//when there is no dirt 
		if (bottom[i] == 0) {

			coefficient = coefficient - 0.5;

		}
		/*x x x
		x
		*/
		if (bottom[i + 1] > bottom[i]) {

			coefficient = coefficient + 0.5;
		}

		if (if_deposit(s) <coefficient* possibility[i]) {
			bottom[i] = bottom[i] + 1;
		}
		

		
	}
	//wash away dirt
	for (int i = 0; i < number_of_columns; ++i) {
		coefficient = 1;
		possibility[i] = 1 - exp(-initial_velocity*number_of_rows / (top[i] - bottom[i] + 1));
		//cout <<"wash away possibility  "<< possibility[i] << endl;
		//for top
		if (top[i] != number_of_rows + 1) {

			//when the dirt is not on another dirt
			if (top[i] == number_of_rows) {

				coefficient = coefficient + 0.5;

			}
			/*x x x
			x
			*/
			if (top[i + 1] < top[i]) {

				coefficient = coefficient - 0.5;
			}

			if (if_deposit(s) < coefficient* possibility[i]) {
				top[i] = top[i] + 1;
			}

		}
		//for bottom
		if (bottom[i] != 0) {
			
			coefficient = 1;
			
			if (bottom[i] == 1) {

				coefficient = coefficient + 0.5;

			}
			/*x x x
			x
			*/
			if (bottom[i + 1] > bottom[i]) {

				coefficient = coefficient - 0.5;
			}

			if (if_deposit(s) < coefficient* possibility[i]) {
				bottom[i] = bottom[i] - 1;
			}

		}

	}

}



int main(int argc, char* argv[])
{
	ArgParse parser(argc, argv);


	//input parameters

	int number_of_rows = 3;//width
	parser.get("rows", number_of_rows);

	int number_of_columns = 20;//length
	parser.get("columns", number_of_columns);

	int number_of_systems = 3;
	parser.get("systems", number_of_systems);

	int max_number_of_steps = 1000;
	parser.get("steps", max_number_of_steps);

	double initial_velocity = 0.1;
	parser.get("velocity", initial_velocity);

	double density_of_dirt=0.1;
	parser.get("density", density_of_dirt);



	//creat arrays storing data
	double *possibility = new double[number_of_columns];
	int *top = new int[number_of_columns];
	int *bottom = new int[number_of_columns]();
	int *how_many_failed = new int[max_number_of_steps + 1]();

	vector<int> fail_times;

	int how_many_pipe_to_plot = 3;
	char temp[100];
	//draw a pipe at the initial state
	initialize(number_of_rows, number_of_columns, top, bottom);
		ofstream file_for_top("initialtop.csv");
		ofstream file_for_bottom("initialbottom.csv");
		for (int i = 0; i < number_of_columns; ++i) {
			file_for_top << top[i] << endl;
			file_for_bottom << bottom[i] << endl;
		}

		file_for_top.close();
		file_for_bottom.close();
	
	for (int n = 1; n <= number_of_systems; ++n) {
	
		initialize(number_of_rows, number_of_columns, top, bottom);

		//when will this system fail?
		for (int t = 1; t <= max_number_of_steps; ++t) {

			if (if_failed(number_of_columns, top, bottom)) {
				for (int tt = t; tt <= max_number_of_steps; ++tt) {
					how_many_failed[tt] = how_many_failed[tt] + 1;
				}
				fail_times.push_back(t);
				cout << "from " << n << " systems " << fail_times.size() << "systems failed" << endl;
				// output files for drawing some pillars
				if (n <= how_many_pipe_to_plot) {
					sprintf_s(temp, "top%d.txt", n);
					ofstream file_for_top(temp);
					sprintf_s(temp, "bottom%d.txt", n);
					ofstream file_for_bottom(temp);

					for (int i = 0; i < number_of_columns; ++i) {
						file_for_top << top[i] << endl;
						file_for_bottom << bottom[i] << endl;
					}

					file_for_top.close();
					file_for_bottom.close();
				}
				break;
			}
			else {
				clot(number_of_rows,number_of_columns, top, bottom,possibility,initial_velocity,density_of_dirt);
				
			}


		}
	}
	delete[] possibility;
	delete[] top;
	delete[] bottom;

	/*cout << fail_times.size() << "systems failed" << endl;
	ofstream file_for_fail_time("fail_timepp.csv");
	ofstream per("percentagepp.csv");
	for (auto t : fail_times)
		file_for_fail_time << t << endl;
	for (int i = 1; i <= max_number_of_steps; ++i) {
		per << how_many_failed[i] << endl;
	}
	file_for_fail_time.close();
	per.close();

*/
	delete[] how_many_failed;
	
	

	
    return 0;
}

