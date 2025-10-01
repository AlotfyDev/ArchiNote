
#ifndef __PAINPOINTREPOAGENT_HPP__
#define __PAINPOINTREPOAGENT_HPP__




#include <algorithm>

#include "../IAttributeRepoAgent.hpp"
#include "../../ArchiNoteAttribute/ProjectBriefAttributes/VersionedPainPoint.hpp"
#include "../../ArchiNoteStorage/IStorageAdapter.hpp"


// Specialized Repo Agent for Pain Points
class PainPointsRepoAgent : public IAttributeRepoAgent {
public:
    PainPointsRepoAgent() = default;
private:
    std::vector<VersionedPainPoints*> painPoints;  // In-memory storage
    IStorageAdapter* storageAdapter;  // Storage adapter for persistence
    
     // Helper: Find pain point by ID
    VersionedPainPoints* findById(const std::string& id) const {
        auto it = std::find_if(painPoints.begin(), painPoints.end(),
                               [&id](const VersionedPainPoints* pp) { return pp->getID() == id; });
        return (it != painPoints.end()) ? *it : nullptr;
    }

    // Helper: Validate params map
    void validateParams(const std::map<std::string, std::string>& params) const {
        if (params.find("issue") == params.end() || params.at("issue").empty()) {
            throw std::invalid_argument("Missing or empty issue");
        }
        if (params.find("impact") == params.end() || params.at("impact").empty()) {
            throw std::invalid_argument("Missing or empty impact");
        }
        if (params.find("stakeholders") == params.end() || params.at("stakeholders").empty()) {
            throw std::invalid_argument("Missing or empty stakeholders");
        }
    }

public:
    // Constructor with IStorageAdapter
    PainPointsRepoAgent(IStorageAdapter* adapter = nullptr) : storageAdapter(adapter) {}

    ~PainPointsRepoAgent() override {
        for (auto* pp : painPoints) {
            delete pp;  // Manual memory management
        }
        painPoints.clear();
    }

    // No copy (to avoid double-delete)
    PainPointsRepoAgent(const PainPointsRepoAgent&) = delete;
    PainPointsRepoAgent& operator=(const PainPointsRepoAgent&) = delete;

    // Move constructor/assignment
    PainPointsRepoAgent(PainPointsRepoAgent&& other) noexcept 
        : painPoints(std::move(other.painPoints)), storageAdapter(other.storageAdapter) {
        other.painPoints.clear();
        other.storageAdapter = nullptr;
    }
    PainPointsRepoAgent& operator=(PainPointsRepoAgent&& other) noexcept {
        if (this != &other) {
            for (auto* pp : painPoints) {
                delete pp;
            }
            painPoints.clear();
            painPoints = std::move(other.painPoints);
            storageAdapter = other.storageAdapter;
            other.painPoints.clear();
            other.storageAdapter = nullptr;
        }
        return *this;
    }

    // CRUD operations
    void create(const std::string& id, const std::map<std::string, std::string>& params) override {
        if (findById(id)) {
            throw std::invalid_argument("Pain Point ID already exists: " + id);
        }
        validateParams(params);
        auto* painPoint = new VersionedPainPoints(id, params.at("issue"),
                                                 params.at("impact"),
                                                 params.at("stakeholders"));
        painPoints.push_back(painPoint);
        if (storageAdapter) {
            storageAdapter->save(id, toJSON(id));  // Persist to storage
        }
    }

    void* read(const std::string& id) const override {
        auto* pp = findById(id);
        if (!pp && storageAdapter) {
            // Try loading from storage
            std::string data = storageAdapter->load(id);
            if (!data.empty()) {
                // Pseudo-code: Deserialize and add to painPoints
                // painPoints.push_back(deserialize(data));
                // pp = findById(id);
            }
        }
        if (!pp) {
            throw std::invalid_argument("Pain Point not found: " + id);
        }
        return pp;  // Consumer casts to VersionedPainPoints*
    }

    void update(const std::string& id, const std::map<std::string, std::string>& params) override {
        auto* pp = findById(id);
        if (!pp) {
            throw std::invalid_argument("Pain Point not found: " + id);
        }
        validateParams(params);
        pp->updateContent(params.at("issue"), params.at("impact"), params.at("stakeholders"));
        if (storageAdapter) {
            storageAdapter->save(id, toJSON(id));  // Persist to storage
        }
    }

    void remove(const std::string& id) override {
        auto it = std::find_if(painPoints.begin(), painPoints.end(),
                               [&id](const VersionedPainPoints* pp) { return pp->getID() == id; });
        if (it == painPoints.end()) {
            throw std::invalid_argument("Pain Point not found: " + id);
        }
        delete *it;
        painPoints.erase(it);
        if (storageAdapter) {
            storageAdapter->remove(id);  // Remove from storage
        }
    }

    // Serialization
    std::string toJSON(const std::string& id) const override {
        auto* pp = findById(id);
        if (!pp) {
            throw std::invalid_argument("Pain Point not found: " + id);
        }
        return pp->toFullJSON();
    }

    std::string toYAML(const std::string& id) const override {
        auto* pp = findById(id);
        if (!pp) {
            throw std::invalid_argument("Pain Point not found: " + id);
        }
        std::stringstream yaml;
        yaml << "id: " << id << "\n";
        yaml << "type: PAIN_POINTS\n";
        yaml << "content:\n";
        yaml << "  issue: \"" << pp->toFullJSON() /* simplified: extract issue */ << "\"\n";
        yaml << "  impact: \"" << pp->toFullJSON() /* simplified: extract impact */ << "\"\n";
        yaml << "  stakeholders: \"" << pp->toFullJSON() /* simplified: extract stakeholders */ << "\"\n";
        yaml << "relations:\n";
        for (const auto& rel : pp->getRelations()) {
            yaml << "  - " << static_cast<int>(rel) << "\n";
        }
        return yaml.str();
    }

    // KG export
    void exportToKG(const std::string& id) const override {
        auto* pp = findById(id);
        if (!pp) {
            throw std::invalid_argument("Pain Point not found: " + id);
        }
        // Pseudo-code: Export node and edges to KG
        // KGClient::createNode(id, "PAIN_POINTS", toJSON(id));
        // for (rel : pp->getRelations()) KGClient::createEdge(id, rel.target, rel.type);
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




















#endif // __PAINPOINTREPOAGENT_HPP