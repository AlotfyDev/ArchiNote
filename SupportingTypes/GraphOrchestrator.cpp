



#include "GraphOrchestrator.hpp"
#include <sstream>
#include <random>
#include <ctime>
#include <chrono>
#include <queue>
#include <algorithm>
#include <mutex>
#include <unordered_map>
#include <unordered_set>
#include <functional>

using std::lock_guard;
using std::mutex;
using std::unordered_map;
using std::unordered_set;

// 1. Node management
void GraphOrchestrator::addNode(std::shared_ptr<BaseNode> node) {
    std::lock_guard<std::mutex> lock(mutex);
    if (node && node->isValid() && nodes.find(node->getId()) == nodes.end()) {
        nodes[node->getId()] = node;
    }
}

std::shared_ptr<BaseNode> GraphOrchestrator::getNode(const std::string& node_id) const {
    std::lock_guard<std::mutex> lock(mutex);
    auto it = nodes.find(node_id);
    return it != nodes.end() ? it->second : nullptr;
}

void GraphOrchestrator::removeNode(const std::string& node_id) {
    std::lock_guard<std::mutex> lock(mutex);
    if (auto node = getNode(node_id)) {
        // Remove associated edges
        for (const auto& edge_id : node->getIncomingEdgeIds()) {
            removeEdge(edge_id);
        }
        for (const auto& edge_id : node->getOutgoingEdgeIds()) {
            removeEdge(edge_id);
        }
        // Remove associated paths
        for (const auto& path_id : node->getPathIds()) {
            removePath(path_id);
        }
        nodes.erase(node_id);
    }
}

// 2. Edge management
void GraphOrchestrator::addEdge(const std::string& source_id, const std::string& target_id, EdgeRelationshipType type, const std::string& description) {
    std::lock_guard<std::mutex> lock(mutex);
    if (auto source = getNode(source_id)) {
        if (auto target = getNode(target_id)) {
            std::string edge_id = generateEdgeId();
            auto edge = std::make_shared<KGEdge>(edge_id, source_id, target_id, type, description);
            if (isValidEdge(edge_id)) {
                edges[edge_id] = edge;
                source->addOutgoingEdgeId(edge_id);
                target->addIncomingEdgeId(edge_id);
            }
        }
    }
}

std::shared_ptr<KGEdge> GraphOrchestrator::getEdge(const std::string& edge_id) const {
    std::lock_guard<std::mutex> lock(mutex);
    auto it = edges.find(edge_id);
    return it != edges.end() ? it->second : nullptr;
}

void GraphOrchestrator::removeEdge(const std::string& edge_id) {
    std::lock_guard<std::mutex> lock(mutex);
    if (auto edge = getEdge(edge_id)) {
        if (auto source = getNode(edge->getSourceNodeId())) {
            source->removeEdgeId(edge_id);
        }
        if (auto target = getNode(edge->getTargetNodeId())) {
            target->removeEdgeId(edge_id);
        }
        edges.erase(edge_id);
        // Update paths containing this edge
        for (auto it = paths.begin(); it != paths.end();) {
            if (std::any_of(it->second->getEdges().begin(), it->second->getEdges().end(),
                            [&edge_id](const KGEdge& e) { return e.getId() == edge_id; })) {
                removePath(it->first);
            } else {
                ++it;
            }
        }
    }
}

std::vector<std::shared_ptr<KGEdge>> GraphOrchestrator::getEdgesForNode(const std::string& node_id) const {
    std::lock_guard<std::mutex> lock(mutex);
    std::vector<std::shared_ptr<KGEdge>> result;
    if (auto node = getNode(node_id)) {
        for (const auto& edge_id : node->getIncomingEdgeIds()) {
            if (auto edge = getEdge(edge_id)) {
                result.push_back(edge);
            }
        }
        for (const auto& edge_id : node->getOutgoingEdgeIds()) {
            if (auto edge = getEdge(edge_id)) {
                result.push_back(edge);
            }
        }
    }
    return result;
}

// 3. Path management
void GraphOrchestrator::addPath(const std::vector<std::string>& edge_ids, EKGPathType type, const std::string& description) {
    std::lock_guard<std::mutex> lock(mutex);
    std::vector<std::shared_ptr<KGEdge>> path_edges;
    for (const auto& edge_id : edge_ids) {
        if (auto edge = getEdge(edge_id)) {
            path_edges.push_back(edge);
        } else {
            return; // Invalid edge
        }
    }
    if (!path_edges.empty() && !hasCycle(edge_ids)) {
        std::string path_id = generatePathId();
        std::vector<KGEdge> edges;
        for (auto& e : path_edges) edges.push_back(*e);
        auto path = std::make_shared<KGPath>(path_id, edges, type, description);
        if (isValidPath(path_id)) {
            paths[path_id] = path;
            // Update nodes in path
            std::unordered_set<std::string> node_ids;
            for (const auto& edge : path_edges) {
                node_ids.insert(edge->getSourceNodeId());
                node_ids.insert(edge->getTargetNodeId());
            }
            for (const auto& node_id : node_ids) {
                if (auto node = getNode(node_id)) {
                    node->addPathId(path_id);
                }
            }
        }
    }
}

std::shared_ptr<KGPath> GraphOrchestrator::getPath(const std::string& path_id) const {
    std::lock_guard<std::mutex> lock(mutex);
    auto it = paths.find(path_id);
    if (it == paths.end()) {
        // Log: Path not found
        std::cout << "Debug: Path with ID '" << path_id << "' not found in paths map." << std::endl;
    } else {
        // Log: Path found
        std::cout << "Debug: Path with ID '" << path_id << "' found." << std::endl;
    }
    return it != paths.end() ? it->second : nullptr;
}

void GraphOrchestrator::removePath(const std::string& path_id) {
    std::lock_guard<std::mutex> lock(mutex);
    if (auto path = getPath(path_id)) {
        std::unordered_set<std::string> node_ids;
        for (const auto& edge : path->getEdges()) {
            node_ids.insert(edge.getSourceNodeId());
            node_ids.insert(edge.getTargetNodeId());
        }
        for (const auto& node_id : node_ids) {
            if (auto node = getNode(node_id)) {
                node->removePathId(path_id);
            }
        }
        paths.erase(path_id);
    }
}

std::vector<std::shared_ptr<KGPath>> GraphOrchestrator::getPathsForNode(const std::string& node_id) const {
    std::lock_guard<std::mutex> lock(mutex);
    std::vector<std::shared_ptr<KGPath>> result;
    if (auto node = getNode(node_id)) {
        for (const auto& path_id : node->getPathIds()) {
            if (auto path = getPath(path_id)) {
                result.push_back(path);
            }
        }
    }
    return result;
}

std::shared_ptr<KGPath> GraphOrchestrator::findPathBetween(const std::string& start_id, const std::string& end_id) const {
    std::lock_guard<std::mutex> lock(mutex);
    if (!getNode(start_id) || !getNode(end_id)) {
        return nullptr;
    }
    // BFS to find shortest path
    std::queue<std::vector<std::shared_ptr<KGEdge>>> queue;
    std::unordered_set<std::string> visited;
    std::unordered_map<std::string, std::vector<std::shared_ptr<KGEdge>>> parent;

    for (const auto& edge : edges) {
        if (edge.second->getSourceNodeId() == start_id) {
            queue.push({edge.second});
            visited.insert(edge.second->getId());
            parent[edge.second->getTargetNodeId()] = {edge.second};
        }
    }

    while (!queue.empty()) {
        auto current_path = queue.front();
        queue.pop();
        auto last_edge = current_path.back();
        auto current_node = last_edge->getSourceNodeId();

        if (current_node == end_id) {
            std::string path_id = generatePathId();
            std::vector<KGEdge> edges;
            for (auto& e : current_path) edges.push_back(*e);
            auto path = std::make_shared<KGPath>(path_id, edges, EKGPathType::DEPENDENCY, "Shortest path");
            return path;
        }

        for (const auto& edge : edges) {
            if (visited.find(edge.second->getId()) == visited.end() && 
                edge.second->getSourceNodeId() == current_node) {
                visited.insert(edge.second->getId());
                auto new_path = current_path;
                new_path.push_back(edge.second);
                queue.push(new_path);
                parent[edge.second->getTargetNodeId()] = new_path;
            }
        }
    }
    return nullptr;
}

// 4. Validation
bool GraphOrchestrator::isValidEdge(const std::string& edge_id) const {
    std::lock_guard<std::mutex> lock(mutex);
    if (auto edge = getEdge(edge_id)) {
        return edge->isValid() && getNode(edge->getSourceNodeId()) && getNode(edge->getTargetNodeId());
    }
    return false;
}

bool GraphOrchestrator::isValidPath(const std::string& path_id) const {
    std::lock_guard<std::mutex> lock(mutex);
    if (auto path = getPath(path_id)) {
        if (path->getEdges().empty()) return false;
        for (size_t i = 0; i < path->getEdges().size(); ++i) {
            if (!isValidEdge(path->getEdges()[i].getId())) return false;
            if (i > 0 && path->getEdges()[i-1].getTargetNodeId() != path->getEdges()[i].getSourceNodeId()) {
                return false; // Edges not connected
            }
        }
        std::vector<std::string> edge_ids;
        for (const auto& edge : path->getEdges()) {
            edge_ids.push_back(edge.getId());
        }
        return !hasCycle(edge_ids);
    }
    return false;
}

// 5. Querying
std::vector<std::shared_ptr<BaseNode>> GraphOrchestrator::queryNodesByType(NodeType type) const {
    std::lock_guard<std::mutex> lock(mutex);
    std::vector<std::shared_ptr<BaseNode>> result;
    for (const auto& node : nodes) {
        if (node.second->getType() == type) {
            result.push_back(node.second);
        }
    }
    return result;
}

std::vector<std::shared_ptr<KGEdge>> GraphOrchestrator::queryEdgesByType(EdgeRelationshipType type) const {
    std::lock_guard<std::mutex> lock(mutex);
    std::vector<std::shared_ptr<KGEdge>> result;
    for (const auto& edge : edges) {
        if (edge.second->getRelationship() == type) {
            result.push_back(edge.second);
        }
    }
    return result;
}

std::vector<std::shared_ptr<KGPath>> GraphOrchestrator::queryPathsByType(EKGPathType type) const {
    std::lock_guard<std::mutex> lock(mutex);
    std::vector<std::shared_ptr<KGPath>> result;
    for (const auto& path : paths) {
        if (path.second->getType() == type) {
            result.push_back(path.second);
        }
    }
    return result;
}

std::string GraphOrchestrator::getSubgraph(const std::vector<std::string>& node_ids) const {
    std::lock_guard<std::mutex> lock(mutex);
    std::stringstream ss;
    ss << "subgraph:\n";
    for (const auto& node_id : node_ids) {
        if (auto node = getNode(node_id)) {
            ss << node->toYAML(2);
            for (const auto& edge_id : node->getIncomingEdgeIds()) {
                if (auto edge = getEdge(edge_id)) {
                    ss << edge->toYAML(2);
                }
            }
            for (const auto& edge_id : node->getOutgoingEdgeIds()) {
                if (auto edge = getEdge(edge_id)) {
                    ss << edge->toYAML(2);
                }
            }
            for (const auto& path_id : node->getPathIds()) {
                if (auto path = getPath(path_id)) {
                    ss << path->toYAML();
                }
            }
        }
    }
    return ss.str();
}

// 6. Serialization
std::string GraphOrchestrator::toYAML(int indent) const {
    std::lock_guard<std::mutex> lock(mutex);
    std::stringstream ss;
    std::string indentStr(indent, ' ');
    ss << indentStr << "graph:\n";
    ss << indentStr << "  nodes:\n";
    for (const auto& node : nodes) {
        ss << node.second->toYAML(indent + 4);
    }
    ss << indentStr << "  edges:\n";
    for (const auto& edge : edges) {
        ss << edge.second->toYAML(indent + 4);
    }
    ss << indentStr << "  paths:\n";
    for (const auto& path : paths) {
        ss << path.second->toYAML();
    }
    return ss.str();
}

std::string GraphOrchestrator::toJSON(int indent) const {
    
    std::lock_guard<std::mutex> lock(mutex);
    std::stringstream ss;
    std::string indentStr(indent, ' ');
    ss << indentStr << "{\n";
    ss << indentStr << "  \"graph\": {\n";
    ss << indentStr << "    \"nodes\": [\n";
    for (auto it = nodes.begin(); it != nodes.end(); ++it) {
        ss << it->second->toJSON(indent + 6);
        if (std::next(it) != nodes.end()) ss << ",";
        ss << "\n";
    }
    ss << indentStr << "    ],\n";
    ss << indentStr << "    \"edges\": [\n";
    for (auto it = edges.begin(); it != edges.end(); ++it) {
        ss << it->second->toJSON(indent + 6);
        if (std::next(it) != edges.end()) ss << ",";
        ss << "\n";
    }
    ss << indentStr << "    ],\n";
    ss << indentStr << "    \"paths\": [\n";
    for (auto it = paths.begin(); it != paths.end(); ++it) {
        ss << it->second->toJSON();
        if (std::next(it) != paths.end()) ss << ",";
        ss << "\n";
    }
    ss << indentStr << "    ]\n";
    ss << indentStr << "  }\n";
    ss << indentStr << "}";
    return ss.str();
}

// 7. Sync
void GraphOrchestrator::syncWithRepo(const std::string& repo_url, const std::string& commit_id) {
    std::lock_guard<std::mutex> lock(mutex);
    // Placeholder: Sync with repo (e.g., fetch attributes->sources, update edges/paths)
    for (auto& node : nodes) {
        for (const auto& attr : node.second->getAttributes()) {
            if (attr->getName() == "source" && attr->getValue() == repo_url + ":" + commit_id) {
                // Update node/edge/path based on repo data
            }
        }
    }
}

// 8. Helper methods
std::string GraphOrchestrator::generateEdgeId() const {
    auto now = std::chrono::high_resolution_clock::now().time_since_epoch().count();
    std::random_device rd;
    std::mt19937 rng(rd());
    std::uniform_int_distribution<long long> dist(100000, 999999);
    std::stringstream ss;
    ss << "EDGE_" << now << "_" << dist(rng);
    return ss.str();
}

std::string GraphOrchestrator::generatePathId() const {
    auto now = std::chrono::high_resolution_clock::now().time_since_epoch().count();
    std::random_device rd;
    std::mt19937 rng(rd());
    std::uniform_int_distribution<long long> dist(100000, 999999);
    std::stringstream ss;
    ss << "PATH_" << now << "_" << dist(rng);
    return ss.str();
}

bool GraphOrchestrator::hasCycle(const std::vector<std::string>& edge_ids) const {
    std::unordered_map<std::string, std::vector<std::string>> adj_list;
    std::unordered_set<std::string> all_nodes;
    for (const auto& edge_id : edge_ids) {
        if (auto edge = getEdge(edge_id)) {
            adj_list[edge->getSourceNodeId()].push_back(edge->getTargetNodeId());
            all_nodes.insert(edge->getSourceNodeId());
            all_nodes.insert(edge->getTargetNodeId());
        }
    }
    std::unordered_set<std::string> visited, rec_stack;
    std::function<bool(const std::string&)> dfs = [&](const std::string& node) {
        visited.insert(node);
        rec_stack.insert(node);
        for (const auto& neighbor : adj_list[node]) {
            if (visited.find(neighbor) == visited.end() && dfs(neighbor)) {
                return true;
            } else if (rec_stack.find(neighbor) != rec_stack.end()) {
                return true;
            }
        }
        rec_stack.erase(node);
        return false;
    };
    for (const auto& node : all_nodes) {
        if (visited.find(node) == visited.end() && dfs(node)) {
            return true;
        }
    }
    return false;
}