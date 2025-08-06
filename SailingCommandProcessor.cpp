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
    std::cout << "\n\033[94m[\033[96mCREATE NEW SAILING\033[94m]\n"
              << "\033[94m-------------------------------------------\n";
    // ─── Eat one leftover newline (from previous cin>> or getline) ───
    if (std::cin.peek() == '\n')
        std::cin.get();

    // 1) Prompt for vessel name
    char vesselName[26];
    std::cout << "\033[1;97mEnter Vessel Name (max 25 characters): \033[0m";
    std::cin.getline(vesselName, sizeof(vesselName));
    if (!std::cin || vesselName[0] == '\0') {
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Clear the entire input buffer
        std::cout << "\033[31mError: no vessel name entered\n\033[0m";
        return;
    }

    // 2) Validate vessel existence
    auto vesselOpt = getVesselByName(vesselName);
    if (!vesselOpt.has_value())
    {
        std::cout << "\033[31mError: Vessel not found\n\033[0m";
        return;
    }
    Vessel vesselUsed = vesselOpt.value();

    // 3) Prompt for terminal
    char terminal[4]; // 3 char for ferry code + null terminator
    std::cout << "\033[1;97mEnter Departure Terminal (3 character ferry code): \033[0m";
    std::cin.getline(terminal, sizeof(terminal));
    if (!std::cin || std::strlen(terminal) != 3) {
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Clear the entire input buffer
        std::cout << "\033[31mError: Invalid terminal code\n\033[0m";
        return;
    }

    // 4) Prompt for departure date
    char departureDate[4]; // Changed from 3 to 4 to allow for overflow detection
    std::cout << "\033[1;97mEnter Departure Date (2 digits): \033[0m";
    std::cin.getline(departureDate, sizeof(departureDate));
    if (!std::cin || std::strlen(departureDate) != 2 || !isdigit(departureDate[0]) || !isdigit(departureDate[1])) {
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cout << "\033[31mError: Invalid date format\n\033[0m";
        return;
    }

    // 5) Prompt for departure time
    char departureTime[4]; // Changed from 3 to 4 to allow for overflow detection
    std::cout << "\033[1;97mEnter Departure Time (2 digits): \033[0m";
    std::cin.getline(departureTime, sizeof(departureTime));
    if (!std::cin || std::strlen(departureTime) != 2 || !isdigit(departureTime[0]) || !isdigit(departureTime[1])) {
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cout << "\033[31mError: Invalid time format\n\033[0m";
        return;
    }


    // 6) Create the Sailing ID and confirm the sailing does not already exist
    char sailingID[11];
    std::snprintf(sailingID, sizeof(sailingID), "%s-%s-%s", terminal, departureDate, departureTime);
    if (getSailingByID(sailingID))
    {
        std::cout << "\033[31mError: Sailing ID conflict\n\033[0m";
        return;
    }

    // 7) Add new sailing
    Sailing newSailing;
    strncpy(newSailing.id, sailingID, sizeof(newSailing.id) - 1);
    newSailing.id[sizeof(newSailing.id) - 1] = '\0';
    strncpy(newSailing.vesselName, vesselName, sizeof(newSailing.vesselName) - 1);
    newSailing.vesselName[sizeof(newSailing.vesselName) - 1] = '\0';
    newSailing.LRL = vesselUsed.lowCap;  // Low Remaining Length
    newSailing.HRL = vesselUsed.highCap; // High Remaining Length
    newSailing.reservationsCount = 0;    // Initialize reservations count

    vesselOpt = getVesselByName(vesselName);
    if (vesselOpt.has_value()) {
        Vessel vessel = vesselOpt.value();
        newSailing.LRL = vessel.lowCap;
        newSailing.HRL = vessel.highCap;
    } else {
        std::cout << "\033[31mError: Vessel lookup failed during sailing creation\n\033[0m";
        return;
    }
    if (!addSailing(newSailing))
    {
        std::cout << "\033[31mError: Failed to create sailing.\n\033[0m";
        return;
    }

    // 8) Confirmation
    std::cout << "\033[32mSailing Created\n\033[0m";
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
    std::cout << "\033[1;97mEnter Sailing ID (format: XXX-DD-HH): \033[0m";
    std::cin.getline(sailingID, sizeof(sailingID));
    if (!std::cin || std::strlen(sailingID) != 9) {
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Clear the entire input buffer
        std::cout << "\033[31mError: No sailing ID entered\n\033[0m";
        return;
    }

    // 2) Validate Sailing ID format (XXX-DD-HH)
    if (strlen(sailingID) != 9 || 
        !isalpha(sailingID[0]) || !isalpha(sailingID[1]) || !isalpha(sailingID[2]) ||  // Terminal ID must be 3 alphabetic characters
        !isdigit(sailingID[4]) || !isdigit(sailingID[5]) ||  // Departure Date must be 2 numeric digits
        !isdigit(sailingID[7]) || !isdigit(sailingID[8]))   // Departure Hour must be 2 numeric digits
    {
        std::cout << "\033[31mError: Sailing ID not named correctly\n\033[0m";
        return;
    }
    // 3) Validate sailing existence
    if (!getSailingByID(sailingID))
    {
        std::cout << "\033[31mError: Sailing not found\n\033[0m";
        return;
    }
    // 4) Delete reservations associated with the sailing
    if (!deleteReservationsBySailingID(sailingID))
    {
        std::cout << "\033[31mError: Failed to delete reservations\n\033[0m";
        return;
    }
    // 5) Delete the sailing record
    if (!deleteSailing(sailingID))
    {
        std::cout << "\033[31mError: Failed to delete sailing\n\033[0m";
        return;
    }

    // Confirmation (Updated message as per your request)
    std::cout << "\n\033[32mSailing Canceled\n\033[0m";
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
        std::cout << "\033[31mError: No sailing records found\n\033[0m";
        return; // No sailings in the system
    }

    // Display sailings in batches of 5 until all are shown or user chooses to stop
    bool loadMore = true;
    int index = 0;

    time_t now = time(nullptr);
    tm* localTime = localtime(&now);

    while (loadMore && totalSailings > index) {
        // Print report header with current date and time
        std::cout << "\n\033[32m[VIEW SAILING REPORT]" << std::endl;
        std::cout << std::string(79, '-') << std::endl;
        std::cout << "     Date: " << std::put_time(localTime, "%y-%m-%d") << "      |      Time: " << std::put_time(localTime, "%H:%M") << std::endl;
        std::cout << std::string(79, '-') << std::endl;

        // Set up table headers with proper alignment
        std::cout << std::left
             << std::setw(5)  << "#"     // Changed from 4 to 5
             << std::setw(26) << "Vessel Name"  // Changed from 27 to 26
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
                // Calculate the total capacity (sum of low and high lanes)
                float totalCapacity = v.value().lowCap + v.value().highCap;

                // Calculate remaining capacity (including the buffer for each vehicle)
                float remainingCapacity = getRemainingCapacity(s.id).first + getRemainingCapacity(s.id).second;

                // Subtract the 0.5m buffer from the remaining capacity
                remainingCapacity -= 0.5 * TV;  // Subtract buffer for all vehicles (TV is the number of vehicles)

                // Calculate capacity factor (CF) and handle the case where totalCapacity is zero
                if (totalCapacity > 0) {
                    CF = ((totalCapacity - remainingCapacity) / totalCapacity) * 100;
                }
            }
            std::cout << std::setw(2) << (index + 1) << ")  "  // Changed from setw(1) to setw(2)
                      << std::left  << std::setw(26) << s.vesselName  // Changed from 27 to 26
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
            std::cout << "\033[1;97mLoad More? [y/n]: \033[0m";
            char input;
            std::cin >> input;
            std::cout << std::endl;

            if (input == 'n')
            {
                loadMore = false;
            }
            else if (input != 'y')
            {
                std::cout << "\033[31mError: Invalid input, exiting report.\033[0m" << std::endl;
                return; // Invalid input
            }

            if (index >= totalSailings)
            {
                std::cout << "\033[31mError: End of sailing records\n\033[0m";
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
    std::cout << "\n\033[96m[\033[1;96mFILTER SAILING REPORT\033[96m]\n";
    std::cout << "\033[94m-------------------------------------------\n";

    // Use a fixed-size char array instead of std::string for Sailing ID
    char sailingID[11];  // 10 characters + null terminator
    std::cout << "\033[1;97mEnter Sailing ID (format: XXX-DD-HH): \033[0m";
    std::cin >> sailingID;

    // Check for valid Sailing ID format
    if (strlen(sailingID) != 9 || 
        !isalpha(sailingID[0]) || !isalpha(sailingID[1]) || !isalpha(sailingID[2]) || 
        !isdigit(sailingID[4]) || !isdigit(sailingID[5]) || 
        !isdigit(sailingID[7]) || !isdigit(sailingID[8]))
    {
        std::cout << "\033[31mError: Sailing ID not named correctly\n\033[0m";
        return; // Return to the main menu after error
    }

    // Call the function to fetch sailing by ID
    auto sailing = getSailingByID(sailingID);

    // Check if sailing exists
    if (!sailing)
    {
        std::cout << "\033[31mError: No sailings found matching your criteria\n\033[0m";
        return; // Return to the main menu if not found
    }

    // Display the Sailing Information in the correct format
    std::cout << "\n\033[32m[SAILING REPORT]\n";
    std::cout << std::string(79, '-') << std::endl
              << std::left
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

    // Calculate total vehicles reserved for this sailing (TV = Total Vehicles)
    int TV = countReservationsBySailing(sailing->id);
    auto v = getVesselByName(sailing->vesselName);
    float CF = 0.0f;

    if (v.has_value()) {
        // Calculate the total capacity (sum of low and high lanes)
        float totalCapacity = v.value().lowCap + v.value().highCap;

        // Calculate remaining capacity (including the buffer for each vehicle)
        float remainingCapacity = getRemainingCapacity(sailing->id).first + getRemainingCapacity(sailing->id).second;

        // Subtract the 0.5m buffer from the remaining capacity
        remainingCapacity -= 0.5 * TV;  // Subtract buffer for all vehicles (TV is the number of vehicles)

        // Calculate capacity factor (CF) and handle the case where totalCapacity is zero
        if (totalCapacity > 0) {
            CF = ((totalCapacity - remainingCapacity) / totalCapacity) * 100;
        }
    }

    // Output the specific sailing's details (only one sailing will be displayed)
    std::cout << std::setw(1) << 1 << ")  " // Only one sailing, so always index 1
              << std::left  << std::setw(27) << sailing->vesselName
              << std::setw(12)   << sailing->id
              << std::fixed  << std::right
              << std::setw(10)   << std::setprecision(1) << sailing->LRL
              << std::setw(11)   << std::setprecision(1) << sailing->HRL
              << std::setw(6)    << TV
              << std::setw(7)    << std::setprecision(1) << CF << "%"
              << "\n";
    
    // Ask user if they want to return to the main menu
    std::cout << "\n\033[1;97mEnter [0] to return to Sub Menu: \033[0m";
    int choice;
    std::cin >> choice;

    if (choice != 0)
    {
        std::cout << "\033[31mError: Invalid input\n\033[0m";
    }
    else
    {
        return;
    }
}
