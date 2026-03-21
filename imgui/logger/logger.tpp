#pragma once
#include <charconv>
#include <type_traits>
#include <sstream>
#include <iomanip>
#include <iostream>
#include <algorithm>

template <class num>
inline std::string numToStr(num n, int precision){
    static_assert(std::is_integral_v<num>, "numToStr requires an integral type");

    char buf[sizeof(num) * 8];
    std::to_chars_result r = std::to_chars(buf, buf + sizeof(num)* 8, n);
    return std::string(buf, r.ptr);
}

template <class num>
inline std::string numToStrBin(num n, int precision){
    static_assert((std::is_integral_v<num>), "numToStrBin requires an integral or floating-point type");
    using unum = std::make_unsigned_t<num>;
    unum v = static_cast<unum>(n);

    std::string result;
    result.reserve(sizeof(num)* 8);
    unum one = unum(1);
    for (int i = sizeof(num)* 8 - 1; i >= 0; --i) {
        result.push_back((v & (one << i)) ? '1' : '0');
    }

    return result;
}

template <class flt>
inline std::string fltToStr(flt n, int precision) {
    static_assert((std::is_floating_point_v<flt>), "fltToStr requires a floating-point type");

    std::ostringstream oss;
    oss.setf(std::ios::fixed, std::ios::floatfield);
    oss << std::setprecision(precision) << n;
    return oss.str();
}

template <class flt>
inline std::string fltToStrSci(flt n, int precision) {
    static_assert((std::is_floating_point_v<flt>), "fltToStrSci requires a floating-point type");

    std::ostringstream oss;
    oss.setf(std::ios::scientific, std::ios::floatfield);
    oss << std::setprecision(precision) << n;
    return oss.str();
}

inline std::string simpleStr(const std::string& s, int precision = -1){
    return s;
}

template <class typ>
inline std::string ptrToStr(typ* p, int size, std::string (*logger)(typ, int), const std::string& seperator, int precision){

    constexpr bool is_flt = std::is_floating_point_v<typ>;

    if (logger == nullptr){
        if constexpr (std::is_floating_point_v<typ>) 
            logger = fltToStr<typ>;
        else if constexpr (std::is_integral_v<typ>) 
            logger = numToStr<typ>;
        else if constexpr (std::is_same_v<std::decay_t<typ>, std::string>) 
            logger = simpleStr;
        else{
            std::string err = "ptrToStr: No default logger for this type. Please provide a custom logger function.";
            log( Error, err, "\n");
            std::cerr << err << std::endl;
            return err;
        }
    }



    std::string result;
    result.reserve(sizeof(typ) * size + seperator.length() * size); 

    for(int i = 0; i < size; ++i)
        result += logger(p[i], precision) + seperator;
    

    return result;
}

inline void push_to_log_lines(std::pair<std::string, LogLevel>&& value)
{
    using std::max;

    max_log_lines = max(1, max_log_lines);
    if (log_lines.size() >= max_log_lines) {
        logToCout(Info, "resizing lines", "\n");
        log_lines.erase(log_lines.begin(), log_lines.begin() + (log_lines.size() - max_log_lines));   // drop oldest
    }
    log_lines.push_back(std::move(value));
}

inline void log(const LogLevel lvl, const std::string& item, const std::string& seperator){
    push_to_log_lines(std::make_pair(item + seperator, lvl));
}
inline void logToCout(const LogLevel lvl, const std::string& item, const std::string& seperator){
    std::cout << log_level_names[lvl] << " ";
    if(seperator == "\\endl"){
        std::cout << item << std::endl;
        return;
    }
    std::cout << item << seperator;
}
inline void logToFile(const LogLevel lvl, const std::string& item, const std::string& seperator){
    if (!log_file.is_open()){
        log(Error, "logToFile: log file is not open!");
        logToCout(Error, "logToFile: log file is not open!");
        return;
    }
    log_file << log_level_names[lvl] << " ";
    if(seperator == "\\endl"){
        log_file << item << std::endl;
        return;
    }

    log_file << item << seperator;
    
}






inline std::string makeClipboardText(){
    using std::min;
    std::string text = "";
    int n = min(copy_to_clipbrd_count, static_cast<int>(log_lines.size()));


    for(int i = log_lines.size() - n; i < log_lines.size(); ++i){

        text += log_level_names[log_lines[i].second] + " " + log_lines[i].first;
    }
    return text;

}


inline void layoutButtons(){
    if (ImGui::Button("Clear")) {
        log_lines.clear();
        lastIdx = 0;
    }
    ImGui::SameLine();
    if (ImGui::Button("Copy Last")) {
        ImGui::SetClipboardText(makeClipboardText().c_str());
    }
    ImGui::SameLine();
    ImGui::SetNextItemWidth(120);
    ImGui::InputInt("lines", &copy_to_clipbrd_count);
    ImGui::SameLine();

    ImGui::SameLine();
    if (ImGui::Button("Scroll to Bottom")) {
        ImGui::SetScrollHereY(1.0f);
    }
    ImGui::SameLine();
    if (ImGui::Button(log_to_cout ? "Log to Cout: ON" : "Log to Cout: OFF")) log_to_cout = !log_to_cout;
    ImGui::SameLine();
    if (ImGui::Button("Test Log")) log(Info, "Test Log Message");

    if (ImGui::Button(log_level_enabled[0] ? "Debug: ON" : "Debug: OFF"))log_level_enabled[0] = !log_level_enabled[0];

    ImGui::SameLine();
    if (ImGui::Button(log_level_enabled[1] ? "Info: ON" : "Info: OFF"))log_level_enabled[1] = !log_level_enabled[1];

    ImGui::SameLine();
    if (ImGui::Button(log_level_enabled[2] ? "Warn: ON" : "Warn: OFF"))log_level_enabled[2] = !log_level_enabled[2];

    ImGui::SameLine();
    if (ImGui::Button(log_level_enabled[3] ? "Error: ON" : "Error: OFF"))log_level_enabled[3] = !log_level_enabled[3];

    ImGui::SameLine();
    if (ImGui::Button(log_level_enabled[4] ? "Critical: ON" : "Critical: OFF"))log_level_enabled[4] = !log_level_enabled[4];
    ImGui::SameLine();
    ImGui::SetNextItemWidth(120);
    ImGui::InputInt("Show Lines", &max_log_lines);

    if (ImGui::Button(log_to_file ? "Log to File: ON" : "Log to File: OFF")){
        log_to_file = !log_to_file;
        if( log_to_file ){
            reopen_file = true;
        } else {
            log_file.close();
        }
    }
    ImGui::SameLine();
    ImGui::InputText("Log File Path", log_file_path, sizeof(log_file_path));
    
    
}

inline void handleLogFile(){
    if(!log_file.is_open() || reopen_file){
        log_file.close();
        log_file.clear();
        log_file.open(log_file_path,  std::ios::app);
        reopen_file = false;
    }
    if(!log_file.is_open()){
        logToCout(Error, "handleLogFile: Could not open log file at path: " + std::string(log_file_path));
    }
}

inline void DrawLogWindow(){
    

    if (ImGui::IsKeyPressed(ImGuiKey_L) && (ImGui::GetIO().KeyCtrl)) {
        show_log_window = !show_log_window;
    }


    if (!show_log_window) {
        log_file.close();
        return;
    }
    
    if(log_to_file) handleLogFile();
    
    ImGui::SetNextWindowSize(ImVec2(600, 300), ImGuiCond_Once);
    ImGui::SetNextWindowPos(ImVec2(100, 100), ImGuiCond_Once);
    ImGui::Begin("Log", &show_log_window); 
    layoutButtons();


    ImGui::Separator();

    ImGui::BeginChild("LogScroll", ImVec2(0, 0), false,
                      ImGuiWindowFlags_HorizontalScrollbar);

    int i = 0;
    for (const std::pair<std::string, LogLevel>& line : log_lines) {
        if (log_level_enabled[line.second]){
            ImGui::TextColored(log_level_colors[line.second], "%s", line.first.c_str());
            if(log_to_cout && i > lastIdx ) {
                logToCout(line.second, line.first, "");
            }
            if(log_to_file && i > lastIdx) {
                logToFile(line.second, line.first, "");
            }
        }
        ++i;
    }
    if(log_to_file) log_file.flush();
    if(log_to_cout) std::cout.flush();

    lastIdx = static_cast<int>(log_lines.size())-1;
    // Auto-scroll to bottom
    if (ImGui::GetScrollY() >= ImGui::GetScrollMaxY())
        ImGui::SetScrollHereY(1.0f);
    ImGui::EndChild();

    ImGui::End();
}