//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
/*          
    Module: MenuUI.cpp
    Revision History: 
    Revision 2.0: 2025/07/22 - Updated by Arsh Garcha
    Revision 1.0: 2025/07/07 - Original by Team
    Purpose: 
        This module implements the main menu UI loop and all submenu interactions
        for the ferry reservation system. It provides input validation and dispatches
        user selections to appropriate command processors. The module uses a 
        hierarchical menu structure with the main menu branching to specialized
        submenus for sailings and reservations.
*/

//============================================

#include <iostream>
#include <limits>
#include "MenuUI.h"
#include "SailingCommandProcessor.h"
#include "ReservationCommandProcessor.h"
#include "VesselCommandProcessor.h"

//-----------------------------------------------
int getMenuSelection()
{
    int choice = -1;  // stores user input for validation
    
    while (true)  // Loop goal: obtain valid integer input in range [0..5]
    {
        std::cout << "Choose an option [0-5] and press ENTER: ";
        
        if (!(std::cin >> choice))  // Check if input extraction failed (non-integer)
        {
            std::cin.clear();  // Reset stream error flags
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');  // Discard invalid input
            std::cout << "Invalid input. Please enter a number.\n";
            continue;  // Retry input
        }
        
        if (choice >= 0 && choice <= 5)  // Valid range check
            return choice;
            
        std::cout << "Choice must be between 0 and 5. Try again.\n";
    }
}

//-----------------------------------------------
void showSailingSubMenu()
{
    bool exitSubmenu = false;  // controls submenu loop termination
    
    while (!exitSubmenu)  // Loop goal: display sailing submenu until user exits
    {
        std::cout << "\n[MANAGE SAILINGS]\n";
        std::cout << "-------------------------------------------\n";
        std::cout << "[1] Create New Sailing\n";
        std::cout << "[2] Delete Existing Sailings\n";
        std::cout << "[3] Search Sailing by ID\n";
        std::cout << "[0] Exit to Main Menu\n";
        std::cout << "-------------------------------------------\n";
        
        int userChoice = getMenuSelection();  // validated user selection
        
        switch (userChoice)  // Dispatch to appropriate sailing operation
        {
            case 1:
                createSailing();  // Adds new sailing record to end of file
                break;
            case 2:
                deleteSailing();  // Removes or marks sailing record as deleted
                break;
            case 3:
                findSailingByID();  // Performs linear search through sailing records
                break;
            case 0:
                exitSubmenu = true;  // Set flag to exit submenu loop
                break;
            default:
                std::cout << "Invalid choice. Try again.\n";
                break;
        }
    }
}

//-----------------------------------------------
void showReservationSubMenu()
{
    bool exitSubmenu = false;  // controls reservation submenu loop
    
    while (!exitSubmenu)  // Loop goal: display reservation submenu until user exits
    {
        std::cout << "\n[MANAGE RESERVATIONS]\n";
        std::cout << "-------------------------------------------\n";
        std::cout << "[1] Create New Reservation\n";
        std::cout << "[2] Delete Existing Reservation\n";
        std::cout << "[0] Exit to Main Menu\n";
        std::cout << "-------------------------------------------\n";
        
        int userChoice = getMenuSelection();  // validated menu selection
        
        switch (userChoice)  // Route to reservation management functions
        {
            case 1:
                createReservation();  // Appends new reservation record to file
                break;
            case 2:
                cancelReservation();  // Marks reservation as cancelled in record
                break;
            case 0:
                exitSubmenu = true;  // Exit reservation submenu
                break;
            default:
                std::cout << "Invalid choice. Try again.\n";
                break;
        }
    }
}

//-----------------------------------------------
void handleMenuSelection(int choice)
{
    // Dispatch main menu selection to appropriate handler
    // Uses switch for O(1) lookup performance vs. if-else chain
    switch (choice)
    {
        case 1:
            createVessel();      // Launch vessel creation and management
            break;
        case 2:
            showSailingSubMenu();  // Enter sailing management submenu
            break;
        case 3:
            showReservationSubMenu();  // Enter reservation management submenu
            break;
        case 4:
            checkInReservation();  // Process vehicle check-in procedure
            break;
        case 5:
            viewSailingReport();   // Generate and display sailing report
            break;
        case 0:
            // Exit case: no action needed, handled by caller
            break;
        default:
            std::cout << "Invalid choice.\n";  // Should not occur due to validation
    }
}

//-----------------------------------------------
void runMainMenu()
{
    bool programExit = false;  // main program loop control flag
    
    while (!programExit)  // Loop goal: run main menu until user chooses to quit
    {
        showMainMenu();                    // Display menu options to user
        int userSelection = getMenuSelection();  // Get validated menu choice
        
        if (userSelection == 0)  // Check for exit condition
            programExit = true;
        else
            handleMenuSelection(userSelection);  // Process non-exit selections
    }
}

//-----------------------------------------------
void showMainMenu()
{
    // Display main menu options using consistent formatting
    // No input validation needed - this is display-only function
    std::cout << "[Main Menu]\n";
    std::cout << "-------------------------------------------\n";
    std::cout << "[1] Manage Vessels\n";
    std::cout << "[2] Manage Sailings\n";
    std::cout << "[3] Manage Reservations\n";
    std::cout << "[4] Check-in Vehicle\n";
    std::cout << "[5] View Sailing Report\n";
    std::cout << "[0] Quit\n";
    std::cout << "-------------------------------------------\n";
}