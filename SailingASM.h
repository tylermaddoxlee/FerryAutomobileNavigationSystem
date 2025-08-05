//============================================
/*
    Module: SailingASM.h
    Revision History:
    Revision 1.0: 2025-07-07 - Original by Brandon Landa-Ahn
    Purpose:
        Declaration of Sailing Abstract Storage Module API.
*/

//===============================================
#ifndef SAILING_ASM_H
#define SAILING_ASM_H

#include <optional>
#include <string>
#include <utility>
#include <vector>
#include "SailingCommandProcessor.h"
#include "Vehicle.h"
#include "Sailing.h"

//-----------------------------------------------
void initializeSailingStorage();
// in: none
// out: none
// Purpose: Open sailing data file

//-----------------------------------------------
void shutdownSailingStorage();
// in: none
// out: none
// Purpose: Close sailing data file

//-----------------------------------------------
bool addSailing(
    const Sailing &s  // in: sailing to add
);
// Purpose: Append a new sailing record

//-----------------------------------------------
bool deleteSailing(
    const char* id  // in: ID of sailing to remove
);
// Purpose: Remove a sailing record by ID

//-----------------------------------------------
bool updateSailing(const Sailing &s);  // in-place update

//-----------------------------------------------
std::optional<Sailing> getSailingByID(
    const char *id  // in: ID to look up
);
// out: optional sailing if found
// Purpose: Retrieve a single sailing record

//-----------------------------------------------
std::pair<float, float> getRemainingCapacity(
    const char *sailingID  // in: ID of sailing
);
// out: remaining capacity count
// Purpose: Get the LRL and HRL in meters. 
// The first value is the LRL and the second is the HRL

//-----------------------------------------------
std::vector<Sailing> getAllSailings();
// out: vector of all sailings
// Purpose: Retrieve all sailings in the system



#endif // SAILING_ASM_H
