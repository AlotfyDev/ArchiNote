
#ifndef __EXPORTEVENT_HPP__
#define __EXPORTEVENT_HPP__

#include <string>

using std::string;



struct ExportEvent {
    std::string attrId;
    std::string exportFormat;  // e.g., "JSON", "YAML"
    bool success;
};













#endif // __EXPORTEVENT_HPP__