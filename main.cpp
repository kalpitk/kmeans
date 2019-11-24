#include <bits/stdc++.h>
#include <fstream>
using namespace std;

#include "points.h"

const int NUM = 6;
const int K = 2;

void updateCenters(Point pts[], Point center[], int num, int k) {
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

	for(int i=0;i<k;i++) {
		center[i].updateCoordinates(xAvg[i], yAvg[i]);
	}
}

void initialise(Point pts[], Point center[], int num, int k) {
	bool selected[num] = {};
	int totalSelected = 0;

	while(totalSelected<k) {
		int index = rand()%num;
		if(!selected[index]) {
			selected[index] = true;
			center[totalSelected] = pts[index];
			totalSelected++;
		}
	}
}

void printCenters(Point center[], int k) {
	cout<<"------------\n";
	for(int i=0;i<k;i++) {
		double x, y;
		tie(x, y) = center[i].getCoordinates();
		cout<<std::fixed<<x<<" "<<y<<endl;
	}
}

void writeToCSV(Point pts[], int num, string fileName, bool writeLabel = false) {
	std::ofstream fout;
	fout.open("./CSV/" + fileName);

	fout<<"\n";

	for(int i=0;i<num;i++) {
		double x, y;
		tie(x, y) = pts[i].getCoordinates();
		int label = pts[i].getLabel();

		fout<<std::fixed<<x<<","<<y<<",";
		if(writeLabel) fout<<label<<",";
		fout<<"\n";
	}

	fout.close();
}

int main() {
	srand(time(0));

    Point pts[NUM] = {
    	Point(1.0, 2.0, -1),
    	Point(2.0, 2.0, -1),
    	Point(3.0, 2.0, -1),
    	Point(4.0, 2.0, -1),
    	Point(5.0, 2.0, -1),
    	Point(6.0, 2.0, -1)
    };

    Point center[K];

    initialise(pts, center, NUM, K);
    printCenters(center, K);

    writeToCSV(center, K, "C0.csv");
    writeToCSV(pts, NUM, "P0.csv", true);

    for(int iter = 0; iter<5;iter++) {
    	updateCenters(pts, center, NUM, K);
    	printCenters(center, K);

    	writeToCSV(center, K, "C"+to_string(iter)+".csv");
    	writeToCSV(pts, NUM, "P"+to_string(iter)+".csv", true);
    }

    return 0;
}