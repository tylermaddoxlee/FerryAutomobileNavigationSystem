//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
/*          
    Module: ReservationASM.cpp
    Revision History: 
    Revision 2.0: 2025-08-02 – Updated by Tyler Lee
    Revision 1.0: 2025/07/18 - Original by Tyler Lee
    Purpose: 
        This module implements the Reservation Access Storage Manager (ASM) which 
        provides low-level file operations for the ferry reservation system. It 
        manages a binary file containing reservation records and supports CRUD 
        operations (Create, Read, Update, Delete). The module uses a single file 
        stream for efficient access and implements algorithms for record management 
        including linear search for lookups and swap-with-last for deletions. 
        Fee calculation is based on vehicle dimensions with tiered pricing.
        
        Data Structure: Binary file with fixed-size Reservation records
        Algorithm: Linear search O(n) for lookups, swap-delete O(n) for removal
*/

//============================================

#include <iostream>
#include <fstream>
#include <filesystem>
#include <vector>
#include "ReservationASM.h"
#include "Reservation.h"
#include <cstring>
using namespace std;

//============================================

static fstream reservationFile;  // persistent file stream for reservation binary file operations

//-----------------------------------------------
void initializeReservationStorage()
{
    // Attempt to open existing reservation file for read/write binary operations
    reservationFile.open("reservations.dat", ios::binary | ios::in | ios::out);

    if (!reservationFile.is_open())  // File doesn't exist, create it
    {
        // Create new binary file using ofstream constructor
        ofstream createFile("reservations.dat", ios::binary);
        reservationFile.clear();  // Clear any error flags from failed open
        createFile.close();
        
        // Reopen as bidirectional fstream for subsequent operations
        reservationFile.open("reservations.dat", ios::binary | ios::in | ios::out);
        if (!reservationFile.is_open())  // Creation failed
        {
            cerr << "Failed to create reservation file." << endl;
        }
    }
}

//-----------------------------------------------
void shutdownReservationStorage()
{
    if (reservationFile.is_open())  // Prevent closing unopened file
    {
        reservationFile.close();  // Flush buffers and release file handle
    }
}

//-----------------------------------------------
bool addReservation(const Reservation &r)
{
    if (!reservationFile.is_open())  // Validate file state before operation
    {
        cerr << "Error: reservation file is not open." << endl;
        return false;
    }
    reservationFile.clear();  // Clear any previous EOF or error flags
    // Position write pointer at end for append operation
    reservationFile.seekp(0, ios::end);

    // Write entire Reservation struct as binary data block
    // Uses reinterpret_cast to convert struct pointer to char* for binary write
    reservationFile.write(reinterpret_cast<const char*>(&r), sizeof(Reservation));
    reservationFile.flush();
    // Return success status based on stream state
    return reservationFile.good();
}

//-----------------------------------------------
bool deleteReservation(const std::string &id)
{
    if (!reservationFile.is_open()) return false;
    
    reservationFile.clear();  // Clear any error/EOF flags
    reservationFile.seekg(0, ios::beg);  // Position at file start

    streampos targetPosition = -1;  // position of record to delete
    streampos lastRecordPosition;   // position of last record in file
    Reservation currentRecord;

    // Linear search through file to find target record and track last position
    while (reservationFile.read(reinterpret_cast<char*>(&currentRecord), sizeof(Reservation)))
    {
        streampos recordPosition = reservationFile.tellg() - static_cast<streamoff>(sizeof(Reservation));
        
        if (id == currentRecord.licensePlate)  // Found target record
        {
            targetPosition = recordPosition;
        }
        lastRecordPosition = recordPosition;  // Update last known position
    }

    if (targetPosition == -1) return false;  // Record not found

    // Implement swap-with-last deletion algorithm to avoid shifting all records
    if (targetPosition != lastRecordPosition)  // Not deleting the last record
    {
        // Read the last record from file
        reservationFile.seekg(lastRecordPosition);
        Reservation lastRecord;
        reservationFile.read(reinterpret_cast<char*>(&lastRecord), sizeof(Reservation));

        // Overwrite target record with last record data
        reservationFile.seekp(targetPosition);
        reservationFile.write(reinterpret_cast<const char*>(&lastRecord), sizeof(Reservation));
    }

    // Truncate file to remove the now-duplicate last record
    reservationFile.close();
    size_t newFileSize = static_cast<size_t>(lastRecordPosition);
    filesystem::resize_file("reservations.dat", newFileSize);

    // Reopen file for subsequent operations
    reservationFile.open("reservations.dat", ios::binary | ios::in | ios::out);
    return true;
}

//-----------------------------------------------
bool deleteReservationsBySailingID(const std::string &sailingID)
{
    if (!reservationFile.is_open()) return false;

    reservationFile.clear();  // Reset stream state
    reservationFile.seekg(0, ios::beg);  // Start from beginning

    std::vector<Reservation> recordsToKeep;  // temporary storage for non-matching records
    Reservation currentReservation;

    // Read all records and collect those that don't match the target sailing ID
    while (reservationFile.read(reinterpret_cast<char*>(&currentReservation), sizeof(Reservation)))
    {
        if (sailingID != currentReservation.sailingID)  // Keep records for other sailings
        {
            recordsToKeep.push_back(currentReservation);
        }
        // Records matching sailingID are implicitly discarded (not added to vector)
    }

    // Rewrite entire file with only the records to keep
    reservationFile.close();
    ofstream rewriteFile("reservations.dat", ios::binary | ios::trunc);  // Truncate existing file
    if (!rewriteFile.is_open()) return false;
    
    // Write each kept record back to file
    for (const auto& record : recordsToKeep)  // Iterate through preserved records
    {
        rewriteFile.write(reinterpret_cast<const char*>(&record), sizeof(Reservation));
        if (!rewriteFile.good())  // Check for write errors
        {
            rewriteFile.close();
            return false;
        }
    }
    rewriteFile.close();

    // Reopen as fstream for subsequent operations
    reservationFile.open("reservations.dat", ios::binary | ios::in | ios::out);
    return reservationFile.is_open();
}

//-----------------------------------------------
std::optional<Reservation> getReservationByID(const char* reservationID)
{
    if (!reservationFile.is_open())  // Validate file accessibility
        return std::nullopt;

    reservationFile.clear();  // Clear any EOF/error flags from previous operations
    reservationFile.seekg(0, std::ios::beg);  // Position at file start

    Reservation tempRecord;
    
    while (reservationFile.read(reinterpret_cast<char*>(&tempRecord), sizeof(Reservation)))
    {
        if (strncmp(tempRecord.id, reservationID, sizeof(tempRecord.id)) == 0)  // Found matching reservation
        {
            return tempRecord;  // Return copy of found record
        }
    }

    return std::nullopt;  // No matching record found
}

//-----------------------------------------------
std::optional<Reservation> getReservationByLicenseAndID(const char* reservationID)
{
    if (!reservationFile.is_open())
        return std::nullopt;

    reservationFile.clear();
    reservationFile.seekg(0, std::ios::beg);

    Reservation tempRecord;
    
    while (reservationFile.read(reinterpret_cast<char*>(&tempRecord), sizeof(Reservation)))
    {
        // Create composite key from the record using C-style string concatenation
        char recordCompositeKey[21]; // licensePlate (10) + sailingID (10) + null terminator
        snprintf(recordCompositeKey, sizeof(recordCompositeKey), "%s%s", 
                 tempRecord.licensePlate, tempRecord.sailingID);
        
        if (strcmp(recordCompositeKey, reservationID) == 0)  // Compare C-style strings
        {
            return tempRecord;
        }
    }

    return std::nullopt;
}


//-----------------------------------------------
double calculateFee(const std::string &reservationID, const Date & /*actualReturnDate*/)
{
    auto reservationOption = getReservationByID(reservationID.c_str());
    if (!reservationOption.has_value()) return -1.0;  // Reservation not found

    // Fee structure constants based on business rules
    const double NORMAL_VEHICLE_FEE = 14.0;              // flat rate for standard vehicles
    const double LONG_LOW_SPECIAL_RATE = 2.0;            // per meter for long low vehicles
    const double LONG_OVERHEIGHT_SPECIAL_RATE = 3.0;     // per meter for long overheight vehicles

    double calculatedFee = 0.0;
    
    // Tiered pricing algorithm based on vehicle dimensions
    if (reservationOption->vehicleLength <= 7.0 && reservationOption->vehicleHeight <= 2.0)
    {
        // Standard vehicle: flat fee regardless of exact dimensions
        calculatedFee = NORMAL_VEHICLE_FEE;
    } 
    else if (reservationOption->vehicleLength > 7.0 && reservationOption->vehicleHeight <= 2.0)
    {
        // Long but low vehicle: rate per meter of length
        calculatedFee = reservationOption->vehicleLength * LONG_LOW_SPECIAL_RATE;
    }
    else if (reservationOption->vehicleLength > 7.0 && reservationOption->vehicleHeight > 2.0)
    {
        // Long and tall vehicle: higher rate per meter due to space constraints
        calculatedFee = reservationOption->vehicleLength * LONG_OVERHEIGHT_SPECIAL_RATE;
    }

    return calculatedFee;
}

//-----------------------------------------------
bool setOnboardStatus(const std::string &reservationID, bool onboardStatus)
{
    if (!reservationFile.is_open()) return false;

    reservationFile.clear();  // Clear stream flags
    reservationFile.seekg(0, ios::beg);  // Start search from beginning

    Reservation recordBuffer;
    
    // Linear search for matching reservation ID
    while (reservationFile.read(reinterpret_cast<char*>(&recordBuffer), sizeof(Reservation)))
    {
        streampos recordPosition = reservationFile.tellg() - static_cast<streamoff>(sizeof(Reservation));
        
        if (strncmp(recordBuffer.id, reservationID.c_str(), sizeof(recordBuffer.id)) == 0)  // Found target record
        {
            recordBuffer.onboard = onboardStatus;  // Update onboard flag

            // Write modified record back to same file position
            reservationFile.seekp(recordPosition);
            reservationFile.write(reinterpret_cast<const char*>(&recordBuffer), sizeof(Reservation));
            return reservationFile.good();  // Return write success status
        }
    }

    return false;  // Reservation ID not found
}

//-----------------------------------------------
bool getOnboardStatus(const std::string &reservationID)
{
    if (!reservationFile.is_open()) return false;

    reservationFile.clear();  // Reset stream state
    reservationFile.seekg(0, ios::beg);  // Position at file start

    Reservation recordBuffer;
    
    // Linear search through records for matching ID
    while (reservationFile.read(reinterpret_cast<char*>(&recordBuffer), sizeof(Reservation)))
    {
        if (recordBuffer.licensePlate == reservationID)  // Found matching record
        {
            return recordBuffer.onboard;  // Return current onboard status
        }
    }

    return false;  // Not found or not onboard (default to false for safety)
}

//-----------------------------------------------
double checkInAndCalcFee(const std::string &reservationID, const Date &actualReturnDate)
{
    // Calculate fee first to validate reservation exists
    double feeAmount = calculateFee(reservationID, actualReturnDate);
    
    if (feeAmount >= 0)  // Valid reservation found and fee calculated
    {
        setOnboardStatus(reservationID, true);  // Mark vehicle as checked in
    }
    
    return feeAmount;  // Return calculated fee or -1 if reservation not found
}

//-----------------------------------------------
int countReservationsBySailing(const char* targetSailingID)
{
    if (!reservationFile.is_open())  // Validate file state
    {
        cerr << "Error: reservation file is not open.\n";
        return 0;
    }

    reservationFile.clear();  // Clear any EOF/error flags
    reservationFile.seekg(0, ios::beg);  // Position at file start

    Reservation recordBuffer;
    int matchingCount = 0;  // counter for reservations matching target sailing

    // Linear scan through all records counting matches
    while (reservationFile.read(reinterpret_cast<char*>(&recordBuffer), sizeof(Reservation)))
    {
        // Use strncmp for safe string comparison within fixed-size char array
        if (strncmp(recordBuffer.sailingID, targetSailingID, sizeof(recordBuffer.sailingID)) == 0)
        {
            ++matchingCount;  // Increment counter for each match found
        }
    }

    return matchingCount;
}

//-----------------------------------------------
// helper: build a 20-char ID padded with '1' + '\0'
void makeReservationID(const char* licensePlate,
                       const char* sailingID,
                       char outID[21])
{
    // 1) write plate+sailing (snprintf always NUL-terminates if size>=1)
    int written = snprintf(outID,
                           21,
                           "%s%s",
                           licensePlate,
                           sailingID);
    if (written < 0 || written >= 20) {
        // handle error: either snprintf failed, or overflowed.
        // e.g. set outID[0]='\0' or throw, or print error and return.
        outID[0] = '\0';
        return;
    }

    // 2) pad with '1' up to index 19
    for (int i = written; i < 20; ++i) {
        outID[i] = '*';
    }
    // 3) ensure NUL
    outID[20] = '\0';
}
// --------------------------------------
