/*
    Module: Reservation.h
    Revision History:
    Revision 2.0: 2025-08-02 – Updated by Tyler Lee
    Revision 1.0: 2025/07/07 - Created by Brandon Landa-Ahn, Tyler Lee
    Purpose:
        This is the file that implements the main menu displays
*/


#ifndef RESERVATION_H
#define RESERVATION_H

#include <string>
#include "Date.h"

// Define lane types
enum class Lane {
    LOW,
    HIGH
};

struct Reservation { //vehicle data + reservation
    char id[21];             // reservation ID (licensePlate + sailingID + null)    
    char licensePlate[11];   // vehicle plate (max 10 + null)
    char sailingID[10];      // nsailing ID (e.g., XXX-DD-HH + null)
    float vehicleLength;     // 4 bytes (in meters)
    float vehicleHeight;     // 4 bytes (in meters)
    char phone[15];          // contact phone (eg, 1-604-333-2222 + null)
    bool onboard;            // 1 byte
    Date expectedReturnDate;
    Lane reservedLane;       // Tracks the lane where the vehicle was reserved (LOW or HIGH)
};

#endif

