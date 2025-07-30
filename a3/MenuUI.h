// File: MenuUI.h
// Module: MenuUI.h
// Revision History:
//   Revision 1.0: 2025-07-07 – Original by Brandon Landa‑Ahn
//   Revision 2.0: 2025-07-22 – Updated by Arsh Garcha
// Purpose:
//   This presents the UI of the main menu to the user and allows them to select commands.

#ifndef MENU_UI_H
#define MENU_UI_H

//-----------------------------------------------
// Function: showMainMenu
// in:       none
// out:      none
// Purpose:  Display the main menu options to the user.
void showMainMenu();

//-----------------------------------------------
// Function: getMenuSelection
// in:       none
// out:      valid choice (int)
// Purpose:  Prompt and return the user's menu selection.
int getMenuSelection();

//-----------------------------------------------
// Function: showSailingSubMenu
// in:       none
// out:      none
// Purpose:  Display the sailing submenu and handle input.
void showSailingSubMenu();

//-----------------------------------------------
// Function: showReservationSubMenu
// in:       none
// out:      none
// Purpose:  Display the reservation submenu and handle input.
void showReservationSubMenu();

//-----------------------------------------------
// Function: handleMenuSelection
// in:       choice (int)
// out:      none
// Purpose:  Dispatch the choice to the corresponding command processor.
void handleMenuSelection(int choice);

//-----------------------------------------------
// Function: runMainMenu
// in:       none
// out:      none
// Purpose:  Control loop for the main menu until user exits.
void runMainMenu();

#endif  // MENU_UI_H
