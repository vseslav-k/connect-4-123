#pragma once
#include <chrono>
#include <vector>
#include <string>
#include <unordered_map>
#include <algorithm>
#include <stdexcept>
typedef std::chrono::steady_clock::time_point time_point;



class Timer{
    
    public:

    std::unordered_map<std::string, time_point> time_records;
    
    static inline long long int microPassed(const time_point& start, const time_point& end);
    inline double microPassedPrcs(const time_point& start, const time_point& end);
    static inline double milliPassed(const time_point& start, const time_point& end);
    static inline long long int nanoPassed(const time_point& start, const time_point& end);

    inline long long int microPassed(const std::string& start, const std::string& end);
    inline double microPassedPrcs(const std::string& start, const std::string& end);
    inline double milliPassed(const std::string& start, const std::string& end);
    inline long long int nanoPassed(const std::string& start, const std::string& end);

    inline void setPt(const std::string& name);
    inline void clearPts();
    inline int deletePt(const std::string& name);
    inline int deletePts(const std::string& prefix);
    inline int deletePts(const time_point& start, const time_point& end);
    inline bool ptExists(const std::string& name);
    inline const time_point& pt(const std::string & name) ;

    inline Timer() = default;
    inline ~Timer() = default;
};

#include "Timer.tpp"