#pragma once
#include <vector>

struct Point
{
	int x, y, z;
	Point(int x_val = 0, int y_val = 0, int z_val = 0) : x(x_val), y(y_val), z(z_val) {}
};

struct FigureData
{
	int color;
	std::vector<std::vector<Point>> points;	
	FigureData(int color_val);
	static std::vector<FigureData> ReadFigures();
};

struct Figure
{
	int type;
	int rotation;
	Point pos;
	Figure(int type_val = 0, int rotation_val = 0, Point pos_val = Point()) : type(type_val), rotation(rotation_val), pos(pos_val) {}
};
