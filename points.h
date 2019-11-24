class Point
{
	double x, y;
	int label;

public:
	Point(){}
	Point(double x, double y, int label) {
		this->x = x;
		this->y = y;
		this->label = label;
	}

	double distance(double x, double y) {
		double dist = (this->x - x)*(this->x - x)
					  + (this->y - y)*(this->y - y);
		return dist;
	}

	double distance(Point pt) {
		return this->distance(pt.x, pt.y);
	}

	void updateLabel(Point center[], int k) {
		double min = 1e+18;

		for(int i=0;i<k;i++) {
			double dist = this->distance(center[i]);
			if(dist<min) {
				min = dist;
				this->label = i;
			}
		}
	}

	int getLabel() {
		return this->label;
	}

	void updateCoordinates(double x, double y) {
		this->x = x;
		this->y = y;
	}

	pair<double,double> getCoordinates() {
		return {x, y};
	}
};