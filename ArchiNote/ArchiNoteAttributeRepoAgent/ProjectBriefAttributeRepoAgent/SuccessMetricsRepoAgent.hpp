
#ifndef __SUCCESSMETRICSREPOAGENT_HPP__
#define __SUCCESSMETRICSREPOAGENT_HPP__




#include <string>
#include <map>
#include <vector>
#include <functional>
#include <stdexcept>
#include <algorithm>
#include <sstream>
#include <algorithm>


#include "../IAttributeRepoAgent.hpp"
#include "../../ArchiNoteAttribute/ProjectBriefAttributes/VersionedSuccessMetric.hpp"
#include "../../ArchiNoteStorage/IStorageAdapter.hpp"



// Specialized Repo Agent for Success Metrics
class SuccessMetricsRepoAgent : public IAttributeRepoAgent {
private:
    std::vector<VersionedSuccessMetric*> metrics;  // In-memory storage
    IStorageAdapter* storageAdapter;  // Storage adapter for persistence

    // Helper: Find success metric by ID
    VersionedSuccessMetric* findById(const std::string& id) const {
        auto it = std::find_if(metrics.begin(), metrics.end(),
                               [&id](const VersionedSuccessMetric* metric) { return metric->getID() == id; });
        return (it != metrics.end()) ? *it : nullptr;
    }

    // Helper: Validate params map
    void validateParams(const std::map<std::string, std::string>& params) const {
        if (params.find("metric") == params.end() || params.at("metric").empty()) {
            throw std::invalid_argument("Missing or empty metric");
        }
        if (params.find("target") == params.end() || params.at("target").empty()) {
            throw std::invalid_argument("Missing or empty target");
        }
        if (params.find("measurement") == params.end() || params.at("measurement").empty()) {
            throw std::invalid_argument("Missing or empty measurement");
        }
    }

public:
    // Constructor with IStorageAdapter
    SuccessMetricsRepoAgent(IStorageAdapter* adapter = nullptr) : storageAdapter(adapter) {}

    ~SuccessMetricsRepoAgent() override {
        for (auto* metric : metrics) {
            delete metric;  // Manual memory management
        }
        metrics.clear();
    }

    // No copy (to avoid double-delete)
    SuccessMetricsRepoAgent(const SuccessMetricsRepoAgent&) = delete;
    SuccessMetricsRepoAgent& operator=(const SuccessMetricsRepoAgent&) = delete;

    // Move constructor/assignment
    SuccessMetricsRepoAgent(SuccessMetricsRepoAgent&& other) noexcept 
        : metrics(std::move(other.metrics)), storageAdapter(other.storageAdapter) {
        other.metrics.clear();
        other.storageAdapter = nullptr;
    }
    SuccessMetricsRepoAgent& operator=(SuccessMetricsRepoAgent&& other) noexcept {
        if (this != &other) {
            for (auto* metric : metrics) {
                delete metric;
            }
            metrics.clear();
            metrics = std::move(other.metrics);
            storageAdapter = other.storageAdapter;
            other.metrics.clear();
            other.storageAdapter = nullptr;
        }
        return *this;
    }

    // CRUD operations
    void create(const std::string& id, const std::map<std::string, std::string>& params) override {
        if (findById(id)) {
            throw std::invalid_argument("Success Metric ID already exists: " + id);
        }
        validateParams(params);
        auto* metric = new VersionedSuccessMetric(id, params.at("metric"),
                                                 params.at("target"),
                                                 params.at("measurement"));
        metrics.push_back(metric);
        if (storageAdapter) {
            storageAdapter->save(id, toJSON(id));  // Persist to storage
        }
    }

    void* read(const std::string& id) const override {
        auto* metric = findById(id);
        if (!metric && storageAdapter) {
            // Try loading from storage
            std::string data = storageAdapter->load(id);
            if (!data.empty()) {
                // Pseudo-code: Deserialize and add to metrics
                // metrics.push_back(deserialize(data));
                // metric = findById(id);
            }
        }
        if (!metric) {
            throw std::invalid_argument("Success Metric not found: " + id);
        }
        return metric;  // Consumer casts to VersionedSuccessMetric*
    }

    void update(const std::string& id, const std::map<std::string, std::string>& params) override {
        auto* metric = findById(id);
        if (!metric) {
            throw std::invalid_argument("Success Metric not found: " + id);
        }
        validateParams(params);
        metric->updateContent(params.at("metric"), params.at("target"), params.at("measurement"));
        if (storageAdapter) {
            storageAdapter->save(id, toJSON(id));  // Persist to storage
        }
    }

    void remove(const std::string& id) override {
        auto it = std::find_if(metrics.begin(), metrics.end(),
                               [&id](const VersionedSuccessMetric* metric) { return metric->getID() == id; });
        if (it == metrics.end()) {
            throw std::invalid_argument("Success Metric not found: " + id);
        }
        delete *it;
        metrics.erase(it);
        if (storageAdapter) {
            storageAdapter->remove(id);  // Remove from storage
        }
    }

    // Serialization
    std::string toJSON(const std::string& id) const override {
        auto* metric = findById(id);
        if (!metric) {
            throw std::invalid_argument("Success Metric not found: " + id);
        }
        return metric->toFullJSON();
    }

    std::string toYAML(const std::string& id) const override {
        auto* metric = findById(id);
        if (!metric) {
            throw std::invalid_argument("Success Metric not found: " + id);
        }
        std::stringstream yaml;
        yaml << "id: " << id << "\n";
        yaml << "type: SUCCESS_METRICS\n";
        yaml << "content:\n";
        yaml << "  metric: \"" << metric->toFullJSON() /* simplified: extract metric */ << "\"\n";
        yaml << "  target: \"" << metric->toFullJSON() /* simplified: extract target */ << "\"\n";
        yaml << "  measurement: \"" << metric->toFullJSON() /* simplified: extract measurement */ << "\"\n";
        yaml << "relations:\n";
        for (const auto& rel : metric->getRelations()) {
            yaml << "  - " << static_cast<int>(rel) << "\n";
        }
        return yaml.str();
    }

    // KG export
    void exportToKG(const std::string& id) const override {
        auto* metric = findById(id);
        if (!metric) {
            throw std::invalid_argument("Success Metric not found: " + id);
        }
        // Pseudo-code: Export node and edges to KG
        // KGClient::createNode(id, "SUCCESS_METRICS", toJSON(id));
        // for (rel : metric->getRelations()) KGClient::createEdge(id, rel.target, rel.type);
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










#endif // __SUCCESSMETRICSREPOAGENT_HPP__