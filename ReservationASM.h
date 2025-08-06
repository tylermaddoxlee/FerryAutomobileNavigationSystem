// File: ReservationASM.h
/*
    Module: ReservationASM.h
    Revision History:
    Revision 2.0: 2025-08-02 – Updated by Tyler Lee
    Revision 1.0: 2025-07-18 - Original by Tyler Lee
    Purpose:
        Declaration of Reservation Abstract Storage Module API.
*/

#ifndef RESERVATION_ASM_H
#define RESERVATION_ASM_H

#include <optional>
#include <string>
#include "Reservation.h"

//-----------------------------------------------
void initializeReservationStorage();
//opens reservation binary file for read/write
//creates file if doesnt exist

//-----------------------------------------------
void shutdownReservationStorage();
// Close reservation binary file

//-----------------------------------------------
bool addReservation(
    const Reservation &r  // in: reservation to add
);
//appends reservation to end of binary file
//returns true if write was succesfull

//-----------------------------------------------
bool deleteReservation(
    const std::string &id  // in: ID of reservation to remove
);
//deletes reservation by id from binary file
//if found overwrites the record with last record
//return true if successful else false if id not found or not open

//-----------------------------------------------
bool deleteReservationsBySailingID(
    const std::string &sailingID  // in: sailing ID to remove
);

// removes all reservations for a given sailing

//-----------------------------------------------
std::optional<Reservation> getReservationByID(
    const char* reservationID  // in: ID to look up
);
//find reservation id by license plate
//returns optional<Reservation> if found or nullopt otherwise

//-----------------------------------------------
std::optional<Reservation> getReservationByLicenseAndID(
    const char* reservationID
);

//-----------------------------------------------
double calculateFee(
    const std::string &reservationID  // in: ID to calculate for
);
// calculates fare based on vehicle size
// return fee based on size categories: normal, long low special, long overheight special
// return -1 if not found

//-----------------------------------------------
bool setOnboardStatus(
    const std::string &reservationID,  // in: ID of reservation
    bool onboard                        // in: onboard status to set
);
//updates the onboard status for a given reservation ID
//returns true if the update was successful

//-----------------------------------------------
bool getOnboardStatus(
    const std::string &reservationID  // in: ID of reservation
);
//display check in status to the user
//test whether a specific vehicle has already checked in

//-----------------------------------------------

//reads file from the top
//-----------------------------------------------
double checkInAndCalcFee(
    const std::string &reservationID,
    const Date &actualReturnDate
);
//sets onboard status to true and calculates fee
//-----------------------------------------------
int countReservationsBySailing(
    const char* targetID  // target Sailing ID to count
);

//------------------------------------------------
void makeReservationID(
    const char* licensePlate,
    const char* sailingID,
    char outID[21]
);
#endif // RESERVATION_ASM_H