// File: Utilities.h
// Module: Utilities.h
// Revision History:
//   Revision 2.0: 2025-07-22 – Updated by Arsh Garcha
//   Revision 1.0: 2025-07-07 – Original by Brandon Landa‑Ahn
// Purpose:
//   Provides functions that control the overall lifecycle of the system.

#ifndef UTILITIES_H
#define UTILITIES_H

//-----------------------------------------------
// Function: startup
// in:       none
// out:      none
// Purpose:  Initialize all ASM storage modules.
void startup();

//-----------------------------------------------
// Function: shutdown
// in:       none
// out:      none
// Purpose:  Shutdown all ASM storage modules in reverse order.
void shutdown();

//-----------------------------------------------
// Function: reset
// in:       none
// out:      none
// Purpose:  Reset the system by shutting down and then restarting.
void reset();

#endif  // UTILITIES_H