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
    char sailingID[11];              // sailing identifier in format XXX-DD-HH
    char licensePlate[11];          // vehicle license plate number

    // Step 1: Collect license plate and validate vehicle registration
    cout << "\033[1;97mEnter License Plate Number (max 10 characters): \033[0m";
    cin >> licensePlate;

    // Perform database lookup to retrieve existing vehicle information
    auto existingVehicleOpt = getVehicleByLicensePlate(licensePlate);
    if (!existingVehicleOpt)  // Vehicle not found in registered database
    {
        cout << "\033[31mError: License plate not found\n\033[0m";
        return;
    }
    vehicleRecord = *existingVehicleOpt;  // Extract vehicle data from optional

    // Step 2: Collect sailing information and validate sailing exists
    cout << "\033[1;97mEnter Sailing ID (format: XXX-DD-HH): \033[0m";
    cin >> sailingID;

    // Step 3: Retrieve and validate sailing data for capacity checking
    auto sailingOpt = getSailingByID(sailingID);
    if (!sailingOpt)  // Invalid sailing ID provided
    {
        cout << "\033[31mError: Sailing ID does not exist\n\033[0m";
        return;
    }
    sailingRecord = *sailingOpt;

    // Step 4: Generate composite reservation ID using concatenation algorithm
    char reservationID[21];
    makeReservationID(licensePlate, sailingID, reservationID);

    // Check for duplicate reservation to prevent double-booking
    auto existingReservationOpt = getReservationByID(reservationID);
    if (existingReservationOpt)  // Reservation already exists for this combination
    {
        cout << "\033[31mError: A reservation already exists for this vehicle on this sailing.\n\033[0m";
        return;
    }


    // Step 5: Implement lane assignment algorithm based on vehicle dimensions
    Lane assignedLane;
    float vehicleWithBuffer = vehicleRecord.vehicleLength + 0.5;

    if (vehicleRecord.vehicleHeight <= 2)  // Vehicle can use either lane
    {
        // Priority algorithm: try low lane first, then high lane
        if (sailingRecord.LRL >= vehicleWithBuffer)  // Low lane has capacity
        {
            assignedLane = Lane::LOW;
        } 
        else if (sailingRecord.HRL >= vehicleWithBuffer)  // High lane has capacity
        {
            assignedLane = Lane::HIGH;
        } 
        else  // No lane has sufficient remaining capacity
        {
            cout << "\033[31mError: No sufficient capacity in either lane\n\033[0m";
            return;
        }
    } 
    else  // Tall vehicle (>2m) must use high lane only
    {
        if (sailingRecord.HRL >= vehicleWithBuffer)  // High lane capacity check
        {
            assignedLane = Lane::HIGH;
        } 
        else  // High lane insufficient for tall vehicle
        {
            cout << "\033[31mError: Not enough capacity in the high lane\n\033[0m";
            return;
        }
    }

    // Step 6: Construct reservation record with collected data
    snprintf(newReservation.id, sizeof(newReservation.id), "%s", reservationID);
    
    // Zero out destination arrays before copying to ensure no garbage data
    memset(newReservation.licensePlate, 0, sizeof(newReservation.licensePlate));
    memset(newReservation.sailingID, 0, sizeof(newReservation.sailingID));
    memset(newReservation.phone, 0, sizeof(newReservation.phone));
    
    strncpy(newReservation.licensePlate, licensePlate, sizeof(newReservation.licensePlate) - 1);
    strncpy(newReservation.sailingID, sailingID, sizeof(newReservation.sailingID) - 1);
    strncpy(newReservation.phone, vehicleRecord.phone, sizeof(newReservation.phone) - 1);
    
    newReservation.vehicleLength = vehicleRecord.vehicleLength;  // Copy from registered vehicle
    newReservation.vehicleHeight = vehicleRecord.vehicleHeight;  // Copy from registered vehicle
    newReservation.onboard = false;  // Initial onboard status

    newReservation.expectedReturnDate = {0, 0, 0}; 
    newReservation.reservedLane = assignedLane;  // Store calculated lane assignment

    // Attempt to persist reservation to storage
    if (!addReservation(newReservation))  // Storage operation failed
    {
        cout << "\033[31mError: Reservation could not be created.\n\033[0m";
        return;
    }

    // Step 7: Update sailing capacity based on successful reservation
    if (assignedLane == Lane::LOW)  // Update low lane remaining capacity
    {
        sailingRecord.LRL -= vehicleWithBuffer;
    } 
    else  // Update high lane remaining capacity
    {
        sailingRecord.HRL -= vehicleWithBuffer;
    }

    sailingRecord.reservationsCount++;  // Increment total reservation counter

    // Persist updated sailing data with new capacity and count
    updateSailing(sailingRecord);

    cout << "\n\033[32mReservation Created\n\033[0m";
}

//-----------------------------------------------
void createReservationForUnregisteredVehicle()
{
    Vehicle tempVehicle;            // temporary vehicle record for unregistered vehicle
    Reservation newReservation;     // reservation to be created
    Sailing sailingRecord;          // sailing data for capacity validation
    char sailingID[11];              // sailing identifier
    char licensePlate[11];          // vehicle license plate
    char phoneNumber[13];           // contact phone number

    // Step 1: Collect and validate sailing information first
    cout << "\033[1;97mEnter Sailing ID (format: XXX-DD-HH): \033[0m";
    cin >> sailingID;

    // Retrieve sailing data to validate sailing exists and check capacity
    auto sailingOpt = getSailingByID(sailingID);
    if (!sailingOpt)  // Sailing ID not found in database
    {
        cout << "\033[31mError: Sailing ID does not exist\n\033[0m";
        return;
    }
    sailingRecord = sailingOpt.value();

    // Step 2: Collect vehicle identification
    cout << "\033[1;97mEnter License Plate Number (max 10 characters): \033[0m";
    cin >> licensePlate;

    // Step 3: Generate reservation ID and check for duplicates
    char reservationID[21];
    makeReservationID(licensePlate, sailingID, reservationID);

    // Prevent double-booking by checking existing reservations
    auto existingReservationOpt = getReservationByID(reservationID);
    if (existingReservationOpt)  // Duplicate reservation found
    {
        cout << "\033[31mError: A reservation already exists for this vehicle on this sailing.\n\033[0m";
        return;
    }

    // Step 4: Collect vehicle dimensions for unregistered vehicle
    cout << "\033[1;97mEnter Vehicle Length (max 99.9m): \033[0m";
    cin >> tempVehicle.vehicleLength;
    cout << "\033[1;97mEnter Vehicle Height (max 9.9m): \033[0m";
    cin >> tempVehicle.vehicleHeight;

    // Add 0.5-meter buffer for vehicle length
    float vehicleWithBuffer = tempVehicle.vehicleLength + 0.5;

    // Step 5: Apply lane assignment algorithm based on collected dimensions
    Lane assignedLane;
    if (tempVehicle.vehicleHeight <= 2)  // Standard height vehicle - can use either lane
    {
        // Priority-based assignment: prefer low lane, fallback to high lane
        if (sailingRecord.LRL >= vehicleWithBuffer)  // Low lane available
        {
            assignedLane = Lane::LOW;
        } 
        else if (sailingRecord.HRL >= vehicleWithBuffer)  // High lane available
        {
            assignedLane = Lane::HIGH;
        } 
        else  // Neither lane has sufficient capacity
        {
            cout << "\033[31mError: No sufficient capacity in either lane\n\033[0m";
            return;
        }
    } 
    else  // Overheight vehicle - high lane only
    {
        if (sailingRecord.HRL >= vehicleWithBuffer)  // High lane capacity check
        {
            assignedLane = Lane::HIGH;
        } 
        else  // High lane insufficient for overheight vehicle
        {
            cout << "\033[31mError: Not enough capacity in the high lane\n\033[0m";
            return;
        }
    }

    // Step 6: Collect contact information for unregistered vehicle
    cout << "\033[1;97mEnter Phone Number (max 14 characters): \033[0m";
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
        cout << "\033[31mError: Reservation could not be created.\n\033[0m";
        return;
    }

    // Step 8: Update sailing capacity after successful reservation creation
    if (assignedLane == Lane::LOW)  // Deduct from low lane capacity
    {
        sailingRecord.LRL -= vehicleWithBuffer;
    } 
    else  // Deduct from high lane capacity
    {
        sailingRecord.HRL -= vehicleWithBuffer;
    }

    sailingRecord.reservationsCount++;  // Increment reservation counter

    // Persist updated sailing data
    updateSailing(sailingRecord);

    // Step 10: Add vehicle to vehicle database for future use
    // Populate tempVehicle with all collected information
    memset(tempVehicle.licensePlate, 0, sizeof(tempVehicle.licensePlate));
    memset(tempVehicle.phone, 0, sizeof(tempVehicle.phone));
    
    strncpy(tempVehicle.licensePlate, licensePlate, sizeof(tempVehicle.licensePlate) - 1);
    strncpy(tempVehicle.phone, phoneNumber, sizeof(tempVehicle.phone) - 1);
    // vehicleLength and vehicleHeight already set above

    // Add vehicle to database (ignore failure - reservation is already created)
    if (!addVehicle(tempVehicle))
    {
        cout << "\033[31mWarning: Vehicle could not be added to database,\033[32m but reservation was created successfully.\n\033[0m";
    }

    cout << "\n\033[32mReservation Created\n\033[0m";
}

//-----------------------------------------------
void createReservation()
{
    char userResponse = '\0';  // stores user's choice for vehicle registration status
    
    while (true)  // Loop goal: obtain valid yes/no response from user
    {
        cout << "\n\033[94m[\033[1;96mCREATE NEW RESERVATION\033[94m]\033[0m" << endl;
        cout << "\033[94m-------------------------------------------------------------------------------" << endl;
        std::cout << "\033[1;97mHave You Made a Reservation with this License Plate [y/n]: \033[0m";
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
            std::cout << "\033[31mError: Invalid input\n\033[0m";
            return;
        }
    }
}

//-----------------------------------------------
void cancelReservation()
{
    char licensePlate[11];          // license plate identifier
    char sailingID[11];             // sailing identifier (increased size)

    // Step 1: Collect reservation identification information
    cout << "\n\033[94m[\033[1;96mDELETE EXISTING RESERVATION\033[94m]\033[0m" << endl;
    cout << "\033[94m-------------------------------------------------------------------------------" << endl;
    
    // Clear input buffer
    cin.clear();
    cin.ignore(numeric_limits<streamsize>::max(), '\n');

    cout << "\033[1;97mEnter License Plate (max 10 characters): \033[0m";
    cin >> licensePlate;

    // Step 2: Collect sailing information
    cout << "\033[1;97mEnter Sailing ID (format: XXX-DD-HH): \033[0m";
    cin >> sailingID;

    // Step 3: Generate composite reservation ID for lookup
    char reservationID[21];
    makeReservationID(licensePlate, sailingID, reservationID);

    // Retrieve reservation record using composite key
    optional<Reservation> reservationOpt = getReservationByID(reservationID);
    if (!reservationOpt)  // Reservation not found in database
    {
        cout << "\033[31mError: Reservation not found\n\033[0m";
        return;
    }
    Reservation reservationRecord = *reservationOpt;

    // Step 4: Business rule validation - prevent cancellation after check-in
    if (reservationRecord.onboard)  // Vehicle already checked in
    {
        cout << "\033[31mError: Customer already checked in\n\033[0m";
        return;
    }

    // Step 5: Retrieve sailing data for capacity restoration
    optional<Sailing> sailingOpt = getSailingByID(sailingID);
    if (!sailingOpt)  // Sailing data not found
    {
        cout << "\033[31mError: Sailing not found\n\033[0m";
        return;
    }
    Sailing sailingRecord = *sailingOpt;

    // Step 6: Restore sailing capacity by adding back canceled vehicle's space
    const float buffer = 0.5f;  // 0.5 meter buffer
        
        if (reservationRecord.reservedLane == Lane::LOW)  // Add back to low lane
        {
            sailingRecord.LRL += (reservationRecord.vehicleLength + buffer);  // Add the vehicle length + buffer
        } 
        else  // Add back to high lane
        {
            sailingRecord.HRL += (reservationRecord.vehicleLength + buffer);  // Add the vehicle length + buffer
        }

    // Step 7: Remove reservation record from persistent storage
    if (!deleteReservation(reservationRecord.licensePlate))  // Deletion operation failed
    {
        cout << "\033[31mError: Reservation could not be deleted.\n\033[0m";
        return;
    }

    // Step 8: Update sailing statistics after successful cancellation
    sailingRecord.reservationsCount--;  // Decrement reservation counter

    // Step 9: Persist updated sailing data with restored capacity
    updateSailing(sailingRecord);

    cout << "\n\033[32mCancelation Successful\033[0m\n";
}

//-----------------------------------------------
void checkInReservation()
{
    char licensePlate[11];          // vehicle license plate identifier
    char sailingID[11];              // sailing identifier

    while (true)  // Loop goal: process multiple check-ins until user exits
    {
        // Step 1: Display check-in interface and collect vehicle information
        cout << "\n\033[94m[\033[1;96mCHECK-IN VEHICLE\033[94m]" << endl;
        cout << "\033[94m-------------------------------------------------------------------------------\033[0m" << endl;

        // Clear input buffer (same as cancelReservation)
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        
        cout << "\033[1;97mEnter Vehicle Plate Number (max 10 characters) or [0] to exit: \033[0m";
        cin >> licensePlate;

        // Exit condition check
        if (strcmp(licensePlate, "0") == 0)  // User wants to exit check-in process
        {
            break;
        }

        // Step 2: Collect sailing information for reservation lookup
        cout << "\033[1;97mEnter Sailing ID (format: XXX-DD-HH): \033[0m";
        cin >> sailingID;

        // Step 3: Generate reservation ID for database lookup
        char reservationID[21];
        makeReservationID(licensePlate, sailingID, reservationID);

        // Retrieve reservation using composite key (same method as cancelReservation)
        optional<Reservation> reservationOpt = getReservationByID(reservationID);
        if (!reservationOpt)  // Reservation not found
        {
            cout << "\033[31mError: Reservation not found\n\033[0m";
            continue;  // Continue to next iteration for another vehicle
        }

        Reservation reservationRecord = *reservationOpt;

        // Step 4: Validate check-in eligibility - prevent double check-in
        if (reservationRecord.onboard)  // Vehicle already checked in
        {
            cout << "\033[31mError: Customer already checked-in\n\033[0m";
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
        cout << "\033[32mCollect $" << calculatedFare << endl;

        // Step 7: Update reservation status to indicate successful check-in
        if (!setOnboardStatus(reservationID, true))  // Status update failed
        {
            cout << "\033[31mError: Failed to update onboard status\n\033[0m";
            continue;  // Continue to next iteration
        }

        cout << "\n\033[32mCheck-in Successful\033[0m\n";
    }
}