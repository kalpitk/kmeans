#include <bits/stdc++.h>
#include <fstream>

#include <chrono>
using namespace std::chrono;
typedef high_resolution_clock::time_point mytime;

using namespace std;

#include "points.h"

const int NUM = 100;
const int K = 4;
const int MAX_ITER = 100;

double totalTime = 0;

void randPointGen(vector<Point> &pts,int num){
	for(int i=0;i<num;i++){
		double x = (rand()%250000)/1000;
		double y = (rand()%250000)/1000;

		if(i>50) x+=400;

		if(i>25&&i<50) y+= 400;
		if(i>75&&i<100) y+= 400;

		pts[i].setCoordinates(x,y);
	}
}

void readPointsTxt(vector<Point> &pts, int num, string filePath) {
	std::ifstream fin;
	fin.open(filePath);

	for(int i=0;i<num;i++) {
		int x, y;
		fin>>x>>y;
		pts[i].setCoordinates(x,y);
	}

	fin.close();
}

void readPointsCSV(vector<Point> &pts, int num, string filePath) {
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

void printCenters(vector<Point> &center, int k) {
	cout<<"Center List:\n";
	for(int i=0;i<k;i++) {
		double x, y;
		x = center[i].x;
		y = center[i].y;
		cout<<std::fixed<<x<<" "<<y<<" "<<center[i].label<<endl;
	}
}

void writeToCSV(vector<Point> &pts, int num, string fileName) {
	std::ofstream fout;
	fout.open("./CSV/" + fileName);

	fout<<"x,y,label\n";

	for(int i=0;i<num;i++) {
		double x, y;
		x = pts[i].x;
		y = pts[i].y;
		int label = pts[i].label;

		fout<<std::fixed<<x<<","<<y<<","<<label<<"\n";
	}

	fout.close();
}

bool updateCenters(vector<Point> &pts, vector<Point> &center, int num, int k) {
	mytime st, end;
	st = high_resolution_clock::now();

	vector<double> xAvg(k, 0);
	vector<double> yAvg(k, 0);
	vector<double> cnt(k, 0);

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

	end = high_resolution_clock::now();
	totalTime += duration_cast<duration<double>>(end - st).count();

	return changed;
}

void initialise(vector<Point> &pts, vector<Point> &center, int num, int k) {
	mytime st, end;
	st = high_resolution_clock::now();

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

	end = high_resolution_clock::now();
	totalTime += duration_cast<duration<double>>(end - st).count();
}

int main() {
	srand(time(0));

    vector<Point> pts(NUM);

	randPointGen(pts,NUM);
	// readPointsTxt(pts, NUM, "./Dataset/birch100k.txt");
	// readPointsCSV(pts, NUM, "./Dataset/kg.csv");

    for(int initialPts=0;initialPts<5;initialPts++) {
    	vector<Point> center(K);

	    initialise(pts, center, NUM, K);
	    // printCenters(center, K);

	    for(int iter = 0; iter<MAX_ITER;iter++) {

	    	mytime st, end;
	    	st = high_resolution_clock::now();

	    	for(int i=0;i<NUM;i++) {
				pts[i].updateLabel(center, K);
			}

			end = high_resolution_clock::now();
			totalTime += duration_cast<duration<double>>(end - st).count();

			writeToCSV(center, K, "C" + to_string(initialPts) + "_" + to_string(iter)+".csv");
			writeToCSV(pts, NUM, "P" + to_string(initialPts) + "_" + to_string(iter)+".csv");
	    	
	    	if(updateCenters(pts, center, NUM, K) && iter) break;
	    	
	    	// printCenters(center, K);
	    }
    }

	cout<<"\nSequential Algorithm\n";
	cout<<"Time taken : "<<totalTime<<endl;

    return 0;
}