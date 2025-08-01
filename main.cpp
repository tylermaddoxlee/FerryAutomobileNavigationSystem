//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
/*          
    Module: main.cpp
    Revision History: 
    Revision 1.0: 2025/07/07 - Original by Brandon Landa-Ahn
    Purpose: 
        This module is the main module for the ferry reservation system. 
*/

//============================================
#include "Utilities.h"
#include "MenuUI.h"
#include "ReservationASM.h"
#include "ReservationCommandProcessor.h"
#include "SailingASM.h"
#include "SailingCommandProcessor.h"
#include "VehicleASM.h"
#include "VesselASM.h"
#include "VesselCommandProcessor.h"

int main() 
{
    startup();

    runMainMenu();

    shutdown();
    return 0;
}   


/*
Coding Conventions

1. File Header

    Module: <filename>
    Revision History: 
    Revision X.Y: YYYY-MM-DD - Original/Modified by <name>
    Purpose: 
        <small description about the purpose of the module>


2. Section Dividers
    - Before major sections (above sections):
    ===============================================
    - Between each function (above function):
    -----------------------------------------------


3. Function Prototypes
    -----------------------------------------------
    ReturnType functionName(
        ParamType1 param1,  // Description of param1, is it in, out, or in/out?
        ParamType2 param2,  // Description of param2, is it in, out, or in/out?
        ...
    );
    
    // Place the thin divider immediately above the prototype
    // After the prototype, add a short paragraph describing:
    //      - Purpose of the function, preconditions, and side effects
    // Do NOT describe internal algorithm steps here


4. Paramater Annotations
    - One parameter per line
    - Comment to the right: direction (in, out, in/out) and unit if needed


5. Naming Conventions
    - Types & classes: PascalCase (e.g. ExampleModule)
    - Functions & variables: camelCase (e.g. exampleFunction, exampleVariable)
    - Constants & macros: ALL_CAPS (e.g. EXAMPLE_CONSTANT, EXAMPLE_MACRO)


6. Indentation & Spacing
    - One tab per indent level
    - Brace '}' on the same line as keyword
    - Surround binary operators with spaces (e.g. c = a + b)
    - One space after commas

7. Revision History Order
    - List recent revisions first and in order

Example Skeleton Code (ExampleModule.h)
============================================
    Module: ExampleModule.h
    Revision History: 
    Revision 1 - 2025/07/07 Created by Team
    Purpose:
        Declaration of ExampleModule API
    
============================================

#include "ExampleModule.h"

--------------------------------------------
int addNumber(
    int input1,     // in: an integer to process
    int input2,     // in: an integer to process
    int& result      // out: the computed result
)

// Process two input values and compute the sum 
// of both of them. Return the result by reference. 
// No side effects



*/
