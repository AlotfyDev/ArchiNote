
#ifndef __GRAPHORCHESTRATOR_HPP__
#define __GRAPHORCHESTRATOR_HPP__


#include <string>
#include <unordered_map>
#include <memory>
#include <mutex>
#include "KGBaseNode.hpp"
#include "KGEdge.hpp"
#include "KGPath.hpp"
#include "EKGNodeType.hpp"
#include "EKGRelation.hpp"
#include "EKGPathType.hpp"

class GraphOrchestrator {
private:
    // 1. Storage for nodes, edges, and paths
    std::unordered_map<std::string, std::shared_ptr<BaseNode>> nodes;
    std::unordered_map<std::string, std::shared_ptr<KGEdge>> edges;
    std::unordered_map<std::string, std::shared_ptr<KGPath>> paths;
    mutable std::mutex mutex;  // Thread safety

public:
    // 3. Constructor
    GraphOrchestrator() = default;

    // 4. Node management
    void addNode(std::shared_ptr<BaseNode> node);
    std::shared_ptr<BaseNode> getNode(const std::string& node_id) const;
    void removeNode(const std::string& node_id);

    // 5. Edge management
    void addEdge(const std::string& source_id, const std::string& target_id, EdgeRelationshipType type, const std::string& description = "");
    std::shared_ptr<KGEdge> getEdge(const std::string& edge_id) const;
    void removeEdge(const std::string& edge_id);
    std::vector<std::shared_ptr<KGEdge>> getEdgesForNode(const std::string& node_id) const;

    // 6. Path management
    void addPath(const std::vector<std::string>& edge_ids, EKGPathType type, const std::string& description = "");
    std::shared_ptr<KGPath> getPath(const std::string& path_id) const;
    void removePath(const std::string& path_id);
    std::vector<std::shared_ptr<KGPath>> getPathsForNode(const std::string& node_id) const;
    std::shared_ptr<KGPath> findPathBetween(const std::string& start_id, const std::string& end_id) const;

    // 7. Validation
    bool isValidEdge(const std::string& edge_id) const;
    bool isValidPath(const std::string& path_id) const;

    // 8. Querying
    std::vector<std::shared_ptr<BaseNode>> queryNodesByType(NodeType type) const;
    std::vector<std::shared_ptr<KGEdge>> queryEdgesByType(EdgeRelationshipType type) const;
    std::vector<std::shared_ptr<KGPath>> queryPathsByType(EKGPathType type) const;
    std::string getSubgraph(const std::vector<std::string>& node_ids) const;

    // 9. Serialization
    std::string toYAML(int indent = 0) const;
    std::string toJSON(int indent = 0) const;

    // 10. Sync
    void syncWithRepo(const std::string& repo_url, const std::string& commit_id);

private:
    // 11. Helper methods
    std::string generateEdgeId() const;
    std::string generatePathId() const;
    bool hasCycle(const std::vector<std::string>& edge_ids) const;
};


















#endif // __GRAPHORCHESTRATOR_HPP__