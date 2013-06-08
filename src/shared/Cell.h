

#include "Part.h"


const int   CELL_STRIDE = 20;
const float CELL_WIDTH = 2;
const float CELL_MASS = 1000;
const float CELL_MOMENT = CELL_MASS*(CELL_WIDTH*CELL_WIDTH)/6;  // http://en.wikipedia.org/wiki/List_of_moments_of_inertia


struct Cell {
	bool there;
	Part* part;
	
	Cell () : there(false), part(0) {}
	Cell (const Cell& cell) { *this = cell; if (part) part = new Part(*cell.part); }
	~Cell () { if (part) ; }
};
