


// KGPath.cpp
#include "KGPath.hpp"
#include <algorithm>
#include <numeric>
#include <atomic>



/*
KGPath::KGPath() : totalStrength(0.0), depth(0) {}

KGPath::KGPath(const std::string& startNodeId) : 
    totalStrength(0.0), depth(0) {
    nodeIds.push_back(startNodeId);
}

void KGPath::addSegment(const std::string& nextNodeId, const KGEdge& edge) {
    if (nodeIds.empty()) {
        nodeIds.push_back(edge.getSourceNodeId());
    }
    
    nodeIds.push_back(nextNodeId);
    edges.push_back(edge);
    totalStrength += edge.getStrength();
    depth++;
}

void KGPath::removeLastSegment() {
    if (!edges.empty()) {
        totalStrength -= edges.back().getStrength();
        edges.pop_back();
        nodeIds.pop_back();
        depth--;
    }
}

bool KGPath::containsNode(const std::string& nodeId) const {
    return std::find(nodeIds.begin(), nodeIds.end(), nodeId) != nodeIds.end();
}

bool KGPath::containsEdge(const std::string& edgeId) const {
    for (const auto& edge : edges) {
        if (edge.getId() == edgeId) {
            return true;
        }
    }
    return false;
}

const std::vector<std::string>& KGPath::getNodeIds() const {
    return nodeIds;
}

const std::vector<KGEdge>& KGPath::getEdges() const {
    return edges;
}

double KGPath::getTotalStrength() const {
    return totalStrength;
}

int KGPath::getDepth() const {
    return depth;
}

size_t KGPath::getLength() const {
    return nodeIds.size();
}

std::string KGPath::getStartNodeId() const {
    return nodeIds.empty() ? "" : nodeIds.front();
}

std::string KGPath::getEndNodeId() const {
    return nodeIds.empty() ? "" : nodeIds.back();
}

KGEdge KGPath::getLastEdge() const {
    return edges.empty() ? KGEdge() : edges.back();
}

bool KGPath::isValid() const {
    if (nodeIds.size() != edges.size() + 1) {
        return false;
    }
    
    // التأكد من أن الـ Edges تربط الـ Nodes بشكل صحيح
    for (size_t i = 0; i < edges.size(); ++i) {
        if (edges[i].getSourceNodeId() != nodeIds[i] || 
            edges[i].getTargetNodeId() != nodeIds[i + 1]) {
            return false;
        }
    }
    
    return true;
}

bool KGPath::isCyclic() const {
    if (nodeIds.size() < 2) return false;
    
    // المسار فيه دورة إذا كان آخر node موجود في أي مكان آخر في المسار
    const std::string& lastNode = nodeIds.back();
    for (size_t i = 0; i < nodeIds.size() - 1; ++i) {
        if (nodeIds[i] == lastNode) {
            return true;
        }
    }
    
    return false;
}

void KGPath::clear() {
    nodeIds.clear();
    edges.clear();
    totalStrength = 0.0;
    depth = 0;
}

bool KGPath::isEmpty() const {
    return nodeIds.empty();
}

std::string KGPath::toString() const {
    if (nodeIds.empty()) return "Empty Path";
    
    std::string result = "Path: " + nodeIds[0];
    for (size_t i = 0; i < edges.size(); ++i) {
        result += " --" + edges[i].getRelationshipTypeAsString() + "--> " + nodeIds[i + 1];
    }
    result += " (Strength: " + std::to_string(totalStrength) + ")";
    return result;
}
*/



// 1. Default constructor
KGPath::KGPath()
    : path_id(generateId()), edges(), nodeIds(), last_updated(std::time(nullptr)), description(""), depth(0), totalStrength(0.0), type(EKGPathType::Unknown) {}

// 2. Constructor with description
KGPath::KGPath(const std::string& desc)
    : path_id(generateId()), edges(), nodeIds(), last_updated(std::time(nullptr)), description(desc), depth(0), totalStrength(0.0), type(EKGPathType::Unknown) {}

// 3. Constructor with start node ID
KGPath::KGPath(const std::string& startNodeId, const std::string& desc)
    : path_id(generateId()), edges(), nodeIds{startNodeId}, last_updated(std::time(nullptr)), description(desc), depth(0), totalStrength(0.0), type(EKGPathType::Unknown) {}

// 4. Copy constructor
KGPath::KGPath(const KGPath& other)
    : path_id(other.path_id), edges(other.edges), nodeIds(other.nodeIds),
      last_updated(other.last_updated), description(other.description),
      depth(other.depth), totalStrength(other.totalStrength), type(other.type) {}

// 5. Constructor with pathId, edges, type, desc
KGPath::KGPath(const std::string& pathId, const std::vector<KGEdge>& pathEdges, EKGPathType pathType, const std::string& desc)
    : path_id(pathId), edges(pathEdges), nodeIds(), last_updated(std::time(nullptr)), description(desc), depth(static_cast<int>(pathEdges.size())), totalStrength(0.0), type(pathType) {
    if (!edges.empty()) {
        nodeIds.push_back(edges[0].getSourceNodeId());
        for (const auto& edge : edges) {
            nodeIds.push_back(edge.getTargetNodeId());
        }
        for (const auto& edge : edges) {
            totalStrength += edge.getStrength();
        }
        totalStrength /= edges.size();
    }
}

// 6. Move constructor
KGPath::KGPath(KGPath&& other) noexcept
    : path_id(std::move(other.path_id)), edges(std::move(other.edges)), nodeIds(std::move(other.nodeIds)),
      last_updated(other.last_updated), description(std::move(other.description)),
      depth(other.depth), totalStrength(other.totalStrength), type(other.type) {
    other.depth = 0;
    other.totalStrength = 0.0;
    other.type = EKGPathType::Unknown;
}

// 7. Generate unique path ID
std::string KGPath::generateId() {
    return "PATH_" + std::to_string(std::time(nullptr));
}

// 8. Semantic check for edge types (software engineering domain)
bool KGPath::isValidEdgeForTypes(NodeType from, NodeType to, const std::string& relationship) const {
    // 8.1 Rule: PROJECT -> BACKLOG with DEPENDS_ON
    if (from == NodeType::PROJECT && to == NodeType::BACKLOG && relationship == "DEPENDS_ON") {
        return true;
    }
    // 8.2 Rule: BACKLOG -> TASK with CONTAINS
    if (from == NodeType::BACKLOG && to == NodeType::TASK && relationship == "CONTAINS") {
        return true;
    }
    // 8.3 Rule: PROJECT -> TASK with IMPLEMENTS
    if (from == NodeType::PROJECT && to == NodeType::TASK && relationship == "IMPLEMENTS") {
        return true;
    }
    // 8.4 Open for additional rules for software engineering specs
    return false;
}