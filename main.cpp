#include <bits/stdc++.h>
#include <fstream>

#include <chrono>
using namespace std::chrono;
typedef high_resolution_clock::time_point mytime;

using namespace std;

#include "points.h"

const int NUM = 100;
const int K = 10;
const int MAX_ITER = 100;

void randPointGen(vector<Point> &pts,int num){
	for(int i=0;i<num;i++){
		double x = (rand()%25000)/100;
		double y = (rand()%25000)/100;

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

double sumParallel(vector<Point> inp,double &sumX,double &sumY){
	int num = inp.size();

	if(num==0) {
		sumX = 0;
		sumY = 0;
		return 0;
	}
	sumX = 0;
		sumY = 0;
	for(int i=0;i<num;i++) {
		sumX += inp[i].x;
		sumY += inp[i].y;
	}
	return 0;

	int numBatches = 1<<((int)log2(num));
	/*make Batches*/
	mytime st = high_resolution_clock::now();

	#pragma omp parallel for
	for(int i=numBatches;i<num;i++) {
		inp[i%numBatches].x += inp[i].x;
		inp[i%numBatches].y += inp[i].y;
	}

	for(int i=0;i<numBatches;i++) {
		sumX += inp[i].x;
		sumY += inp[i].y;
		return 0;
	}

	/*set is power of two*/
	int n =numBatches;
	while(n){
		n>>=1;
		// #pragma omp parallel for
		for(int i=0;i<n;i++){
			inp[i].x += inp[i+n].x;
			inp[i].y += inp[i+n].y;
		}
	}
	sumX = inp[0].x;
	sumY = inp[0].y;
	
	mytime end = high_resolution_clock::now();
	duration<double> time_span = duration_cast<duration<double>>(end - st);
	return time_span.count();	
}

double listPacking(vector<Point> pts, double &sumX, double &sumY, int label) {
	double tm = 0;
	double sz=0;

	int n = pts.size();
	vector<int> val(n+1, 0);
	vector<int> next(n);

	for(int i=0;i<n;i++) {
		val[i] = (pts[i].getLabel()==label);
	}
	
	#pragma omp parallel for
	for(int i=0;i<n;i++) next[i] = i+1;
	next[n-1] = -1;

	int h = log2(n) + 1;

	// Time Start
	mytime st = high_resolution_clock::now();
	
	for(;h;h--){
		#pragma omp parallel for
		for(int i=0;i<n;i++) {
			//#pragma omp critical
			if(next[i]!=-1){
				val[i] += val[next[i]];
				next[i] = next[next[i]];
			}
		}
	}

	mytime end = high_resolution_clock::now();
	duration<double> time_span = duration_cast<duration<double>>(end - st);
	tm += time_span.count();

	sz = val[0];

	vector<Point> labelVals(sz,Point(0,0,0));

	st = high_resolution_clock::now();

	#pragma omp parallel for
	for(int i=0;i<n-1;i++) {
		if(val[i]>val[i+1]) {
			labelVals[val[i]-1] = pts[i];
			sz--;
		}
	}
	if(val[n-1]==1) {labelVals[0] = pts[n-1];sz--;}
	// assert(sz==0);

	// return 0;

	end = high_resolution_clock::now();
	time_span = duration_cast<duration<double>>(end - st);
	tm += time_span.count();

	tm += sumParallel(labelVals, sumX, sumY);

	// assert(labelVals1==labelVals);

	// if(sz==0) {
	// 	sumX = sumY = 1e+18;
	// 	return tm;
	// }

	// sumX /= sz*1.0;
	// sumY /= sz*1.0;

	return tm;
}

bool updateCenters(vector<Point> &pts, vector<Point> &center, int num, int k) {
	vector<double> xAvg(k, 0);
	vector<double> yAvg(k, 0);
	vector<double> cnt(k, 0);

	for(int i=0;i<k;i++) listPacking(pts,xAvg[i],yAvg[i],i);

	#pragma omp parallel for
	for(int i=0;i<num;i++) {
		double x, y;
		tie(x,y) = pts[i].getCoordinates();
		int label = pts[i].getLabel();

		// xAvg[label] += x;
		// yAvg[label] += y;
		cnt[label]++;
	}

	for(int i=0;i<k;i++) {
		xAvg[i] /= cnt[i];
		yAvg[i] /= cnt[i];
	}

	bool changed = false;

	for(int i=0;i<k;i++) {
		if(xAvg[i]==1e+18&&yAvg[i]==1e+18) continue;

		changed |= (center[i].getCoordinates()==make_pair(xAvg[i], yAvg[i]));
		center[i].setCoordinates(xAvg[i], yAvg[i]);
	}

	return changed;
}

void initialise(vector<Point> &pts, vector<Point> &center, int num, int k) {
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

void printCenters(vector<Point> &center, int k) {
	cout<<"------------\n";
	for(int i=0;i<k;i++) {
		double x, y;
		tie(x, y) = center[i].getCoordinates();
		cout<<std::fixed<<x<<" "<<y<<" "<<center[i].getLabel()<<endl;
	}
}

void writeToCSV(vector<Point> &pts, int num, string fileName) {
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

    vector<Point> pts(NUM);

	randPointGen(pts,NUM);
	// readPointsTxt(pts, NUM, "./Dataset/birch100k.txt");
	// readPointsCSV(pts, NUM, "./Dataset/kg.csv");

	mytime st = high_resolution_clock::now();

    for(int initialPts=0;initialPts<40;initialPts++) {
    	vector<Point> center(K);

	    initialise(pts, center, NUM, K);
	    // printCenters(center, K);

	    for(int iter = 0; iter<MAX_ITER;iter++) {

	    	#pragma omp parallel for
	    	for(int i=0;i<NUM;i++) {
				pts[i].updateLabel(center, K);
			}

			writeToCSV(center, K, "C" + to_string(initialPts) + "_" + to_string(iter)+".csv");
			writeToCSV(pts, NUM, "P" + to_string(initialPts) + "_" + to_string(iter)+".csv");
	    	
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