

#include "stdafx.h"
#include <iostream> 
#include <stdlib.h> 
#include <time.h>  
#include <fstream>
#include<string>
using namespace std;

const int maxrow = 15, maxcolumn = 12, number_of_objects = 100, tmax = 1000;

int right_limit(int row[5 * maxcolumn*maxrow]) {
	for (int i = 5 * maxcolumn*maxrow-1; i >= 0; i--) {
		if (row[i] == 1) {
			return i;
		}

	}
	return -1;
}
int left_limit(int row[5 * maxcolumn*maxrow]) {
	for (int i = 0; i <5 * maxcolumn*maxrow; i++) {
		if (row[i] == 1) {
			return i;
		}

	}
	return -1;
}

int number_of_blocks_in_a_row(int row[5 * maxcolumn*maxrow]) {
	int j = 0;
	for (int i = 0; i <5 * maxcolumn*maxrow; i++) {

		j = row[i] + j;

	}
	return j;

}

bool if_failed(int system[maxrow][5 * maxcolumn*maxrow]) {

	int n = 0;
	for (int j = 0; j <maxrow; j++) {
		n = 0;
		for (int i = 0; i < 5 * maxcolumn*maxrow; i++) {
			n = system[j][i] + n;
		}
		if (n == 0) {
			return true;
		}
	}
	return false;
}

int main()
{
	int emsemble[number_of_objects][maxrow][5 * maxcolumn*maxrow] = { 0 };
	
	int number_of_failed_objects = 0;
	double possiblity_of_move_this_row = 0;
	int which_row_to_place=0;
	int number_of_moved_blocks = 0;
	
	int temp = 0;
	ofstream results;
	results.open("conductor.txt", ios::in| ios::trunc|fstream::out);
	results << "the conductor has " << maxrow << " rows " << maxcolumn << " columns " << "and there are total " << number_of_objects << " systems" << endl;
	//initialize
	for (int i = 0; i <maxrow; i++) {
		for (int j = 0; j <5 * maxcolumn*maxrow; j++) {
			for (int n = 0; n < number_of_objects; n++) {	
					if (j > 2 * maxcolumn*maxrow && j <= 2 * maxcolumn*maxrow + maxcolumn) {
						emsemble[n][i][j] = 1;	
				}
			}
		}
	}

	std::cout << "steps number of failed systems" << endl;
	results << "steps number of failed systems" <<endl;
	for (int t = 1; t <=tmax; t++) {
		number_of_failed_objects = 0;
		for (int n = 0; n < number_of_objects; n++) {
			if (if_failed(emsemble[n])) {//in time t if the system n failed?
				number_of_failed_objects++;
			}
			else {
				number_of_moved_blocks = 0;

				//pick up blocks
				for (int r = 0; r < maxrow; r++) {//select a row,
					possiblity_of_move_this_row= 1.0/ number_of_blocks_in_a_row(emsemble[n][r]);
					
					//if move a block from the row
					if (rand() / double(RAND_MAX) <= possiblity_of_move_this_row) {
						//how many blocks need to move
						number_of_moved_blocks++;
						
						//move from right or left
						if (rand() % 2 == 0) {
							temp = right_limit(emsemble[n][r]);
							emsemble[n][r][temp] = 0;
							
						}
						else {
							temp = left_limit(emsemble[n][r]);
							emsemble[n][r][temp] = 0;
							
						}
					}
					//std::cout <<"step "<<t<<" "<<n<<"th "<< number_of_moved_blocks<<endl;
				}
				//after picking up some blocks, failed?
				if (if_failed(emsemble[n])) {
					number_of_failed_objects++;
				}
				else {
					//place blocks
					for (int i = 1; i <= number_of_moved_blocks; i++) {
						//choose a row to place
						
						which_row_to_place = rand() % maxrow;
						//place the block on the right or on the left?
					
						if (rand() % 2 == 0) {//place the block on the right
							temp = right_limit(emsemble[n][which_row_to_place]) + 1;
							emsemble[n][which_row_to_place][temp] = 1;
							
						}
						else {//place the block on the left
							temp = left_limit(emsemble[n][which_row_to_place]) - 1;
							emsemble[n][which_row_to_place][temp] = 1;
							
						}
					}
				}




			}
		}
	
		std::cout <<t<<"       " << number_of_failed_objects << endl;
		results << t << "       " << number_of_failed_objects << endl;
	}



	results.close();

	return 0;
}

