//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
/*
    Module: SailingASM.cpp
    Revision History:
        Revision 1.0: 2025/07/18 - Original by Brandon Landa-Ahn
    Purpose:  
        This module implements the ASM (Application Service Module) layer for
        sailing records in the ferry reservation system. It manages all file-based
        operations on `sailings.dat`, including initialization, record creation,
        deletion, lookup, capacity calculations, and paginated reporting.
        
        Algorithm:
            - File I/O using binary reads/writes of fixed-size Sailing structs
            - Deletion: swap-last-record into target position then truncate file
            - Reporting: fixed-width pagination of 5 entries per page
        Data validation:
            - File open/create success checks
            - ID matching via `strncmp`
            - Goodbit checks on I/O operations
*/

#include <iostream>
#include <fstream>
#include <filesystem>
#include <utility>
#include <vector>
#include <chrono>
#include <ctime>
#include <iomanip>
#include "SailingASM.h"
#include "Sailing.h"
#include "ReservationASM.h"
#include "VesselASM.h"
#include <cstring>
using namespace std;

static fstream sailingFile;  // Module-scope file handle for sailing storage access

//------------------------------------------------------------------------
void initializeSailingStorage()
// Initializes the sailing storage by opening or creating the sailings.dat file.
// Ensures the file is available for read and write operations.
{
    // Attempt to open the file for reading and writing
    sailingFile.open("sailings.dat", ios::binary | ios::in | ios::out);
    if (!sailingFile.is_open()) 
    {
        // If the file does not exist, create it
        ofstream createFile("sailings.dat", ios::binary);
        createFile.close();
        sailingFile.clear();
        sailingFile.open("sailings.dat", ios::binary | ios::in | ios::out);
        if (!sailingFile.is_open()) 
        {
            cerr << "Error: Failed to create sailings.dat file." << endl;
        }
    }
}

//------------------------------------------------------------------------
void shutdownSailingStorage()
// Closes the sailing data file if it is open, ensuring resources are released.
{
    if (sailingFile.is_open()) 
    {
        sailingFile.close();  // Close the file handle
    }
}

//------------------------------------------------------------------------
bool addSailing(const Sailing &s)
// Appends a new sailing record to the end of the sailings.dat file.
// Returns true if the write operation is successful, false otherwise.
{
    if (!sailingFile.is_open()) return false;  // Check if file is initialized
    sailingFile.clear();
    sailingFile.seekp(0, ios::end);  // Move to the end of the file for appending
    sailingFile.write(reinterpret_cast<const char*>(&s), sizeof(Sailing));
    return sailingFile.good();  // Return the status of the write operation
}

//------------------------------------------------------------------------
bool deleteSailing(const char *id)
// Deletes a sailing record by ID using the swap-last-record method.
// The target record is overwritten with the last record, and the file is truncated.
{
    sailingFile.clear();
    sailingFile.seekg(0, ios::beg);  // Rewind to the beginning of the file

    streamoff targetIndex = -1;  // Index of the record to delete
    streamoff lastIndex = -1;    // Index of the last record in the file
    streamoff idx = 0;           // Current record index
    Sailing current;             // Buffer for reading records

    // Loop goal: Find the target record and the last record
    while (sailingFile.read(reinterpret_cast<char*>(&current), sizeof(Sailing))) 
    {
        if (strncmp(current.id, id, sizeof(current.id)) == 0) 
        {
            targetIndex = idx;  // Mark the target record's index
        }
        lastIndex = idx;  // Update the last record's index
        idx++;
    }

    if (targetIndex >= 0 && targetIndex != lastIndex) 
    {
        // Read the last record to overwrite the target
        Sailing lastRec;
        sailingFile.clear();
        sailingFile.seekg(lastIndex * sizeof(Sailing), ios::beg);
        sailingFile.read(reinterpret_cast<char*>(&lastRec), sizeof(Sailing));

        // Overwrite the target record with the last record
        sailingFile.clear();
        sailingFile.seekp(targetIndex * sizeof(Sailing), ios::beg);
        sailingFile.write(reinterpret_cast<const char*>(&lastRec), sizeof(Sailing));
        sailingFile.flush();
    }

    // Truncate the file to remove the last record
    sailingFile.close();
    filesystem::resize_file("sailings.dat", static_cast<size_t>((lastIndex) * sizeof(Sailing)));
    // Reopen the file for further operations
    sailingFile.open("sailings.dat", ios::binary | ios::in | ios::out);
    return true;
}

//------------------------------------------------------------------------
optional<Sailing> getSailingByID(const char *id)
// Retrieves a sailing record by its ID.
// Returns the sailing if found, otherwise nullopt.
{
    sailingFile.clear();
    sailingFile.seekg(0, ios::beg);  // Rewind to the start of the file
    Sailing rec;  // Buffer for reading records

    // Loop goal: Search sequentially for the matching ID
    while (sailingFile.read(reinterpret_cast<char*>(&rec), sizeof(Sailing))) 
    {
        if (strncmp(rec.id, id, sizeof(rec.id)) == 0) 
        {
            return rec;  // Return the found record
        }
    }
    return nullopt;  // Indicate not found
}

//------------------------------------------------------------------------
pair<float, float> getRemainingCapacity(const char *sailingID)
// Retrieves the remaining capacity (low and high lanes) for a given sailing ID.
// Returns a pair of floats representing LRL and HRL, or {-1.0f, -1.0f} if not found.
{
    sailingFile.clear();
    sailingFile.seekg(0, ios::beg);  // Rewind to the beginning
    Sailing rec;  // Buffer for reading records

    // Loop goal: Find the sailing and extract capacity fields
    while (sailingFile.read(reinterpret_cast<char*>(&rec), sizeof(Sailing))) 
    {
        if (strncmp(rec.id, sailingID, sizeof(rec.id)) == 0) 
        {
            return {rec.LRL, rec.HRL};  // Return low and high remaining lengths
        }
    }
    return {-1.0f, -1.0f};  // Indicate sailing not found
}

//------------------------------------------------------------------------
vector<Sailing> getAllSailings()
// Retrieves all sailing records from the file and returns them in a vector.
{
    vector<Sailing> sailings;  // Vector to hold all sailing records
    sailingFile.clear();
    sailingFile.seekg(0, ios::beg);  // Rewind to the start
    Sailing s;  // Buffer for reading records

    // Read all records into the vector
    while (sailingFile.read(reinterpret_cast<char*>(&s), sizeof(Sailing))) 
    {
        sailings.push_back(s);
    }
    return sailings;
}