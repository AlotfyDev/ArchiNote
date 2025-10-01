
#ifndef __RISKSREPOAGENT_HPP__
#define __RISKSREPOAGENT_HPP__



#include <string>
#include <map>
#include <vector>
#include <functional>
#include <stdexcept>
#include <algorithm>
#include <sstream>
#include <algorithm>

#include "../IAttributeRepoAgent.hpp"
#include "../../ArchiNoteAttribute/ProjectBriefAttributes/VersionedRisk.hpp"
#include "../../ArchiNoteStorage/IStorageAdapter.hpp"




// Specialized Repo Agent for Risks
class RiskRepoAgent : public IAttributeRepoAgent {
private:
    std::vector<VersionedRisk*> risks;  // In-memory storage
    IStorageAdapter* storageAdapter;  // Storage adapter for persistence

    // Helper: Find risk by ID
    VersionedRisk* findById(const std::string& id) const {
        auto it = std::find_if(risks.begin(), risks.end(),
                               [&id](const VersionedRisk* risk) { return risk->getID() == id; });
        return (it != risks.end()) ? *it : nullptr;
    }

    // Helper: Validate params map
    void validateParams(const std::map<std::string, std::string>& params) const {
        if (params.find("riskDescription") == params.end() || params.at("riskDescription").empty()) {
            throw std::invalid_argument("Missing or empty riskDescription");
        }
        if (params.find("impact") == params.end() || params.at("impact").empty()) {
            throw std::invalid_argument("Missing or empty impact");
        }
        if (params.find("mitigation") == params.end() || params.at("mitigation").empty()) {
            throw std::invalid_argument("Missing or empty mitigation");
        }
    }

public:
    // Constructor with IStorageAdapter
    RiskRepoAgent(IStorageAdapter* adapter = nullptr) : storageAdapter(adapter) {}

    ~RiskRepoAgent() override {
        for (auto* risk : risks) {
            delete risk;  // Manual memory management
        }
        risks.clear();
    }

    // No copy (to avoid double-delete)
    RiskRepoAgent(const RiskRepoAgent&) = delete;
    RiskRepoAgent& operator=(const RiskRepoAgent&) = delete;

    // Move constructor/assignment
    RiskRepoAgent(RiskRepoAgent&& other) noexcept 
        : risks(std::move(other.risks)), storageAdapter(other.storageAdapter) {
        other.risks.clear();
        other.storageAdapter = nullptr;
    }
    RiskRepoAgent& operator=(RiskRepoAgent&& other) noexcept {
        if (this != &other) {
            for (auto* risk : risks) {
                delete risk;
            }
            risks.clear();
            risks = std::move(other.risks);
            storageAdapter = other.storageAdapter;
            other.risks.clear();
            other.storageAdapter = nullptr;
        }
        return *this;
    }

    // CRUD operations
    void create(const std::string& id, const std::map<std::string, std::string>& params) override {
        if (findById(id)) {
            throw std::invalid_argument("Risk ID already exists: " + id);
        }
        validateParams(params);
        auto* risk = new VersionedRisk(id, params.at("riskDescription"),
                                      params.at("impact"),
                                      params.at("mitigation"));
        risks.push_back(risk);
        if (storageAdapter) {
            storageAdapter->save(id, toJSON(id));  // Persist to storage
        }
    }

    void* read(const std::string& id) const override {
        auto* risk = findById(id);
        if (!risk && storageAdapter) {
            // Try loading from storage
            std::string data = storageAdapter->load(id);
            if (!data.empty()) {
                // Pseudo-code: Deserialize and add to risks
                // risks.push_back(deserialize(data));
                // risk = findById(id);
            }
        }
        if (!risk) {
            throw std::invalid_argument("Risk not found: " + id);
        }
        return risk;  // Consumer casts to VersionedRisk*
    }

    void update(const std::string& id, const std::map<std::string, std::string>& params) override {
        auto* risk = findById(id);
        if (!risk) {
            throw std::invalid_argument("Risk not found: " + id);
        }
        validateParams(params);
        risk->updateContent(params.at("riskDescription"), params.at("impact"), params.at("mitigation"));
        if (storageAdapter) {
            storageAdapter->save(id, toJSON(id));  // Persist to storage
        }
    }

    void remove(const std::string& id) override {
        auto it = std::find_if(risks.begin(), risks.end(),
                               [&id](const VersionedRisk* risk) { return risk->getID() == id; });
        if (it == risks.end()) {
            throw std::invalid_argument("Risk not found: " + id);
        }
        delete *it;
        risks.erase(it);
        if (storageAdapter) {
            storageAdapter->remove(id);  // Remove from storage
        }
    }

    // Serialization
    std::string toJSON(const std::string& id) const override {
        auto* risk = findById(id);
        if (!risk) {
            throw std::invalid_argument("Risk not found: " + id);
        }
        return risk->toFullJSON();
    }

    std::string toYAML(const std::string& id) const override {
        auto* risk = findById(id);
        if (!risk) {
            throw std::invalid_argument("Risk not found: " + id);
        }
        std::stringstream yaml;
        yaml << "id: " << id << "\n";
        yaml << "type: INITIAL_RISKS\n";
        yaml << "content:\n";
        yaml << "  riskDescription: \"" << risk->toFullJSON() /* simplified: extract riskDescription */ << "\"\n";
        yaml << "  impact: \"" << risk->toFullJSON() /* simplified: extract impact */ << "\"\n";
        yaml << "  mitigation: \"" << risk->toFullJSON() /* simplified: extract mitigation */ << "\"\n";
        yaml << "relations:\n";
        for (const auto& rel : risk->getRelations()) {
            yaml << "  - " << static_cast<int>(rel) << "\n";
        }
        return yaml.str();
    }

    // KG export
    void exportToKG(const std::string& id) const override {
        auto* risk = findById(id);
        if (!risk) {
            throw std::invalid_argument("Risk not found: " + id);
        }
        // Pseudo-code: Export node and edges to KG
        // KGClient::createNode(id, "INITIAL_RISKS", toJSON(id));
        // for (rel : risk->getRelations()) KGClient::createEdge(id, rel.target, rel.type);
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










#endif // __RISKSREPOAGENT_HPP__