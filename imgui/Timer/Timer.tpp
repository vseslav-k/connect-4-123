#pragma once

inline long long int Timer::microPassed(const time_point& start, const time_point& end){
    return std::chrono::duration_cast<std::chrono::microseconds>(end-start).count();
}
inline double Timer::microPassedPrcs(const time_point& start, const time_point& end){
    return std::chrono::duration_cast<std::chrono::duration<double, std::micro>>(end-start).count();
}
inline double Timer::milliPassed(const time_point& start, const time_point& end){
    return std::chrono::duration_cast<std::chrono::duration<double, std::milli>>(end-start).count();
}

inline long long int Timer::nanoPassed(const time_point& start, const time_point& end){
    return std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();
}


inline long long int Timer::microPassed(const std::string& start, const std::string& end){
 
    return std::chrono::duration_cast<std::chrono::microseconds>(time_records[end]-time_records[start]).count();
}

inline double Timer::microPassedPrcs(const std::string& start, const std::string& end){
    return std::chrono::duration_cast<std::chrono::duration<double, std::micro>>(time_records[end]-time_records[start]).count();
}

inline double Timer::milliPassed(const std::string& start, const std::string& end){
    return std::chrono::duration_cast<std::chrono::duration<double, std::milli>>(time_records[end]-time_records[start]).count();
}



inline long long int Timer::nanoPassed(const std::string& start, const std::string& end){
    return std::chrono::duration_cast<std::chrono::nanoseconds>(time_records[end]-time_records[start]).count();
}

inline void Timer::setPt(const std::string& name){
    time_records[name] =  std::chrono::steady_clock::now();
}
inline void Timer::clearPts(){
    time_records.clear();
}

inline int Timer::deletePt(const std::string& name){
    if(!time_records.count(name)) return 0;
    time_records.erase(name);
    return 1;
}
inline int Timer::deletePts(const time_point& start, const time_point& end){
    int count = 0;
    for(auto itr  = time_records.begin(); itr != time_records.end();){
        if (itr->second > start && itr->second < end){
            itr = time_records.erase(itr);
            ++count;
        }
        else ++itr;
    }
    return count;
}

inline bool isPrefix(const std::string &s1, const std::string &s2){
    using std::min;
    for(int i = 0; i < min(s1.length(), s2.length()); ++i)
        if(s1[i]!= s2[i]) return false;

    return true;
}

inline int Timer::deletePts(const std::string& prefix){
    int count = 0;
    for(auto itr  = time_records.begin(); itr != time_records.end();){
        if (isPrefix(itr->first, prefix)){
            itr = time_records.erase(itr);
            ++count;
        }
        else ++itr;
    }
    return count;
}
inline bool Timer::ptExists(const std::string& name){
    return time_records.count(name);
}
inline const time_point& Timer::pt(const std::string & name){
    if(time_records.count(name)) return time_records[name];
    throw std::runtime_error("Timer::getTimePoint: Time point with name '" + name + "' does not exist!");
    return std::chrono::steady_clock::now();
}