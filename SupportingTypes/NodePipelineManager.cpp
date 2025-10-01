

// NodePipelineManager.cpp
#include "NodePipelineManager.hpp"
#include "GraphOrchestrator.hpp"
#include <sstream>
#include <algorithm>
#include <mutex>
using std::mutex;
using std::lock_guard;

// 1. Constructor
NodePipelineManager::NodePipelineManager(const std::string& nodeId, std::shared_ptr<GraphOrchestrator> orch)
    : node_id(nodeId), edge_ids(), path_ids(), orchestrator(orch) {}

// 2. Edge ID management
void NodePipelineManager::addEdgeId(const std::string& edgeId, bool is_incoming) {
    std::lock_guard<std::mutex> lock(mutex);
    if (!edgeId.empty() && isValidEdgeId(edgeId)) {
        edge_ids.insert(edgeId);
        if (auto node = getNode()) {
            if (is_incoming) {
                node->addIncomingEdgeId(edgeId);
            } else {
                node->addOutgoingEdgeId(edgeId);
            }
        }
    }
}

void NodePipelineManager::removeEdgeId(const std::string& edgeId) {
    std::lock_guard<std::mutex> lock(mutex);
    edge_ids.erase(edgeId);
    if (auto node = getNode()) {
        node->removeEdgeId(edgeId);
    }
}

const std::unordered_set<std::string>& NodePipelineManager::getEdgeIds() const {
    return edge_ids;
}

// 3. Path ID management
void NodePipelineManager::addPathId(const std::string& pathId) {
    std::lock_guard<std::mutex> lock(mutex);
    if (!pathId.empty() && isValidPathId(pathId)) {
        path_ids.insert(pathId);
        if (auto node = getNode()) {
            node->addPathId(pathId);
        }
    }
}

void NodePipelineManager::removePathId(const std::string& pathId) {
    std::lock_guard<std::mutex> lock(mutex);
    path_ids.erase(pathId);
    if (auto node = getNode()) {
        node->removePathId(pathId);
    }
}

const std::unordered_set<std::string>& NodePipelineManager::getPathIds() const {
    return path_ids;
}

// 4. Validation
bool NodePipelineManager::isValidEdgeId(const std::string& edgeId) const {
    if (auto orch = orchestrator.lock()) {
        if (auto edge = orch->getEdge(edgeId)) {
            return edge->isValid() && 
                   (edge->getSourceNodeId() == node_id || edge->getTargetNodeId() == node_id);
        }
    }
    return false;
}

bool NodePipelineManager::isValidPathId(const std::string& pathId) const {
    if (auto orch = orchestrator.lock()) {
        if (auto path = orch->getPath(pathId)) {
            return path->isValid() && 
                   std::any_of(path->getEdges().begin(), path->getEdges().end(),
                               [this](const std::shared_ptr<KGEdge>& edge) {
                                   return edge->getSourceNodeId() == node_id || 
                                          edge->getTargetNodeId() == node_id;
                               });
        }
    }
    return false;
}

// 5. Querying
std::shared_ptr<BaseNode> NodePipelineManager::getNode() const {
    if (auto orch = orchestrator.lock()) {
        return orch->getNode(node_id);
    }
    return nullptr;
}

std::shared_ptr<KGEdge> NodePipelineManager::fetchEdge(const std::string& edgeId) const {
    if (edge_ids.count(edgeId) && isValidEdgeId(edgeId)) {
        if (auto orch = orchestrator.lock()) {
            return orch->getEdge(edgeId);
        }
    }
    return nullptr;
}

std::shared_ptr<KGPath> NodePipelineManager::fetchPath(const std::string& pathId) const {
    if (path_ids.count(pathId) && isValidPathId(pathId)) {
        if (auto orch = orchestrator.lock()) {
            return orch->getPath(pathId);
        }
    }
    return nullptr;
}

std::string NodePipelineManager::getNodePipeline() const {
    std::lock_guard<std::mutex> lock(mutex);
    if (auto node = getNode()) {
        return node->toString();
    }
    return "";
}

// 6. Serialization
std::string NodePipelineManager::toYAML(int indent) const {
    std::lock_guard<std::mutex> lock(mutex);
    std::stringstream ss;
    std::string indentStr(indent, ' ');
    ss << indentStr << "node_pipeline:\n";
    if (auto node = getNode()) {
        ss << node->toYAML(indent + 2);
    } else {
        ss << indentStr << "  id: " << node_id << " (not found)\n";
    }
    return ss.str();
}

std::string NodePipelineManager::toJSON(int indent) const {
    std::lock_guard<std::mutex> lock(mutex);
    std::stringstream ss;
    std::string indentStr(indent, ' ');
    ss << indentStr << "{\n";
    ss << indentStr << "  \"node_pipeline\": ";
    if (auto node = getNode()) {
        ss << node->toJSON(indent + 4);
    } else {
        ss << "{\"id\": \"" << node_id << "\", \"status\": \"not found\"}";
    }
    ss << "\n" << indentStr << "}";
    return ss.str();
}

// 7. Helper methods
bool NodePipelineManager::nodeExists() const {
    if (auto orch = orchestrator.lock()) {
        return orch->getNode(node_id) != nullptr;
    }
    return false;
}