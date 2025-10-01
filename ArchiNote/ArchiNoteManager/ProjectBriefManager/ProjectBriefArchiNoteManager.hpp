
#ifndef __PROJECTBRIEFARCHINOTEMANAGER_HPP__
#define __PROJECTBRIEFARCHINOTEMANAGER_HPP__


#include "../../ArchiNoteAdmin/IArchiNoteManager.hpp"
#include "../../ArchiNoteAttributeRepoAgent/IAttributeRepoAgent.hpp"
#include "../../ArchiNoteStorage/IStorageAdapter.hpp"

// Project Brief Repos Agents:

#include "../../ArchiNoteAttributeRepoAgent/ProjectBriefAttributeRepoAgent/ObjectivesRepoAgent.hpp"
#include "../../ArchiNoteAttributeRepoAgent/ProjectBriefAttributeRepoAgent/TargetAudienceRepoAgent.hpp"
#include "../../ArchiNoteAttributeRepoAgent/ProjectBriefAttributeRepoAgent/PainPointsRepoAgent.hpp"
#include "../../ArchiNoteAttributeRepoAgent/ProjectBriefAttributeRepoAgent/AssumptionsRepoAgent.hpp"
#include "../../ArchiNoteAttributeRepoAgent/ProjectBriefAttributeRepoAgent/ConstraintsRepoAgent.hpp"
#include "../../ArchiNoteAttributeRepoAgent/ProjectBriefAttributeRepoAgent/ProjectOverviewRepoAgent.hpp"
#include "../../ArchiNoteAttributeRepoAgent/ProjectBriefAttributeRepoAgent/RiskRepoAgent.hpp"
#include "../../ArchiNoteAttributeRepoAgent/ProjectBriefAttributeRepoAgent/SuccessMetricsRepoAgent.hpp"
#include "../../ArchiNoteAttributeRepoAgent/ProjectBriefAttributeRepoAgent/HighLevelFeaturesRepoAgent.hpp"
#include "../../ArchiNoteAttributeRepoAgent/ProjectBriefAttributeRepoAgent/ReviewChecklistRepoAgent.hpp"








// Concrete ArchiNoteManager for Project Brief
class ProjectBriefArchiNoteManager : public IArchiNoteManager {
private:
    std::map<ArchiNoteAttributeType, IAttributeRepoAgent*> repoAgents;
    IStorageAdapter* storageAdapter = nullptr;

    // Helper: Validate intra-document relation
    bool validateIntraDocumentRelation(const std::string& sourceId, ArchiNoteAttributeType sourceType,
                                      const std::string& targetId, ArchiNoteAttributeType targetType) {
        auto sourceAgent = repoAgents.find(sourceType);
        auto targetAgent = repoAgents.find(targetType);
        if (sourceAgent == repoAgents.end() || targetAgent == repoAgents.end()) {
            return false;
        }
        return sourceAgent->second->read(sourceId) && targetAgent->second->read(targetId);
    }

public:
    ProjectBriefArchiNoteManager() {
        // Initialize default Repo Agents for Project Brief attributes
        createAgent(ArchiNoteAttributeType::KEY_OBJECTIVES);
        createAgent(ArchiNoteAttributeType::TARGET_AUDIENCE);
        createAgent(ArchiNoteAttributeType::PAIN_POINTS);
        createAgent(ArchiNoteAttributeType::ASSUMPTIONS);
        createAgent(ArchiNoteAttributeType::CONSTRAINTS);
        createAgent(ArchiNoteAttributeType::OVERVIEW);
        createAgent(ArchiNoteAttributeType::INITIAL_RISKS);
        createAgent(ArchiNoteAttributeType::SUCCESS_METRICS);
        createAgent(ArchiNoteAttributeType::HIGH_LEVEL_FEATURES);
        createAgent(ArchiNoteAttributeType::REVIEW_CHECKLIST);
    }

    ~ProjectBriefArchiNoteManager() override {
        for (auto& pair : repoAgents) {
            delete pair.second;
        }
        repoAgents.clear();
        delete storageAdapter;
    }

    // No copy
    ProjectBriefArchiNoteManager(const ProjectBriefArchiNoteManager&) = delete;
    ProjectBriefArchiNoteManager& operator=(const ProjectBriefArchiNoteManager&) = delete;

    // Move constructor/assignment
    ProjectBriefArchiNoteManager(ProjectBriefArchiNoteManager&& other) noexcept 
        : repoAgents(std::move(other.repoAgents)), storageAdapter(other.storageAdapter) {
        other.repoAgents.clear();
        other.storageAdapter = nullptr;
    }
    ProjectBriefArchiNoteManager& operator=(ProjectBriefArchiNoteManager&& other) noexcept {
        if (this != &other) {
            for (auto& pair : repoAgents) {
                delete pair.second;
            }
            repoAgents.clear();
            delete storageAdapter;
            repoAgents = std::move(other.repoAgents);
            storageAdapter = other.storageAdapter;
            other.repoAgents.clear();
            other.storageAdapter = nullptr;
        }
        return *this;
    }

    // CRUD operations for Repo Agents
    // CRUD operations for Repo Agents
    void createAgent(ArchiNoteAttributeType type) override {
        if (repoAgents.find(type) != repoAgents.end()) {
            throw std::invalid_argument("Repo Agent already exists for type");
        }
        switch (type) {
            case ArchiNoteAttributeType::KEY_OBJECTIVES:
                repoAgents[type] = new ObjectivesRepoAgent(storageAdapter);
                break;
            case ArchiNoteAttributeType::TARGET_AUDIENCE:
                repoAgents[type] = new TargetAudienceRepoAgent(storageAdapter);
                break;
            case ArchiNoteAttributeType::PAIN_POINTS:
                repoAgents[type] = new PainPointsRepoAgent(storageAdapter);
                break;
            case ArchiNoteAttributeType::ASSUMPTIONS:
                repoAgents[type] = new AssumptionsRepoAgent(storageAdapter);
                break;
            case ArchiNoteAttributeType::CONSTRAINTS:
                repoAgents[type] = new ConstraintsRepoAgent(storageAdapter);
                break;
            case ArchiNoteAttributeType::OVERVIEW:
                repoAgents[type] = new ProjectOverviewRepoAgent(storageAdapter);
                break;
            case ArchiNoteAttributeType::INITIAL_RISKS:
                repoAgents[type] = new RiskRepoAgent(storageAdapter);
                break;
            case ArchiNoteAttributeType::SUCCESS_METRICS:
                repoAgents[type] = new SuccessMetricsRepoAgent(storageAdapter);
                break;
            case ArchiNoteAttributeType::HIGH_LEVEL_FEATURES:
                repoAgents[type] = new HighLevelFeaturesRepoAgent(storageAdapter);
                break;
            case ArchiNoteAttributeType::REVIEW_CHECKLIST:
                repoAgents[type] = new ReviewChecklistRepoAgent(storageAdapter);
                break;
            default:
                throw std::invalid_argument("Unsupported attribute type for Project Brief");
        }
    }

    IAttributeRepoAgent* readAgent(ArchiNoteAttributeType type) const override {
        auto it = repoAgents.find(type);
        if (it == repoAgents.end()) {
            throw std::invalid_argument("Repo Agent not found for type");
        }
        return it->second;
    }

    void updateAgent(ArchiNoteAttributeType type, IAttributeRepoAgent* agent) override {
        auto it = repoAgents.find(type);
        if (it == repoAgents.end()) {
            throw std::invalid_argument("Repo Agent not found for type");
        }
        delete it->second;
        it->second = agent;
    }

    void removeAgent(ArchiNoteAttributeType type) override {
        auto it = repoAgents.find(type);
        if (it == repoAgents.end()) {
            throw std::invalid_argument("Repo Agent not found for type");
        }
        delete it->second;
        repoAgents.erase(it);
    }

    // Access to Repo Agents
    IAttributeRepoAgent* getRepoAgent(ArchiNoteAttributeType type) override {
        auto it = repoAgents.find(type);
        if (it == repoAgents.end()) {
            throw std::invalid_argument("Unsupported attribute type for Project Brief");
        }
        return it->second;
    }

    // Relations management (intra-document)
    void addRelation(const std::string& sourceId, ArchiNoteAttributeType sourceType, 
                     const std::string& targetId, ArchiNoteAttributeType targetType, 
                     ArchiNoteAttributeRelationType relationType) override {
        if (!validateIntraDocumentRelation(sourceId, sourceType, targetId, targetType)) {
            throw std::invalid_argument("Invalid source or target for intra-document relation");
        }
        // Update source attribute relations (pseudo-code, assumes attribute has addRelation method)
        // auto* sourceAgent = repoAgents[sourceType];
        // sourceAgent->addRelation(sourceId, {targetId, targetType, relationType});
    }

    // Serialization
    std::string toJSON(ArchiNoteAttributeType type, const std::string& id) const override {
        auto it = repoAgents.find(type);
        if (it == repoAgents.end()) {
            throw std::invalid_argument("Unsupported attribute type for Project Brief");
        }
        return it->second->toJSON(id);
    }

    std::string toYAML(ArchiNoteAttributeType type, const std::string& id) const override {
        auto it = repoAgents.find(type);
        if (it == repoAgents.end()) {
            throw std::invalid_argument("Unsupported attribute type for Project Brief");
        }
        return it->second->toYAML(id);
    }

    std::string exportAllToJSON() const override {
        std::stringstream json;
        json << "{ \"project_brief\": {";
        bool first = true;
        for (const auto& pair : repoAgents) {
            if (!first) json << ",";
            json << "\"" << static_cast<int>(pair.first) << "\": []"; // Placeholder for attributes
            first = false;
        }
        json << "} }";
        return json.str();
    }

    // Async communication
    void createAsync(ArchiNoteAttributeType type, const std::string& id, 
                     const std::map<std::string, std::string>& params, 
                     std::function<void(const std::string&, bool)> callback) override {
        auto it = repoAgents.find(type);
        if (it == repoAgents.end()) {
            callback(id, false);
            return;
        }
        it->second->createAsync(id, params, callback);
    }

    // Storage management
    void setStorageAdapter(IStorageAdapter* adapter) override {
        delete storageAdapter;
        storageAdapter = adapter;
    }

    void saveAll() override {
        if (!storageAdapter) {
            throw std::runtime_error("No storage adapter set");
        }
        for (const auto& pair : repoAgents) {
            // Pseudo-code: Save each attribute via its Repo Agent
            // std::string json = pair.second->toJSONAll(); // Assume Agent has toJSONAll
            // storageAdapter->save(pair.first, json);
        }
    }

    void loadAll() override {
        if (!storageAdapter) {
            throw std::runtime_error("No storage adapter set");
        }
        for (const auto& pair : repoAgents) {
            // Pseudo-code: Load each attribute via its Repo Agent
            // std::string json = storageAdapter->load(pair.first);
            // pair.second->loadFromJSON(json); // Assume Agent has loadFromJSON
        }
    }
};
















#endif // __PROJECTBRIEFARCHINOTEMANAGER_HPP__