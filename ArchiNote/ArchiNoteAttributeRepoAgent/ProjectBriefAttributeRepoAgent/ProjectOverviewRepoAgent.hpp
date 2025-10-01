
#ifndef __PROJECTBRIEFATTRIBUTEAGENT_HPP__
#define __PROJECTBRIEFATTRIBUTEAGENT_HPP__





#include <algorithm>

#include "../IAttributeRepoAgent.hpp"
#include "../../ArchiNoteAttribute/ProjectBriefAttributes/VersionedProjectOverview.hpp"
#include "../../ArchiNoteStorage/IStorageAdapter.hpp"



class ProjectOverviewRepoAgent : public IAttributeRepoAgent {
private:
    std::vector<VersionedProjectOverview*> overviews;  // In-memory storage
    IStorageAdapter* storageAdapter;  // Storage adapter for persistence

    // Helper: Find project overview by ID
    VersionedProjectOverview* findById(const std::string& id) const {
        auto it = std::find_if(overviews.begin(), overviews.end(),
                               [&id](const VersionedProjectOverview* ov) { return ov->getID() == id; });
        return (it != overviews.end()) ? *it : nullptr;
    }

    // Helper: Validate params map
    void validateParams(const std::map<std::string, std::string>& params) const {
        if (params.find("title") == params.end() || params.at("title").empty()) {
            throw std::invalid_argument("Missing or empty title");
        }
        if (params.find("brief") == params.end() || params.at("brief").empty()) {
            throw std::invalid_argument("Missing or empty brief");
        }
        if (params.find("date") == params.end() || params.at("date").empty()) {
            throw std::invalid_argument("Missing or empty date");
        }
    }

public:
    // Constructor with IStorageAdapter
    ProjectOverviewRepoAgent(IStorageAdapter* adapter = nullptr) : storageAdapter(adapter) {}

    ~ProjectOverviewRepoAgent() override {
        for (auto* ov : overviews) {
            delete ov;  // Manual memory management
        }
        overviews.clear();
    }

    // No copy (to avoid double-delete)
    ProjectOverviewRepoAgent(const ProjectOverviewRepoAgent&) = delete;
    ProjectOverviewRepoAgent& operator=(const ProjectOverviewRepoAgent&) = delete;

    // Move constructor/assignment
    ProjectOverviewRepoAgent(ProjectOverviewRepoAgent&& other) noexcept 
        : overviews(std::move(other.overviews)), storageAdapter(other.storageAdapter) {
        other.overviews.clear();
        other.storageAdapter = nullptr;
    }
    ProjectOverviewRepoAgent& operator=(ProjectOverviewRepoAgent&& other) noexcept {
        if (this != &other) {
            for (auto* ov : overviews) {
                delete ov;
            }
            overviews.clear();
            overviews = std::move(other.overviews);
            storageAdapter = other.storageAdapter;
            other.overviews.clear();
            other.storageAdapter = nullptr;
        }
        return *this;
    }

    // CRUD operations
    void create(const std::string& id, const std::map<std::string, std::string>& params) override {
        if (findById(id)) {
            throw std::invalid_argument("Project Overview ID already exists: " + id);
        }
        validateParams(params);
        auto* overview = new VersionedProjectOverview(id, params.at("title"),
                                                     params.at("brief"),
                                                     params.at("date"));
        overviews.push_back(overview);
        if (storageAdapter) {
            storageAdapter->save(id, toJSON(id));  // Persist to storage
        }
    }

    void* read(const std::string& id) const override {
        auto* ov = findById(id);
        if (!ov && storageAdapter) {
            // Try loading from storage
            std::string data = storageAdapter->load(id);
            if (!data.empty()) {
                // Pseudo-code: Deserialize and add to overviews
                // overviews.push_back(deserialize(data));
                // ov = findById(id);
            }
        }
        if (!ov) {
            throw std::invalid_argument("Project Overview not found: " + id);
        }
        return ov;  // Consumer casts to VersionedProjectOverview*
    }

    void update(const std::string& id, const std::map<std::string, std::string>& params) override {
        auto* ov = findById(id);
        if (!ov) {
            throw std::invalid_argument("Project Overview not found: " + id);
        }
        validateParams(params);
        ov->updateContent(params.at("title"), params.at("brief"), params.at("date"));
        if (storageAdapter) {
            storageAdapter->save(id, toJSON(id));  // Persist to storage
        }
    }

    void remove(const std::string& id) override {
        auto it = std::find_if(overviews.begin(), overviews.end(),
                               [&id](const VersionedProjectOverview* ov) { return ov->getID() == id; });
        if (it == overviews.end()) {
            throw std::invalid_argument("Project Overview not found: " + id);
        }
        delete *it;
        overviews.erase(it);
        if (storageAdapter) {
            storageAdapter->remove(id);  // Remove from storage
        }
    }

    // Serialization
    std::string toJSON(const std::string& id) const override {
        auto* ov = findById(id);
        if (!ov) {
            throw std::invalid_argument("Project Overview not found: " + id);
        }
        return ov->toFullJSON();
    }

    std::string toYAML(const std::string& id) const override {
        auto* ov = findById(id);
        if (!ov) {
            throw std::invalid_argument("Project Overview not found: " + id);
        }
        std::stringstream yaml;
        yaml << "id: " << id << "\n";
        yaml << "type: OVERVIEW\n";
        yaml << "content:\n";
        yaml << "  title: \"" << ov->toFullJSON() /* simplified: extract title */ << "\"\n";
        yaml << "  brief: \"" << ov->toFullJSON() /* simplified: extract brief */ << "\"\n";
        yaml << "  date: \"" << ov->toFullJSON() /* simplified: extract date */ << "\"\n";
        yaml << "relations:\n";
        for (const auto& rel : ov->getRelations()) {
            yaml << "  - " << static_cast<int>(rel) << "\n";
        }
        return yaml.str();
    }

    // KG export
    void exportToKG(const std::string& id) const override {
        auto* ov = findById(id);
        if (!ov) {
            throw std::invalid_argument("Project Overview not found: " + id);
        }
        // Pseudo-code: Export node and edges to KG
        // KGClient::createNode(id, "OVERVIEW", toJSON(id));
        // for (rel : ov->getRelations()) KGClient::createEdge(id, rel.target, rel.type);
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

















#endif // __PROJECTBRIEFATTRIBUTEAGENT_HPP__