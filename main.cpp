#include <bits/stdc++.h>
#include <fstream>

#include <chrono>
using namespace std::chrono;
typedef high_resolution_clock::time_point mytime;

using namespace std;

#include "points.h"

const int NUM = 100000;
const int K = 100;
const int MAX_ITER = 100;

void randPointGen(Point inp[],int num){
	for(int i=0;i<num;i++){
		double x = (rand()%25000)/100;
		double y = (rand()%25000)/100;

		inp[i].setCoordinates(x,y);
	}
}

void readPointsTxt(Point pts[], int num, string filePath) {
	std::ifstream fin;
	fin.open(filePath);

	for(int i=0;i<num;i++) {
		int x, y;
		fin>>x>>y;
		pts[i].setCoordinates(x,y);
	}

	fin.close();
}

void readPointsCSV(Point pts[], int num, string filePath) {
	std::ifstream fin;
	fin.open(filePath);

	string junk;
	getline(fin, junk);

	for(int i=0;i<num;i++) {
		double x, y;
		char ch;
		fin>>x>>ch>>y;
		pts[i].setCoordinates(x,y);
	}

	fin.close();
}

bool updateCenters(Point pts[], Point center[], int num, int k) {
	vector<double> xAvg(k, 0);
	vector<double> yAvg(k, 0);
	vector<double> cnt(k, 0);

	#pragma omp parallel for
	for(int i=0;i<num;i++) {
		double x, y;
		tie(x,y) = pts[i].getCoordinates();
		int label = pts[i].getLabel();

		xAvg[label] += x;
		yAvg[label] += y;
		cnt[label]++;
	}

	for(int i=0;i<k;i++) {
		xAvg[i] /= cnt[i];
		yAvg[i] /= cnt[i];
	}

	bool changed = false;

	for(int i=0;i<k;i++) {
		changed |= (center[i].getCoordinates()==make_pair(xAvg[i], yAvg[i]));
		center[i].setCoordinates(xAvg[i], yAvg[i]);
	}

	return changed;
}

void initialise(Point pts[], Point center[], int num, int k) {
	bool selected[num] = {};
	int totalSelected = 0;

	while(totalSelected<k) {
		int index = rand()%num;
		if(!selected[index]) {
			selected[index] = true;

			center[totalSelected] = pts[index];
			center[totalSelected].setLabel(totalSelected);

			totalSelected++;
		}
	}
}

void printCenters(Point center[], int k) {
	cout<<"------------\n";
	for(int i=0;i<k;i++) {
		double x, y;
		tie(x, y) = center[i].getCoordinates();
		cout<<std::fixed<<x<<" "<<y<<" "<<center[i].getLabel()<<endl;
	}
}

void writeToCSV(Point pts[], int num, string fileName) {
	std::ofstream fout;
	fout.open("./CSV/" + fileName);

	fout<<"x,y,label\n";

	for(int i=0;i<num;i++) {
		double x, y;
		tie(x, y) = pts[i].getCoordinates();
		int label = pts[i].getLabel();

		fout<<std::fixed<<x<<","<<y<<","<<label<<"\n";
	}

	fout.close();
}

int main() {
	srand(time(0));

    Point pts[NUM];

	// randPointGen(pts,NUM);
	readPointsTxt(pts, NUM, "./Dataset/birch100k.txt");
	// readPointsCSV(pts, NUM, "./Dataset/kg.csv");

	mytime st = high_resolution_clock::now();

    for(int initialPts=0;initialPts<40;initialPts++) {
    	Point center[K];

	    initialise(pts, center, NUM, K);
	    // printCenters(center, K);

	    for(int iter = 0; iter<MAX_ITER;iter++) {

	    	#pragma omp parallel for
	    	for(int i=0;i<NUM;i++) {
				pts[i].updateLabel(center, K);
			}

			// writeToCSV(center, K, "C" + to_string(initialPts) + "_" + to_string(iter)+".csv");
			// writeToCSV(pts, NUM, "P" + to_string(initialPts) + "_" + to_string(iter)+".csv");
	    	
	    	if(updateCenters(pts, center, NUM, K) && iter) {
	    		// cout<<"$"<<iter<<endl;
	    		break;
	    	}
	    	// printCenters(center, K);
	    }
    }

    mytime end = high_resolution_clock::now();
	duration<double> time_span = duration_cast<duration<double>>(end - st);

	cout<<time_span.count();

    return 0;
}
