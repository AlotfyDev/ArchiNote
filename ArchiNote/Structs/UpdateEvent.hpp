
#ifndef __UPDATEEVENT_HPP__
#define __UPDATEEVENT_HPP__


#include <string>
#include <ctime>
#include <cstdint>  // Required for std::int64_t




// Event structs (simple payloads)
struct UpdateEvent {
    std::string attrId;
    std::time_t timestamp;
    std::string changeDesc;
    std::string diffSummary;  // Brief diff info
};











#endif // __UPDATEEVENT_HPP