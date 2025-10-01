


#include "KGEdge.hpp"
#include <sstream>
#include <random>
#include <iomanip>
#include <atomic>












/*
// 1. Default constructor
KGEdge::KGEdge() 
    : id(generateId()), sourceNodeId(""), targetNodeId(""), 
      type(EdgeRelationshipType::Unknown), strength(0.0), 
      description(""), sources(), timestamp(std::time(nullptr)) {}

// 2. Constructor with parameters
KGEdge::KGEdge(const std::string& sourceId, const std::string& targetId,
               EdgeRelationshipType edgeType, double edgeStrength)
    : id(generateId()), sourceNodeId(sourceId), targetNodeId(targetId),
      type(edgeType), strength(std::max(0.0, std::min(1.0, edgeStrength))),
      description(""), sources(), timestamp(std::time(nullptr)) {}

// 3. Constructor with id and description
KGEdge::KGEdge(const std::string& id, const std::string& sourceId, const std::string& targetId,
               EdgeRelationshipType edgeType, const std::string& description)
    : id(id), sourceNodeId(sourceId), targetNodeId(targetId),
      type(edgeType), strength(0.5), description(description), sources(), timestamp(std::time(nullptr)) {}

// 3. Generate unique ID
std::string KGEdge::generateId() {
    std::time_t now = std::time(nullptr);
    std::mt19937 rng(static_cast<unsigned int>(now));
    std::uniform_int_distribution<int> dist(1000, 9999);
    std::stringstream ss;
    ss << "EDGE_" << now << "_" << dist(rng);
    return ss.str();
}

// 4. Convert relationship type to string
std::string KGEdge::getRelationshipTypeAsString() const {
    switch (type) {
        case EdgeRelationshipType::DEPENDS_ON: return "DEPENDS_ON";
        case EdgeRelationshipType::IMPLEMENTS: return "IMPLEMENTS";
        case EdgeRelationshipType::USES: return "USES";
        case EdgeRelationshipType::CONTAINS: return "CONTAINS";
        case EdgeRelationshipType::EXTENDS: return "EXTENDS";
        case EdgeRelationshipType::RELATED_TO: return "RELATED_TO";
        case EdgeRelationshipType::ANY: return "ANY";
        case EdgeRelationshipType::Unknown: return "Unknown";
        default: return "Unknown";
    }
}

// 5. Setters
void KGEdge::setSourceNodeId(const std::string& sourceId) {
    sourceNodeId = sourceId;
    timestamp = std::time(nullptr);
}

void KGEdge::setTargetNodeId(const std::string& targetId) {
    targetNodeId = targetId;
    timestamp = std::time(nullptr);
}

void KGEdge::setRelationship(EdgeRelationshipType edgeType) {
    type = edgeType;
    timestamp = std::time(nullptr);
}

void KGEdge::setStrength(double newStrength) {
    strength = std::max(0.0, std::min(1.0, newStrength));
    timestamp = std::time(nullptr);
}

void KGEdge::setDescription(const std::string& desc) {
    description = desc;
    timestamp = std::time(nullptr);
}

void KGEdge::addSource(const std::string& source) {
    if (!source.empty()) {
        sources.push_back(source);
        timestamp = std::time(nullptr);
    }
}

void KGEdge::removeSource(const std::string& source) {
    sources.erase(std::remove(sources.begin(), sources.end(), source), sources.end());
    timestamp = std::time(nullptr);
}

// 6. Validation methods
bool KGEdge::isValid() const {
    return !id.empty() && 
           !sourceNodeId.empty() && 
           !targetNodeId.empty() &&
           sourceNodeId != targetNodeId &&
           type != EdgeRelationshipType::Unknown &&
           strength >= 0.0 && strength <= 1.0;
}

bool KGEdge::connectsNodes(const std::string& node1, const std::string& node2) const {
    return (sourceNodeId == node1 && targetNodeId == node2) ||
           (sourceNodeId == node2 && targetNodeId == node1);
}

bool KGEdge::involvesNode(const std::string& nodeId) const {
    return sourceNodeId == nodeId || targetNodeId == nodeId;
}

std::string KGEdge::getOtherNode(const std::string& nodeId) const {
    if (sourceNodeId == nodeId) return targetNodeId;
    if (targetNodeId == nodeId) return sourceNodeId;
    return "";
}

// 7. Serialization
std::string KGEdge::toYAML(int indent) const {
    std::string indentStr(indent, ' ');
    std::stringstream ss;
    ss << indentStr << "-\n";
    ss << indentStr << "  id: " << id << "\n";
    ss << indentStr << "  source: " << sourceNodeId << "\n";
    ss << indentStr << "  target: " << targetNodeId << "\n";
    ss << indentStr << "  type: " << getRelationshipTypeAsString() << "\n";
    ss << indentStr << "  strength: " << std::fixed << std::setprecision(2) << strength << "\n";
    ss << indentStr << "  timestamp: " << timestamp << "\n";
    if (!description.empty()) {
        ss << indentStr << "  description: \"" << description << "\"\n";
    }
    if (!sources.empty()) {
        ss << indentStr << "  sources:\n";
        for (const auto& source : sources) {
            ss << indentStr << "    - " << source << "\n";
        }
    }
    return ss.str();
}

std::string KGEdge::toJSON(int indent) const {
    std::string indentStr(indent, ' ');
    std::stringstream ss;
    ss << indentStr << "{\n";
    ss << indentStr << "  \"id\": \"" << id << "\",\n";
    ss << indentStr << "  \"source\": \"" << sourceNodeId << "\",\n";
    ss << indentStr << "  \"target\": \"" << targetNodeId << "\",\n";
    ss << indentStr << "  \"type\": \"" << getRelationshipTypeAsString() << "\",\n";
    ss << indentStr << "  \"strength\": " << std::fixed << std::setprecision(2) << strength << ",\n";
    ss << indentStr << "  \"timestamp\": " << timestamp;
    if (!description.empty()) {
        ss << ",\n" << indentStr << "  \"description\": \"" << description << "\"";
    }
    if (!sources.empty()) {
        ss << ",\n" << indentStr << "  \"sources\": [";
        for (size_t i = 0; i < sources.size(); ++i) {
            ss << "\"" << sources[i] << "\"" << (i < sources.size() - 1 ? ", " : "");
        }
        ss << "]";
    }
    ss << "\n" << indentStr << "}";
    return ss.str();
}


*/


#include "KGEdge.hpp"
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <sstream>
#include <iomanip>
#include <ctime>
#include <random>

// Constructor implementations
KGEdge::KGEdge() : id(""), sourceNodeId(""), targetNodeId(""), 
                   type(EdgeRelationshipType::UNKNOWN), strength(0.0),
                   description(""), sources(), timestamp(std::time(nullptr)) {
    id = generateId();
}

KGEdge::KGEdge(const std::string& sourceId, const std::string& targetId,
               EdgeRelationshipType edgeType, double edgeStrength) 
    : sourceNodeId(sourceId), targetNodeId(targetId), 
      type(edgeType), strength(edgeStrength),
      description(""), sources(), timestamp(std::time(nullptr)) {
    id = generateId();
}

KGEdge::KGEdge(const std::string& id, const std::string& sourceId, const std::string& targetId,
               EdgeRelationshipType edgeType, const std::string& description)
    : id(id), sourceNodeId(sourceId), targetNodeId(targetId),
      type(edgeType), strength(0.5), description(description),
      timestamp(std::time(nullptr)) {
}

// Private method to generate unique ID
std::string KGEdge::generateId() {
    std::time_t now = std::time(0);
    std::mt19937 rng(static_cast<unsigned int>(now));
    std::uniform_int_distribution<int> dist(1000, 9999);
    
    std::stringstream ss;
    ss << "EDGE_" << now << "_" << dist(rng);
    return ss.str();
}

// Get relationship type as string
std::string KGEdge::getRelationshipTypeAsString() const {
    // You'll need to implement this based on your EdgeRelationshipType enum
    // This is just a template - adjust according to your actual enum values
    switch(type) {
        case EdgeRelationshipType::DEPENDS_ON: return "DEPENDS_ON";
        case EdgeRelationshipType::IMPLEMENTS: return "IMPLEMENTS";
        case EdgeRelationshipType::USES: return "USES";
        case EdgeRelationshipType::CONTAINS: return "CONTAINS";
        case EdgeRelationshipType::EXTENDS: return "EXTENDS";
        case EdgeRelationshipType::RELATED_TO: return "RELATED_TO";
        case EdgeRelationshipType::UNKNOWN: 
        default: return "UNKNOWN";
    }
}

// Setter implementations
void KGEdge::setSourceNodeId(const std::string& sourceId) {
    sourceNodeId = sourceId;
}

void KGEdge::setTargetNodeId(const std::string& targetId) {
    targetNodeId = targetId;
}

void KGEdge::setRelationship(EdgeRelationshipType edgeType) {
    type = edgeType;
}

void KGEdge::setStrength(double newStrength) {
    strength = std::max(0.0, std::min(1.0, newStrength));
}

void KGEdge::setDescription(const std::string& desc) {
    description = desc;
}

void KGEdge::addSource(const std::string& source) {
    if (!source.empty()) {
        sources.push_back(source);
    }
}

void KGEdge::removeSource(const std::string& source) {
    sources.erase(std::remove(sources.begin(), sources.end(), source), 
                  sources.end());
}

// Validation methods
bool KGEdge::isValid() const {
    return !id.empty() && 
           !sourceNodeId.empty() && 
           !targetNodeId.empty() &&
           sourceNodeId != targetNodeId &&
           type != EdgeRelationshipType::UNKNOWN &&
           strength >= 0.0 && strength <= 1.0;
}

bool KGEdge::connectsNodes(const std::string& node1, const std::string& node2) const {
    return (sourceNodeId == node1 && targetNodeId == node2) ||
           (sourceNodeId == node2 && targetNodeId == node1);
}

bool KGEdge::involvesNode(const std::string& nodeId) const {
    return sourceNodeId == nodeId || targetNodeId == nodeId;
}

std::string KGEdge::getOtherNode(const std::string& nodeId) const {
    if (sourceNodeId == nodeId) return targetNodeId;
    if (targetNodeId == nodeId) return sourceNodeId;
    return "";
}

// Serialization methods
std::string KGEdge::toYAML(int indent) const {
    std::string indentStr(indent, ' ');
    std::stringstream ss;
    
    ss << indentStr << "-\n";
    ss << indentStr << "  id: " << id << "\n";
    ss << indentStr << "  source: " << sourceNodeId << "\n";
    ss << indentStr << "  target: " << targetNodeId << "\n";
    ss << indentStr << "  type: " << getRelationshipTypeAsString() << "\n";
    ss << indentStr << "  strength: " << std::fixed << std::setprecision(2) << strength << "\n";
    ss << indentStr << "  timestamp: " << timestamp << "\n";
    
    if (!description.empty()) {
        ss << indentStr << "  description: " << description << "\n";
    }
    
    if (!sources.empty()) {
        ss << indentStr << "  sources:\n";
        for (const auto& source : sources) {
            ss << indentStr << "    - " << source << "\n";
        }
    }
    
    return ss.str();
}

std::string KGEdge::toJSON(int indent) const {
    std::string indentStr(indent, ' ');
    std::stringstream ss;
    
    ss << indentStr << "{\n";
    ss << indentStr << "  \"id\": \"" << id << "\",\n";
    ss << indentStr << "  \"source\": \"" << sourceNodeId << "\",\n";
    ss << indentStr << "  \"target\": \"" << targetNodeId << "\",\n";
    ss << indentStr << "  \"type\": \"" << getRelationshipTypeAsString() << "\",\n";
    ss << indentStr << "  \"strength\": " << strength << ",\n";
    ss << indentStr << "  \"timestamp\": " << timestamp << ",\n";
    
    if (!description.empty()) {
        ss << indentStr << "  \"description\": \"" << description << "\",\n";
    }
    
    ss << indentStr << "  \"sources\": [";
    for (size_t i = 0; i < sources.size(); ++i) {
        if (i > 0) ss << ", ";
        ss << "\"" << sources[i] << "\"";
    }
    ss << "]\n";
    ss << indentStr << "}";
    
    return ss.str();
}