//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
// File: ReservationCommandProcessor.h
/*
    Module: ReservationCommandProcessor.h
    Revision History:
    Revision 2.0: 2025-08-02 – Updated by Arsh Garcha
    Revision 1.0: 2025-07-24 - Updated by Arsh Garcha
    Purpose:
        Declaration of the ReservationCommandProcessor API for reservation workflows.
*/

#ifndef RESERVATION_COMMAND_PROCESSOR_H
#define RESERVATION_COMMAND_PROCESSOR_H

//-----------------------------------------------
void createReservation();
// in:  none
// out: none
// Purpose: Ask whether vehicle is registered and dispatch to the appropriate create function.

//-----------------------------------------------
void createReservationForRegisteredVehicle();
// in:  none
// out: none
// Purpose: Prompt for existing vehicle’s license plate and sailing ID, then create reservation.

//-----------------------------------------------
void createReservationForUnregisteredVehicle();
// in:  none
// out: none
// Purpose: Prompt for sailing ID and new vehicle info, register vehicle, then create reservation.

//-----------------------------------------------
void cancelReservation();
// in:  none
// out: none
// Purpose: Prompt for reservation ID and cancel (delete) the reservation.

//-----------------------------------------------
void checkInReservation();
// in:  none
// out: none
// Purpose: Prompt for reservation ID, compute fee, and update onboard status.

#endif // RESERVATION_COMMAND_PROCESSOR_H