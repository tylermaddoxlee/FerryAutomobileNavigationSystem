//============================================
/*
    Module: SailingASM.cpp
    Revision History:
    Revision 1.0: 2025-07-18 - Original by Brandon Landa-Ahn
    Purpose:
        Implementation of SailingASM file operations including:
        -initialize vehicle storage file
        -add vehicle
        -close vehicle data
        -find vehicle by license plate
*/

//============================================
#include <iostream>
#include <fstream>
#include <filesystem>
#include "SailingASM.h"
#include "Sailing.h"

using namespace std;

static fstream sailingFile;

void initializeSailingStorage()
{
    sailingFile.open("sailings.dat", ios::binary | ios::out | ios::in);

    if (!sailingFile.is_open())
    {
        ofstream createFile("sailings.dat", ios::out | ios::binary | ios::in);
        sailingFile.clear();
        createFile.close();
        sailingFile.open("sailing.dat", ios::binary | ios::in | ios::out);
        if (!sailingFile.is_open()) {
            cerr << "Failed to create sailing file." << endl;
        }
    }
};

void shutdowd

