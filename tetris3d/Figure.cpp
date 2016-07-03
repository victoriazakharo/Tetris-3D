#include "Figure.h"
#include <fstream>

FigureData::FigureData(int color_val = 0)
{
	color = color_val;
}

std::vector<FigureData> FigureData::ReadFigures()
{
	const int size = 4;
	int figures, rotations;
	std::ifstream in("figures.txt");
	in >> figures;
	std::vector<FigureData> figureList;
	FigureData f;
	for(int i = 1; i <= figures; i++) {
		f = FigureData(i);
		in >> rotations;	
		for(int j = 0; j < rotations; j++) {
			std::vector<Point> v;
			for(int k = 0; k < size; k++) {
				Point p;
				in >> p.x >> p.y >> p.z;
				v.push_back(p);
			}				
			f.points.push_back(v);
		}			
		figureList.push_back(f);
	}
	in.close();
	return figureList;
}
