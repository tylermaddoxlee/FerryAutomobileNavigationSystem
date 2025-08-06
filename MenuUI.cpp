//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
/*          
    Module: MenuUI.cpp
    Revision History: 
    Revision 3.0: 2025/08/02 - Original by Brandon Landa-Ahn and Raj Chowdhury
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
int getMenuSelection(int min, int max)
{
    int choice = -1;
    while (true)
    {
        std::cout << "\033[1;97mChoose an option ["<<min<<"-"<<max<<"] and press ENTER: \033[0m";
        if (!(std::cin >> choice)) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cout << "\033[31mInvalid input. Please enter a number.\n\033[0m";
            continue;
        }
        if (choice >= min && choice <= max)
            return choice;
        std::cout << "\033[31mChoice must be between " << min << " and " << max << ". Try again.\n\033[0m";
    }
}

//-----------------------------------------------
void showSailingSubMenu()
{
    bool exitSubmenu = false;  // controls submenu loop termination
    
    while (!exitSubmenu)  // Loop goal: display sailing submenu until user exits
    {
        std::cout << "\n\033[94m[\033[1;96mMANAGE SAILINGS\033[94m]\n";
        std::cout << "\033[94m-------------------------------------------------------------------------------\n";
        std::cout << "\033[94m[1] \033[1;96mCreate New Sailing\n";
        std::cout << "\033[94m[2] \033[1;96mDelete Existing Sailings\n";
        std::cout << "\033[94m[3] \033[1;96mSearch Sailing by ID\n";
        std::cout << "\033[94m[0] \033[1;96mExit to Main Menu\n";
        std::cout << "\033[94m-------------------------------------------------------------------------------\n";
        
        int userChoice = getMenuSelection(0, 3);  // validated user selection
        
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
                std::cout << "\033[31mInvalid choice. Try again.\n\033[0m";
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
        std::cout << "\n\033[94m[\033[1;96mMANAGE RESERVATIONS\033[94m]\n";
        std::cout << "\033[94m-------------------------------------------------------------------------------\n";
        std::cout << "\033[94m[1] \033[1;96mCreate New Reservation\n";
        std::cout << "\033[94m[2] \033[1;96mDelete Existing Reservation\n";
        std::cout << "\033[94m[0] \033[1;96mExit to Main Menu\n";
        std::cout << "\033[94m-------------------------------------------------------------------------------\n";
        
        int userChoice = getMenuSelection(0, 2);  // validated menu selection
        
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
                std::cout << "\033[31mInvalid choice. Try again.\n\033[0m";
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
            std::cout << "\033[31mInvalid choice.\n\033[0m";  // Should not occur due to validation
    }
}

//-----------------------------------------------
void runMainMenu()
{
    bool programExit = false;  // main program loop control flag
    
    while (!programExit)  // Loop goal: run main menu until user chooses to quit
    {
        showMainMenu();                    // Display menu options to user
        int userSelection = getMenuSelection(0, 5);  // Get validated menu choice
        
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
    std::cout << "\n\033[94m[\033[1;97mMAIN MENU\033[94m]\n";        // Blue brackets, bold bright white text
    std::cout << "\033[94m-------------------------------------------------------------------------------\n";
    std::cout << "\033[94m[1] \033[96mCreate Vessels\n";     // Blue digits/brackets, cyan text
    std::cout << "\033[94m[2] \033[96mManage Sailings\n";   // Blue digits/brackets, cyan text
    std::cout << "\033[94m[3] \033[96mManage Reservations\n"; // Blue digits/brackets, cyan text
    std::cout << "\033[94m[4] \033[96mCheck-in Vehicle\n";   // Blue digits/brackets, cyan text
    std::cout << "\033[94m[5] \033[96mView Sailing Report\n"; // Blue digits/brackets, cyan text
    std::cout << "\033[94m[0] \033[96mQuit\n";        // Blue digits/brackets, cyan text
    std::cout << "\033[94m-------------------------------------------------------------------------------\n";
}