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

/*
 * Generate random points with
 * approximately 4 clusters
 */
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
 * Computes Sum in Parallel
 * Time Complexity - O(logn)
 * Work, Cost - O(n)
 */
void sumParallel(vector<Point> &inp,double &sumX,double &sumY){
	int num = inp.size();

	if(num==0) {
		sumX = 1e+18;
		sumY = 1e+18;
		return;
	}

	/* make Batches */
	int numBatches = 1<<((int)log2(num/(log2(num))));
	Timer st = high_resolution_clock::now();

	#pragma omp parallel for
	for(int i=0;i<numBatches;i++){
		#pragma omp parallel for
		for(int j=i+numBatches;j<num;j+=numBatches){
			inp[i].x += inp[j].x;			
			inp[i].y += inp[j].y;			
		}
	}

	/* n is power of two */
	int n = numBatches;
	while(n){
		n >>= 1;
		#pragma omp parallel for
		for(int i=0;i<n;i++){
			inp[i].x += inp[i+n].x;
			inp[i].y += inp[i+n].y;
		}
	}
	sumX = inp[0].x;
	sumY = inp[0].y;
	
	Timer end = high_resolution_clock::now();
	totalTime += duration_cast<duration<double>>(end - st).count();
}

/*
 * For each label, creates a list of Points
 * which need to be summed up to find mean
 * Time Complexity - O(logn)
 * Work, Cost - O(nlogn)
 */
void listPacking(vector<Point> &pts, double &sumX, double &sumY, int label) {
	double time1 = 0;
	double sz=0;

	int n = pts.size();
	vector<int> val(n+1, 0);
	vector<int> next(n);

	for(int i=0;i<pts.size();i++) {
		if(pts[i].label==label) {
			val[i] = 1;
		}
	}
	
	#pragma omp parallel for
	for(int i=0;i<n;i++) next[i] = i+1;
	next[n-1] = -1;

	int h = log2(n) + 1;

	Timer st, end;

	for(;h;h--){
		vector<int> nextOld = next;
		vector<int> valOld = val;

		st = high_resolution_clock::now();

		#pragma omp parallel for
		for(int i=0;i<n;i++) {
			if(nextOld[i]!=-1){
				val[i] = valOld[i] + valOld[nextOld[i]];
				next[i] = nextOld[nextOld[i]];
			}
		}

		end = high_resolution_clock::now();
		totalTime += duration_cast<duration<double>>(end - st).count();
	}

	sz = val[0];

	if(sz==0) {
		sumX = sumY = 1e+18;
		return;
	}

	vector<Point> labelVals(sz);

	st = high_resolution_clock::now();

	#pragma omp parallel for
	for(int i=0;i<n-1;i++) {
		if(val[i]>val[i+1]) {
			labelVals[val[i]-1] = pts[i];
		}
	}
	if(val[n-1]==1) labelVals[0] = pts[n-1];

	end = high_resolution_clock::now();
	totalTime += duration_cast<duration<double>>(end - st).count();

	sumParallel(labelVals, sumX, sumY);

	sumX /= sz*1.0;
	sumY /= sz*1.0;

	return;
}

/*
 * Update centers to mean of points with corresponding label
 */
inline bool updateCenters(vector<Point> &pts, vector<Point> &center, int num, int k) {
	vector<double> xAvg(k, 0);
	vector<double> yAvg(k, 0);

	for(int i=0;i<k;i++) {
		listPacking(pts,xAvg[i],yAvg[i],i);
	}

	bool changed = false;

	Timer st, end;
	st = high_resolution_clock::now();

	#pragma omp parallel for
	for(int i=0;i<k;i++) {
		if(xAvg[i]==1e+18 && yAvg[i]==1e+18) continue;

		changed |= (center[i].getCoordinates()==make_pair(xAvg[i], yAvg[i]));
		center[i].setCoordinates(xAvg[i], yAvg[i]);
	}

	end = high_resolution_clock::now();
	totalTime += duration_cast<duration<double>>(end - st).count();

	return changed;
}

/*
 * Initialize with random centers
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

	randPointGen(pts, NUM);
	// readPointsTxt(pts, NUM, "./Dataset/birch100k.txt");
	// readPointsCSV(pts, NUM, "./Dataset/kg.csv");

	int iterCount = 0;

    for(int initialPts=0;initialPts<40;initialPts++) {
    	vector<Point> center(K);

	    initialise(pts, center, NUM, K);

	    for(int iter = 0; iter<MAX_ITER;iter++) {

	    	#pragma omp parallel for
	    	for(int i=0;i<NUM;i++) {
				pts[i].updateLabelParallel(center, K);
			}

			writeToCSV(center, K, "C" + to_string(initialPts) + "_" + to_string(iter) + ".csv");
			writeToCSV(pts, NUM, "P" + to_string(initialPts) + "_" + to_string(iter) + ".csv");
	    	
			iterCount++;
	    	if(updateCenters(pts, center, NUM, K) && iter) break;
	    }
    }

    cout<<"Parallel Algorithm\n";
	cout<<"Average Time/Iteration : "<<totalTime/iterCount<<endl;

    return 0;
}
