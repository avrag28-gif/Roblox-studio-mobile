#pragma once
#include <string>
#include <vector>
#include <cstdint>
namespace rsm {
enum class LogLevel{Info,Warning,Error,Print};
struct LogEntry{LogLevel level;std::string message;uint64_t timestamp=0;};
class OutputConsole {
 std::vector<LogEntry>entries_;size_t max_=2000;
public:
 void Push(LogLevel l,std::string m,uint64_t t=0){if(entries_.size()>=max_)entries_.erase(entries_.begin());entries_.push_back({l,std::move(m),t});}
 void Clear(){entries_.clear();}const auto&Entries()const{return entries_;}void SetMax(size_t n){max_=n;}
};
}