// =============================================================================
// conversion.cpp  —  Saudi Umm al-Qura conversion engine  (Qt6)
// =============================================================================

#include "conversion.h"
#include <algorithm>
#include <cstring>
#include <cassert>

static int greg_to_jdn(int y, int m, int d)
{
    if (m <= 2) { y -= 1; m += 12; }
    int A = y / 100;
    int B = 2 - A + A / 4;
    return (int)(365.25 * (y + 4716))
         + (int)(30.6001 * (m + 1))
         + d + B - 1524;
}

static void jdn_to_greg(int jdn, int& y, int& m, int& d)
{
    int Z = jdn;
    int A = (int)((Z - 1867216.25) / 36524.25);
    A = Z + 1 + A - A / 4;
    int B = A + 1524;
    int C = (int)((B - 122.1) / 365.25);
    int D = (int)(365.25 * C);
    int E = (int)((B - D) / 30.6001);
    d = B - D - (int)(30.6001 * E);
    m = (E < 14) ? E - 1 : E - 13;
    y = (m > 2)  ? C - 4716 : C - 4715;
}

static int  g_monthStart[UMM_TABLE_SIZE];
static bool g_initialized = false;

static inline int flat_idx(int hy, int hm) {
    return (hy - UMM_MIN_YEAR) * 12 + (hm - 1);
}

static int tabular_year_start(int hy)
{
    return 1948439 + (hy - 1) * 354 + (11 * hy + 3) / 30;
}

static const int kMonthCum[12] = {
    0, 30, 59, 89, 118, 148, 177, 207, 236, 266, 295, 325,
};

struct YearJDN { int year; int jdn; };

static const YearJDN kYearOverrides[] = {
    { 1400, 2444198 },
    { 1440, 2458373 },
    { 1441, 2458727 },
    { 1442, 2459082 },
    { 1443, 2459436 },
    { 1444, 2459791 },
    { 1445, 2460145 },
    { 1446, 2460499 },
    { 1447, 2460853 },
};

static int find_year_override(int hy)
{
    for (const auto& o : kYearOverrides)
        if (o.year == hy) return o.jdn;
    return -1;
}

struct MonthCorr { int year; int month; int delta; };

static const MonthCorr kMonthCorrections[] = {
    { 1440,  8, +1 },
    { 1441,  8, +1 },
    { 1446,  8, +1 },
    { 1447,  2, +1 },
    { 1447,  8, +1 },
};

void umm_init()
{
    if (g_initialized) return;
    g_initialized = true;

    static int yearStart[UMM_NUM_YEARS + 1];
    for (int i = 0; i <= UMM_NUM_YEARS; i++) {
        int hy = UMM_MIN_YEAR + i;
        int ov = find_year_override(hy);
        yearStart[i] = (ov >= 0) ? ov : tabular_year_start(hy);
    }

    for (int i = 0; i < UMM_NUM_YEARS; i++) {
        int base    = yearStart[i];
        int startM  = i * 12;
        for (int m = 0; m < 11; m++) {
            g_monthStart[startM + m] = base + kMonthCum[m];
        }
        g_monthStart[startM + 11] = base + kMonthCum[11];
    }
    g_monthStart[UMM_NUM_MONTHS] = yearStart[UMM_NUM_YEARS];

    for (const auto& c : kMonthCorrections) {
        if (c.year < UMM_MIN_YEAR || c.year > UMM_MAX_YEAR) continue;
        int base_idx = flat_idx(c.year, c.month);
        int end_idx  = flat_idx(c.year, 12) + 1;
        for (int idx = base_idx; idx <= end_idx && idx < UMM_TABLE_SIZE; idx++) {
            g_monthStart[idx] += c.delta;
        }
        if (c.year < UMM_MAX_YEAR) {
            int restore_idx = flat_idx(c.year + 1, 1);
            if (restore_idx < UMM_TABLE_SIZE) {
                int ov = find_year_override(c.year + 1);
                g_monthStart[restore_idx] = (ov >= 0) ? ov : tabular_year_start(c.year + 1);
            }
        } else {
            g_monthStart[UMM_NUM_MONTHS] = yearStart[UMM_NUM_YEARS];
        }
    }
}

GregorianDate HijriToGregorian(int hy, int hm, int hd)
{
    GregorianDate result{0, 0, 0, false};
    if (!g_initialized) umm_init();
    if (!IsValidHijriDate(hy, hm, hd)) return result;
    int idx = flat_idx(hy, hm);
    int jdn = g_monthStart[idx] + hd - 1;
    jdn_to_greg(jdn, result.year, result.month, result.day);
    result.valid = true;
    return result;
}

HijriDate GregorianToHijri(int gy, int gm, int gd)
{
    HijriDate result{0, 0, 0, false};
    if (!g_initialized) umm_init();
    if (!IsValidGregorianDate(gy, gm, gd)) return result;
    int jdn = greg_to_jdn(gy, gm, gd);
    if (jdn < g_monthStart[0] || jdn >= g_monthStart[UMM_NUM_MONTHS]) {
        result.valid = false;
        return result;
    }
    const int* begin = g_monthStart;
    const int* end   = g_monthStart + UMM_TABLE_SIZE;
    const int* it    = std::upper_bound(begin, end, jdn);
    --it;
    int idx = (int)(it - begin);
    result.year  = UMM_MIN_YEAR + idx / 12;
    result.month = idx % 12 + 1;
    result.day   = jdn - *it + 1;
    result.valid = true;
    return result;
}

bool UmmYearToGregorianRange(int hy, GregorianDate& start, GregorianDate& end)
{
    if (!g_initialized) umm_init();
    if (hy < UMM_MIN_YEAR || hy > UMM_MAX_YEAR) return false;
    int jdn_s = g_monthStart[flat_idx(hy,  1)];
    int jdn_e = g_monthStart[flat_idx(hy, 12) + 1] - 1;
    jdn_to_greg(jdn_s, start.year, start.month, start.day); start.valid = true;
    jdn_to_greg(jdn_e, end.year,   end.month,   end.day);   end.valid   = true;
    return true;
}

bool UmmMonthToGregorianRange(int hy, int hm, GregorianDate& start, GregorianDate& end)
{
    if (!g_initialized) umm_init();
    if (hy < UMM_MIN_YEAR || hy > UMM_MAX_YEAR) return false;
    if (hm < 1 || hm > 12) return false;
    int idx   = flat_idx(hy, hm);
    int jdn_s = g_monthStart[idx];
    int jdn_e = g_monthStart[idx + 1] - 1;
    jdn_to_greg(jdn_s, start.year, start.month, start.day); start.valid = true;
    jdn_to_greg(jdn_e, end.year,   end.month,   end.day);   end.valid   = true;
    return true;
}

bool IsInsideUmmRange(int hijriYear)
{
    return hijriYear >= UMM_MIN_YEAR && hijriYear <= UMM_MAX_YEAR;
}

int HijriMonthLength(int hy, int hm)
{
    if (!g_initialized) umm_init();
    if (hy < UMM_MIN_YEAR || hy > UMM_MAX_YEAR) return -1;
    if (hm < 1 || hm > 12) return -1;
    int idx = flat_idx(hy, hm);
    return g_monthStart[idx + 1] - g_monthStart[idx];
}

bool IsValidHijriDate(int hy, int hm, int hd)
{
    if (!IsInsideUmmRange(hy))        return false;
    if (hm < 1 || hm > 12)           return false;
    int mlen = HijriMonthLength(hy, hm);
    if (mlen < 0)                     return false;
    return hd >= 1 && hd <= mlen;
}

static bool is_greg_leap(int y)
{
    return (y % 4 == 0 && y % 100 != 0) || (y % 400 == 0);
}
static const int kDaysInMonth[12] = {31,28,31,30,31,30,31,31,30,31,30,31};

bool IsValidGregorianDate(int gy, int gm, int gd)
{
    if (gm < 1 || gm > 12) return false;
    if (gd < 1) return false;
    int maxDay = kDaysInMonth[gm - 1];
    if (gm == 2 && is_greg_leap(gy)) maxDay = 29;
    return gd <= maxDay;
}

static const char* kHijriMonths[13] = {
    "",
    "Muharram",
    "Safar",
    "Rabi' al-Awwal",
    "Rabi' al-Thani",
    "Jumada al-Awwal",
    "Jumada al-Thani",
    "Rajab",
    "Sha'ban",
    "Ramadan",
    "Shawwal",
    "Dhu al-Qi'dah",
    "Dhu al-Hijjah",
};

static const char* kGregMonths[13] = {
    "",
    "January", "February", "March",     "April",
    "May",     "June",     "July",      "August",
    "September", "October", "November", "December",
};

QString HijriMonthName(int m)
{
    if (m < 1 || m > 12) return QString();
    return QString::fromUtf8(kHijriMonths[m]);
}

QString GregorianMonthName(int m)
{
    if (m < 1 || m > 12) return QString();
    return QString::fromUtf8(kGregMonths[m]);
}

struct TestCase {
    int hy, hm, hd;
    int gy, gm, gd;
    const char* note;
};

static const TestCase kTests[] = {
    { 1400,  1,  1, 1979, 11, 20, "1 Muharram 1400" },
    { 1440,  1,  1, 2018,  9, 11, "1 Muharram 1440" },
    { 1441,  1,  1, 2019,  8, 31, "1 Muharram 1441" },
    { 1442,  1,  1, 2020,  8, 20, "1 Muharram 1442" },
    { 1443,  1,  1, 2021,  8,  9, "1 Muharram 1443" },
    { 1444,  1,  1, 2022,  7, 30, "1 Muharram 1444" },
    { 1445,  1,  1, 2023,  7, 19, "1 Muharram 1445" },
    { 1446,  1,  1, 2024,  7,  7, "1 Muharram 1446" },
    { 1447,  1,  1, 2025,  6, 26, "1 Muharram 1447" },
    { 1440,  9,  1, 2019,  5,  6, "1 Ramadan 1440"  },
    { 1441,  9,  1, 2020,  4, 24, "1 Ramadan 1441"  },
    { 1442,  9,  1, 2021,  4, 13, "1 Ramadan 1442"  },
    { 1443,  9,  1, 2022,  4,  2, "1 Ramadan 1443"  },
    { 1444,  9,  1, 2023,  3, 23, "1 Ramadan 1444"  },
    { 1445,  9,  1, 2024,  3, 11, "1 Ramadan 1445"  },
    { 1446,  9,  1, 2025,  3,  1, "1 Ramadan 1446"  },
    { 1447,  9,  1, 2026,  2, 20, "1 Ramadan 1447"  },
    { 1440,  8, 30, 2019,  5,  5, "30 Sha'ban 1440" },
};

int umm_run_tests()
{
    if (!g_initialized) umm_init();
    int failures = 0;
    for (const auto& t : kTests) {
        GregorianDate g = HijriToGregorian(t.hy, t.hm, t.hd);
        bool ok = g.valid && g.year == t.gy && g.month == t.gm && g.day == t.gd;
        if (!ok) failures++;
        if (ok) {
            HijriDate h = GregorianToHijri(t.gy, t.gm, t.gd);
            bool rt_ok = h.valid && h.year == t.hy && h.month == t.hm && h.day == t.hd;
            if (!rt_ok) failures++;
        }
    }
    return failures;
}
