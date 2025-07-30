//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
/*
    Module: testSailingReport.cpp
    Revision History:
    Revision 1.0: 2025-07-24 - Created by Brandon Landa-Ahn
    Purpose:
        Unit tests for displaySailingReport2:
          - test_single_sailing: one record, no pagination prompt
          - test_six_sailings: six records, pagination prompt after five
        g++ test_displaySailingReport.cpp SailingASM.cpp ReservationASM.cpp -o testSailingReport.exe
*/

#include <iostream>
#include <sstream>
#include <vector>
#include <iomanip>
#include <ctime>
#include <cstring>
#include <fstream>

#include "Sailing.h"
#include "SailingASM.h"
#include "ReservationASM.h"
#include "VesselASM.h"
#include "SailingCommandProcessor.h"

using namespace std;

std::tm g_localTime; // global variable for local time

// Add prototype for viewSailingReport
void viewSailingReport();

// Empty stub for runMainMenu
void runMainMenu() {
    // Stub: do nothing
}

void testOneSailing() {
    std::ofstream New("sailings.dat", std::ios::binary | std::ios::trunc);
    New.close();

    vector<Sailing> single = {
        Sailing{"XXX-DD-HH", "CCCCcCCCCc", 1000.0f, 1000.0f}
    };

    // Add sailing to storage
    bool success = addSailing(single[0]); 
    if (!success) {
        cout << "\033[31mError: \033[34mFailed to add sailing for test.\n";
        return;
    }

    // Capture output
    ostringstream out;
    istringstream in("n\n"); // Simulate user input for no pagination
    // Redirect cout and cin to ostringstream and istringstream
    auto* oldCoutBuf = cout.rdbuf(out.rdbuf());
    auto* oldCinBuf = cin.rdbuf(in.rdbuf());

    viewSailingReport();

    // Restore cout and cin
    cout.rdbuf(oldCoutBuf);
    cin.rdbuf(oldCinBuf);

    // Get current date and time
    std::ostringstream dateStream, timeStream;
    dateStream << std::put_time(&g_localTime, "%y-%m-%d");
    timeStream << std::put_time(&g_localTime, "%H:%M");

    string expected =
            "[VIEW SAILING REPORT]\n"
    "-------------------------------------------------------------------------------\n"
    "    Date: " + dateStream.str() + "      |      Time: " + timeStream.str() + "\n"
    "-------------------------------------------------------------------------------\n"
    "#   Vessel Name                Sailing ID      LRL(m)     HRL(m)    TV     CF\n"
    "-------------------------------------------------------------------------------\n"
    "1)  CCCCcCCCCc                 XXX-DD-HH       1000.0     1000.0     0    0.0%\n"
    "-------------------------------------------------------------------------------\n";


    // Check if output matches expected    
    if (out.str() == expected) {
        cout << "\033[32mTest passed: \033[34mSingle \033[32msailing report displayed correctly.\n";
        cout << "Output:\n" << out.str() << endl;
    } else {
        cout << "\033[31mTest failed: \033[34mSingle \033[31msailing report did not match expected output.\n";
        cout << "Expected:\n" << expected;
        cout << "Got:\n" << out.str() << endl;
    }
}

void testZeroSailings() {

    std::ofstream New("sailings.dat", std::ios::binary | std::ios::trunc);
    New.close();

    // Capture output
    ostringstream out;
    istringstream in("y\n"); // Simulate user input for no pagination
    // Replace cout with ostringstream
    auto* oldCoutBuf = cout.rdbuf(out.rdbuf());
    auto* oldCinBuf = cin.rdbuf(in.rdbuf());

    viewSailingReport();

    // Restore cout
    cout.rdbuf(oldCoutBuf);
    cin.rdbuf(oldCinBuf);

    // Get current date and timestd::ostringstream dateStream, timeStream;
    std::ostringstream dateStream, timeStream;
    dateStream << std::put_time(&g_localTime, "%y-%m-%d");
    timeStream << std::put_time(&g_localTime, "%H:%M");

    string expected =
    "Error: No sailing records found\n";

    // Check if output matches expected    
    if (out.str() == expected) {
        cout << "\033[32mTest passed: \033[34mZero \033[32msailings report displayed correctly.\n";
        cout << "Output:\n" << out.str() << endl;
    } else {
        cout << "\033[31mTest failed: \033[34mZero \033[31msailings report did not match expected output.\n";
        cout << "Expected:\n" << expected;
        cout << "Got:\n" << out.str() << endl;  
    }
} 

void testSevenSailings() {
    // Clear sailings file before test
    std::ofstream New("sailings.dat", std::ios::binary | std::ios::trunc);
    New.close();

    // Same data for ease of testing
    vector<Sailing> seven = {
        Sailing{"XXX-DD-HH", "CCCCcCCCCc", 1000.0f, 1000.0f},
        Sailing{"XXX-DD-HH", "CCCCcCCCCc", 1000.0f, 1000.0f},
        Sailing{"XXX-DD-HH", "CCCCcCCCCc", 1000.0f, 1000.0f},
        Sailing{"XXX-DD-HH", "CCCCcCCCCc", 1000.0f, 1000.0f},
        Sailing{"XXX-DD-HH", "CCCCcCCCCc", 1000.0f, 1000.0f},
        Sailing{"XXX-DD-HH", "CCCCcCCCCc", 1000.0f, 1000.0f},
        Sailing{"XXX-DD-HH", "CCCCcCCCCc", 1000.0f, 1000.0f}
    };
    for (const auto& s : seven) addSailing(s);

    ostringstream out;
    istringstream in("y\n"); // Simulate user entering 'y' to load more
    auto* oldCoutBuf = cout.rdbuf(out.rdbuf());
    auto* oldCinBuf = cin.rdbuf(in.rdbuf());

    viewSailingReport();

    cout.rdbuf(oldCoutBuf);
    cin.rdbuf(oldCinBuf);


    // Optionally, you can still compare to expected and print results
    std::ostringstream dateStream, timeStream;
    dateStream << std::put_time(&g_localTime, "%y-%m-%d");
    timeStream << std::put_time(&g_localTime, "%H:%M");

    string expected =
        "[VIEW SAILING REPORT]\n"
        "-------------------------------------------------------------------------------\n"
        "    Date: " + dateStream.str() + "      |      Time: " + timeStream.str() + "\n"
        "-------------------------------------------------------------------------------\n"
        "#   Vessel Name                Sailing ID      LRL(m)     HRL(m)    TV     CF\n"
        "-------------------------------------------------------------------------------\n"
        "1)  CCCCcCCCCc                 XXX-DD-HH       1000.0     1000.0     0    0.0%\n"
        "2)  CCCCcCCCCc                 XXX-DD-HH       1000.0     1000.0     0    0.0%\n"
        "3)  CCCCcCCCCc                 XXX-DD-HH       1000.0     1000.0     0    0.0%\n"
        "4)  CCCCcCCCCc                 XXX-DD-HH       1000.0     1000.0     0    0.0%\n"
        "5)  CCCCcCCCCc                 XXX-DD-HH       1000.0     1000.0     0    0.0%\n"
        "-------------------------------------------------------------------------------\n"
        "Load More? [y/n]: \n"
        "[VIEW SAILING REPORT]\n"
        "-------------------------------------------------------------------------------\n"
        "    Date: " + dateStream.str() + "      |      Time: " + timeStream.str() + "\n"
        "-------------------------------------------------------------------------------\n"
        "#   Vessel Name                Sailing ID      LRL(m)     HRL(m)    TV     CF\n"
        "-------------------------------------------------------------------------------\n"
        "6)  CCCCcCCCCc                 XXX-DD-HH       1000.0     1000.0     0    0.0%\n"
        "7)  CCCCcCCCCc                 XXX-DD-HH       1000.0     1000.0     0    0.0%\n"
        "-------------------------------------------------------------------------------\n";

    if (out.str() == expected) {
        cout << "\033[32mTest passed: \033[34mSeven \033[32msailings report displayed correctly with pagination.\n";
    } else {
        cout << "\033[31mTest failed: \033[34mSeven \033[31msailings report did not match expected output.\n\n";
        cout << "Expected:\n" << expected << "\n";
        cout << "Got:\n" << out.str();
    }
}

int main() {

    initializeSailingStorage();
    initializeReservationStorage();

    std::time_t now = std::time(nullptr);
    g_localTime = *std::localtime(&now);


    testOneSailing();
    testZeroSailings();
    testSevenSailings();

    shutdownReservationStorage();
    shutdownSailingStorage();

}


