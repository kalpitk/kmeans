#include <bits/stdc++.h>
#include <fstream>

#include <chrono>
using namespace std::chrono;
typedef high_resolution_clock::time_point mytime;

using namespace std;

#include "points.h"

const int NUM = 10000;
const int K = 10;
const int MAX_ITER = 100;

double totalTime = 0;

void randPointGen(vector<Point> &pts,int num){
	for(int i=0;i<num;i++){
		double x = (rand()%250000)/1000;
		double y = (rand()%250000)/1000;

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

void sumParallel(vector<Point> &inp,double &sumX,double &sumY){
	int num = inp.size();

	if(num==0) {
		sumX = 1e+18;
		sumY = 1e+18;
		return;
	}

	/* make Batches */
	int numBatches = 1<<((int)log2(num/(log2(num))));
	mytime st = high_resolution_clock::now();

	#pragma omp parallel for
	for(int i=0;i<numBatches;i++){
		#pragma omp parallel for
		for(int j=i+numBatches;j<num;j+=numBatches){
			inp[i].x+= inp[j].x;			
			inp[i].y+= inp[j].y;			
		}
	}

	/* n is power of two */
	int n = numBatches;
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
	totalTime += duration_cast<duration<double>>(end - st).count();

	return;
}

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

	mytime st, end;

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

bool updateCenters(vector<Point> &pts, vector<Point> &center, int num, int k) {
	vector<double> xAvg(k, 0);
	vector<double> yAvg(k, 0);
	vector<double> cnt(k, 0);

	for(int i=0;i<k;i++) {
		listPacking(pts,xAvg[i],yAvg[i],i);
	}

	bool changed = false;

	mytime st, end;
	st = high_resolution_clock::now();

	for(int i=0;i<k;i++) {
		if(xAvg[i]==1e+18&&yAvg[i]==1e+18) continue;

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

void printCenters(vector<Point> &center, int k) {
	cout<<"------------\n";
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

int main() {
	srand(time(0));

    vector<Point> pts(NUM);

	randPointGen(pts,NUM);
	// readPointsTxt(pts, NUM, "./Dataset/birch100k.txt");
	// readPointsCSV(pts, NUM, "./Dataset/kg.csv");

    for(int initialPts=0;initialPts<40;initialPts++) {
    	vector<Point> center(K);

	    initialise(pts, center, NUM, K);
	    // printCenters(center, K);

	    for(int iter = 0; iter<MAX_ITER;iter++) {

	    	mytime st, end;
	    	st = high_resolution_clock::now();

	    	#pragma omp parallel for
	    	for(int i=0;i<NUM;i++) {
				pts[i].updateLabel(center, K);
			}

			end = high_resolution_clock::now();
			totalTime += duration_cast<duration<double>>(end - st).count();

			writeToCSV(center, K, "C" + to_string(initialPts) + "_" + to_string(iter)+".csv");
			writeToCSV(pts, NUM, "P" + to_string(initialPts) + "_" + to_string(iter)+".csv");
	    	
	    	if(updateCenters(pts, center, NUM, K) && iter) {
	    		// cout<<"$"<<iter<<endl;
	    		break;
	    	}
	    	// printCenters(center, K);
	    }
    }

	cout<<totalTime;

    return 0;
}