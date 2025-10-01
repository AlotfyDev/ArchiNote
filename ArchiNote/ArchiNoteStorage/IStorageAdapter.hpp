#ifndef __ISTORAGEADAPTER_HPP__
#define __ISTORAGEADAPTER_HPP__


class IArchiNoteManager;

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <memory>
#include <sstream>
#include <functional>


#include "../Enums/ArchiNoteType.hpp"

// Interface for storage adapter
class IStorageAdapter {
public:
    virtual ~IStorageAdapter() = default;
    virtual void save(const std::string& key, const std::string& data) = 0;
    virtual std::string load(const std::string& key) = 0;
    virtual void remove(const std::string& key) = 0;
};











#endif // __ISTORAGEADAPTER_HPP__