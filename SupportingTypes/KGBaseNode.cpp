#include "KGBaseNode.hpp"
#include <sstream>
#include <algorithm>
#include <random>
#include <ctime>













/*
// Helper to generate ID
std::string generateId() {
    std::time_t now = std::time(nullptr);
    std::mt19937 rng(static_cast<unsigned>(now));
    std::uniform_int_distribution<int> dist(1000, 9999);
    std::stringstream ss;
    ss << "NODE_" << now << "_" << dist(rng);
    return ss.str();
}

// Constructor
BaseNode::BaseNode(NodeType nodeType, std::string nodeName) 
    : id(generateId()), name(std::move(nodeName)),type(nodeType), 
    confidence_score(1.0), 
     description(""), sources(),
    timestamp(std::time(nullptr)) {
    // Generate ID logic here
}

// إدارة الـ Attributes
void BaseNode::addAttribute(const std::shared_ptr<KGStructuredAttribute>& attribute) {
    if (attribute) {
        attributes[attribute->getName()] = attribute;
        updateTimestamp();
    }
}

bool BaseNode::removeAttribute(const std::string& attributeName) {
    auto it = attributes.find(attributeName);
    if (it != attributes.end()) {
        attributes.erase(it);
        updateTimestamp();
        return true;
    }
    return false;
}

std::shared_ptr<KGStructuredAttribute> BaseNode::getAttribute(const std::string& attributeName) const {
    auto it = attributes.find(attributeName);
    return (it != attributes.end()) ? it->second : nullptr;
}

bool BaseNode::hasAttribute(const std::string& attributeName) const {
    return attributes.find(attributeName) != attributes.end();
}

std::vector<std::string> BaseNode::getAttributeNames() const {
    std::vector<std::string> names;
    for (const auto& pair : attributes) {
        names.push_back(pair.first);
    }
    return names;
}

const std::map<std::string, std::shared_ptr<KGStructuredAttribute>>& BaseNode::getAllAttributes() const {
    return attributes;
}

// Helper methods
void BaseNode::updateTimestamp() {
    timestamp = std::time(nullptr);
}

void BaseNode::addSource(std::string source) {
    if (!source.empty()) {
        sources.push_back(source);
    }
}

void BaseNode::setConfidence(double confidence) {
    confidence_score = std::max(0.0, std::min(1.0, confidence));
}

void BaseNode::setDescription(const std::string& desc) {
    description = desc;
    updateTimestamp();
}






void BaseNode::addEdge(const KGEdge& edge) {
    Edges.push_back(edge);
}

void BaseNode::removeEdge(const std::string& edgeId) {
    Edges.erase(std::remove_if(Edges.begin(), Edges.end(), 
        [edgeId](const KGEdge& e) { return e.getId() == edgeId; }), 
        Edges.end());
}

const std::vector<KGEdge>& BaseNode::getEdges() const {
    return Edges;
}

std::vector<KGEdge>& BaseNode::getEdges() {
    return Edges;
}

std::vector<KGEdge> BaseNode::getEdgesByType(RelationshipType type) const {
    std::vector<KGEdge> filtered;
    for (const auto& edge : Edges) {
        if (edge.getType() == type) {
            filtered.push_back(edge);
        }
    }
    return filtered;
}

bool BaseNode::hasEdgeTo(const std::string& targetNodeId) const {
    for (const auto& edge : Edges) {
        if (edge.getTargetNodeId() == targetNodeId) {
            return true;
        }
    }
    return false;
}

bool BaseNode::hasEdgeType(RelationshipType type) const {
    for (const auto& edge : Edges) {
        if (edge.getType() == type) {
            return true;
        }
    }
    return false;
}

void BaseNode::addSource(std::string source) {
    if (!source.empty()) {
        sources.push_back(std::move(source));
    }
}

void BaseNode::setConfidence(double confidence) {
    confidence_score = std::max(0.0, std::min(1.0, confidence));
}






// Serialization methods
std::string BaseNode::toYAML() const {
    std::stringstream ss;
    ss << "node:\n";
    ss << "  id: " << id << "\n";
    ss << "  name: " << name << "\n";
    ss << "  type: " << static_cast<int>(type) << "\n";
    ss << "  description: " << description << "\n";
    ss << "  confidence_score: " << confidence_score << "\n";
    ss << "  timestamp: " << timestamp << "\n";
    
    ss << "  sources:\n";
    for (const auto& source : sources) {
        ss << "    - " << source << "\n";
    }
    
    ss << "  attributes:\n";
    for (const auto& pair : attributes) {
        std::string attrYAML = pair.second->toYAML();
        // Add indentation
        std::stringstream attrSS;
        std::string line;
        std::istringstream attrStream(attrYAML);
        while (std::getline(attrStream, line)) {
            attrSS << "    " << line << "\n";
        }
        ss << attrSS.str();
    }
    
    ss << "  edges:\n";
    for (const auto& edge : Edges) {
        ss << edge.toYAML(4);
    }
    
    return ss.str();
}

std::string BaseNode::serializeAttributesToYAML() const {
    std::stringstream ss;
    for (const auto& pair : attributes) {
        ss << pair.second->toYAML() << "\n";
    }
    return ss.str();
}

std::string BaseNode::serializeAttributesToJSON() const {
    std::stringstream ss;
    ss << "\"attributes\": {";
    bool first = true;
    for (const auto& pair : attributes) {
        if (!first) ss << ", ";
        ss << "\"" << pair.first << "\": " << pair.second->toJSON();
        first = false;
    }
    ss << "}";
    return ss.str();
}

std::string BaseNode::serializeAttributesToPOML() const {
    std::stringstream ss;
    for (const auto& pair : attributes) {
        ss << pair.second->toPOML() << "\n";
    }
    return ss.str();
}


*/



// 1. Default constructor
BaseNode::BaseNode()
    : id(generateId()), type(NodeType::Unknown), attributes(),
      description(""), timestamp(std::time(nullptr)),
      incoming_edge_ids(), outgoing_edge_ids(), path_ids() {}

// 2. Constructor with parameters
BaseNode::BaseNode(const std::string& id, NodeType type, const std::string& desc)
    : id(id.empty() ? generateId() : id), type(type),
      attributes(), description(desc), timestamp(std::time(nullptr)),
      incoming_edge_ids(), outgoing_edge_ids(), path_ids() {}

// 3. Generate unique ID
std::string BaseNode::generateId() {
    std::time_t now = std::time(nullptr);
    std::mt19937 rng(static_cast<unsigned int>(now));
    std::uniform_int_distribution<int> dist(1000, 9999);
    std::stringstream ss;
    ss << "NODE_" << now << "_" << dist(rng);
    return ss.str();
}

// 4. Attribute management
void BaseNode::addAttribute(std::shared_ptr<KGStructuredAttribute> attr) {
    if (attr && attr->isValid()) {
        attributes.push_back(attr);
        timestamp = std::time(nullptr);
    }
}

void BaseNode::removeAttribute(const std::string& attrId) {
    attributes.erase(
        std::remove_if(attributes.begin(), attributes.end(),
                       [&attrId](const std::shared_ptr<KGStructuredAttribute>& attr) {
                           return attr->getId() == attrId;
                       }),
        attributes.end());
    timestamp = std::time(nullptr);
}

std::shared_ptr<KGStructuredAttribute> BaseNode::getAttributeByName(const std::string& name) const {
    for (const auto& attr : attributes) {
        if (attr->getName() == name) {
            return attr;
        }
    }
    return nullptr;
}

// 5. Edge ID management
void BaseNode::addIncomingEdgeId(const std::string& edgeId) {
    if (!edgeId.empty() && 
        std::find(incoming_edge_ids.begin(), incoming_edge_ids.end(), edgeId) == incoming_edge_ids.end()) {
        incoming_edge_ids.push_back(edgeId);
        timestamp = std::time(nullptr);
    }
}

void BaseNode::addOutgoingEdgeId(const std::string& edgeId) {
    if (!edgeId.empty() && 
        std::find(outgoing_edge_ids.begin(), outgoing_edge_ids.end(), edgeId) == outgoing_edge_ids.end()) {
        outgoing_edge_ids.push_back(edgeId);
        timestamp = std::time(nullptr);
    }
}

void BaseNode::removeEdgeId(const std::string& edgeId) {
    incoming_edge_ids.erase(
        std::remove(incoming_edge_ids.begin(), incoming_edge_ids.end(), edgeId),
        incoming_edge_ids.end());
    outgoing_edge_ids.erase(
        std::remove(outgoing_edge_ids.begin(), outgoing_edge_ids.end(), edgeId),
        outgoing_edge_ids.end());
    timestamp = std::time(nullptr);
}

// 6. Path ID management
void BaseNode::addPathId(const std::string& pathId) {
    if (!pathId.empty() && 
        std::find(path_ids.begin(), path_ids.end(), pathId) == path_ids.end()) {
        path_ids.push_back(pathId);
        timestamp = std::time(nullptr);
    }
}

void BaseNode::removePathId(const std::string& pathId) {
    path_ids.erase(
        std::remove(path_ids.begin(), path_ids.end(), pathId),
        path_ids.end());
    timestamp = std::time(nullptr);
}

// 7. Setters
void BaseNode::setType(NodeType type) {
    this->type = type;
    timestamp = std::time(nullptr);
}

void BaseNode::setDescription(const std::string& desc) {
    description = desc;
    timestamp = std::time(nullptr);
}

// 8. Serialization
std::string BaseNode::toYAML(int indent) const {
    std::string indentStr(indent, ' ');
    std::stringstream ss;
    ss << indentStr << "-\n";
    ss << indentStr << "  id: " << id << "\n";
    ss << indentStr << "  type: " << NodeTypeUtils::NodeTypeString(type) << "\n";
    if (!description.empty()) {
        ss << indentStr << "  description: \"" << description << "\"\n";
    }
    if (!attributes.empty()) {
        ss << indentStr << "  attributes:\n";
        for (const auto& attr : attributes) {
            ss << attr->toYAML(indent + 4);
        }
    }
    if (!incoming_edge_ids.empty()) {
        ss << indentStr << "  incoming_edge_ids:\n";
        for (const auto& edgeId : incoming_edge_ids) {
            ss << indentStr << "    - " << edgeId << "\n";
        }
    }
    if (!outgoing_edge_ids.empty()) {
        ss << indentStr << "  outgoing_edge_ids:\n";
        for (const auto& edgeId : outgoing_edge_ids) {
            ss << indentStr << "    - " << edgeId << "\n";
        }
    }
    if (!path_ids.empty()) {
        ss << indentStr << "  path_ids:\n";
        for (const auto& pathId : path_ids) {
            ss << indentStr << "    - " << pathId << "\n";
        }
    }
    return ss.str();
}

std::string BaseNode::toJSON(int indent) const {
    std::string indentStr(indent, ' ');
    std::stringstream ss;
    ss << indentStr << "{\n";
    ss << indentStr << "  \"id\": \"" << id << "\",\n";
    ss << indentStr << "  \"type\": \"" << NodeTypeUtils::NodeTypeString(type) << "\"";
    if (!description.empty()) {
        ss << ",\n" << indentStr << "  \"description\": \"" << description << "\"";
    }
    if (!attributes.empty()) {
        ss << ",\n" << indentStr << "  \"attributes\": [\n";
        for (size_t i = 0; i < attributes.size(); ++i) {
            ss << attributes[i]->toJSON(indent + 4);
            if (i < attributes.size() - 1) ss << ",";
            ss << "\n";
        }
        ss << indentStr << "  ]";
    }
    if (!incoming_edge_ids.empty()) {
        ss << ",\n" << indentStr << "  \"incoming_edge_ids\": [\n";
        for (size_t i = 0; i < incoming_edge_ids.size(); ++i) {
            ss << indentStr << "    \"" << incoming_edge_ids[i] << "\"";
            if (i < incoming_edge_ids.size() - 1) ss << ",";
            ss << "\n";
        }
        ss << indentStr << "  ]";
    }
    if (!outgoing_edge_ids.empty()) {
        ss << ",\n" << indentStr << "  \"outgoing_edge_ids\": [\n";
        for (size_t i = 0; i < outgoing_edge_ids.size(); ++i) {
            ss << indentStr << "    \"" << outgoing_edge_ids[i] << "\"";
            if (i < outgoing_edge_ids.size() - 1) ss << ",";
            ss << "\n";
        }
        ss << indentStr << "  ]";
    }
    if (!path_ids.empty()) {
        ss << ",\n" << indentStr << "  \"path_ids\": [\n";
        for (size_t i = 0; i < path_ids.size(); ++i) {
            ss << indentStr << "    \"" << path_ids[i] << "\"";
            if (i < path_ids.size() - 1) ss << ",";
            ss << "\n";
        }
        ss << indentStr << "  ]";
    }
    ss << "\n" << indentStr << "}";
    return ss.str();
}

std::string BaseNode::toString() const {
    std::stringstream ss;
    ss << "Node(id=" << id << ", type=" << NodeTypeUtils::NodeTypeString(type)  << ")";
    if (!attributes.empty()) {
        ss << ", attributes=[";
        for (size_t i = 0; i < attributes.size(); ++i) {
            ss << attributes[i]->toString();
            if (i < attributes.size() - 1) ss << ", ";
        }
        ss << "]";
    }
    if (!incoming_edge_ids.empty()) {
        ss << ", incoming_edge_ids=[";
        for (size_t i = 0; i < incoming_edge_ids.size(); ++i) {
            ss << incoming_edge_ids[i];
            if (i < incoming_edge_ids.size() - 1) ss << ", ";
        }
        ss << "]";
    }
    if (!outgoing_edge_ids.empty()) {
        ss << ", outgoing_edge_ids=[";
        for (size_t i = 0; i < outgoing_edge_ids.size(); ++i) {
            ss << outgoing_edge_ids[i];
            if (i < outgoing_edge_ids.size() - 1) ss << ", ";
        }
        ss << "]";
    }
    if (!path_ids.empty()) {
        ss << ", path_ids=[";
        for (size_t i = 0; i < path_ids.size(); ++i) {
            ss << path_ids[i];
            if (i < path_ids.size() - 1) ss << ", ";
        }
        ss << "]";
    }
    return ss.str();
}

// 9. Validation
bool BaseNode::isValid() const {
    return !id.empty() && type != NodeType::Unknown &&
           std::all_of(attributes.begin(), attributes.end(),
                       [](const std::shared_ptr<KGStructuredAttribute>& attr) {
                           return attr->isValid();
                       });
}

// 10. Clone
std::shared_ptr<BaseNode> BaseNode::clone() const {
    auto clone = std::make_shared<BaseNode>(id, type, description);
    for (const auto& attr : attributes) {
        clone->addAttribute(attr->clone());
    }
    for (const auto& edgeId : incoming_edge_ids) {
        clone->addIncomingEdgeId(edgeId);
    }
    for (const auto& edgeId : outgoing_edge_ids) {
        clone->addOutgoingEdgeId(edgeId);
    }
    for (const auto& pathId : path_ids) {
        clone->addPathId(pathId);
    }
    return clone;
}
