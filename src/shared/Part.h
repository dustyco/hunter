

typedef int PartID;
const PartID SHIP_PART_STRIDE = 4;
const PartID FUEL_TANK = 0;
const PartID POWER_GEN = 1;
const PartID CREW_AREA = 2;
const PartID CARGO_BAY = 3;
const PartID THRUSTER = 4;
//const PartID AVAILABLE = 5;
//const PartID AVAILABLE = 6;
const PartID TRACTOR_BEAM = 7;
const PartID ENGINE_0 = 8;
//const PartID AVAILABLE = 9;
//const PartID AVAILABLE = 10;
//const PartID AVAILABLE = 11;
const PartID ENGINE_1 = 12;
//const PartID AVAILABLE = 13;
//const PartID AVAILABLE = 14;
//const PartID AVAILABLE = 15;


struct Part {
	bool there;
	PartID id;
	
	Part () : there(false) {}
	Part (PartID id_) : there(false), id(id_) {}
};
