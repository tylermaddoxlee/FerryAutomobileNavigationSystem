//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
/* 
    Module: Utilities.cpp
    Revision History:
    Revision 3.0: 2025-08-02 - Updated by Raj Chowdhury
    Revision 2.0: 2025-07-22 – Updated by Arsh Garcha
    Revision 1.0: 2025-07-07 – Original by Brandon Landa‑Ahn
    Purpose:
    Provides functions that control the overall lifecycle of the system.
*/
#include "Utilities.h"
#include "SailingASM.h"
#include "ReservationASM.h"
#include "VesselASM.h"
#include "VehicleASM.h"

//===============================================
// Function: startup
// in:       none
// out:      none
// Purpose:  Initialize all ASM storage modules.
void startup()
{
    initializeSailingStorage();
    initializeReservationStorage();
    initializeVesselStorage();
    initializeVehicleStorage();
}

//-----------------------------------------------
// Function: shutdown
// in:       none
// out:      none
// Purpose:  Shutdown all ASM storage modules in reverse order.
void shutdown()
{
    shutdownVehicleStorage();
    shutdownVesselStorage();
    shutdownReservationStorage();
    shutdownSailingStorage();
}

//-----------------------------------------------
// Function: reset
// in:       none
// out:      none
// Purpose:  Reset the system by shutting down and then restarting.
void reset()
{
    shutdown();
    startup();
}
