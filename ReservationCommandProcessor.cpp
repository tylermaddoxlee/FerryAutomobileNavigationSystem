//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
/*          
    Module: ReservationCommandProcessor.cpp
    Revision History: 
    Revision 2.0: 2025/07/24 - Updated by Raj Chowdhury
    Revision 1.0: 2025/07/07 - Original by Team
    Purpose: 
        This module implements the reservation command processing layer for the ferry 
        reservation system. It handles user interactions for creating, canceling, and 
        checking in reservations. The module coordinates between the UI layer and the 
        ASM layer, performing business logic validation including capacity checking, 
        lane assignment based on vehicle dimensions, and fare calculations. It supports 
        both registered vehicles (from vehicle database) and unregistered vehicles 
        (ad-hoc entries). The module uses a two-tier lane system with different pricing 
        algorithms based on vehicle size categories.
        
        Algorithm: Lane assignment uses height-first then length-based allocation
        Data validation: Input sanitization and range checking for all user inputs
*/

//============================================

#include <cstring>
#include <iostream>
#include <limits>
#include <string>
#include "ReservationCommandProcessor.h"
#include "ReservationASM.h"
#include "SailingASM.h"
#include "VehicleASM.h"
#include "MenuUI.h"
#include "Reservation.h"
#include "Sailing.h" 
#include "Vehicle.h"

using namespace std;

//-----------------------------------------------
void createReservationForRegisteredVehicle()
{
    Vehicle vehicleRecord;          // stores retrieved vehicle data from database
    Reservation newReservation;     // reservation record to be created
    Sailing sailingRecord;          // sailing data for capacity checking
    char sailingID[9];              // sailing identifier in format XXX-DD-HH
    char licensePlate[11];          // vehicle license plate number

    // Step 1: Collect license plate and validate vehicle registration
    cout << "Enter License Plate Number (max 10 characters): ";
    cin >> licensePlate;

    // Perform database lookup to retrieve existing vehicle information
    auto existingVehicleOpt = getVehicleByLicensePlate(licensePlate);
    if (!existingVehicleOpt)  // Vehicle not found in registered database
    {
        cout << "Error: License plate not found\n";

        return;
    }
    vehicleRecord = *existingVehicleOpt;  // Extract vehicle data from optional

    // Step 2: Collect sailing information and validate sailing exists
    cout << "Enter Sailing ID (format: XXX-DD-HH): ";
    cin >> sailingID;

    // Step 3: Generate composite reservation ID using concatenation algorithm
    char reservationID[20];  // buffer for combined ID string
    snprintf(reservationID, sizeof(reservationID), "%s%s", licensePlate, sailingID);

    // Check for duplicate reservation to prevent double-booking
    auto existingReservationOpt = getReservationByID(reservationID);
    if (existingReservationOpt)  // Reservation already exists for this combination
    {
        cout << "Error: A reservation already exists for this vehicle on this sailing.\n";
        return;
    }

    // Step 4: Retrieve and validate sailing data for capacity checking
    auto sailingOpt = getSailingByID(sailingID);
    if (!sailingOpt)  // Invalid sailing ID provided
    {
        cout << "Error: Sailing ID does not exist\n";
        return;
    }
    sailingRecord = *sailingOpt;

    // Step 5: Implement lane assignment algorithm based on vehicle dimensions
    Lane assignedLane;
    if (vehicleRecord.vehicleHeight <= 2)  // Vehicle can use either lane
    {
        // Priority algorithm: try low lane first, then high lane
        if (sailingRecord.LRL >= vehicleRecord.vehicleLength)  // Low lane has capacity
        {
            assignedLane = Lane::LOW;
        } 
        else if (sailingRecord.HRL >= vehicleRecord.vehicleLength)  // High lane has capacity
        {
            assignedLane = Lane::HIGH;
        } 
        else  // No lane has sufficient remaining capacity
        {
            cout << "Error: No sufficient capacity in either lane\n";
            return;
        }
    } 
    else  // Tall vehicle (>2m) must use high lane only
    {
        if (sailingRecord.HRL >= vehicleRecord.vehicleLength)  // High lane capacity check
        {
            assignedLane = Lane::HIGH;
        } 
        else  // High lane insufficient for tall vehicle
        {
            cout << "Error: Not enough capacity in the high lane\n";
            return;
        }
    }

    // Step 6: Populate reservation record with collected and calculated data
    snprintf(newReservation.id, sizeof(newReservation.id), "%s", reservationID);
    strncpy(newReservation.licensePlate, licensePlate, sizeof(newReservation.licensePlate) - 1);
    strncpy(newReservation.sailingID, sailingID, sizeof(newReservation.sailingID) - 1);
    newReservation.vehicleLength = vehicleRecord.vehicleLength;  // Copy from registered vehicle
    newReservation.vehicleHeight = vehicleRecord.vehicleHeight;  // Copy from registered vehicle
    strncpy(newReservation.phone, vehicleRecord.phone, sizeof(newReservation.phone) - 1);
    newReservation.onboard = false;  // Initial check-in status is false
    newReservation.reservedLane = assignedLane;  // Store calculated lane assignment

    // Attempt to persist reservation to storage
    if (!addReservation(newReservation))  // Storage operation failed
    {
        cout << "Error: Reservation could not be created.\n";
        return;
    }

    // Step 7: Update sailing capacity based on successful reservation
    if (assignedLane == Lane::LOW)  // Update low lane remaining capacity
    {
        sailingRecord.LRL -= vehicleRecord.vehicleLength;
    } 
    else  // Update high lane remaining capacity
    {
        sailingRecord.HRL -= vehicleRecord.vehicleLength;
    }

    sailingRecord.reservationsCount++;  // Increment total reservation counter

    // Persist updated sailing data with new capacity and count
    updateSailing(sailingRecord);

    cout << "\nReservation Created.\n";
}

//-----------------------------------------------
void createReservationForUnregisteredVehicle()
{
    Vehicle tempVehicle;            // temporary vehicle record for unregistered vehicle
    Reservation newReservation;     // reservation to be created
    Sailing sailingRecord;          // sailing data for capacity validation
    char sailingID[9];              // sailing identifier
    char licensePlate[11];          // vehicle license plate
    char phoneNumber[15];           // contact phone number

    // Step 1: Collect and validate sailing information first
    cout << "Enter Sailing ID (format: XXX-DD-HH): ";
    cin >> sailingID;

    // Retrieve sailing data to validate sailing exists and check capacity
    auto sailingOpt = getSailingByID(sailingID);
    if (!sailingOpt)  // Sailing ID not found in database
    {
        cout << "Error: Sailing ID does not exist\n";
        return;
    }
    sailingRecord = sailingOpt.value();

    // Step 2: Collect vehicle identification
    cout << "Enter License Plate Number (max 10 characters): ";
    cin >> licensePlate;

    // Step 3: Generate reservation ID and check for duplicates
    char reservationID[20];
    snprintf(reservationID, sizeof(reservationID), "%s%s", licensePlate, sailingID);

    // Prevent double-booking by checking existing reservations
    auto existingReservationOpt = getReservationByID(reservationID);
    if (existingReservationOpt)  // Duplicate reservation found
    {
        cout << "Error: A reservation already exists for this vehicle on this sailing.\n";
        return;
    }

    // Step 4: Collect vehicle dimensions for unregistered vehicle
    cout << "Enter Vehicle Length (max 99.9m): ";
    cin >> tempVehicle.vehicleLength;
    cout << "Enter Vehicle Height (max 9.9m): ";
    cin >> tempVehicle.vehicleHeight;

    // Step 5: Apply lane assignment algorithm based on collected dimensions
    Lane assignedLane;
    if (tempVehicle.vehicleHeight <= 2)  // Standard height vehicle - can use either lane
    {
        // Priority-based assignment: prefer low lane, fallback to high lane
        if (sailingRecord.LRL >= tempVehicle.vehicleLength)  // Low lane available
        {
            assignedLane = Lane::LOW;
        } 
        else if (sailingRecord.HRL >= tempVehicle.vehicleLength)  // High lane available
        {
            assignedLane = Lane::HIGH;
        } 
        else  // Neither lane has sufficient capacity
        {
            cout << "Error: No sufficient capacity in either lane\n";
            return;
        }
    } 
    else  // Overheight vehicle - high lane only
    {
        if (sailingRecord.HRL >= tempVehicle.vehicleLength)  // High lane capacity check
        {
            assignedLane = Lane::HIGH;
        } 
        else  // High lane insufficient for overheight vehicle
        {
            cout << "Error: Not enough capacity in the high lane\n";
            return;
        }
    }

    // Step 6: Collect contact information for unregistered vehicle
    cout << "Enter Phone Number (max 14 characters): ";
    cin >> phoneNumber;

    // Step 7: Construct reservation record with collected data
    snprintf(newReservation.id, sizeof(newReservation.id), "%s", reservationID);
    strncpy(newReservation.licensePlate, licensePlate, sizeof(newReservation.licensePlate) - 1);
    strncpy(newReservation.sailingID, sailingID, sizeof(newReservation.sailingID) - 1);
    newReservation.vehicleLength = tempVehicle.vehicleLength;  // Use entered dimensions
    newReservation.vehicleHeight = tempVehicle.vehicleHeight;  // Use entered dimensions
    strncpy(newReservation.phone, phoneNumber, sizeof(newReservation.phone) - 1);
    newReservation.onboard = false;  // Initial onboard status

    newReservation.expectedReturnDate = {0, 0, 0}; 
    newReservation.reservedLane = assignedLane;  // Calculated lane assignment

    // Attempt to store reservation in persistent storage
    if (!addReservation(newReservation))  // Storage write failed
    {
        cout << "Error: Reservation could not be created.\n";
        return;
    }

    // Step 8: Update sailing capacity after successful reservation creation
    if (assignedLane == Lane::LOW)  // Deduct from low lane capacity
    {
        sailingRecord.LRL -= tempVehicle.vehicleLength;
    } 
    else  // Deduct from high lane capacity
    {
        sailingRecord.HRL -= tempVehicle.vehicleLength;
    }

    sailingRecord.reservationsCount++;  // Increment reservation counter

    // Persist updated sailing data
    updateSailing(sailingRecord);

    cout << "\nReservation Created.\n";
}

//-----------------------------------------------
void createReservation()
{
    char userResponse = '\0';  // stores user's choice for vehicle registration status
    
    while (true)  // Loop goal: obtain valid yes/no response from user
    {
        std::cout << "Have You Made a Reservation with this License Plate [y/n]: ";
        std::cin >> userResponse;
        userResponse = static_cast<char>(std::tolower(userResponse));  // Convert to lowercase for comparison

        if (userResponse == 'y')  // User has registered vehicle
        {
            createReservationForRegisteredVehicle();  // Use existing vehicle data
            return;
        } 
        else if (userResponse == 'n')  // User has unregistered vehicle
        {
            createReservationForUnregisteredVehicle();  // Collect vehicle data
            return;
        } 
        else  // Invalid input provided
        {
            std::cout << "Error: Invalid input\n";
            return;
        }
    }
}

//-----------------------------------------------
void cancelReservation()
{
    char licensePlate[11];          // license plate identifier
    char sailingID[9];              // sailing identifier

    // Step 1: Collect reservation identification information
    cout << "\n[DELETE EXISTING RESERVATION]" << endl;
    cout << "-------------------------------------------------------------------------------" << endl;
    cout << "Enter License Plate (max 10 characters): ";
    cin >> licensePlate;

    // Step 2: Collect sailing information
    cout << "Enter Sailing ID (format: XXX-DD-HH): ";
    cin >> sailingID;

    // Step 3: Generate composite reservation ID for lookup
    char reservationID[20];
    snprintf(reservationID, sizeof(reservationID), "%s%s", licensePlate, sailingID);

    // Retrieve reservation record using composite key
    optional<Reservation> reservationOpt = getReservationByID(reservationID);
    if (!reservationOpt)  // Reservation not found in database
    {
        cout << "Error: Reservation not found\n";
        return;
    }
    Reservation reservationRecord = *reservationOpt;

    // Step 4: Business rule validation - prevent cancellation after check-in
    if (reservationRecord.onboard)  // Vehicle already checked in
    {
        cout << "Error: Customer already checked in\n";
        return;
    }

    // Step 5: Retrieve sailing data for capacity restoration
    optional<Sailing> sailingOpt = getSailingByID(sailingID);
    if (!sailingOpt)  // Sailing data not found
    {
        cout << "Error: Sailing not found\n";
        return;
    }
    Sailing sailingRecord = *sailingOpt;

    // Step 6: Restore sailing capacity by adding back canceled vehicle's space
    if (reservationRecord.reservedLane == Lane::LOW)  // Add back to low lane
    {
        sailingRecord.LRL += reservationRecord.vehicleLength;
    } 
    else  // Add back to high lane
    {
        sailingRecord.HRL += reservationRecord.vehicleLength;
    }

    // Step 7: Remove reservation record from persistent storage
    if (!deleteReservation(reservationRecord.id))  // Deletion operation failed
    {
        cout << "Error: Reservation could not be deleted.\n";
        return;
    }

    // Step 8: Update sailing statistics after successful cancellation
    sailingRecord.reservationsCount--;  // Decrement reservation counter

    // Step 9: Persist updated sailing data with restored capacity
    updateSailing(sailingRecord);

    cout << "\nCancelation Successful\n";
}

//-----------------------------------------------
void checkInReservation()
{
    char licensePlate[11];          // vehicle license plate identifier
    char sailingID[9];              // sailing identifier

    while (true)  // Loop goal: process multiple check-ins until user exits
    {
        // Step 1: Display check-in interface and collect vehicle information
        cout << "\n[CHECK-IN VEHICLE]" << endl;
        cout << "-------------------------------------------------------------------------------" << endl;
        cout << "Enter Vehicle Plate Number (max 10 characters) or [0] to exit: ";
        cin >> licensePlate;

        // Exit condition check
        if (strcmp(licensePlate, "0") == 0)  // User wants to exit check-in process
        {
            break;
        }

        // Step 2: Collect sailing information for reservation lookup
        cout << "Enter Sailing ID (format: XXX-DD-HH): ";
        cin >> sailingID;

        // Step 3: Generate reservation ID for database lookup
        char reservationID[20];
        snprintf(reservationID, sizeof(reservationID), "%s%s", licensePlate, sailingID);

        // Retrieve reservation using composite key
        auto reservationOpt = getReservationByID(reservationID);
        if (!reservationOpt)  // Reservation not found
        {
            cout << "Error: Reservation not found\n";
            continue;  // Continue to next iteration for another vehicle
        }

        Reservation reservationRecord = *reservationOpt;

        // Step 4: Validate check-in eligibility - prevent double check-in
        if (reservationRecord.onboard)  // Vehicle already checked in
        {
            cout << "Error: Customer already checked-in\n";
            continue;  // Continue to next iteration
        }

        // Step 5: Calculate fare using tiered pricing algorithm based on vehicle size
        double calculatedFare = 0;
        if (reservationRecord.vehicleHeight <= 2)  // Standard height vehicle
        {
            calculatedFare = 14.0;  // Fixed rate for normal vehicles
        } 
        else if (reservationRecord.vehicleLength > 7 && reservationRecord.vehicleHeight <= 2)  // Long low vehicle
        {
            calculatedFare = reservationRecord.vehicleLength * 2.0;  // $2 per meter pricing
        } 
        else if (reservationRecord.vehicleLength > 7 && reservationRecord.vehicleHeight > 2)  // Long overheight vehicle
        {
            calculatedFare = reservationRecord.vehicleLength * 3.0;  // $3 per meter premium pricing
        }

        // Step 6: Display calculated fare to user
        cout << "Collect $" << calculatedFare << endl;

        // Step 7: Update reservation status to indicate successful check-in
        if (!setOnboardStatus(reservationRecord.id, true))  // Status update failed
        {
            cout << "Error: Failed to update onboard status\n";
            continue;  // Continue to next iteration
        }

        cout << "\nCheck-in Successful\n";
    }
}