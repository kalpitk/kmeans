#include <bits/stdc++.h>
#include <fstream>
using namespace std;

#include "points.h"

const int NUM = 1000;
const int K = 5;
const int MAX_ITER = 100;

Point* rand_gen(Point inp[],int num){
	for(int i=0;i<num;i++){
		double x = (rand()%25000)/100;
		double y = (rand()%25000)/100;

		inp[i].setCoordinates(x,y);
	}
}

bool updateCenters(Point pts[], Point center[], int num, int k) {
	double xAvg[k];
	double yAvg[k];
	double cnt[k];

	memset(xAvg, 0, k);
	memset(yAvg, 0, k);
	memset(cnt, 0, k);

	for(int i=0;i<num;i++) {
		pts[i].updateLabel(center, k);
	}

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

    Point pts[NUM];/* = {
    	Point(1.0, 2.0, -1),
    	Point(2.0, 2.0, -1),
    	Point(3.0, 2.0, -1),
    	Point(4.0, 2.0, -1),
    	Point(5.0, 2.0, -1),
    	Point(6.0, 2.0, -1)
    };*/

	rand_gen(pts,NUM);
    for(int initialPts=0;initialPts<4;initialPts++) {
    	Point center[K];

	    initialise(pts, center, NUM, K);
	    printCenters(center, K);

	    for(int iter = 0; iter<MAX_ITER;iter++) {
	    	writeToCSV(center, K, "C" + to_string(initialPts) + "_" + to_string(iter)+".csv");
	    	
	    	if(updateCenters(pts, center, NUM, K) && iter) {
	    		writeToCSV(pts, NUM, "P" + to_string(initialPts) + "_" + to_string(iter)+".csv");
	    		break;
			}
			writeToCSV(pts, NUM, "P" + to_string(initialPts) + "_" + to_string(iter)+".csv");
	    	printCenters(center, K);	    	
	    }
    }

    return 0;
}
