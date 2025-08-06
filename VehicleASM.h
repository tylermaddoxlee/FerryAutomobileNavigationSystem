//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//============================================
/*
    Module: VehicleASM.h
    Revision History:
    Revision 2.0: 2025-08-04 - Updated by Tyler Lee
    Revision 1.0: 2025-07-07 - Original by Brandon Landa-Ahn
    Purpose:
        Declaration of Vehicle Abstract Storage Module API.
*/

//===============================================
#ifndef VEHICLE_ASM_H
#define VEHICLE_ASM_H

#include <optional>
#include <string>
#include "Vehicle.h"    

//-----------------------------------------------
void initializeVehicleStorage();
// in: none
// out: none
// Purpose: Open vehicle data file

//-----------------------------------------------
void shutdownVehicleStorage();
// in: none
// out: none
// Purpose: Close vehicle data file

//-----------------------------------------------
std::optional<Vehicle> getVehicleByLicensePlate(
    const std::string &licensePlate  // in: plate to look up
);
// out: optional vehicle if found
// Purpose: Retrieve vehicle by license plate

//-----------------------------------------------
bool addVehicle(
    const Vehicle &v  // in: vehicle to add
);
// Purpose: Append a new vehicle record

//-----------------------------------------------
void seekToBeginningOfFile(); 
// in: none
// out: none
// Purpose: Reset the file's read/write pointer to the beginning of the binary file
#endif // VEHICLE_ASM_H