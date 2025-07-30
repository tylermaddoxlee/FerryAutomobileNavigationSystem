#include <iostream>
#include <vector>
#include <string>
#include <chrono>
#include <ctime>
#include <iomanip>
#include "ReservationASM.h"
#include "Sailing.h"
#include "SailingASM.h"
using namespace std;



void displaySailingReport(
    const std::vector<Sailing> &results  // in: list of sailings to display
)
{
    int sailingCount = 1;
    int total = results.size();
    int pageStart = 0;

    time_t now = time(nullptr);
    tm* localTime = localtime(&now);


    while (total > pageStart) {
        cout << "[VIEW SAILING REPORT]" << endl;
        cout << string(79, '-') << endl;
        cout << "    Date: " << put_time(localTime, "%y-%m-%d") << "      |      Time: " << put_time(localTime, "%H:%M") << endl;
        cout << string(79, '-') << endl;

        cout << left 
             << setw(4)  << "#"
             << setw(27) << "Vessel Name"
             << setw(12) << "Sailing ID"
             << right
             << setw(10) << "LRL(m)"
             << setw(11) << "HRL(m)"
             << setw(6)  << "TV"
             << setw(7)  << "CF"
             << endl
             << string(79, '-') << endl;

        for (int i = 0; i < 5 && pageStart < total; i++, pageStart++) {
            const Sailing &s = results[pageStart];
            int TV = countReservationsBySailing(s.id);
            float CF = 100; // percentage of how many cars are left or how many meters are left
            cout << setw(1) << (sailingCount) << ")  "
                      << left  << setw(27) << s.vesselName
                      << setw(12)   << s.id
                      << fixed  << right
                      << setw(10)   << setprecision(1) << s.LRL
                      << setw(11)   << setprecision(1) << s.HRL
                      << setw(6)    << TV
                      << setw(7)    << setprecision(1) << CF << "%"
                      << "\n";
                      sailingCount++;
        }

        cout << string(79, '-') << "\n";

        if (pageStart < total) {
            cout << "Load More? [y/n]: ";
            char answer;
            cin >> answer;
            if (answer != 'y' && answer != 'Y') break;

            cout << endl;
        }
    }
}

int main() {
// --- prepare some test data ---
std::vector<Sailing> testData = {
        Sailing{"VIC-15-08", "Coastal Voyager", 1800.5f, 1000.0f},
        Sailing{"TSN-15-11", "Pacific Riser",   1500.5f,  800.5f},
        Sailing{"NAN-15-15", "Ocean Trail",     2000.0f, 1200.5f},
        Sailing{"SEA-15-20", "Sea Explorer",    1700.0f,  900.0f},
        Sailing{"HOP-15-25", "Island Hopper",   1600.5f, 1100.0f},
        Sailing{"WAV-15-30", "Wave Rider",      1900.2f, 1050.3f},
        Sailing{"WND-15-35", "Wind Chaser",     1750.0f,  950.0f},
        Sailing{"BLU-15-40", "Blue Horizon",    1850.3f,  970.0f},
        Sailing{"SUN-15-45", "Sun Dancer",      1725.0f,  890.5f},
        Sailing{"LUN-15-50", "Lunar Current",   1605.4f,  830.3f},
        Sailing{"TID-15-55", "Tidal Runner",    1980.0f, 1020.0f},
        Sailing{"COR-15-60", "Coral Spirit",    1675.0f,  920.0f},
        Sailing{"GAL-15-65", "Gale Breaker",    1780.0f,  940.0f},
        Sailing{"NEB-15-70", "Nebula Wind",     1830.5f, 1010.0f},
        Sailing{"STR-15-75", "Star Voyager",    1660.2f,  850.5f},
        Sailing{"NRT-15-80", "North Tracker",   1760.0f,  910.2f},
        Sailing{"ECL-15-85", "Eclipse Runner",  1545.0f,  790.0f},
        Sailing{"SOL-15-90", "Solar Chaser",    1620.0f,  845.0f},
        Sailing{"FOG-15-95", "Fog Breaker",     1740.0f,  905.0f},
        Sailing{"ARC-16-00", "Arctic Skimmer",  1590.5f,  810.5f},
        Sailing{"WLD-16-05", "Wild Horizon",    1815.0f,  990.0f}
    };

    // --- call the report function ---
    displaySailingReport(testData);

    std::cout << "\nReport complete. Exiting.\n";
    return 0;
}
