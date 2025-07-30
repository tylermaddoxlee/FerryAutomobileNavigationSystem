//============================================
/*
    Module: Vehicle.h
    Revision History:
    Revision 1.0: 2025-07-18 - Created by Tyler
    Purpose:
        Defines the Vehicle data structure for storage and lookup.
*/

//============================================
#ifndef VEHICLE_H
#define VEHICLE_H

struct Vehicle {
    char licensePlate[11];   // vehicle plate (max 10 + null) \0 marks end of string
    char phone[13];          // contact phone (eg, 604-333-2222 + null)
    float vehicleLength;     // 4 bytes
    float vehicleHeight;     // 4 bytes
}; //32 bytes

#endif