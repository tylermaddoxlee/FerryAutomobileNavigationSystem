// File: VesselCommandProcessor.cpp
// Module: VesselCommandProcessor.cpp
// Revision History:
//   Revision 2.0: 2025-07-22 – Updated by Raj Chowdhury
//   Revision 1.0: 2025-07-07 – Original by Team
// Purpose:
//   Implementation of vessel command workflows.

#include <iostream>
#include <limits>
#include "VesselCommandProcessor.h"
#include "VesselASM.h"    // addVessel(const Vessel&), getVesselByName(const char*)
#include "Vessel.h"       // struct Vessel { char name[25]; int lowCap; int highCap; };
#include "MenuUI.h"       // For returning to main menu

namespace
{
    //-----------------------------------------------
    // Function: promptCString
    // in:       prompt – the message to display
    //           buf    – buffer to receive input
    //           maxLen – maximum characters (including null)
    // out:      true if input succeeded, false on EOF/error
    // Purpose:  Read a fixed-length C-string from stdin.
    bool promptCString(const char* prompt, char* buf, std::size_t maxLen)
    {
        std::cout << prompt;
        std::cin.clear(); // Clear any error flags
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Discard previous input

        std::cin.getline(buf, maxLen);
        if (!std::cin)
            return false;
        return static_cast<bool>(std::cin);
    }

    //-----------------------------------------------
    // Function: promptInt
    // in:       prompt – the message to display
    //           out    – reference to store the integer
    // out:      true if numeric input, false otherwise
    // Purpose:  Read and validate an integer capacity.
    bool promptInt(const char* prompt, int &out)
    {
        std::cout << prompt;
        if (!(std::cin >> out))
        {
            std::cout << "Error: Lane capacity must be numeric characters.\n";
            return false;
        }
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        return true;
    }
}

//-----------------------------------------------
// Function: createVessel
// in:       none
// out:      none
// Purpose:  Prompt for vessel details and add a new vessel record.
void createVessel()
{
    std::cout << "\n[CREATE NEW VESSEL]\n"
              << "-------------------------------------------\n";

    Vessel v;
    // ─── Eat one leftover newline (from previous cin>> or getline) ───
    if (std::cin.peek() == '\n')
        std::cin.get();

    std::cout << "Enter Vessel Name (max 25 characters): ";
    std::cin.getline(v.name, sizeof(v.name));
    if (!std::cin || v.name[0] == '\0')
    {        
        std::cin.clear();
        std::cout << "Error: No vessel name entered\n";
        return;  // Go back to main menu after error
    }

    if (getVesselByName(v.name))
    {
        std::cout << "Error: Vessel name already exists.\n";
        return;
    }

    char value[4];

    std::cout << "Enter Total Low Lane Capacity (max 3600): "; //Error: Lane capacity must be numeric characters/////////////////////////////////////500
    std::cin >> value;
    int intValueLow = atoi(value);
    if (!std::cin || intValueLow < 0 || intValueLow > 3600)
    {
        std::cin.clear();
        std::cout << "Error: Invalid low lane capacity.\n";
        return;  // Go back to main menu after error
    }
    v.lowCap = intValueLow;

    
    std::cout << "Enter Total High Lane Capacity (max 3600): "; //Error: Lane capacity must be numeric characters/////////////////////////////////////
    std::cin >> value;
    int intValueHigh = atoi(value);
    if (!std::cin || intValueHigh < 0 || intValueHigh > 3600)
    {        
        std::cin.clear();
        std::cout << "Error: Invalid high lane capacity.\n";
        return;  // Go back to main menu after error
    }
    v.highCap = intValueHigh;

    const Vessel& vesselRef = v;
    if (!addVessel(vesselRef))
    {
        std::cout << "Error: failed to save vessel.\n";
        return;
    }

    std::cout << "\nVessel Created\n";
}
