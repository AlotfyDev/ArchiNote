
#ifndef __IARCHINOTEMANAGER_HPP__
#define __IARCHINOTEMANAGER_HPP__





#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <memory>
#include <sstream>
#include <functional>




#include "../../Enums/ArchiNoteAttributeType.hpp"
#include "../../Enums/ArchiNoteAttributeRelationType.hpp"
#include "../ArchiNoteAttributeRepoAgent/IAttributeRepoAgent.hpp"

#include "../../Enums/ArchiNoteType.hpp"




// Interface for ArchiNoteManager
class IArchiNoteManager {
public:
    virtual ~IArchiNoteManager() = default;

    // CRUD operations for Repo Agents
    virtual void createAgent(ArchiNoteAttributeType type) = 0;
    virtual IAttributeRepoAgent* readAgent(ArchiNoteAttributeType type) const = 0;
    virtual void updateAgent(ArchiNoteAttributeType type, IAttributeRepoAgent* agent) = 0;
    virtual void removeAgent(ArchiNoteAttributeType type) = 0;

    // Access to Repo Agents
    virtual IAttributeRepoAgent* getRepoAgent(ArchiNoteAttributeType type) = 0;

    // Relations management (intra-document)
    virtual void addRelation(const std::string& sourceId, ArchiNoteAttributeType sourceType, 
                            const std::string& targetId, ArchiNoteAttributeType targetType, 
                            ArchiNoteAttributeRelationType relationType) = 0;

    // Serialization
    virtual std::string toJSON(ArchiNoteAttributeType type, const std::string& id) const = 0;
    virtual std::string toYAML(ArchiNoteAttributeType type, const std::string& id) const = 0;
    virtual std::string exportAllToJSON() const = 0;

    // Async communication for attributes (delegated to Agents)
    virtual void createAsync(ArchiNoteAttributeType type, const std::string& id, 
                            const std::map<std::string, std::string>& params, 
                            std::function<void(const std::string&, bool)> callback) = 0;

    // Storage management
    virtual void setStorageAdapter(IStorageAdapter* adapter) = 0;
    virtual void saveAll() = 0;
    virtual void loadAll() = 0;
};










#endif // __IARCHINOTEMANAGER_HPP__