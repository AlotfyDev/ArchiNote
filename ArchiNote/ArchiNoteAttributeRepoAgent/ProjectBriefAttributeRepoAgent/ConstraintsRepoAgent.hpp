
#ifndef __CONSTRAINTSREPOAGENT_HPP__
#define __CONSTRAINTSREPOAGENT_HPP__



#include <algorithm>
#include <functional>
#include <sstream>
#include <stdexcept>
#include <iostream>

#include "../IAttributeRepoAgent.hpp"
#include "../../ArchiNoteAttribute/ProjectBriefAttributes/VersionedConstraint.hpp"
#include "../../ArchiNoteStorage/IStorageAdapter.hpp"

// Specialized Repo Agent for Constraints.hpp"


class ConstraintsRepoAgent : public IAttributeRepoAgent {
private:
    std::vector<VersionedConstraint*> constraints;  // In-memory storage
    IStorageAdapter* storageAdapter;  // Storage adapter for persistence

    // Helper: Find constraint by ID
    VersionedConstraint* findById(const std::string& id) const {
        auto it = std::find_if(constraints.begin(), constraints.end(),
                               [&id](const VersionedConstraint* cons) { return cons->getID() == id; });
        return (it != constraints.end()) ? *it : nullptr;
    }

    // Helper: Validate params map
    void validateParams(const std::map<std::string, std::string>& params) const {
        if (params.find("constraint") == params.end() || params.at("constraint").empty()) {
            throw std::invalid_argument("Missing or empty constraint");
        }
        if (params.find("impact") == params.end() || params.at("impact").empty()) {
            throw std::invalid_argument("Missing or empty impact");
        }
    }

public:
    // Constructor with IStorageAdapter
    ConstraintsRepoAgent(IStorageAdapter* adapter = nullptr) : storageAdapter(adapter) {}

    ~ConstraintsRepoAgent() override {
        for (auto* cons : constraints) {
            delete cons;  // Manual memory management
        }
        constraints.clear();
    }

    // No copy (to avoid double-delete)
    ConstraintsRepoAgent(const ConstraintsRepoAgent&) = delete;
    ConstraintsRepoAgent& operator=(const ConstraintsRepoAgent&) = delete;

    // Move constructor/assignment
    ConstraintsRepoAgent(ConstraintsRepoAgent&& other) noexcept 
        : constraints(std::move(other.constraints)), storageAdapter(other.storageAdapter) {
        other.constraints.clear();
        other.storageAdapter = nullptr;
    }
    ConstraintsRepoAgent& operator=(ConstraintsRepoAgent&& other) noexcept {
        if (this != &other) {
            for (auto* cons : constraints) {
                delete cons;
            }
            constraints.clear();
            constraints = std::move(other.constraints);
            storageAdapter = other.storageAdapter;
            other.constraints.clear();
            other.storageAdapter = nullptr;
        }
        return *this;
    }

    // CRUD operations
    void create(const std::string& id, const std::map<std::string, std::string>& params) override {
        if (findById(id)) {
            throw std::invalid_argument("Constraint ID already exists: " + id);
        }
        validateParams(params);
        auto* constraint = new VersionedConstraint(id, params.at("constraint"),
                                                  params.at("impact"));
        constraints.push_back(constraint);
        if (storageAdapter) {
            storageAdapter->save(id, toJSON(id));  // Persist to storage
        }
    }

    void* read(const std::string& id) const override {
        auto* cons = findById(id);
        if (!cons && storageAdapter) {
            // Try loading from storage
            std::string data = storageAdapter->load(id);
            if (!data.empty()) {
                // Pseudo-code: Deserialize and add to constraints
                // constraints.push_back(deserialize(data));
                // cons = findById(id);
            }
        }
        if (!cons) {
            throw std::invalid_argument("Constraint not found: " + id);
        }
        return cons;  // Consumer casts to VersionedConstraint*
    }

    void update(const std::string& id, const std::map<std::string, std::string>& params) override {
        auto* cons = findById(id);
        if (!cons) {
            throw std::invalid_argument("Constraint not found: " + id);
        }
        validateParams(params);
        cons->updateContent(params.at("constraint"), params.at("impact"));
        if (storageAdapter) {
            storageAdapter->save(id, toJSON(id));  // Persist to storage
        }
    }

    void remove(const std::string& id) override {
        auto it = std::find_if(constraints.begin(), constraints.end(),
                               [&id](const VersionedConstraint* cons) { return cons->getID() == id; });
        if (it == constraints.end()) {
            throw std::invalid_argument("Constraint not found: " + id);
        }
        delete *it;
        constraints.erase(it);
        if (storageAdapter) {
            storageAdapter->remove(id);  // Remove from storage
        }
    }

    // Serialization
    std::string toJSON(const std::string& id) const override {
        auto* cons = findById(id);
        if (!cons) {
            throw std::invalid_argument("Constraint not found: " + id);
        }
        return cons->toFullJSON();
    }

    std::string toYAML(const std::string& id) const override {
        auto* cons = findById(id);
        if (!cons) {
            throw std::invalid_argument("Constraint not found: " + id);
        }
        std::stringstream yaml;
        yaml << "id: " << id << "\n";
        yaml << "type: CONSTRAINTS\n";
        yaml << "content:\n";
        yaml << "  constraint: \"" << cons->toFullJSON() /* simplified: extract constraint */ << "\"\n";
        yaml << "  impact: \"" << cons->toFullJSON() /* simplified: extract impact */ << "\"\n";
        yaml << "relations:\n";
        for (const auto& rel : cons->getRelations()) {
            yaml << "  - " << static_cast<int>(rel) << "\n";
        }
        return yaml.str();
    }

    // KG export
    void exportToKG(const std::string& id) const override {
        auto* cons = findById(id);
        if (!cons) {
            throw std::invalid_argument("Constraint not found: " + id);
        }
        // Pseudo-code: Export node and edges to KG
        // KGClient::createNode(id, "CONSTRAINTS", toJSON(id));
        // for (rel : cons->getRelations()) KGClient::createEdge(id, rel.target, rel.type);
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
















#endif // __CONSTRAINTSREPOAGENT_HPP