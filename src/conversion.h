#pragma once
// =============================================================================
// conversion.h  —  Saudi Umm al-Qura Hijri/Gregorian conversion engine  (Qt6)
// =============================================================================

#ifndef CONVERSION_H
#define CONVERSION_H

#include <QString>
#include <cstdint>

constexpr int UMM_MIN_YEAR   = 1356;
constexpr int UMM_MAX_YEAR   = 1500;
constexpr int UMM_NUM_YEARS  = UMM_MAX_YEAR - UMM_MIN_YEAR + 1;
constexpr int UMM_NUM_MONTHS = UMM_NUM_YEARS * 12;
constexpr int UMM_TABLE_SIZE = UMM_NUM_MONTHS + 1;

struct HijriDate {
    int  year;
    int  month;
    int  day;
    bool valid;
};

struct GregorianDate {
    int  year;
    int  month;
    int  day;
    bool valid;
};

void umm_init();

GregorianDate  HijriToGregorian(int hy, int hm, int hd);
HijriDate      GregorianToHijri(int gy, int gm, int gd);

bool UmmYearToGregorianRange (int hy,          GregorianDate& start, GregorianDate& end);
bool UmmMonthToGregorianRange(int hy, int hm,  GregorianDate& start, GregorianDate& end);

bool IsInsideUmmRange    (int hijriYear);
bool IsValidHijriDate    (int hy, int hm, int hd);
bool IsValidGregorianDate(int gy, int gm, int gd);
int  HijriMonthLength    (int hy, int hm);

QString HijriMonthName    (int month);
QString GregorianMonthName(int month);

int umm_run_tests();

#endif // CONVERSION_H
