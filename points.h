class Point
{
public:
	double x, y;
	int label;

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

	void updateLabel(vector<Point> &center, int k) {
		double min = 1e+18;

		# pragma omp parallel for
		for(int i=0;i<k;i++) {
			double dist = this->distance(center[i]);
			if(dist<min) {
				min = dist;
				this->label = i;
			}
		}
	}

	void updateLabelParallel(vector<Point> &center, int k) {
		vector<Point> arr = center;
		for(int i=0;i<k;i++) arr[i].label = i;

		int h = log2(k) + 2;
		for(;h;h--) {
			k >>= 1;
			# pragma omp parallel for
			for(int i=0;i<k;i++) {
				if(this->distance(arr[i]) > this->distance(arr[i+k]))
					arr[i] = arr[i+k];
			}
		}

		this->label = arr[0].label;
	}

	void updateLabel(Point center[], int k) {
		double min = 1e+18;

		# pragma omp parallel for
		for(int i=0;i<k;i++) {
			double dist = this->distance(center[i]);
			if(dist<min) {
				min = dist;
				this->label = i;
			}
		}
	}

	void setLabel(int label) {
		this->label = label;
	}

	int getLabel() {
		return this->label;
	}

	void setCoordinates(double x, double y) {
		this->x = x;
		this->y = y;
	}

	pair<double,double> getCoordinates() {
		return {x, y};
	}
};