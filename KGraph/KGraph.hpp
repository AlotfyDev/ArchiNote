
#ifndef __KGRAPH_HPP__
#define __KGRAPH_HPP__










#include <string>
#include <vector>
#include <memory>
#include <mutex>
#include "../SupportingTypes/GraphOrchestrator.hpp"
#include "../SupportingTypes/NodePipelineManager.hpp"
#include "../SupportingTypes/KGBaseNode.hpp"
#include "../SupportingTypes/KGEdge.hpp"
#include "../SupportingTypes/KGPath.hpp"
#include "../SupportingTypes/KGStructuredAttribute.hpp"



#include <iostream>

#include <map>

#include <set>







// knowledge-graph.h


/*

class KnowledgeGraph {
private:
    std::map<std::string, std::shared_ptr<BaseNode>> nodes;
    // مش محتاجين نخزن الـ relationships بشكل منفصل بعد كده

public:
    // Node management - نفسها
    void addNode(std::shared_ptr<BaseNode> node);
    void removeNode(std::string nodeId);
    std::shared_ptr<BaseNode> getNode(std::string nodeId) const;
    
    // Relationship management - بتتغير
    void addRelationship(std::shared_ptr<BaseNode> source,
                        std::shared_ptr<BaseNode> target,
                        RelationshipType relType,
                        double strength = 0.5);
    
    void removeRelationship(const std::string& edgeId);
    void removeRelationships(const std::string& nodeId, RelationshipType type);
    
    // Query methods - أسهل بكثير
    std::vector<std::shared_ptr<BaseNode>> getNeighbors(
        const std::string& nodeId, 
        RelationshipType filterType = RelationshipType::ANY) const;
    
    std::vector<KGEdge> getEdgesBetween(
        const std::string& sourceId, 
        const std::string& targetId) const;
    
    // دورات والكلام ده
    bool hasDirectConnection(const std::string& sourceId, 
                            const std::string& targetId) const;
    std::vector<std::vector<std::string>> findPaths(
        const std::string& sourceId, 
        const std::string& targetId, 
        int maxDepth = 5) const;
    
    // Validation
    bool validateGraph() const;
    std::vector<std::string> findOrphanedNodes() const;
    std::vector<std::string> findDisconnectedSubgraphs() const;

    // Path finding methods
    std::vector<KGPath> findAllPaths(
        const std::string& sourceId,
        const std::string& targetId,
        int maxDepth = 5,
        double minStrength = 0.3) const;
    
    KGPath findShortestPath(
        const std::string& sourceId,
        const std::string& targetId,
        double minStrength = 0.3) const;
    
    KGPath findStrongestPath(
        const std::string& sourceId,
        const std::string& targetId,
        int maxDepth = 5) const;
    
    std::vector<KGPath> findPathsByType(
        const std::string& sourceId,
        const std::string& targetId,
        RelationshipType requiredType,
        int maxDepth = 5) const;
    
    // Cycle detection
    std::vector<KGPath> findCycles() const;
    std::vector<KGPath> findCyclesInvolvingNode(const std::string& nodeId) const;
    
    // Subgraph extraction
    KnowledgeGraph extractSubgraph(const std::vector<std::string>& nodeIds) const;
    KnowledgeGraph extractNeighborhood(
        const std::string& centerNodeId, 
        int radius = 2) const;

};




class KnowledgeGraph {
private:
    std::map<std::string, std::shared_ptr<BaseNode>> nodes;

    // مساعد functions للـ path finding
    void findAllPathsRecursive(const std::string& currentNodeId,
                              const std::string& targetId,
                              KGPath& currentPath,
                              std::vector<KGPath>& allPaths,
                              int maxDepth,
                              double minStrength,
                              std::set<std::string>& visited) const;

    void findCyclesRecursive(const std::string& currentNodeId,
                           std::vector<KGPath>& cycles,
                           std::set<std::string>& visited,
                           std::set<std::string>& recStack,
                           std::vector<std::string>& currentPathNodes,
                           int maxDepth) const;

public:
    KnowledgeGraph() = default;
    ~KnowledgeGraph() = default;

    // Node management
    void addNode(std::shared_ptr<BaseNode> node);
    void removeNode(const std::string& nodeId);
    std::shared_ptr<BaseNode> getNode(const std::string& nodeId) const;
    bool containsNode(const std::string& nodeId) const;
    size_t getNodeCount() const;
    std::vector<std::string> getAllNodeIds() const;
    std::vector<std::shared_ptr<BaseNode>> getAllNodes() const;

    // Relationship management
    void addRelationship(std::shared_ptr<BaseNode> source,
                        std::shared_ptr<BaseNode> target,
                        EdgeRelationshipType relType,
                        double strength = 0.5);
    
    void removeRelationship(const std::string& edgeId);
    void removeRelationships(const std::string& nodeId, EdgeRelationshipType type);
    void removeAllRelationships(const std::string& nodeId);
    
    // Query methods
    std::vector<std::shared_ptr<BaseNode>> getNeighbors(
        const std::string& nodeId, 
        EdgeRelationshipType filterType = EdgeRelationshipType::ANY) const;
    
    std::vector<KGEdge> getEdgesBetween(
        const std::string& sourceId, 
        const std::string& targetId) const;
    
    std::vector<KGEdge> getEdgesFromNode(
        const std::string& nodeId,
        EdgeRelationshipType filterType = EdgeRelationshipType::ANY) const;
    
    std::vector<KGEdge> getEdgesToNode(
        const std::string& nodeId,
        EdgeRelationshipType filterType = EdgeRelationshipType::ANY) const;

    // Connection checking
    bool hasDirectConnection(const std::string& sourceId, 
                            const std::string& targetId) const;
    
    bool hasConnection(const std::string& sourceId,
                      const std::string& targetId,
                      int maxDepth = 5) const;

    // Path finding
    std::vector<KGPath> findAllPaths(
        const std::string& sourceId,
        const std::string& targetId,
        int maxDepth = 5,
        double minStrength = 0.3) const;
    
    KGPath findShortestPath(
        const std::string& sourceId,
        const std::string& targetId,
        double minStrength = 0.3) const;
    
    KGPath findStrongestPath(
        const std::string& sourceId,
        const std::string& targetId,
        int maxDepth = 5) const;
    
    std::vector<KGPath> findPathsByType(
        const std::string& sourceId,
        const std::string& targetId,
        EdgeRelationshipType requiredType,
        int maxDepth = 5) const;

    // Cycle detection
    std::vector<KGPath> findCycles() const;
    std::vector<KGPath> findCyclesInvolvingNode(const std::string& nodeId) const;

    // Validation and analysis
    bool validateGraph() const;
    std::vector<std::string> findOrphanedNodes() const;
    std::vector<std::string> findDisconnectedSubgraphs() const;
    std::vector<std::vector<std::string>> findConnectedComponents() const;
    
    // Subgraph extraction
    KnowledgeGraph extractSubgraph(const std::vector<std::string>& nodeIds) const;
    KnowledgeGraph extractNeighborhood(
        const std::string& centerNodeId, 
        int radius = 2) const;

    // Serialization
    std::string toYAML() const;
    std::string toJSON() const;
    std::string toPOML() const;

    // Statistics
    size_t getEdgeCount() const;
    size_t getRelationshipCountByType(EdgeRelationshipType type) const;
    double getAverageNodeDegree() const;
    double getGraphDensity() const;

    // Import/Export
    bool importFromYAML(const std::string& yamlContent);
    bool importFromJSON(const std::string& jsonContent);
    bool exportToFile(const std::string& filename, const std::string& format = "yaml") const;
};

*/



// Forward declarations
class GraphOrchestrator;
class NodePipelineManager;

class KGraph {
private:
    // Storage for global orchestrator
    std::shared_ptr<GraphOrchestrator> orchestrator;

    // Mutex for thread safety
    mutable std::mutex mutex;

public:
    // Constructor
    KGraph();

    // Destructor
    virtual ~KGraph() = default;

    // 1. Ingestion & Initialization
    void ingestFromArchiNotes(const std::string& yaml_data);
    void initializeGraph(const std::string& db_path = "");

    // 2. Node Management
    void addNode(std::shared_ptr<BaseNode> node);
    std::shared_ptr<BaseNode> getNode(const std::string& node_id) const;
    void removeNode(const std::string& node_id);
    bool containsNode(const std::string& node_id) const;
    size_t getNodeCount() const;
    std::vector<std::string> getAllNodeIds() const;
    std::vector<std::shared_ptr<BaseNode>> getAllNodes() const;

    // 3. Relationship Management
    void addRelationship(std::shared_ptr<BaseNode> source, std::shared_ptr<BaseNode> target, EdgeRelationshipType rel_type, double strength = 1.0);
    void removeRelationship(const std::string& edge_id);
    void removeRelationships(const std::string& node_id, EdgeRelationshipType type);
    void removeAllRelationships(const std::string& node_id);

    // 4. Querying
    std::vector<std::shared_ptr<BaseNode>> queryNodesByType(NodeType type) const;
    std::vector<std::shared_ptr<KGEdge>> queryEdgesByType(EdgeRelationshipType type) const;
    std::vector<std::shared_ptr<KGPath>> queryPathsByType(EKGPathType type) const;
    std::shared_ptr<KGPath> findPathBetween(const std::string& start_id, const std::string& end_id) const;
    std::string getSubgraph(const std::vector<std::string>& node_ids) const;
    std::vector<std::shared_ptr<BaseNode>> getNeighbors(const std::string& node_id, EdgeRelationshipType filter_type = EdgeRelationshipType::ANY) const;
    std::vector<KGEdge> getEdgesBetween(const std::string& source_id, const std::string& target_id) const;
    std::vector<KGEdge> getEdgesFromNode(const std::string& node_id, EdgeRelationshipType filter_type = EdgeRelationshipType::ANY) const;
    std::vector<KGEdge> getEdgesToNode(const std::string& node_id, EdgeRelationshipType filter_type = EdgeRelationshipType::ANY) const;

    // 5. Path Finding
    std::vector<KGPath> findAllPaths(const std::string& source_id, const std::string& target_id, int max_depth = 10, double min_strength = 0.1) const;
    KGPath findShortestPath(const std::string& source_id, const std::string& target_id, double min_strength = 0.1) const;
    KGPath findStrongestPath(const std::string& source_id, const std::string& target_id, int max_depth = 10) const;
    std::vector<KGPath> findPathsByType(const std::string& source_id, const std::string& target_id, EdgeRelationshipType required_type, int max_depth = 10) const;
    std::vector<KGPath> findCycles() const;
    std::vector<KGPath> findCyclesInvolvingNode(const std::string& node_id) const;

    // 6. Validation and Analysis
    bool validateGraph() const;
    std::vector<std::string> findOrphanedNodes() const;

    // 7. Lite RAG Integration
    std::string exportToRAG(const std::vector<std::string>& node_ids) const;
    std::string retrieveForQuery(const std::string& query) const;

    // 8. Persistence & Storage
    void saveToDatabase(const std::string& db_path = "");
    void loadFromDatabase(const std::string& db_path = "");
    void syncWithRepo(const std::string& repo_url, const std::string& commit_id);

    // 9. Statistics
    size_t getEdgeCount() const;

private:
    // Helper methods
    std::shared_ptr<NodePipelineManager> createNodePipeline(const std::string& node_id);
};































#endif // __KGRAPH_HPP