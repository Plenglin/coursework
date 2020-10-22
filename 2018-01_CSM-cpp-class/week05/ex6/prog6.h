class Box {
	double width, height, depth;

public:

	Box();
	Box(double w, double h, double d);

	double getWidth();
	double getHeight();
	double getDepth();
	void setWidth(double width);
	void setHeight(double height);
	void setDepth(double depth);

	double calcArea();
	double calcVolume();

	bool isValid();
};
