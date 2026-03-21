#pragma once
#include <string>
#include <vector>
#include <array>
#include "fstream"
#include <chrono>


const enum LogLevel : int {Debug, Info, Warn, Error, Critical };
inline bool log_to_file = true;
inline bool log_to_cout = false;
inline bool show_log_window = true;

inline char log_file_path[300] = "C:\\Users\\vsesl\\OneDrive\\Documents\\log.txt\0";
inline int copy_to_clipbrd_count = 100;
inline int max_log_lines = 9999;
inline std::ofstream log_file;
inline bool reopen_file = false;
inline std::vector<std::pair<std::string, LogLevel>> log_lines;
inline int lastIdx = 0;
inline std::array<bool, 5> log_level_enabled = { true, true, true, true, true };

const inline std::array<std::string, 5> log_level_names = { "[DEBUG]", "[INFO]", "[WARN]", "[ERROR]", "[CRITICAL]" };
const inline std::array<ImVec4, 5> log_level_colors = { ImVec4(1.0f, 1.0f, 1.0f, 1.0f), 
                                                  ImVec4(0.5f, 0.5f, 0.5f, 1.0f), 
                                                  ImVec4(1.0f, 1.0f, 0.0f, 1.0f), 
                                                  ImVec4(1.0f, 0.3f, 0.3f, 1.0f), 
                                                  ImVec4(0.7f, 0.0f, 0.0f, 1.0f) };




void DrawLogWindow();

template <class num>
inline std::string numToStr(num n, int precision = -1);

template <class num>
inline std::string numToStrBin(num n, int precision = -1);


template <class flt>
inline std::string fltToStrSci(flt n, int precision = 4);

template <class flt>
inline std::string fltToStr(flt n, int precision = 4);

template <class typ>
inline std::string ptrToStr(typ* p, int size = 1, std::string (*logger)(typ, int) = nullptr, const std::string& seperator = ", ", int precision = 4);


inline void log(const LogLevel lvl, const std::string& item, const std::string& seperator = "\n");
inline void logToCout(const LogLevel lvl, const std::string& item, const std::string& seperator = "\\endl");
inline void logToFile(const LogLevel lvl, const std::string& item, const std::string& seperator = "\\endl");

inline void push_to_log_lines(std::pair<std::string, LogLevel>&& value);
inline std::string makeClipboardText();
inline void layoutButtons();
inline void handleLogFile();

#include "logger.tpp"

