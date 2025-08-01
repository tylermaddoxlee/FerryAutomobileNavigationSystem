//============================================
/*
    Module: VesselASM.h
    Revision History:
    Revision 1.0: 2025-07-07 - Original by Brandon Landa-Ahn
    Purpose:
        Declaration of Vessel Abstract Storage Module API.
*/

//===============================================
#ifndef VESSEL_ASM_H
#define VESSEL_ASM_H

#include <optional>
#include <string>
#include "Vessel.h"


//-----------------------------------------------
void initializeVesselStorage();
// in: none
// out: none
// Purpose: Open vessel data file

//-----------------------------------------------
void shutdownVesselStorage();
// in: none
// out: none
// Purpose: Close vessel data file

//-----------------------------------------------
bool addVessel(
    const Vessel &v  // in: vessel to add
);
// Purpose: Append a new vessel record (alias for createVessel)

//-----------------------------------------------
std::optional<Vessel> getVesselByName(
    const char* name  // in: vessel ID to look up
);
// out: Vessel record
// Purpose: Retrieve a single vessel record by ID

//-----------------------------------------------
std::optional<Vessel> getVesselByID(
    const char* &id  // in: ID to look up
);
// out: optional vessel if found
// Purpose: Retrieve a single vessel record

#endif // VESSEL_ASM_H
