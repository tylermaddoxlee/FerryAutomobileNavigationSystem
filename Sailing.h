//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//============================================
/*
    Module: Sailing.h
    Revision History:
    Revision 2.0: 2025-08-02 – Updated by Brandon Landa‑Ahn and Raj Chowdhury
    Revision 1.0: 2025-07-18 - Created by Tyler
    Purpose:
        Defines the Sailing data structure for storage and lookup.
*/

//============================================
#ifndef SAILING_H
#define SAILING_H

struct Sailing {
    char id[10];           /// SailingID (primary key)
    char vesselName[26];  /// Foreign key to Vessel.vesselName
    float LRL;            /// Low Remaining Length (In meters)
    float HRL;            /// High Remaining Length (In meters)
    int reservationsCount; // Track the number of reservations
};

#endif