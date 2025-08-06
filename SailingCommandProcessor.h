// File: SailingCommandProcessor.h
/*
    Module: SailingCommandProcessor.h
    Revision History:
    Revision 3.0: 2025-08-01 - Updated by Brandon Landa‑Ahn and Raj Chowdhury
    Revision 2.0: 2025-07-22 - Updated by Arsh Garcha
    Revision 1.0: 2025-07-07 - Created by Brandon Landa-Ah
    Purpose:
        Declaration of SailingCommandProcessor API for sailing workflows.
*/

#ifndef SAILING_COMMAND_PROCESSOR_H
#define SAILING_COMMAND_PROCESSOR_H

//-----------------------------------------------
void createSailing();
// in:       none
// out:      none
// Purpose:  Create a new sailing, prompting for vessel name, terminal, date, and time.

//-----------------------------------------------
void deleteSailing();
// in:       none
// out:      none
// Purpose:  Deletes an existing sailing, including associated reservations.

//-----------------------------------------------
void viewSailingReport();
// in:       none
// out:      none
// Purpose:  Display the sailing report, showing up to 5 sailings at a time and providing an option to load more.

//-----------------------------------------------
void findSailingByID();
// in:       none
// out:      none
// Purpose:  Search for and display a sailing by its unique sailing ID.

#endif // SAILING_COMMAND_PROCESSOR_H