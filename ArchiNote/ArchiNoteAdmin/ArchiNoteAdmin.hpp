
#ifndef __ARCHINOTEADMIN_HPP__
#define __ARCHINOTEADMIN_HPP__

#include "../ArchiNoteManager/IArchiNoteManager.hpp"
#include "../ArchiNoteAttributeRepoAgent/IAttributeRepoAgent.hpp"
#include "../ArchiNoteStorage/IStorageAdapter.hpp"

class ProjectBriefArchiNoteManager;


// ArchiNoteAdmin for managing ArchiNoteManagers and cross-document relations
class ArchiNoteAdmin {
private:
    std::map<ArchiNoteType, IArchiNoteManager*> managers;
    IStorageAdapter* storageAdapter = nullptr;

    // Helper: Validate cross-document relation
    bool validateCrossDocumentRelation(ArchiNoteType sourceNoteType, const std::string& sourceId, 
                                      ArchiNoteAttributeType sourceType,
                                      ArchiNoteType targetNoteType, const std::string& targetId, 
                                      ArchiNoteAttributeType targetType) {
        auto sourceManager = managers.find(sourceNoteType);
        auto targetManager = managers.find(targetNoteType);
        if (sourceManager == managers.end() || targetManager == managers.end()) {
            return false;
        }
        return sourceManager->second->getRepoAgent(sourceType)->read(sourceId) && 
               targetManager->second->getRepoAgent(targetType)->read(targetId);
    }

public:
    ArchiNoteAdmin() {
        // Initialize managers for supported ArchiNote types
        managers[ArchiNoteType::PROJECT_BRIEF] = new ProjectBriefArchiNoteManager();
        // Add other managers as needed (e.g., ArchitectureDesignArchiNoteManager)
    }

    ~ArchiNoteAdmin() {
        for (auto& pair : managers) {
            delete pair.second;
        }
        managers.clear();
        delete storageAdapter;
    }

    // No copy
    ArchiNoteAdmin(const ArchiNoteAdmin&) = delete;
    ArchiNoteAdmin& operator=(const ArchiNoteAdmin&) = delete;

    // Move constructor/assignment
    ArchiNoteAdmin(ArchiNoteAdmin&& other) noexcept 
        : managers(std::move(other.managers)), storageAdapter(other.storageAdapter) {
        other.managers.clear();
        other.storageAdapter = nullptr;
    }
    ArchiNoteAdmin& operator=(ArchiNoteAdmin&& other) noexcept {
        if (this != &other) {
            for (auto& pair : managers) {
                delete pair.second;
            }
            managers.clear();
            delete storageAdapter;
            managers = std::move(other.managers);
            storageAdapter = other.storageAdapter;
            other.managers.clear();
            other.storageAdapter = nullptr;
        }
        return *this;
    }

    // Lifecycle management
    void createManager(ArchiNoteType type) {
        if (managers.find(type) != managers.end()) {
            throw std::invalid_argument("ArchiNoteManager already exists for type");
        }
        switch (type) {
            case ArchiNoteType::PROJECT_BRIEF:
                managers[type] = new ProjectBriefArchiNoteManager();
                break;
            // Add cases for other ArchiNote types
            default:
                throw std::invalid_argument("Unsupported ArchiNote type");
        }
    }

    void destroyManager(ArchiNoteType type) {
        auto it = managers.find(type);
        if (it == managers.end()) {
            throw std::invalid_argument("ArchiNoteManager not found for type");
        }
        delete it->second;
        managers.erase(it);
    }

    // Access to ArchiNoteManager
    IArchiNoteManager* getManager(ArchiNoteType noteType) {
        auto it = managers.find(noteType);
        if (it == managers.end()) {
            throw std::invalid_argument("ArchiNoteManager not found for type");
        }
        return it->second;
    }

    // Serialization
    std::string exportAllArchiNotesToJSON() const {
        std::stringstream json;
        json << "{ \"archinotes\": {";
        bool first = true;
        for (const auto& pair : managers) {
            if (!first) json << ",";
            json << "\"" << static_cast<int>(pair.first) << "\": " << pair.second->exportAllToJSON();
            first = false;
        }
        json << "} }";
        return json.str();
    }

    // Cross-document relations management
    void addCrossDocumentRelation(ArchiNoteType sourceNoteType, const std::string& sourceId, 
                                  ArchiNoteAttributeType sourceType,
                                  ArchiNoteType targetNoteType, const std::string& targetId, 
                                  ArchiNoteAttributeType targetType,
                                  ArchiNoteAttributeRelationType relationType) {
        // Centralized management with validation and approval
        if (!validateCrossDocumentRelation(sourceNoteType, sourceId, sourceType, 
                                          targetNoteType, targetId, targetType)) {
            throw std::invalid_argument("Invalid source or target for cross-document relation");
        }
        // Policy: Placeholder for relation policies (e.g., only allow VERIFIES for certain types)
        // if (!isValidRelationPolicy(relationType, sourceType, targetType)) {
        //     throw std::invalid_argument("Relation type not allowed");
        // }
        // Update source attribute relations (pseudo-code)
        // auto sourceManager = managers[sourceNoteType];
        // sourceManager->addRelation(sourceId, sourceType, targetId, targetType, relationType);
    }

    void delegateCrossDocumentRelation(ArchiNoteType sourceNoteType, const std::string& sourceId, 
                                      ArchiNoteAttributeType sourceType,
                                      ArchiNoteType targetNoteType, const std::string& targetId, 
                                      ArchiNoteAttributeType targetType,
                                      ArchiNoteAttributeRelationType relationType) {
        // Delegated management with approval
        if (!validateCrossDocumentRelation(sourceNoteType, sourceId, sourceType, 
                                          targetNoteType, targetId, targetType)) {
            throw std::invalid_argument("Invalid source or target for delegated cross-document relation");
        }
        // Policy: Placeholder for approval logic
        // if (!approveDelegation(sourceNoteType, relationType)) {
        //     throw std::invalid_argument("Delegation not approved");
        // }
        auto sourceManager = managers[sourceNoteType];
        sourceManager->addRelation(sourceId, sourceType, targetId, targetType, relationType);
    }

    // Storage management
    void setStorageAdapter(IStorageAdapter* adapter) {
        delete storageAdapter;
        storageAdapter = adapter;
        // Propagate to all managers
        for (auto& pair : managers) {
            pair.second->setStorageAdapter(adapter);
        }
    }

    void saveAllArchiNotes() {
        if (!storageAdapter) {
            throw std::runtime_error("No storage adapter set");
        }
        for (auto& pair : managers) {
            pair.second->saveAll();
        }
    }

    void loadAllArchiNotes() {
        if (!storageAdapter) {
            throw std::runtime_error("No storage adapter set");
        }
        for (auto& pair : managers) {
            pair.second->loadAll();
        }
    }
};












#endif // __ARCHINOTEADMIN_HPP