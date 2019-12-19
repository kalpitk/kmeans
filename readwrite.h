/*
 * Author : Kalpit Kothari, Arjit Arora
 *
 * CS 359 Parallel Computing Project
 */

#include <fstream>

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
