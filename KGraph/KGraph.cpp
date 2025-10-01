
#include "KGraph.hpp"



// In KGraph.cpp
#include <queue>
#include <stack>
#include <set>
#include <algorithm>
#include <iostream>

#include <stdexcept>



/*

// Node management
void KnowledgeGraph::addNode(std::shared_ptr<BaseNode> node) {
    if (node && !node->getId().empty()) {
        nodes[node->getId()] = node;
    }
}

void KnowledgeGraph::removeNode(const std::string& nodeId) {
    // أولاً: نمسح كل الـ edges المتعلقة بالـ node
    removeAllRelationships(nodeId);
    
    // ثانياً: نمسح الـ node نفسه
    nodes.erase(nodeId);
    
    // ثالثاً: نمسح أي references للـ node من الـ edges الأخرى
    for (auto& pair : nodes) {
        auto& edges = pair.second->getEdges();
        for (auto it = edges.begin(); it != edges.end(); ) {
            if (it->getOtherNode(pair.first) == nodeId) {
                it = edges.erase(it);
            } else {
                ++it;
            }
        }
    }
}

std::shared_ptr<BaseNode> KnowledgeGraph::getNode(const std::string& nodeId) const {
    auto it = nodes.find(nodeId);
    return (it != nodes.end()) ? it->second : nullptr;
}

bool KnowledgeGraph::containsNode(const std::string& nodeId) const {
    return nodes.find(nodeId) != nodes.end();
}

size_t KnowledgeGraph::getNodeCount() const {
    return nodes.size();
}

std::vector<std::string> KnowledgeGraph::getAllNodeIds() const {
    std::vector<std::string> ids;
    for (const auto& pair : nodes) {
        ids.push_back(pair.first);
    }
    return ids;
}

std::vector<std::shared_ptr<BaseNode>> KnowledgeGraph::getAllNodes() const {
    std::vector<std::shared_ptr<BaseNode>> result;
    for (const auto& pair : nodes) {
        result.push_back(pair.second);
    }
    return result;
}

// Relationship management
void KnowledgeGraph::addRelationship(std::shared_ptr<BaseNode> source,
                                   std::shared_ptr<BaseNode> target,
                                   EdgeRelationshipType relType,
                                   double strength) {
    if (!source || !target) return;
    
    KGEdge edge(source->getId(), target->getId(), relType, strength);
    source->addEdge(edge);
}

void KnowledgeGraph::removeRelationship(const std::string& edgeId) {
    for (auto& pair : nodes) {
        auto& edges = pair.second->getEdges();
        for (auto it = edges.begin(); it != edges.end(); ) {
            if (it->getId() == edgeId) {
                it = edges.erase(it);
                break;
            } else {
                ++it;
            }
        }
    }
}

void KnowledgeGraph::removeRelationships(const std::string& nodeId, EdgeRelationshipType type) {
    auto node = getNode(nodeId);
    if (!node) return;
    
    auto& edges = node->getEdges();
    for (auto it = edges.begin(); it != edges.end(); ) {
        if (it->getType() == type) {
            it = edges.erase(it);
        } else {
            ++it;
        }
    }
}

void KnowledgeGraph::removeAllRelationships(const std::string& nodeId) {
    auto node = getNode(nodeId);
    if (!node) return;
    
    node->getEdges().clear();
}

// Query methods
std::vector<std::shared_ptr<BaseNode>> KnowledgeGraph::getNeighbors(
    const std::string& nodeId, 
    EdgeRelationshipType filterType) const {
    
    std::vector<std::shared_ptr<BaseNode>> neighbors;
    auto node = getNode(nodeId);
    if (!node) return neighbors;
    
    for (const auto& edge : node->getEdges()) {
        if (filterType == EdgeRelationshipType::ANY || edge.getType() == filterType) {
            auto neighbor = getNode(edge.getTargetNodeId());
            if (neighbor) {
                neighbors.push_back(neighbor);
            }
        }
    }
    
    return neighbors;
}

std::vector<KGEdge> KnowledgeGraph::getEdgesBetween(
    const std::string& sourceId, 
    const std::string& targetId) const {
    
    std::vector<KGEdge> edges;
    auto sourceNode = getNode(sourceId);
    if (!sourceNode) return edges;
    
    for (const auto& edge : sourceNode->getEdges()) {
        if (edge.getTargetNodeId() == targetId) {
            edges.push_back(edge);
        }
    }
    
    return edges;
}

std::vector<KGEdge> KnowledgeGraph::getEdgesFromNode(
    const std::string& nodeId,
    EdgeRelationshipType filterType) const {
    
    std::vector<KGEdge> edges;
    auto node = getNode(nodeId);
    if (!node) return edges;
    
    for (const auto& edge : node->getEdges()) {
        if (filterType == EdgeRelationshipType::ANY || edge.getType() == filterType) {
            edges.push_back(edge);
        }
    }
    
    return edges;
}

std::vector<KGEdge> KnowledgeGraph::getEdgesToNode(
    const std::string& nodeId,
    EdgeRelationshipType filterType) const {
    
    std::vector<KGEdge> edges;
    
    for (const auto& pair : nodes) {
        for (const auto& edge : pair.second->getEdges()) {
            if (edge.getTargetNodeId() == nodeId && 
                (filterType == EdgeRelationshipType::ANY || edge.getType() == filterType)) {
                edges.push_back(edge);
            }
        }
    }
    
    return edges;
}

// Connection checking
bool KnowledgeGraph::hasDirectConnection(const std::string& sourceId, 
                                       const std::string& targetId) const {
    return !getEdgesBetween(sourceId, targetId).empty();
}

bool KnowledgeGraph::hasConnection(const std::string& sourceId,
                                 const std::string& targetId,
                                 int maxDepth) const {
    return !findAllPaths(sourceId, targetId, maxDepth).empty();
}

// Path finding - Recursive helper function
void KnowledgeGraph::findAllPathsRecursive(const std::string& currentNodeId,
                                         const std::string& targetId,
                                         KGPath& currentPath,
                                         std::vector<KGPath>& allPaths,
                                         int maxDepth,
                                         double minStrength,
                                         std::set<std::string>& visited) const {
    
    if (currentPath.getDepth() >= maxDepth) return;
    
    if (currentNodeId == targetId) {
        allPaths.push_back(currentPath);
        return;
    }
    
    visited.insert(currentNodeId);
    
    auto currentNode = getNode(currentNodeId);
    if (!currentNode) return;
    
    for (const auto& edge : currentNode->getEdges()) {
        if (edge.getStrength() < minStrength) continue;
        
        std::string nextNodeId = edge.getTargetNodeId();
        
        if (visited.find(nextNodeId) == visited.end()) {
            KGPath newPath = currentPath;
            newPath.addSegment(nextNodeId, edge);
            
            findAllPathsRecursive(nextNodeId, targetId, newPath, allPaths, 
                                maxDepth, minStrength, visited);
        }
    }
    
    visited.erase(currentNodeId);
}

std::vector<KGPath> KnowledgeGraph::findAllPaths(
    const std::string& sourceId,
    const std::string& targetId,
    int maxDepth,
    double minStrength) const {
    
    std::vector<KGPath> allPaths;
    if (maxDepth <= 0 || !containsNode(sourceId) || !containsNode(targetId)) {
        return allPaths;
    }
    
    KGPath startPath(sourceId);
    std::set<std::string> visited;
    
    findAllPathsRecursive(sourceId, targetId, startPath, allPaths, 
                         maxDepth, minStrength, visited);
    
    return allPaths;
}

KGPath KnowledgeGraph::findShortestPath(
    const std::string& sourceId,
    const std::string& targetId,
    double minStrength) const {
    
    auto allPaths = findAllPaths(sourceId, targetId, 10, minStrength);
    if (allPaths.empty()) return KGPath();
    
    KGPath shortestPath = allPaths[0];
    for (const auto& path : allPaths) {
        if (path.getDepth() < shortestPath.getDepth()) {
            shortestPath = path;
        }
    }
    
    return shortestPath;
}

KGPath KnowledgeGraph::findStrongestPath(
    const std::string& sourceId,
    const std::string& targetId,
    int maxDepth) const {
    
    auto allPaths = findAllPaths(sourceId, targetId, maxDepth, 0.1);
    if (allPaths.empty()) return KGPath();
    
    KGPath strongestPath = allPaths[0];
    for (const auto& path : allPaths) {
        if (path.getTotalStrength() > strongestPath.getTotalStrength()) {
            strongestPath = path;
        }
    }
    
    return strongestPath;
}

std::vector<KGPath> KnowledgeGraph::findPathsByType(
    const std::string& sourceId,
    const std::string& targetId,
    EdgeRelationshipType requiredType,
    int maxDepth) const {
    
    auto allPaths = findAllPaths(sourceId, targetId, maxDepth);
    std::vector<KGPath> filteredPaths;
    
    for (const auto& path : allPaths) {
        bool hasRequiredType = true;
        for (const auto& edge : path.getEdges()) {
            if (edge.getType() != requiredType) {
                hasRequiredType = false;
                break;
            }
        }
        
        if (hasRequiredType) {
            filteredPaths.push_back(path);
        }
    }
    
    return filteredPaths;
}

// Cycle detection - Recursive helper function
void KnowledgeGraph::findCyclesRecursive(const std::string& currentNodeId,
                                       std::vector<KGPath>& cycles,
                                       std::set<std::string>& visited,
                                       std::set<std::string>& recStack,
                                       std::vector<std::string>& currentPathNodes,
                                       int maxDepth) const {
    if (currentPathNodes.size() > static_cast<size_t>(maxDepth)) return;
    
    std::cout << "Exploring node: " << currentNodeId << " (depth: " << currentPathNodes.size() - 1 << ")" << std::endl;
    
    if (visited.find(currentNodeId) != visited.end()) {
        return;
    }
    
    visited.insert(currentNodeId);
    recStack.insert(currentNodeId);
    currentPathNodes.push_back(currentNodeId);
    
    auto currentNode = getNode(currentNodeId);
    if (!currentNode) {
        recStack.erase(currentNodeId);
        currentPathNodes.pop_back();
        visited.erase(currentNodeId);
        return;
    }
    
    for (const auto& edge : currentNode->getEdges()) {
        std::string nextNodeId = edge.getTargetNodeId();
        
        if (recStack.find(nextNodeId) != recStack.end()) {
            // Cycle found
            std::cout << "Cycle detected back to: " << nextNodeId << std::endl;
            // Find start of cycle in current path
            auto it = std::find(currentPathNodes.begin(), currentPathNodes.end(), nextNodeId);
            if (it != currentPathNodes.end()) {
                KGPath cyclePath;
                cyclePath = KGPath(*it);  // Start from cycle start
                for (auto pathIt = it; pathIt != currentPathNodes.end(); ++pathIt) {
                    // Add edges between consecutive nodes (simplified, assume edges added appropriately)
                    // Note: This is approximate; in full impl, track edges in path
                    if (pathIt + 1 != currentPathNodes.end()) {
                        // Find edge between *pathIt and *(pathIt+1)
                        // For simplicity, skip detailed edge lookup
                    }
                }
                // Close with back edge to nextNodeId
                cycles.push_back(cyclePath);
                std::cout << "Cycle added (length: " << std::distance(it, currentPathNodes.end()) + 1 << ")" << std::endl;
            }
        } else if (visited.find(nextNodeId) == visited.end()) {
            findCyclesRecursive(nextNodeId, cycles, visited, recStack, currentPathNodes, maxDepth);
        }
    }
    
    recStack.erase(currentNodeId);
    currentPathNodes.pop_back();
    // Don't erase from visited for full graph traversal
}

std::vector<KGPath> KnowledgeGraph::findCycles() const {
    std::vector<KGPath> cycles;
    std::set<std::string> visited;
    
    for (const auto& pair : nodes) {
        std::cout << "Starting cycle detection from node: " << pair.first << std::endl;
        std::set<std::string> recStack;
        std::vector<std::string> currentPathNodes;
        findCyclesRecursive(pair.first, cycles, visited, recStack, currentPathNodes, 10);
    }
    
    std::cout << "Total cycles found: " << cycles.size() << std::endl;
    return cycles;
}

std::vector<KGPath> KnowledgeGraph::findCyclesInvolvingNode(const std::string& nodeId) const {
    std::vector<KGPath> allCycles = findCycles();
    std::vector<KGPath> filteredCycles;
    
    for (const auto& cycle : allCycles) {
        if (cycle.containsNode(nodeId)) {
            filteredCycles.push_back(cycle);
        }
    }
    
    return filteredCycles;
}

// Validation and analysis
bool KnowledgeGraph::validateGraph() const {
    for (const auto& pair : nodes) {
        for (const auto& edge : pair.second->getEdges()) {
            if (!containsNode(edge.getTargetNodeId())) {
                return false; // edge يشير إلى node غير موجود
            }
        }
    }
    return true;
}

std::vector<std::string> KnowledgeGraph::findOrphanedNodes() const {
    std::vector<std::string> orphanedNodes;
    
    for (const auto& pair : nodes) {
        bool hasIncoming = false;
        for (const auto& otherPair : nodes) {
            for (const auto& edge : otherPair.second->getEdges()) {
                if (edge.getTargetNodeId() == pair.first) {
                    hasIncoming = true;
                    break;
                }
            }
            if (hasIncoming) break;
        }
        
        if (!hasIncoming && pair.second->getEdges().empty()) {
            orphanedNodes.push_back(pair.first);
        }
    }
    
    return orphanedNodes;
}









// path resolvers








// Serialization
std::string KnowledgeGraph::toYAML() const {
    std::stringstream ss;
    ss << "knowledge_graph:\n";
    ss << "  nodes:\n";
    
    for (const auto& pair : nodes) {
        ss << pair.second->toYAML();
    }
    
    ss << "  edges:\n";
    for (const auto& pair : nodes) {
        for (const auto& edge : pair.second->getEdges()) {
            ss << edge.toYAML(4);
        }
    }
    
    return ss.str();
}

std::string KnowledgeGraph::toJSON() const {
    std::stringstream ss;
    ss << "{\n";
    ss << "  \"nodes\": [\n";
    for (size_t i = 0; i < nodes.size(); ++i) {
        const auto& pair = *std::next(nodes.begin(), i);
        ss << pair.second->toJSON();
        if (i < nodes.size() - 1) ss << ",\n";
    }
    ss << "\n  ],\n";
    ss << "  \"edges\": [\n";
    size_t edgeIndex = 0;
    for (const auto& pair : nodes) {
        for (const auto& edge : pair.second->getEdges()) {
            ss << edge.toJSON(4);
            if (++edgeIndex < getEdgeCount()) ss << ",\n";
        }
    }
    ss << "\n  ]\n";
    ss << "}";
    return ss.str();
}

std::string KnowledgeGraph::toPOML() const {
    std::stringstream ss;
    ss << "[knowledge_graph]\n";
    ss << "nodes = [\n";
    for (const auto& pair : nodes) {
        ss << pair.second->toPOML() << "\n";
    }
    ss << "]\n";
    ss << "edges = [\n";
    for (const auto& pair : nodes) {
        for (const auto& edge : pair.second->getEdges()) {
            ss << edge.toPOML(4) << "\n";
        }
    }
    ss << "]\n";
    return ss.str();
}

// Statistics
size_t KnowledgeGraph::getEdgeCount() const {
    size_t count = 0;
    for (const auto& pair : nodes) {
        count += pair.second->getEdges().size();
    }
    return count;
}









*/



KGraph::KGraph() : orchestrator(std::make_shared<GraphOrchestrator>()) {}

void KGraph::ingestFromArchiNotes(const std::string& yaml_data) {
    // TODO: Parse YAML, create nodes/edges/paths, delegate to orchestrator
}

void KGraph::initializeGraph(const std::string& db_path) {
    // TODO: Load graph from DB, initialize orchestrator
}

void KGraph::addNode(std::shared_ptr<BaseNode> node) {
    std::lock_guard<std::mutex> lock(mutex);
    orchestrator->addNode(node);
}

std::shared_ptr<BaseNode> KGraph::getNode(const std::string& node_id) const {
    std::lock_guard<std::mutex> lock(mutex);
    return orchestrator->getNode(node_id);
}

void KGraph::removeNode(const std::string& node_id) {
    std::lock_guard<std::mutex> lock(mutex);
    orchestrator->removeNode(node_id);
}

bool KGraph::containsNode(const std::string& node_id) const {
    std::lock_guard<std::mutex> lock(mutex);
    // TODO: Delegate to orchestrator->containsNode(node_id)
    return false;
}

size_t KGraph::getNodeCount() const {
    std::lock_guard<std::mutex> lock(mutex);
    // TODO: Delegate to orchestrator->getNodeCount()
    return 0;
}

std::vector<std::string> KGraph::getAllNodeIds() const {
    std::lock_guard<std::mutex> lock(mutex);
    // TODO: Delegate to orchestrator->getAllNodeIds()
    return {};
}

std::vector<std::shared_ptr<BaseNode>> KGraph::getAllNodes() const {
    std::lock_guard<std::mutex> lock(mutex);
    // TODO: Delegate to orchestrator->getAllNodes()
    return {};
}

void KGraph::addRelationship(std::shared_ptr<BaseNode> source, std::shared_ptr<BaseNode> target, EdgeRelationshipType rel_type, double strength) {
    std::lock_guard<std::mutex> lock(mutex);
    if (source && target) {
        orchestrator->addEdge(source->getId(), target->getId(), rel_type, "");
    }
}

void KGraph::removeRelationship(const std::string& edge_id) {
    std::lock_guard<std::mutex> lock(mutex);
    orchestrator->removeEdge(edge_id);
}

void KGraph::removeRelationships(const std::string& node_id, EdgeRelationshipType type) {
    std::lock_guard<std::mutex> lock(mutex);
    // TODO: Delegate to orchestrator->removeRelationships(node_id, type)
}

void KGraph::removeAllRelationships(const std::string& node_id) {
    std::lock_guard<std::mutex> lock(mutex);
    // TODO: Delegate to orchestrator->removeAllRelationships(node_id)
}

std::vector<std::shared_ptr<BaseNode>> KGraph::queryNodesByType(NodeType type) const {
    std::lock_guard<std::mutex> lock(mutex);
    return orchestrator->queryNodesByType(type);
}

std::vector<std::shared_ptr<KGEdge>> KGraph::queryEdgesByType(EdgeRelationshipType type) const {
    std::lock_guard<std::mutex> lock(mutex);
    return orchestrator->queryEdgesByType(type);
}

std::vector<std::shared_ptr<KGPath>> KGraph::queryPathsByType(EKGPathType type) const {
    std::lock_guard<std::mutex> lock(mutex);
    return orchestrator->queryPathsByType(type);
}

std::shared_ptr<KGPath> KGraph::findPathBetween(const std::string& start_id, const std::string& end_id) const {
    std::lock_guard<std::mutex> lock(mutex);
    return orchestrator->findPathBetween(start_id, end_id);
}

std::string KGraph::getSubgraph(const std::vector<std::string>& node_ids) const {
    std::lock_guard<std::mutex> lock(mutex);
    return orchestrator->getSubgraph(node_ids);
}

std::vector<std::shared_ptr<BaseNode>> KGraph::getNeighbors(const std::string& node_id, EdgeRelationshipType filter_type) const {
    std::lock_guard<std::mutex> lock(mutex);
    auto pipeline = createNodePipeline(node_id);
    // TODO: Delegate to pipeline->getNeighbors(filter_type)
    return {};
}

std::vector<KGEdge> KGraph::getEdgesBetween(const std::string& source_id, const std::string& target_id) const {
    std::lock_guard<std::mutex> lock(mutex);
    // TODO: Delegate to orchestrator->getEdgesBetween(source_id, target_id)
    return {};
}

std::vector<KGEdge> KGraph::getEdgesFromNode(const std::string& node_id, EdgeRelationshipType filter_type) const {
    std::lock_guard<std::mutex> lock(mutex);
    auto pipeline = createNodePipeline(node_id);
    // TODO: Delegate to pipeline->getEdgesFromNode(filter_type)
    return {};
}

std::vector<KGEdge> KGraph::getEdgesToNode(const std::string& node_id, EdgeRelationshipType filter_type) const {
    std::lock_guard<std::mutex> lock(mutex);
    // TODO: Delegate to orchestrator->getEdgesToNode(node_id, filter_type)
    return {};
}

std::vector<KGPath> KGraph::findAllPaths(const std::string& source_id, const std::string& target_id, int max_depth, double min_strength) const {
    std::lock_guard<std::mutex> lock(mutex);
    // TODO: Delegate to orchestrator->findAllPaths(source_id, target_id, max_depth, min_strength)
    return {};
}

KGPath KGraph::findShortestPath(const std::string& source_id, const std::string& target_id, double min_strength) const {
    std::lock_guard<std::mutex> lock(mutex);
    // TODO: Delegate to orchestrator->findShortestPath(source_id, target_id, min_strength)
    return KGPath();
}

KGPath KGraph::findStrongestPath(const std::string& source_id, const std::string& target_id, int max_depth) const {
    std::lock_guard<std::mutex> lock(mutex);
    // TODO: Delegate to orchestrator->findStrongestPath(source_id, target_id, max_depth)
    return KGPath();
}

std::vector<KGPath> KGraph::findPathsByType(const std::string& source_id, const std::string& target_id, EdgeRelationshipType required_type, int max_depth) const {
    std::lock_guard<std::mutex> lock(mutex);
    // TODO: Delegate to orchestrator->findPathsByType(source_id, target_id, required_type, max_depth)
    return {};
}

std::vector<KGPath> KGraph::findCycles() const {
    std::lock_guard<std::mutex> lock(mutex);
    // TODO: Delegate to orchestrator->findCycles()
    return {};
}

std::vector<KGPath> KGraph::findCyclesInvolvingNode(const std::string& node_id) const {
    std::lock_guard<std::mutex> lock(mutex);
    auto pipeline = createNodePipeline(node_id);
    // TODO: Delegate to pipeline->findCyclesInvolvingNode()
    return {};
}

bool KGraph::validateGraph() const {
    std::lock_guard<std::mutex> lock(mutex);
    // TODO: Delegate to orchestrator->validateGraph()
    return true;
}

std::vector<std::string> KGraph::findOrphanedNodes() const {
    std::lock_guard<std::mutex> lock(mutex);
    // TODO: Delegate to orchestrator->findOrphanedNodes()
    return {};
}

std::string KGraph::exportToRAG(const std::vector<std::string>& node_ids) const {
    std::lock_guard<std::mutex> lock(mutex);
    // TODO: Delegate to orchestrator->getSubgraph(node_ids) with RAG formatting
    return "";
}

std::string KGraph::retrieveForQuery(const std::string& query) const {
    std::lock_guard<std::mutex> lock(mutex);
    // TODO: Parse query, fetch relevant nodes/edges/paths via orchestrator
    return "";
}

void KGraph::saveToDatabase(const std::string& db_path) {
    std::lock_guard<std::mutex> lock(mutex);
    // TODO: Delegate to orchestrator->saveToDatabase(db_path)
}

void KGraph::loadFromDatabase(const std::string& db_path) {
    std::lock_guard<std::mutex> lock(mutex);
    // TODO: Delegate to orchestrator->loadFromDatabase(db_path)
}

void KGraph::syncWithRepo(const std::string& repo_url, const std::string& commit_id) {
    std::lock_guard<std::mutex> lock(mutex);
    orchestrator->syncWithRepo(repo_url, commit_id);
}

size_t KGraph::getEdgeCount() const {
    std::lock_guard<std::mutex> lock(mutex);
    // TODO: Delegate to orchestrator->getEdgeCount()
    return 0;
}

std::shared_ptr<NodePipelineManager> KGraph::createNodePipeline(const std::string& node_id) {
    std::lock_guard<std::mutex> lock(mutex);
    return std::make_shared<NodePipelineManager>(node_id, orchestrator);
}