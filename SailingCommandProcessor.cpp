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

namespace
{
    //-----------------------------------------------
    // Function: promptCString
    // Purpose:  Prompt for and read a fixed-length C-string.
    bool promptCString(const char* prompt, char* buf, std::size_t maxLen)
    {
        std::cout << prompt;
        std::cin.getline(buf, maxLen);
        if (!std::cin)
            return false;
        return true;
    }

    //-----------------------------------------------
    // Function: promptInt
    // Purpose:  Prompt for an integer and validate it's within an acceptable range.
    bool promptInt(const char* prompt, int &out)
    {
        std::cout << prompt;
        if (!(std::cin >> out))
        {
            return false;
        }
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Clear input buffer
        return true;
    }

}

//-----------------------------------------------
// Function: createSailing
// Purpose:  Prompts user to create a new sailing.
void createSailing()
{
    std::cout << "\n[CREATE NEW SAILING]\n"
              << "-------------------------------------------\n";

    // 1) Prompt for vessel name
    char vesselName[25];
    if (!promptCString("Enter Vessel Name (max 25 characters): ", vesselName, sizeof(vesselName)))
    {
        return; // EOF or error, exit function
    }

    // 2) Validate vessel existence
    if (!getVesselByName(vesselName))
    {
        std::cout << "Error: Vessel not found\n";
        runMainMenu();  // Go back to main menu
        return;
    }

    // 3) Prompt for terminal
    char terminal[4]; // 3 char for ferry code + null terminator
    if (!promptCString("Enter Departure Terminal (3 character ferry code): ", terminal, sizeof(terminal)))
    {
        return; // EOF or error, exit function
    }

    // 4) Prompt for departure date
    int departureDate;
    if (!promptInt("Enter Departure Date (2 digits): ", departureDate))
    {
        return; // Invalid input, exit function
    }

    // 5) Prompt for departure time
    int departureTime;
    if (!promptInt("Enter Departure Time (2 digits): ", departureTime))
    {
        return; // Invalid input, exit function
    }

    // 6) Create the Sailing ID and confirm the sailing does not already exist
    std::string sailingID = std::string(vesselName) + std::to_string(departureDate) + std::to_string(departureTime);
    if (getSailingByID(sailingID.c_str()))
    {
        std::cout << "Error: Sailing ID conflict\n";
        runMainMenu();  // Go back to main menu
        return;
    }

    // 7) Add new sailing
    Sailing newSailing;
    strncpy(newSailing.id, sailingID.c_str(), sizeof(newSailing.id) - 1);
    newSailing.id[sizeof(newSailing.id) - 1] = '\0';
    strncpy(newSailing.vesselName, vesselName, sizeof(newSailing.vesselName) - 1);
    newSailing.vesselName[sizeof(newSailing.vesselName) - 1] = '\0';


    if (!addSailing(newSailing))
    {
        std::cout << "Error: Failed to create sailing.\n";
        runMainMenu();  // Go back to main menu
        return;
    }

    // 8) Confirmation
    std::cout << "Sailing Created\n";
    runMainMenu();  // Go back to main menu after success
}

//-----------------------------------------------
// Function: deleteSailing
// Purpose:  Deletes an existing sailing based on the sailing ID.
void deleteSailing()
{
    // 1) Prompt for sailing ID
    char sailingID[25];
    if (!promptCString("Enter Sailing ID (format: XXX-DD-HH): ", sailingID, sizeof(sailingID)))
    {
        return; // EOF or error, exit function
    }
    // 2) Validate Sailing ID format (XXX-DD-HH)
    if (strlen(sailingID) != 10 || 
        !isalpha(sailingID[0]) || !isalpha(sailingID[1]) || !isalpha(sailingID[2]) ||  // Terminal ID must be 3 alphabetic characters
        !isdigit(sailingID[4]) || !isdigit(sailingID[5]) ||  // Departure Date must be 2 numeric digits
        !isdigit(sailingID[7]) || !isdigit(sailingID[8]))   // Departure Hour must be 2 numeric digits
    {
        std::cout << "Error: Sailing ID not named correctly\n";
        runMainMenu();  // Go back to main menu
        return;
    }
    // 3) Validate sailing existence
    if (!getSailingByID(sailingID))
    {
        std::cout << "Error: Sailing not found\n";
        runMainMenu();  // Go back to main menu
        return;
    }
    // 4) Delete reservations associated with the sailing
    if (!deleteReservationsBySailingID(sailingID))
    {
        std::cout << "Error: Failed to delete reservations\n";
        runMainMenu();  // Go back to main menu
        return;
    }
    // 5) Delete the sailing record
    if (!deleteSailing(sailingID))
    {
        std::cout << "Error: Failed to delete sailing\n";
        runMainMenu();  // Go back to main menu
        return;
    }

    // Confirmation (Updated message as per your request)
    std::cout << "Sailing Canceled\n";
    runMainMenu();  // Go back to main menu after success
}

//------------------------------------------------------------------------
void viewSailingReport()
// Displays a paginated report of sailing records, showing up to 5 sailings per page.
// The report includes vessel name, sailing ID, remaining capacities, total vehicles,
// and capacity factor. Users can choose to load more pages if available.
{
    // Retrieve all sailing records from the system
    auto sailings = getAllSailings();  // Assuming this function returns a list of sailings
    int totalSailings = sailings.size();

    if (totalSailings == 0)
    {
        std::cout << "Error: No sailing records found\n";
        runMainMenu();  // Go back to main menu after error
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
            std::cout << std::setw(1) << (i + 1) << ")  "
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
                runMainMenu();  // Go back to main menu after error
                return; // Invalid input
            }

            if (index >= totalSailings)
            {
                std::cout << "Error: End of sailing records\n";
                runMainMenu();  // Go back to main menu after error
                return; // End of sailing records
            }
        }
    }

    // Return to main menu after report is complete
    runMainMenu();
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
    if (strlen(sailingID) != 10 || 
        !isalpha(sailingID[0]) || !isalpha(sailingID[1]) || !isalpha(sailingID[2]) || 
        !isdigit(sailingID[4]) || !isdigit(sailingID[5]) || 
        !isdigit(sailingID[7]) || !isdigit(sailingID[8]))
    {
        std::cout << "Error: Sailing ID not named correctly\n";
        runMainMenu();  // Return to the main menu after error
        return; // Return to the main menu after error
    }

    // Call the function to fetch sailing by ID
    auto sailing = getSailingByID(sailingID);

    // Check if sailing exists
    if (!sailing)
    {
        std::cout << "Error: No sailings found matching your criteria\n";
        runMainMenu();  // Return to the main menu after error
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
    std::cout << "Enter [0] to return to Main Menu: ";
    int choice;
    std::cin >> choice;

    if (choice != 0)
    {
        std::cout << "Error: Invalid input\n";
        runMainMenu();  // Return to main menu on invalid input
    }
    else
    {
        runMainMenu();  // Return to the main menu after displaying the sailing info
    }
}