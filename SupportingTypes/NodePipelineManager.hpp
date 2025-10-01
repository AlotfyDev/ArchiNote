
#ifndef __NODEPIPELINEMANAGER_HPP__
#define __NODEPIPELINEMANAGER_HPP__





#include <string>
#include <unordered_set>
#include <memory>
#include <mutex>
#include "KGBaseNode.hpp"
#include "KGEdge.hpp"
#include "KGPath.hpp"

class GraphOrchestrator; // Forward declaration

class NodePipelineManager {
private:
    // 1. Node ID this manager handles
    std::string node_id;

    // 2. Cache for edge IDs (quick lookup)
    std::unordered_set<std::string> edge_ids;

    // 3. Cache for path IDs (quick lookup)
    std::unordered_set<std::string> path_ids;

    // 4. Reference to GraphOrchestrator (for fetching edge/path objects)
    std::weak_ptr<GraphOrchestrator> orchestrator;

    // 5. Mutex for thread safety
    mutable std::mutex mutex;

public:
    // 6. Constructor
    NodePipelineManager(const std::string& nodeId, std::shared_ptr<GraphOrchestrator> orch);

    // 7. Edge ID management
    void addEdgeId(const std::string& edgeId, bool is_incoming);
    void removeEdgeId(const std::string& edgeId);
    const std::unordered_set<std::string>& getEdgeIds() const;

    // 8. Path ID management
    void addPathId(const std::string& pathId);
    void removePathId(const std::string& pathId);
    const std::unordered_set<std::string>& getPathIds() const;

    // 9. Validation
    bool isValidEdgeId(const std::string& edgeId) const;
    bool isValidPathId(const std::string& pathId) const;

    // 10. Querying
    std::shared_ptr<BaseNode> getNode() const;
    std::shared_ptr<KGEdge> fetchEdge(const std::string& edgeId) const;
    std::shared_ptr<KGPath> fetchPath(const std::string& pathId) const;
    std::string getNodePipeline() const; // Returns node + edge_ids + path_ids

    // 11. Serialization
    std::string toYAML(int indent = 0) const;
    std::string toJSON(int indent = 0) const;

private:
    // 12. Helper methods
    bool nodeExists() const; // Checks if node_id exists in orchestrator
};



















#endif // NodePipelineManager