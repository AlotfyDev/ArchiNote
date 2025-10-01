
#ifndef __IATTRIBUTEREPOAGENT_HPP__
#define __IATTRIBUTEREPOAGENT_HPP__

#include <string>
#include <map>
#include <vector>
#include <functional>
#include <stdexcept>

#include "../../ArchiNoteStorage/IStorageAdapter.hpp"


// Interface for Attribute Repo Agent (generic CRUD + communication)
class IAttributeRepoAgent {
public:
       virtual ~IAttributeRepoAgent() = default;

    // CRUD operations (generic with params map)
    virtual void create(const std::string& id, const std::map<std::string, std::string>& params) = 0;
    virtual void* read(const std::string& id) const = 0;  // Returns void* (cast to specific type by consumer)
    virtual void update(const std::string& id, const std::map<std::string, std::string>& params) = 0;
    virtual void remove(const std::string& id) = 0;

    // Serialization
    virtual std::string toJSON(const std::string& id) const = 0;
    virtual std::string toYAML(const std::string& id) const = 0;  // For ArchiGraph export
    virtual void exportToKG(const std::string& id) const = 0;  // To ArchiGraph nodes/edges

    // Async communication (optional, for frontend)
    virtual void createAsync(const std::string& id, const std::map<std::string, std::string>& params, 
                            std::function<void(const std::string&, bool)> callback) = 0;
   // set storage:
   virtual void setStorageAdapter(IStorageAdapter* adapter) = 0;
};















#endif // IATTRIBUTEREPOAGENT_HPP__