#ifndef __OBJECTIVESREPOAGENT_HPP__
#define __OBJECTIVESREPOAGENT_HPP__




#include <algorithm>

#include "../IAttributeRepoAgent.hpp"
#include "../../ArchiNoteAttribute/ProjectBriefAttributes/VersionedKeyObjective.hpp"
#include "../../ArchiNoteStorage/IStorageAdapter.hpp"







/*
// Specialized Repo Agent for Key Objectives
class ObjectivesRepoAgent : public IRepoAgent {
private:
    std::vector<VersionedKeyObjective*> objectives;  // In-memory storage (for skeleton)
    // Future: IStorageAdapter* adapter;  // For HDF5, SQLite, etc.

public:
    ~ObjectivesRepoAgent() override {
        for (auto* obj : objectives) {
            delete obj;  // Manual memory management
        }
        objectives.clear();
    }

    // CRUD operations
    void create(const std::string& id, const std::map<std::string, std::string>& params) override {
        // Expected params: description, metric, priority
        // Use VersionedKeyObjectives::CreateKeyObjectivesContent for validation
    }
    void* read(const std::string& id) const override {
        // Return VersionedKeyObjectives* (cast to void*)
        return nullptr;
    }
    void update(const std::string& id, const std::map<std::string, std::string>& params) override {
        // Update via VersionedKeyObjectives::updateContent
    }
    void remove(const std::string& id) override {
        // Delete from vector, manual cleanup
    }

    // Serialization
    std::string toJSON(const std::string& id) const override {
        // Call VersionedKeyObjectives::toFullJSON
        return "";
    }
    std::string toYAML(const std::string& id) const override {
        // Convert JSON to YAML for ArchiGraph
        return "";
    }
    void exportToKG(const std::string& id) const override {
        // Export node (id, type=KEY_OBJECTIVES, content) + edges (relations)
    }

    // Async communication
    void createAsync(const std::string& id, const std::map<std::string, std::string>& params, 
                     std::function<void(const std::string&, bool)> callback) override {
        // Async create, call callback with success/failure
    }
};

*/


class ObjectivesRepoAgent : public IAttributeRepoAgent {
private:
    std::vector<VersionedKeyObjective*> objectives;  // In-memory storage
    IStorageAdapter* storageAdapter;  // Storage adapter for persistence
    // Helper: Find objective by ID
    
    VersionedKeyObjective* findById(const std::string& id) const {
        auto it = std::find_if(objectives.begin(), objectives.end(),
                               [&id](const VersionedKeyObjective* obj) { return obj->getID() == id; });
        return (it != objectives.end()) ? *it : nullptr;
    }
    // Helper: Validate params map
    

    void validateParams(const std::map<std::string, std::string>& params) const {
        if (params.find("description") == params.end() || params.at("description").empty()) {
            throw std::invalid_argument("Missing or empty description");
        }
        if (params.find("metric") == params.end() || params.at("metric").empty()) {
            throw std::invalid_argument("Missing or empty metric");
        }
        if (params.find("priority") == params.end()) {
            throw std::invalid_argument("Missing priority");
        }
        try {
            int priority = std::stoi(params.at("priority"));
            if (priority < 1 || priority > 3) {
                throw std::invalid_argument("Priority must be between 1 and 3");
            }
        } catch (const std::exception&) {
            throw std::invalid_argument("Invalid priority format");
        }
    }



public:
    ObjectivesRepoAgent(IStorageAdapter* adapter = nullptr) : storageAdapter(adapter) {}

    ~ObjectivesRepoAgent() override {
        for (auto* obj : objectives) {
            delete obj;  // Manual memory management
        }
        objectives.clear();
    }
   
    // No copy (to avoid double-delete)
    ObjectivesRepoAgent(const ObjectivesRepoAgent&) = delete;
    ObjectivesRepoAgent& operator=(const ObjectivesRepoAgent&) = delete;

    // Move constructor/assignment
    ObjectivesRepoAgent(ObjectivesRepoAgent&& other) noexcept 
        : objectives(std::move(other.objectives)), storageAdapter(other.storageAdapter) {
        other.objectives.clear();
        other.storageAdapter = nullptr;
    }
    ObjectivesRepoAgent& operator=(ObjectivesRepoAgent&& other) noexcept {
        if (this != &other) {
            for (auto* obj : objectives) {
                delete obj;
            }
            objectives.clear();
            objectives = std::move(other.objectives);
            storageAdapter = other.storageAdapter;
            other.objectives.clear();
            other.storageAdapter = nullptr;
        }
        return *this;
    }

    // CRUD operations
    void create(const std::string& id, const std::map<std::string, std::string>& params) override {
        if (findById(id)) {
            throw std::invalid_argument("Objective ID already exists: " + id);
        }
        validateParams(params);
        auto* objective = new VersionedKeyObjective(id, params.at("description"), 
                                                   params.at("metric"), 
                                                   std::stoi(params.at("priority")));
        objectives.push_back(objective);
        if (storageAdapter) {
            storageAdapter->save(id, toJSON(id));  // Persist to storage
        }
    }

    void* read(const std::string& id) const override {
        auto* obj = findById(id);
        if (!obj && storageAdapter) {
            // Try loading from storage
            std::string data = storageAdapter->load(id);
            if (!data.empty()) {
                // Pseudo-code: Deserialize and add to objectives
                // objectives.push_back(deserialize(data));
                // obj = findById(id);
            }
        }
        if (!obj) {
            throw std::invalid_argument("Objective not found: " + id);
        }
        return obj;  // Consumer casts to VersionedKeyObjective*
    }

    void update(const std::string& id, const std::map<std::string, std::string>& params) override {
        auto* obj = findById(id);
        if (!obj) {
            throw std::invalid_argument("Objective not found: " + id);
        }
        validateParams(params);
        obj->updateContent(params.at("description"), params.at("metric"), std::stoi(params.at("priority")));
        if (storageAdapter) {
            storageAdapter->save(id, toJSON(id));  // Persist to storage
        }
    }

    void remove(const std::string& id) override {
        auto it = std::find_if(objectives.begin(), objectives.end(),
                               [&id](const VersionedKeyObjective* obj) { return obj->getID() == id; });
        if (it == objectives.end()) {
            throw std::invalid_argument("Objective not found: " + id);
        }
        delete *it;
        objectives.erase(it);
        if (storageAdapter) {
            storageAdapter->remove(id);  // Remove from storage
        }
    }

    // Serialization
    std::string toJSON(const std::string& id) const override {
        auto* obj = findById(id);
        if (!obj) {
            throw std::invalid_argument("Objective not found: " + id);
        }
        return obj->toFullJSON();
    }

    std::string toYAML(const std::string& id) const override {
        auto* obj = findById(id);
        if (!obj) {
            throw std::invalid_argument("Objective not found: " + id);
        }
        std::stringstream yaml;
        yaml << "id: " << id << "\n";
        yaml << "type: KEY_OBJECTIVES\n";
        yaml << "content:\n";
        yaml << "  description: \"" << obj->toFullJSON() /* simplified: extract description */ << "\"\n";
        yaml << "  metric: \"" << obj->toFullJSON() /* simplified: extract metric */ << "\"\n";
        yaml << "  priority: " << obj->toFullJSON() /* simplified: extract priority */ << "\n";
        yaml << "relations:\n";
        for (const auto& rel : obj->getRelations()) {
            yaml << "  - " << static_cast<int>(rel) << "\n";
        }
        return yaml.str();
    }

    // KG export
    void exportToKG(const std::string& id) const override {
        auto* obj = findById(id);
        if (!obj) {
            throw std::invalid_argument("Objective not found: " + id);
        }
        // Pseudo-code: Export node and edges to KG
        // KGClient::createNode(id, "KEY_OBJECTIVES", toJSON(id));
        // for (rel : obj->getRelations()) KGClient::createEdge(id, rel.target, rel.type);
    }

    // Async communication
    void createAsync(const std::string& id, const std::map<std::string, std::string>& params, 
                     std::function<void(const std::string&, bool)> callback) override {
        try {
            create(id, params);
            callback(id, true);  // Success
        } catch (const std::exception& e) {
            callback(id, false);  // Failure
        }
    }

    // Storage adapter management
    void setStorageAdapter(IStorageAdapter* adapter) override {
        storageAdapter = adapter;
    }
};







#endif // __OBJECTIVESREPOAGENT_HPP__