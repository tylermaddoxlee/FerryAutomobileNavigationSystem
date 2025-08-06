// File: Vessel.h
// Module: Vessel.h
// Revision History:
// Revision 2.0: 2025-08-01 - Updated by Arsh Garcha
// Revision 1.0: 2025-07-23 – Created by Arsh Garcha
// Purpose:
//   Declaration of the Vessel record used for vessel storage and processing.

#ifndef VESSEL_H
#define VESSEL_H

#include <cstddef>

//-----------------------------------------------
// Constants
static constexpr std::size_t VESSEL_NAME_LEN = 25;  // max characters + null

//-----------------------------------------------
// Struct:  Vessel
// in:      name    – vessel identifier (max 24 chars + '\0')
//          lowCap  – total low‑lane capacity
//          highCap – total high‑lane capacity
// Purpose: Fixed‑length record for binary I/O and ASM storage.
struct Vessel
{
    char name[VESSEL_NAME_LEN];
    int  lowCap;
    int  highCap;
};

#endif  // VESSEL_H