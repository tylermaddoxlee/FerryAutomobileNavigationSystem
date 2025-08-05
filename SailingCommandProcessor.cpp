//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
/*
    Module: SailingCommandProcessor.cpp
    Revision History:
    Revision 1.0: 2025-07-07 - Created by Brandon Landa-Ahn
    Revision 2.0: 2025-07-22 - Updated by Arsh Garcha
    Purpose:
        Implementation of sailing-related workflows.
*/

#include <iostream>
#include <chrono>
#include <ctime>
#include <limits>
#include "SailingCommandProcessor.h"
#include "SailingASM.h"    // addSailing(id)
#include "VesselASM.h"     // getVesselByName(name)
#include "ReservationASM.h" // Ensure no conflicts
#include "MenuUI.h"
#include "Sailing.h"       // Sailing struct (if needed)
#include <cstring>  // for strlen
#include <iomanip>  // for std::put_time
#include <string>
#include <algorithm>
//-----------------------------------------------
// Function: createSailing
// Purpose:  Prompts user to create a new sailing.
void createSailing()
{
    std::cout << "\n[CREATE NEW SAILING]\n"
              << "-------------------------------------------\n";
    // ─── Eat one leftover newline (from previous cin>> or getline) ───
    if (std::cin.peek() == '\n')
        std::cin.get();

    // 1) Prompt for vessel name
    char vesselName[26];
    std::cout << "Enter Vessel Name (max 25 characters): ";
    std::cin.getline(vesselName, sizeof(vesselName));
    if (!std::cin || vesselName[0] == '\0') {
        std::cin.clear();
        std::cout << "Error: no vessel name entered\n";
        return;
    }

    // 2) Validate vessel existence
    if (!getVesselByName(vesselName))
    {
        std::cout << "Error: Vessel not found\n";
        return;
    }

    // 3) Prompt for terminal
    char terminal[4]; // 3 char for ferry code + null terminator
    std::cout << "Enter Departure Terminal (3 character ferry code): ";
    std::cin.getline(terminal, sizeof(terminal));
    if (!std::cin || std::strlen(terminal) != 3) {
        std::cin.clear();
        std::cout << "Error: Invalid terminal code\n";
        return;
    }

    // 4) Prompt for departure date
    char departureDate[3];
    std::cout << "Enter Departure Date (2 digits): ";
    std::cin.getline(departureDate, sizeof(departureDate));
    if (!std::cin || std::strlen(departureDate) != 2 || !isdigit(departureDate[0]) || !isdigit(departureDate[1])) {
        std::cin.clear();
        std::cout << "Error: Invalid date format\n";
        return;
    }


    // 5) Prompt for departure time
    char departureTime[3];
    std::cout << "Enter Departure Time (2 digits): ";
    std::cin.getline(departureTime, sizeof(departureTime));
    if (!std::cin || std::strlen(departureTime) != 2 || !isdigit(departureTime[0]) || !isdigit(departureTime[1])) {
        std::cin.clear();
        return;
    }


    // 6) Create the Sailing ID and confirm the sailing does not already exist
    char sailingID[11];
    std::snprintf(sailingID, sizeof(sailingID), "%s-%s-%s", terminal, departureDate, departureTime);
    if (getSailingByID(sailingID))
    {
        std::cout << "Error: Sailing ID conflict\n";
        return;
    }

    // 7) Add new sailing
    Sailing newSailing;
    strncpy(newSailing.id, sailingID, sizeof(newSailing.id) - 1);
    newSailing.id[sizeof(newSailing.id) - 1] = '\0';
    strncpy(newSailing.vesselName, vesselName, sizeof(newSailing.vesselName) - 1);
    newSailing.vesselName[sizeof(newSailing.vesselName) - 1] = '\0';

    auto vesselOpt = getVesselByName(vesselName);
    if (vesselOpt.has_value()) {
        Vessel vessel = vesselOpt.value();
        newSailing.LRL = vessel.lowCap;
        newSailing.HRL = vessel.highCap;
    } else {
        std::cout << "Error: Vessel lookup failed during sailing creation\n";
        return;
    }
    if (!addSailing(newSailing))
    {
        std::cout << "Error: Failed to create sailing.\n";
        return;
    }

    // 8) Confirmation
    std::cout << "Sailing Created\n";
}

//-----------------------------------------------
// Function: deleteSailing
// Purpose:  Deletes an existing sailing based on the sailing ID.
void deleteSailing()
{
    // ─── Eat one leftover newline (from previous cin>> or getline) ───
    if (std::cin.peek() == '\n')
        std::cin.get();

    // 1) Prompt for sailing ID
    char sailingID[25];
    std::cout << "Enter Sailing ID (format: XXX-DD-HH): ";
    std::cin.getline(sailingID, sizeof(sailingID));
    if (!std::cin || std::strlen(sailingID) != 9) {
        std::cin.clear();
        std::cout << "Error: No sailing ID entered\n";
        return;
    }

    // 2) Validate Sailing ID format (XXX-DD-HH)
    if (strlen(sailingID) != 9 || 
        !isalpha(sailingID[0]) || !isalpha(sailingID[1]) || !isalpha(sailingID[2]) ||  // Terminal ID must be 3 alphabetic characters
        !isdigit(sailingID[4]) || !isdigit(sailingID[5]) ||  // Departure Date must be 2 numeric digits
        !isdigit(sailingID[7]) || !isdigit(sailingID[8]))   // Departure Hour must be 2 numeric digits
    {
        std::cout << "Error: Sailing ID not named correctly\n";
        return;
    }
    // 3) Validate sailing existence
    if (!getSailingByID(sailingID))
    {
        std::cout << "Error: Sailing not found\n";
        return;
    }
    // 4) Delete reservations associated with the sailing
    if (!deleteReservationsBySailingID(sailingID))
    {
        std::cout << "Error: Failed to delete reservations\n";
        return;
    }
    // 5) Delete the sailing record
    if (!deleteSailing(sailingID))
    {
        std::cout << "Error: Failed to delete sailing\n";
        return;
    }

    // Confirmation (Updated message as per your request)
    std::cout << "Sailing Canceled\n";
}

//------------------------------------------------------------------------
void viewSailingReport()
// Displays a paginated report of sailing records, showing up to 5 sailings per page.
// The report includes vessel name, sailing ID, remaining capacities, total vehicles,
// and capacity factor. Users can choose to load more pages if available.
{
    // Retrieve all sailing records from the system
    auto sailings = getAllSailings();  // Assuming this function returns a list of sailings
    std::reverse(sailings.begin(), sailings.end());
    int totalSailings = sailings.size();

    if (totalSailings == 0)
    {
        std::cout << "Error: No sailing records found\n";
        return; // No sailings in the system
    }

    // Display sailings in batches of 5 until all are shown or user chooses to stop
    bool loadMore = true;
    int index = 0;

    time_t now = time(nullptr);
    tm* localTime = localtime(&now);

    while (loadMore && totalSailings > index) {
        // Print report header with current date and time
        std::cout << "[VIEW SAILING REPORT]" << std::endl;
        std::cout << std::string(79, '-') << std::endl;
        std::cout << "    Date: " << std::put_time(localTime, "%y-%m-%d") << "      |      Time: " << std::put_time(localTime, "%H:%M") << std::endl;
        std::cout << std::string(79, '-') << std::endl;

        // Set up table headers with proper alignment
        std::cout << std::left
             << std::setw(4)  << "#"
             << std::setw(27) << "Vessel Name"
             << std::setw(12) << "Sailing ID"
             << std::right
             << std::setw(10) << "LRL(m)"
             << std::setw(11) << "HRL(m)"
             << std::setw(6)  << "TV"
             << std::setw(7)  << "CF"
             << std::endl
             << std::string(79, '-') << std::endl;

        // Display details for up to 5 sailings
        for (int i = 0; i < 5 && index < totalSailings; i++, index++) {
            const Sailing &s = sailings[index];
            // Calculate total vehicles reserved for this sailing
            int TV = countReservationsBySailing(s.id);
            auto v = getVesselByName(s.vesselName);
            float CF = 0.0f;
            if (v.has_value()) {
                // Calculate remaining capacity ratio (CF)
                // Note: CF is remaining / total, displayed with "%" which might be intended as percentage
                CF = (getRemainingCapacity(s.id).first + getRemainingCapacity(s.id).second) / (v.value().highCap + v.value().lowCap);
            }
            std::cout << std::setw(1) << (index + 1) << ")  "
                      << std::left  << std::setw(27) << s.vesselName
                      << std::setw(12)   << s.id
                      << std::fixed  << std::right
                      << std::setw(10)   << std::setprecision(1) << s.LRL
                      << std::setw(11)   << std::setprecision(1) << s.HRL
                      << std::setw(6)    << TV
                      << std::setw(7)    << std::setprecision(1) << CF << "%"
                      << "\n";
        }

        std::cout << std::string(79, '-') << "\n";

        // Check if there are more sailings to display
        if (index < totalSailings) {
            // Prompt user to load more sailings
            std::cout << "Load More? [y/n]: ";
            char input;
            std::cin >> input;
            std::cout << std::endl;

            if (input == 'n')
            {
                loadMore = false;
            }
            else if (input != 'y')
            {
                std::cout << "Error: Invalid input, exiting report." << std::endl;
                return; // Invalid input
            }

            if (index >= totalSailings)
            {
                std::cout << "Error: End of sailing records\n";
                return; // End of sailing records
            }
        }
    }

    // Return to main menu after report is complete
    return;
}

//-----------------------------------------------
// Function: filterSailingReportByID
// Purpose:  Filter the sailing report by a specific Sailing ID entered by the user.
void findSailingByID()
{
    // Display the filter sailing report prompt
    std::cout << "\n[FILTER SAILING REPORT]\n";
    std::cout << "-------------------------------------------\n";

    // Use a fixed-size char array instead of std::string for Sailing ID
    char sailingID[11];  // 10 characters + null terminator
    std::cout << "Enter Sailing ID (format: XXX-DD-HH): ";
    std::cin >> sailingID;

    // Check for valid Sailing ID format
    if (strlen(sailingID) != 9 || 
        !isalpha(sailingID[0]) || !isalpha(sailingID[1]) || !isalpha(sailingID[2]) || 
        !isdigit(sailingID[4]) || !isdigit(sailingID[5]) || 
        !isdigit(sailingID[7]) || !isdigit(sailingID[8]))
    {
        std::cout << "Error: Sailing ID not named correctly\n";
        return; // Return to the main menu after error
    }

    // Call the function to fetch sailing by ID
    auto sailing = getSailingByID(sailingID);

    // Check if sailing exists
    if (!sailing)
    {
        std::cout << "Error: No sailings found matching your criteria\n";
        return; // Return to the main menu if not found
    }

    // Display the Sailing Information
    std::cout << "\n[SAILING REPORT]\n";
    std::cout << "-------------------------------------------\n";
    std::cout << "# Vessel ID  Vessel Name  Sailing ID  Low(m)  High(m)\n";
    std::cout << "-------------------------------------------\n";
    std::cout << "[1] " << sailing->id << " " << sailing->vesselName << " " 
              << sailing->id << " " << sailing->LRL << " " 
              << sailing->HRL << "\n";

    // Ask user if they want to return to the main menu
    std::cout << "Enter [0] to return to Sub Menu: ";
    int choice;
    std::cin >> choice;

    if (choice != 0)
    {
        std::cout << "Error: Invalid input\n";
    }
    else
    {
        return;
        return;  // Return to the main menu after displaying the sailing info
    }
}