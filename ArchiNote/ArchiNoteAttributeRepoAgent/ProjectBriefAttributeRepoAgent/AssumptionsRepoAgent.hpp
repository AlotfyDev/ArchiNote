
#ifndef __ASSUMPTIONSREPOAGENT_HPP__
#define __ASSUMPTIONSREPOAGENT_HPP__




#include <algorithm>

#include "../IAttributeRepoAgent.hpp"
#include "../../ArchiNoteAttribute/ProjectBriefAttributes/VersionedAssumption.hpp"
#include "../../ArchiNoteStorage/IStorageAdapter.hpp"

// Specialized Repo Agent for Assumptions
class AssumptionsRepoAgent : public IAttributeRepoAgent {
private:
    std::vector<VersionedAssumptions*> assumptions;  // In-memory storage
    IStorageAdapter* storageAdapter;  // Storage adapter for persistence

    // Helper: Find assumption by ID
    VersionedAssumptions* findById(const std::string& id) const {
        auto it = std::find_if(assumptions.begin(), assumptions.end(),
                               [&id](const VersionedAssumptions* assump) { return assump->getID() == id; });
        return (it != assumptions.end()) ? *it : nullptr;
    }

    // Helper: Validate params map
    void validateParams(const std::map<std::string, std::string>& params) const {
        if (params.find("assumption") == params.end() || params.at("assumption").empty()) {
            throw std::invalid_argument("Missing or empty assumption");
        }
        if (params.find("rationale") == params.end() || params.at("rationale").empty()) {
            throw std::invalid_argument("Missing or empty rationale");
        }
    }

public:
   // Constructor with IStorageAdapter
    AssumptionsRepoAgent(IStorageAdapter* adapter = nullptr) : storageAdapter(adapter) {}

    ~AssumptionsRepoAgent() override {
        for (auto* assump : assumptions) {
            delete assump;  // Manual memory management
        }
        assumptions.clear();
    }

    // No copy (to avoid double-delete)
    AssumptionsRepoAgent(const AssumptionsRepoAgent&) = delete;
    AssumptionsRepoAgent& operator=(const AssumptionsRepoAgent&) = delete;

    // Move constructor/assignment
    AssumptionsRepoAgent(AssumptionsRepoAgent&& other) noexcept 
        : assumptions(std::move(other.assumptions)), storageAdapter(other.storageAdapter) {
        other.assumptions.clear();
        other.storageAdapter = nullptr;
    }
    AssumptionsRepoAgent& operator=(AssumptionsRepoAgent&& other) noexcept {
        if (this != &other) {
            for (auto* assump : assumptions) {
                delete assump;
            }
            assumptions.clear();
            assumptions = std::move(other.assumptions);
            storageAdapter = other.storageAdapter;
            other.assumptions.clear();
            other.storageAdapter = nullptr;
        }
        return *this;
    }

    // CRUD operations
    void create(const std::string& id, const std::map<std::string, std::string>& params) override {
        if (findById(id)) {
            throw std::invalid_argument("Assumption ID already exists: " + id);
        }
        validateParams(params);
        auto* assumption = new VersionedAssumptions(id, params.at("assumption"),
                                                   params.at("rationale"));
        assumptions.push_back(assumption);
        if (storageAdapter) {
            storageAdapter->save(id, toJSON(id));  // Persist to storage
        }
    }

    void* read(const std::string& id) const override {
        auto* assump = findById(id);
        if (!assump && storageAdapter) {
            // Try loading from storage
            std::string data = storageAdapter->load(id);
            if (!data.empty()) {
                // Pseudo-code: Deserialize and add to assumptions
                // assumptions.push_back(deserialize(data));
                // assump = findById(id);
            }
        }
        if (!assump) {
            throw std::invalid_argument("Assumption not found: " + id);
        }
        return assump;  // Consumer casts to VersionedAssumptions*
    }

    void update(const std::string& id, const std::map<std::string, std::string>& params) override {
        auto* assump = findById(id);
        if (!assump) {
            throw std::invalid_argument("Assumption not found: " + id);
        }
        validateParams(params);
        assump->updateContent(params.at("assumption"), params.at("rationale"));
        if (storageAdapter) {
            storageAdapter->save(id, toJSON(id));  // Persist to storage
        }
    }

    void remove(const std::string& id) override {
        auto it = std::find_if(assumptions.begin(), assumptions.end(),
                               [&id](const VersionedAssumptions* assump) { return assump->getID() == id; });
        if (it == assumptions.end()) {
            throw std::invalid_argument("Assumption not found: " + id);
        }
        delete *it;
        assumptions.erase(it);
        if (storageAdapter) {
            storageAdapter->remove(id);  // Remove from storage
        }
    }

    // Serialization
    std::string toJSON(const std::string& id) const override {
        auto* assump = findById(id);
        if (!assump) {
            throw std::invalid_argument("Assumption not found: " + id);
        }
        return assump->toFullJSON();
    }

    std::string toYAML(const std::string& id) const override {
        auto* assump = findById(id);
        if (!assump) {
            throw std::invalid_argument("Assumption not found: " + id);
        }
        std::stringstream yaml;
        yaml << "id: " << id << "\n";
        yaml << "type: ASSUMPTIONS\n";
        yaml << "content:\n";
        yaml << "  assumption: \"" << assump->toFullJSON() /* simplified: extract assumption */ << "\"\n";
        yaml << "  rationale: \"" << assump->toFullJSON() /* simplified: extract rationale */ << "\"\n";
        yaml << "relations:\n";
        for (const auto& rel : assump->getRelations()) {
            yaml << "  - " << static_cast<int>(rel) << "\n";
        }
        return yaml.str();
    }

    // KG export
    void exportToKG(const std::string& id) const override {
        auto* assump = findById(id);
        if (!assump) {
            throw std::invalid_argument("Assumption not found: " + id);
        }
        // Pseudo-code: Export node and edges to KG
        // KGClient::createNode(id, "ASSUMPTIONS", toJSON(id));
        // for (rel : assump->getRelations()) KGClient::createEdge(id, rel.target, rel.type);
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




















#endif // __ASSUMPTIONSREPOAGENT_HPP__