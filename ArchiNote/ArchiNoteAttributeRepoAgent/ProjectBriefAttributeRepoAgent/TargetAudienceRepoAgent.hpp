#ifndef __TARGETAUDIENCEREPOAGENT_HPP__
#define __TARGETAUDIENCEREPOAGENT_HPP__


#include <algorithm>

#include "../IAttributeRepoAgent.hpp"
#include "../../ArchiNoteAttribute/ProjectBriefAttributes/VersionedTargetAudience.hpp"
#include "../../ArchiNoteStorage/IStorageAdapter.hpp"



// Specialized Repo Agent for Target Audience
class TargetAudienceRepoAgent : public IAttributeRepoAgent {
private:
    std::vector<VersionedTargetAudience*> audiences;  // In-memory storage
    IStorageAdapter* storageAdapter;  // Storage adapter for persistence

    
    // Helper: Find audience by ID
    VersionedTargetAudience* findById(const std::string& id) const {
        auto it = std::find_if(audiences.begin(), audiences.end(),
                               [&id](const VersionedTargetAudience* aud) { return aud->getID() == id; });
        return (it != audiences.end()) ? *it : nullptr;
    }
    
    // Helper: Validate params map
    void validateParams(const std::map<std::string, std::string>& params) const {
        if (params.find("segment") == params.end() || params.at("segment").empty()) {
            throw std::invalid_argument("Missing or empty segment");
        }
        if (params.find("needs") == params.end() || params.at("needs").empty()) {
            throw std::invalid_argument("Missing or empty needs");
        }
        if (params.find("characteristics") == params.end() || params.at("characteristics").empty()) {
            throw std::invalid_argument("Missing or empty characteristics");
        }
    }

public:

    // Constructor with IStorageAdapter
    TargetAudienceRepoAgent(IStorageAdapter* adapter = nullptr) : storageAdapter(adapter) {}

    ~TargetAudienceRepoAgent() override {
        for (auto* aud : audiences) {
            delete aud;  // Manual memory management
        }
        audiences.clear();
    }
    

    // No copy (to avoid double-delete)
    TargetAudienceRepoAgent(const TargetAudienceRepoAgent&) = delete;
    TargetAudienceRepoAgent& operator=(const TargetAudienceRepoAgent&) = delete;

    // Move constructor/assignment
    TargetAudienceRepoAgent(TargetAudienceRepoAgent&& other) noexcept 
        : audiences(std::move(other.audiences)), storageAdapter(other.storageAdapter) {
        other.audiences.clear();
        other.storageAdapter = nullptr;
    }
    TargetAudienceRepoAgent& operator=(TargetAudienceRepoAgent&& other) noexcept {
        if (this != &other) {
            for (auto* aud : audiences) {
                delete aud;
            }
            audiences.clear();
            audiences = std::move(other.audiences);
            storageAdapter = other.storageAdapter;
            other.audiences.clear();
            other.storageAdapter = nullptr;
        }
        return *this;
    }

    void create(const std::string& id, const std::map<std::string, std::string>& params) override {
        if (findById(id)) {
            throw std::invalid_argument("Target Audience ID already exists: " + id);
        }
        validateParams(params);
        auto* audience = new VersionedTargetAudience(id, params.at("segment"), 
                                                    params.at("needs"), 
                                                    params.at("characteristics"));
        audiences.push_back(audience);
        if (storageAdapter) {
            storageAdapter->save(id, toJSON(id));  // Persist to storage
        }
    }

    void* read(const std::string& id) const override {
        auto* aud = findById(id);
        if (!aud && storageAdapter) {
            // Try loading from storage
            std::string data = storageAdapter->load(id);
            if (!data.empty()) {
                // Pseudo-code: Deserialize and add to audiences
                // audiences.push_back(deserialize(data));
                // aud = findById(id);
            }
        }
        if (!aud) {
            throw std::invalid_argument("Target Audience not found: " + id);
        }
        return aud;  // Consumer casts to VersionedTargetAudience*
    }

    void update(const std::string& id, const std::map<std::string, std::string>& params) override {
        auto* aud = findById(id);
        if (!aud) {
            throw std::invalid_argument("Target Audience not found: " + id);
        }
        validateParams(params);
        aud->updateContent(params.at("segment"), params.at("needs"), params.at("characteristics"));
        if (storageAdapter) {
            storageAdapter->save(id, toJSON(id));  // Persist to storage
        }
    }

    void remove(const std::string& id) override {
        auto it = std::find_if(audiences.begin(), audiences.end(),
                               [&id](const VersionedTargetAudience* aud) { return aud->getID() == id; });
        if (it == audiences.end()) {
            throw std::invalid_argument("Target Audience not found: " + id);
        }
        delete *it;
        audiences.erase(it);
        if (storageAdapter) {
            storageAdapter->remove(id);  // Remove from storage
        }
    }

    // Serialization
    std::string toJSON(const std::string& id) const override {
        auto* aud = findById(id);
        if (!aud) {
            throw std::invalid_argument("Target Audience not found: " + id);
        }
        return aud->toFullJSON();
    }

    std::string toYAML(const std::string& id) const override {
        auto* aud = findById(id);
        if (!aud) {
            throw std::invalid_argument("Target Audience not found: " + id);
        }
        std::stringstream yaml;
        yaml << "id: " << id << "\n";
        yaml << "type: TARGET_AUDIENCE\n";
        yaml << "content:\n";
        yaml << "  segment: \"" << aud->toFullJSON() /* simplified: extract segment */ << "\"\n";
        yaml << "  needs: \"" << aud->toFullJSON() /* simplified: extract needs */ << "\"\n";
        yaml << "  characteristics: \"" << aud->toFullJSON() /* simplified: extract characteristics */ << "\"\n";
        yaml << "relations:\n";
        for (const auto& rel : aud->getRelations()) {
            yaml << "  - " << static_cast<int>(rel) << "\n";
        }
        return yaml.str();
    }

    // KG export
    void exportToKG(const std::string& id) const override {
        auto* aud = findById(id);
        if (!aud) {
            throw std::invalid_argument("Target Audience not found: " + id);
        }
        // Pseudo-code: Export node and edges to KG
        // KGClient::createNode(id, "TARGET_AUDIENCE", toJSON(id));
        // for (rel : aud->getRelations()) KGClient::createEdge(id, rel.target, rel.type);
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






















#endif // __TARGETAUDIENCEREPOAGENT_HPP__