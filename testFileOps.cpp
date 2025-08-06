//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
/*
    Module: testFileOps.cpp
    Revision History:
        Revision 2.0: 2025-08-01 - Updated by Tyler Lee
        Revision 1.0: 2025-07-18 - Created by Tyler Lee
    Purpose:
        This module tests the ReservationASM functions, specifically addReservation and getReservationByID.
        It creates sample reservations, adds them to storage, retrieves them by ID, and verifies the results.
        Compilation: g++ testFileOps.cpp ReservationASM.cpp -o testFileOps.exe
*/

#include <fstream>
#include <iostream>
#include <vector>
#include <cstring>
#include "ReservationASM.h"
#include "Reservation.h"

using namespace std;

//------------------------------------------------------------------------
// Function: printReservation
// Purpose: Prints the details of a Reservation object in a readable format
void printReservation(const Reservation& r) {
    cout << "=== Reservation ===" << endl;
    cout << "License Plate:     " << r.licensePlate << endl;
    cout << "Sailing ID:        " << r.sailingID << endl;
    cout << "Vehicle Length:    " << r.vehicleLength << " m" << endl;
    cout << "Vehicle Height:    " << r.vehicleHeight << " m" << endl;
    cout << "Phone:             " << r.phone << endl;
    cout << "Onboard:           " << (r.onboard ? "Yes" : "No") << endl;
    cout << "Expected Return:   "
         << r.expectedReturnDate.year << "-"
         << r.expectedReturnDate.month << "-"
         << r.expectedReturnDate.day << endl;
}

//------------------------------------------------------------------------
int main() {
    // Clear the reservations.dat file before starting the test
    ofstream wipe("reservations.dat", ios::binary | ios::trunc);
    wipe.close();

    // Initialize the reservation storage
    initializeReservationStorage();

    // Create test reservation 1
    Reservation r1 = {
        "",               // id (will be populated later)
        "TEST123",        // licensePlate
        "VIC-15-08",      // sailingID
        4.5f,             // vehicleLength
        2.1f,             // vehicleHeight
        "604-123-4567",   // phone
        false,            // onboard
        {2025, 7, 25},    // expectedReturnDate
        Lane::LOW         // reservedLane
    };
    strcpy(r1.id, r1.licensePlate);
    strcat(r1.id, r1.sailingID);

    // Create test reservation 2
    Reservation r2 = {
        "",               // id
        "ABC789",         // licensePlate
        "VIC-16-08",      // sailingID
        5.0f,             // vehicleLength
        2.2f,             // vehicleHeight
        "778-555-0000",   // phone
        true,             // onboard
        {2025, 7, 30},    // expectedReturnDate
        Lane::HIGH        // reservedLane
    };
    strcpy(r2.id, r2.licensePlate);
    strcat(r2.id, r2.sailingID);

    // Create test reservation 3
    Reservation r3 = {
        "",               // id
        "TYBEAST",        // licensePlate
        "VIC-16-02",      // sailingID
        3.0f,             // vehicleLength
        1.2f,             // vehicleHeight
        "604-808-8008",   // phone
        true,             // onboard
        {2025, 6, 25},    // expectedReturnDate
        Lane::LOW         // reservedLane
    };
    strcpy(r3.id, r3.licensePlate);
    strcat(r3.id, r3.sailingID);

    // Add reservations to the storage
    bool success = addReservation(r1);
    success &= addReservation(r2);
    success &= addReservation(r3);

    if (!success) {
        cout << "FAIL: addReservation() failed." << endl;
        shutdownReservationStorage();
        return 1;
    }

    // Retrieve reservations by ID
    auto res1 = getReservationByID(r1.id);
    auto res2 = getReservationByID(r2.id);
    auto res3 = getReservationByID(r3.id);
    auto notFound = getReservationByID("NOTREAL");

    // Verify and print results
    if (res1) {
        cout << "PASS: TEST123 retrieved." << endl;
        printReservation(res1.value());
    } else {
        cout << "FAIL: TEST123 not found." << endl;
    }

    if (res2) {
        cout << "PASS: ABC789 retrieved." << endl;
        printReservation(res2.value());
    } else {
        cout << "FAIL: ABC789 not found." << endl;
    }

    if (res3) {
        cout << "PASS: TYBEAST retrieved." << endl;
        printReservation(res3.value());
    } else {
        cout << "FAIL: TYBEAST not found." << endl;
    }

    if (!notFound) {
        cout << "PASS: NOTREAL correctly not found (EOF test)." << endl;
    } else {
        cout << "FAIL: NOTREAL should not exist." << endl;
    }

    // Shutdown the reservation storage
    shutdownReservationStorage();
    return 0;
}