//============================================
/*
    Module: Date.h
    Revision History:
    Revision 1.0: 2025-07-18 - Created by Team
    Purpose:
        Definition of a simple Date structure for use in reservations.
*/

//============================================
#ifndef DATE_H
#define DATE_H

struct Date {
    int year;   // in: 4-digit year
    int month;  // in: 1–12
    int day;    // in: 1–31

    // Simple subtraction: returns days late (assumes same year/month)
    int operator-(const Date &other) const {
        return (this->day + (this->month * 30) + (this->year * 365)) -
               (other.day + (other.month * 30) + (other.year * 365));
    }
};

#endif