/*
 * Author : Kalpit Kothari, Arjit Arora
 *
 * CS 359 Parallel Computing Project
 */

#include <bits/stdc++.h>

#include <chrono>
using namespace std::chrono;
typedef high_resolution_clock::time_point Timer;

using namespace std;

#include "points.h"
#include "readwrite.h"

const int NUM = 10000;
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

void printCenters(vector<Point> &center, int k) {
	cout<<"Center List:\n";
	for(int i=0;i<k;i++) {
		double x, y;
		x = center[i].x;
		y = center[i].y;
		cout<<std::fixed<<x<<" "<<y<<" "<<center[i].label<<endl;
	}
}

/*
 * Update centers to mean of points with corresponding label
 */
bool updateCenters(vector<Point> &pts, vector<Point> &center, int num, int k) {
	Timer st, end;
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

/*
 * Initialize with random centers
 * Ensure no point is picked by two centers
 */
inline void initialise(vector<Point> &pts, vector<Point> &center, int num, int k) {
	bool selected[num] = {};

	Timer st, end;
	st = high_resolution_clock::now();

	for(int i=0;i<k;i++) {
		int index = rand()%num;
		center[i] = pts[index];
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

	int iterCount = 0;

    for(int initialPts=0;initialPts<5;initialPts++) {
    	vector<Point> center(K);

	    initialise(pts, center, NUM, K);

	    for(int iter = 0; iter<MAX_ITER;iter++) {

	    	Timer st, end;
	    	st = high_resolution_clock::now();

	    	for(int i=0;i<NUM;i++) {
				pts[i].updateLabel(center, K);
			}

			end = high_resolution_clock::now();
			totalTime += duration_cast<duration<double>>(end - st).count();

			writeToCSV(center, K, "C" + to_string(initialPts) + "_" + to_string(iter)+".csv");
			writeToCSV(pts, NUM, "P" + to_string(initialPts) + "_" + to_string(iter)+".csv");
	    	
			iterCount++;
	    	if(updateCenters(pts, center, NUM, K) && iter) break;
	    }
    }

	cout<<"\nSequential Algorithm\n";
	cout<<"Average Time/Iteration : "<<totalTime/iterCount<<endl;

    return 0;
}
