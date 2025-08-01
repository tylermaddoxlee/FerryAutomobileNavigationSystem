//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
/* 
    Revision History:
    Revision 1.0: 2025-07-24 - Original By Brandon Landa-Ahn
    Purpose:
        This module manages vessel data storage in a binary file (vessels.dat).
        It provides functions to initialize storage, add vessels, retrieve vessels by name,
        and shut down the storage. Data is stored and retrieved using binary I/O for efficiency.
    Algorithm:
        - Binary file I/O for reading and writing fixed-size Vessel structs.
        - Sequential search for vessel lookup by name.
    Data Validation:
        - File open/create success checks.
        - Name matching using `strncmp`.
        - Goodbit checks on I/O operations.
*/
#include <iostream>
#include <fstream>
#include <filesystem>
#include <utility>
#include <vector>
#include <chrono>
#include <ctime>
#include <utility>
#include "Vessel.h"
#include "VesselASM.h"
#include <optional>

using namespace std;

static fstream vesselFile;  // Module-scope file handle for vessel storage

//------------------------------------------------------------------------
void initializeVesselStorage()
// Initializes the vessel storage by opening or creating the vessels.dat file.
// Ensures the file is available for read and write operations.
{
    // Attempt to open the file for reading and writing
    vesselFile.open("vessels.dat", ios::binary | ios::in | ios::out);
    if (!vesselFile.is_open()) 
    {
        // If the file does not exist, create it
        ofstream createFile("vessels.dat", ios::binary);
        createFile.close();
        vesselFile.clear();  // Reset state flags after failed open attempt
        vesselFile.open("vessels.dat", ios::binary | ios::in | ios::out);
        if (!vesselFile.is_open()) 
        {
            cerr << "Error: Failed to create vessels.dat file." << endl;
            // Optional: Could throw an exception or exit gracefully
        }
    }
}

//------------------------------------------------------------------------
void shutdownVesselStorage()
// Closes the vessel data file if it is open, ensuring resources are released.
// Note: Renamed from 'shutdownVehicleStorage' to match 'initializeVesselStorage'.
{
    if (vesselFile.is_open()) 
    {
        vesselFile.close();  // Close the file handle
    }
}

//------------------------------------------------------------------------
bool addVessel(const Vessel &v)
// Appends a new vessel record to the end of the vessels.dat file.
// Returns true if the write operation is successful, false otherwise.
{
    if (!vesselFile.is_open()) 
    {
        cerr << "Error: Vessel storage is not initialized." << endl;
        return false;
    }
    vesselFile.clear();  // Clear any error flags before operation
    vesselFile.seekp(0, ios::end);  // Move to the end of the file for appending
    vesselFile.write(reinterpret_cast<const char*>(&v), sizeof(Vessel));
    return vesselFile.good();  // Return the status of the write operation
}

//------------------------------------------------------------------------
std::optional<Vessel> getVesselByName(const char* targetName)
// Retrieves a vessel record by its name.
// Returns the vessel if found, otherwise nullopt.
{
    if (!vesselFile.is_open()) 
    {
        cerr << "Error: Vessel storage is not initialized." << endl;
        return nullopt;
    }
    vesselFile.clear();  // Clear any error flags before reading
    vesselFile.seekg(0, ios::beg);  // Rewind to the start of the file
    Vessel temp;  // Buffer for reading records

    // Sequentially search for the matching name
    while (vesselFile.read(reinterpret_cast<char*>(&temp), sizeof(Vessel))) 
    {
        if (strncmp(temp.name, targetName, sizeof(temp.name)) == 0) 
        {
            return temp;  // Return the found vessel
        }
    }
    return nullopt;  // Indicate vessel not found
}