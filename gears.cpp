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
#define NDEBUG
#include <assert.h>


using std::cout;
using std::endl;
using std::string;
using std::min;
using std::max;
using std::vector;
using std::ofstream;
std::default_random_engine s((unsigned)time(NULL));

void initialize(int number_of_layers, int *left1, int *right1, int *left2, int *right2) {
	static int n = 0;
	char temp[100];
	for (int i = 0; i < number_of_layers; i++) {
		left1[i] =i/2 ;
		right1[i] = number_of_layers-i / 2;
	}
	for (int i = 0; i < number_of_layers; i++) {
		left2[i] = right1[i];
		right2[i] = right1[i]+ 2 * (i / 2)+2;
	}
	/*
	      x x  X X X X X X     5
		  x x  X X X X X X     4
		x x x x  X X X X       3
		x x x x  X X X X       2
	  x x x x x x  X X         1
	  x x x x x x  X X         0
	 0 1 2 3 4 5 6  7 8
	
	*/
	//for ploting the initial picture
	if (n <1) {
		sprintf_s(temp, "i_left1_%d.txt", n + 1);
		ofstream file_for_left1(temp);
		sprintf_s(temp, "i_left2_%d.txt", n + 1);
		ofstream file_for_left2(temp);
		sprintf_s(temp, "i_right2_%d.txt", n + 1);
		ofstream file_for_right2(temp);
		sprintf_s(temp, "i_right1_%d.txt", n + 1);
		ofstream file_for_right1(temp);

		for (int i = 0; i < number_of_layers; i++) {
			file_for_left1 << left1[i] << endl;
			file_for_left2 << left2[i] << endl;
			file_for_right1 << right1[i] << endl;
			file_for_right2 << right2[i] << endl;
		}

		file_for_left1.close();
		file_for_left2.close();
		file_for_right1.close();
		file_for_right2.close();

		n++;
	}
	


	
}
bool if_failed(int number_of_layers,int *left1, int *right1, int *left2, int *right2) {
	int temp1 = 0 , temp2 = 0;
	for (int i = 0; i < number_of_layers; i++) {
		//from top to bottom
		
		if(right1[number_of_layers - 1 - i] - left1[number_of_layers - 1 - i] > 0&&temp1==0) {
			temp1 = number_of_layers - i;//give the the first layer that is not empty plus 1
			break;
		}
	}

	// every layer is empty
	if (temp1 == 0) {
		return true;
	
	}
   for (int i = 0; i < number_of_layers; i++) {
	   //from bottom to top
			if (right2[i] - left2[i] >0&&temp2==0) {
				temp2 = i+1;
				break;
			}
	}
   
   if (temp1 < temp2) {
	   return true;
   }
	   /*
	  3        xxxx
temp2=2		    xx
temp1=1     xx
	  0    xxxx 
	   */
   
	
	return false;
}
void wear_off(int number_of_layers, int *left1, int *right1, int *left2, int *right2) {
	
	int temp=0;
	int temp2 = 0;
	double possibility= 0;
	int overlap = 0;
	std::uniform_real_distribution<> if_remove(0, 1);

	//how many layers from tooth1 are touching tooth2?
	for (int i = 0; i < number_of_layers; i++) {
		if (right1[i] == left2[i]) {
			temp++;
		}
	}
	assert(temp !=0);

	possibility = 1-exp(-5.0 / temp);//force_on_one_layer = 1 / temp
	//cout << possibility << endl;
	for (int i = 0; i < number_of_layers; i++) {
		//when the layer is not empty and touches tooth2
		if (if_remove(s)<=possibility&&right1[i]!=left1[i]&&right1[i]==left2[i]) {
			right1[i] = right1[i] - 1;
		}
		if (if_remove(s) <= possibility&&right2[i] != left2[i] && right1[i] == left2[i]) {
			left2[i] = left2[i] +1;
		}
	}
	
	for (int i = 0; i < number_of_layers; i++) {
		
		//if any layer of the first tooth become empty? If yes, place all the layers above it to -1,they broke
		//from bottom to top
	
		if (right1[i] == left1[i]){
			//make the layers above the empty layers empty
			for (int j = number_of_layers - 1; j >= i; j--) {
				left1[j] = -1;
				right1[j] = -1;
				
			}
		}
		//if any layer of the second tooth become empty?If yes, place all the layers below it to -1,they broke
		//from top to bottom
		if (right2[number_of_layers - 1 - i] == left2[number_of_layers - 1 - i]) {
			for (int j = 0; j <=number_of_layers - 1 - i; j++) {
				left2[j] = -2;
				right2[j] =-2;
				
			}
		}
	}
	temp = 0;
	//if there is a gap between these two teeth (none of the layers from tooth1 touches tooth2?)?
	for (int i = 0; i < number_of_layers; i++) {
		if (right1[i] == left2[i]) {
			temp++;
		}
	}
	if (temp == 0) {//when temp=0, it fails or exists a gap
		for (int i = 0; i < number_of_layers; i++) {

			if (left1[i] != right1[i] && left2[i] != right2[i]) {//when the same layers from different teeth both are not empty
				temp2 = right1[i];
				right1[i] = left2[i];
				left1[i] = left1[i] + left2[i] - temp2;

			}
		}

	}
	
}
void plot_gear(int how_many_gears_to_plot, int number_of_layers, int *left1, int *right1, int *left2, int *right2) {
	static int n = 0;
	char temp[100];
	if (n <how_many_gears_to_plot) {
		sprintf_s(temp, "left1_%d.txt", n+1);
		ofstream file_for_left1(temp);
		sprintf_s(temp, "left2_%d.txt", n+1);
		ofstream file_for_left2(temp);
		sprintf_s(temp, "right2_%d.txt", n+1);
		ofstream file_for_right2(temp);
		sprintf_s(temp, "right1_%d.txt", n+1);
		ofstream file_for_right1(temp);

		for (int i = 0; i < number_of_layers; i++) {
			file_for_left1 << left1[i] << endl;
			file_for_left2 << left2[i] << endl;
			file_for_right1 << right1[i] << endl;
			file_for_right2 << right2[i] << endl;
		}

		file_for_left1.close();
		file_for_left2.close();
		file_for_right1.close();
		file_for_right2.close();

		n++;
	}
}
int main(int argc, char* argv[])
{
	ArgParse parser(argc, argv);

	//input parameters

	int number_of_layers = 7;
	parser.get("layers", number_of_layers);

	int number_of_systems = 3000;
	parser.get("systems", number_of_systems);

	int steps= 0;
	int *left1 = new int[number_of_layers];
	int *right1 = new int[number_of_layers];
	int *left2 = new int[number_of_layers];
	int *right2 = new int[number_of_layers];

	vector<int> fail_times;
	
	int how_many_gears_to_plot=3;
	parser.get("plots", how_many_gears_to_plot);
	
	
	for (int n = 1; n <= number_of_systems; n++) {
		initialize(number_of_layers, left1, right1, left2, right2);
		steps = 0;
		while (!if_failed(number_of_layers, left1, right1, left2, right2)) {

			wear_off(number_of_layers,left1, right1, left2, right2);
			steps++;

		}
			fail_times.push_back(steps);
			plot_gear(how_many_gears_to_plot,number_of_layers, left1, right1, left2, right2);// output files for drawing some gears
	}
	ofstream file_for_fail_time("fail_time.csv");
	for (auto t : fail_times)
		file_for_fail_time << t << endl;
	
	file_for_fail_time.close();
	delete[]left1;
	delete[]right1;
	delete[]left2;
	delete[]right2;

	return 0;

}
