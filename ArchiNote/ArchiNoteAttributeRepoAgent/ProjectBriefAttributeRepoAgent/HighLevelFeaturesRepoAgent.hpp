
#ifndef __HIGHLEVELFEATURESREPOAGENT_HPP__
#define __HIGHLEVELFEATURESREPOAGENT_HPP__






#include <string>
#include <map>
#include <vector>
#include <functional>
#include <stdexcept>
#include <algorithm>
#include <sstream>
#include <algorithm>


#include "../IAttributeRepoAgent.hpp"
#include "../../ArchiNoteAttribute/ProjectBriefAttributes/VersionedHighLevelFeature.hpp"
#include "../../ArchiNoteStorage/IStorageAdapter.hpp"


// Specialized Repo Agent for High-Level Features

// Specialized Repo Agent for High-Level Features
class HighLevelFeaturesRepoAgent : public IAttributeRepoAgent {
private:
    std::vector<VersionedHighLevelFeature*> features;  // In-memory storage
    IStorageAdapter* storageAdapter;  // Storage adapter for persistence

    // Helper: Find high-level feature by ID
    VersionedHighLevelFeature* findById(const std::string& id) const {
        auto it = std::find_if(features.begin(), features.end(),
                               [&id](const VersionedHighLevelFeature* feature) { return feature->getID() == id; });
        return (it != features.end()) ? *it : nullptr;
    }

    // Helper: Validate params map
    void validateParams(const std::map<std::string, std::string>& params) const {
        if (params.find("feature") == params.end() || params.at("feature").empty()) {
            throw std::invalid_argument("Missing or empty feature");
        }
        if (params.find("description") == params.end() || params.at("description").empty()) {
            throw std::invalid_argument("Missing or empty description");
        }
        if (params.find("priority") == params.end() || params.at("priority").empty()) {
            throw std::invalid_argument("Missing or empty priority");
        }
        // Validate that priority is a valid integer and within range [1, 3]
        try {
            int priority = std::stoi(params.at("priority"));
            if (priority < 1 || priority > 3) {
                throw std::invalid_argument("Priority must be between 1 and 3");
            }
        } catch (const std::exception&) {
            throw std::invalid_argument("Priority must be a valid integer");
        }
    }

public:
    // Constructor with IStorageAdapter
    HighLevelFeaturesRepoAgent(IStorageAdapter* adapter = nullptr) : storageAdapter(adapter) {}

    ~HighLevelFeaturesRepoAgent() override {
        for (auto* feature : features) {
            delete feature;  // Manual memory management
        }
        features.clear();
    }

    // No copy (to avoid double-delete)
    HighLevelFeaturesRepoAgent(const HighLevelFeaturesRepoAgent&) = delete;
    HighLevelFeaturesRepoAgent& operator=(const HighLevelFeaturesRepoAgent&) = delete;

    // Move constructor/assignment
    HighLevelFeaturesRepoAgent(HighLevelFeaturesRepoAgent&& other) noexcept 
        : features(std::move(other.features)), storageAdapter(other.storageAdapter) {
        other.features.clear();
        other.storageAdapter = nullptr;
    }
    HighLevelFeaturesRepoAgent& operator=(HighLevelFeaturesRepoAgent&& other) noexcept {
        if (this != &other) {
            for (auto* feature : features) {
                delete feature;
            }
            features.clear();
            features = std::move(other.features);
            storageAdapter = other.storageAdapter;
            other.features.clear();
            other.storageAdapter = nullptr;
        }
        return *this;
    }

    // CRUD operations
    void create(const std::string& id, const std::map<std::string, std::string>& params) override {
        if (findById(id)) {
            throw std::invalid_argument("High-Level Feature ID already exists: " + id);
        }
        validateParams(params);
        int priority = std::stoi(params.at("priority"));  // Convert string to int
        auto* feature = new VersionedHighLevelFeature(id, params.at("feature"),
                                                     params.at("description"),
                                                     priority);
        features.push_back(feature);
        if (storageAdapter) {
            storageAdapter->save(id, toJSON(id));  // Persist to storage
        }
    }

    void* read(const std::string& id) const override {
        auto* feature = findById(id);
        if (!feature && storageAdapter) {
            // Try loading from storage
            std::string data = storageAdapter->load(id);
            if (!data.empty()) {
                // Pseudo-code: Deserialize and add to features
                // features.push_back(deserialize(data));
                // feature = findById(id);
            }
        }
        if (!feature) {
            throw std::invalid_argument("High-Level Feature not found: " + id);
        }
        return feature;  // Consumer casts to VersionedHighLevelFeature*
    }

    void update(const std::string& id, const std::map<std::string, std::string>& params) override {
        auto* feature = findById(id);
        if (!feature) {
            throw std::invalid_argument("High-Level Feature not found: " + id);
        }
        validateParams(params);
        int priority = std::stoi(params.at("priority"));  // Convert string to int
        feature->updateContent(params.at("feature"), params.at("description"), priority);
        if (storageAdapter) {
            storageAdapter->save(id, toJSON(id));  // Persist to storage
        }
    }

    void remove(const std::string& id) override {
        auto it = std::find_if(features.begin(), features.end(),
                               [&id](const VersionedHighLevelFeature* feature) { return feature->getID() == id; });
        if (it == features.end()) {
            throw std::invalid_argument("High-Level Feature not found: " + id);
        }
        delete *it;
        features.erase(it);
        if (storageAdapter) {
            storageAdapter->remove(id);  // Remove from storage
        }
    }

    // Serialization
    std::string toJSON(const std::string& id) const override {
        auto* feature = findById(id);
        if (!feature) {
            throw std::invalid_argument("High-Level Feature not found: " + id);
        }
        return feature->toFullJSON();
    }

    std::string toYAML(const std::string& id) const override {
        auto* feature = findById(id);
        if (!feature) {
            throw std::invalid_argument("High-Level Feature not found: " + id);
        }
        std::stringstream yaml;
        yaml << "id: " << id << "\n";
        yaml << "type: HIGH_LEVEL_FEATURES\n";
        yaml << "content:\n";
        yaml << "  feature: \"" << feature->toFullJSON() /* simplified: extract feature */ << "\"\n";
        yaml << "  description: \"" << feature->toFullJSON() /* simplified: extract description */ << "\"\n";
        yaml << "  priority: " << feature->toFullJSON() /* simplified: extract priority */ << "\n";
        yaml << "relations:\n";
        for (const auto& rel : feature->getRelations()) {
            yaml << "  - " << static_cast<int>(rel) << "\n";
        }
        return yaml.str();
    }

    // KG export
    void exportToKG(const std::string& id) const override {
        auto* feature = findById(id);
        if (!feature) {
            throw std::invalid_argument("High-Level Feature not found: " + id);
        }
        // Pseudo-code: Export node and edges to KG
        // KGClient::createNode(id, "HIGH_LEVEL_FEATURES", toJSON(id));
        // for (rel : feature->getRelations()) KGClient::createEdge(id, rel.target, rel.type);
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







#endif // __HIGHLEVELFEATURESREPOAGENT_HPP