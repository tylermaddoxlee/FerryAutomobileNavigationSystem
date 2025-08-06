//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
/*
    Module: VehicleASM.cpp
    Revision History:
    Revision 1.0: 2025-07-18 - Original by Tyler Lee
    Purpose:
        Implementation of VehicleASM file operations including:
        -initialize vehicle storage file
        -add vehicle
        -close vehicle data
        -find vehicle by license plate
*/

//============================================
#include <fstream>
#include <iostream>
#include "VehicleASM.h"
#include "Vehicle.h"

using namespace std;

static fstream vehicleFile;  // file-scope binary stream for vehicle data

//============================================
void initializeVehicleStorage()
{
    vehicleFile.open("vehicles.dat", ios::binary | ios::in | ios::out);
	if (!vehicleFile.is_open()) {
		cerr << "vehicles.dat not found. Creating new file." << endl;
		ofstream createFile("vehicles.dat", ios::binary);
		createFile.close();
		vehicleFile.open("vehicles.dat", ios::binary | ios::in | ios::out);
	}
}
//opens vehicle data for read/write binary access
//create file if it doesnt exist

//-----------------------------------------------
bool addVehicle(
	const Vehicle &v  // in: vehicle to add
)
{
	if (!vehicleFile.is_open()) {
		cerr << "vehicle file not open." << endl;
		return false;
	}

	vehicleFile.seekp(0, ios::end);
	vehicleFile.write(reinterpret_cast<const char*>(&v), sizeof(Vehicle));

	return vehicleFile.good();
//appends a new vehicle record to binary file
//returns true if write succeeds
}
//-----------------------------------------------
void shutdownVehicleStorage()

{
    if (vehicleFile.is_open()) {
		vehicleFile.close();
	}
}
//close vehicle data file if open

//-----------------------------------------------
std::optional<Vehicle> getVehicleByLicensePlate(
    const std::string &licensePlate
)
{
    if (!vehicleFile.is_open()) return nullopt;

    vehicleFile.clear();
    vehicleFile.seekg(0, ios::beg);

    Vehicle temp;
    while (vehicleFile.read(reinterpret_cast<char*>(&temp), sizeof(Vehicle))) {
        // Use strncmp to compare C-style strings safely
        if (strncmp(licensePlate.c_str(), temp.licensePlate, sizeof(temp.licensePlate)) == 0) {
            return temp;
        }
    }

    return nullopt;
}
//linear search through binary vehicle file to find a 
//vehicle with a matching license plate
// returns optional<vehicle> if found or nullopt otherwise
//-----------------------------------------------