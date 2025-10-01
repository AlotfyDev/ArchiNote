
#ifndef __KGEDGE_HPP__
#define __KGEDGE_HPP__

#include <iostream>
#include <string>
#include <vector>
#include <algorithm>

#include <sstream>
#include <iomanip>
#include <ctime>
#include <random>
#include <atomic>
#include <cstdint>

#include "EKGRelation.hpp"




class BaseNode; 

class KGEdge {
private:
    // 1. Unique identifier for the edge
    std::string id;
    
    // 2. Source and target node IDs
    std::string sourceNodeId;
    std::string targetNodeId;
    
    // 3. Semantic relationship type
    EdgeRelationshipType type;
    
    // 4. Confidence score (0.0 to 1.0)
    double strength;
    
    // 5. Semantic description
    std::string description;
    
    // 6. List of data sources (e.g., documents, references)
    std::vector<std::string> sources;
    
    // 7. Timestamp for creation/update
    std::time_t timestamp;

public:
    // 8. Constructors
    KGEdge();
    KGEdge(const std::string& sourceId, const std::string& targetId,
           EdgeRelationshipType edgeType, double edgeStrength = 0.5);
    KGEdge(const std::string& id, const std::string& sourceId, const std::string& targetId,
           EdgeRelationshipType edgeType, const std::string& description);

    // 9. Destructor
    ~KGEdge() = default;

    // 10. Getters (inline for performance)
    std::string getId() const { return id; }
    std::string getSourceNodeId() const { return sourceNodeId; }
    std::string getTargetNodeId() const { return targetNodeId; }
    EdgeRelationshipType getRelationship() const { return type; }
    double getStrength() const { return strength; }
    std::string getDescription() const { return description; }
    const std::vector<std::string>& getSources() const { return sources; }
    std::time_t getTimestamp() const { return timestamp; }

    // 11. Convert relationship type to string
    std::string getRelationshipTypeAsString() const;

    // 12. Setters
    void setSourceNodeId(const std::string& sourceId);
    void setTargetNodeId(const std::string& targetId);
    void setRelationship(EdgeRelationshipType edgeType);
    void setStrength(double newStrength);
    void setDescription(const std::string& desc);
    void addSource(const std::string& source);
    void removeSource(const std::string& source);

    // 13. Validation methods
    bool isValid() const;
    bool connectsNodes(const std::string& node1, const std::string& node2) const;
    bool involvesNode(const std::string& nodeId) const;
    std::string getOtherNode(const std::string& nodeId) const;

    // 14. Serialization
    std::string toYAML(int indent = 0) const;
    std::string toJSON(int indent = 0) const;

    // 15. Operators (inline for performance)
    bool operator==(const KGEdge& other) const { return id == other.id; }
    bool operator!=(const KGEdge& other) const { return !(*this == other); }
    friend std::ostream& operator<<(std::ostream& os, const KGEdge& edge) {
        os << edge.toYAML();
        return os;
    }

private:
    // 16. Generate unique ID
    std::string generateId();
};











#endif // __KGEDGE_HPP__



/*
class KGEdge
{
    private:
    std::string id;
    std::string sourceNodeId;  // علشان منع circular dependencies
    std::string targetNodeId;
    RelationshipType type;
    double strength;
    std::string description;
    std::vector<std::string> sources;  // مصادر إثبات العلاقة

public:
    KGEdge() : id(""), sourceNodeId(""), targetNodeId(""), type(RelationshipType::Unknown), strength(0.0), description(""), sources() {}
    KGEdge(const std::string& sourceId,
           const std::string& targetId,
           RelationshipType edgeType,
           double edgeStrength = 0.5);
    
    // Getters
    std::string getId() const { return id; }
    std::string getSourceNodeId() const { return sourceNodeId; }
    std::string getTargetNodeId() const { return targetNodeId; }
    RelationshipType getType() const { return type; }
    double getStrength() const { return strength; }
    std::string getTypeAsString() const {
        switch(type) {
            case RelationshipType::DEPENDS_ON: return "DEPENDS_ON";
            case RelationshipType::IMPLEMENTS: return "IMPLEMENTS";
            case RelationshipType::USES: return "USES";
            case RelationshipType::CONTAINS: return "CONTAINS";
            case RelationshipType::EXTENDS: return "EXTENDS";
            case RelationshipType::RELATED_TO: return "RELATED_TO";
            case RelationshipType::ANY: return "ANY";
            case RelationshipType::Unknown: return "Unknown";
            default: return "Unknown";
        }
    }
    // Setters
    void setStrength(double newStrength);
    void setDescription(const std::string& desc);
    void addSource(const std::string& source);
    
    // Validation
    bool isValid() const;
    
    // Serialization
    std::string toYAML() const;
    std::string toJSON() const;
    std::string toPOML() const;
};




class KGEdge
{
private:
    std::string id;
    std::string sourceNodeId;
    std::string targetNodeId;
    RelationshipType type;
    double strength;
    std::string description;
    std::vector<std::string> sources;
    std::time_t timestamp;

    // Generate unique ID
    std::string generateId() {
        std::time_t now = std::time(0);
        std::mt19937 rng(static_cast<unsigned int>(now));
        std::uniform_int_distribution<int> dist(1000, 9999);
        
        std::stringstream ss;
        ss << "EDGE_" << now << "_" << dist(rng);
        return ss.str();
    }

public:
    KGEdge() : id(""), sourceNodeId(""), targetNodeId(""), 
               type(RelationshipType::Unknown), strength(0.0),
               description(""), sources(), timestamp(std::time(nullptr)) {}
    
    KGEdge(const std::string& sourceId,
           const std::string& targetId,
           RelationshipType edgeType,
           double edgeStrength = 0.5) : 
        sourceNodeId(sourceId), targetNodeId(targetId), 
        type(edgeType), strength(edgeStrength),
        description(""), sources(), timestamp(std::time(nullptr)) {
        id = generateId();
    }
    
    // Getters
    std::string getId() const { return id; }
    std::string getSourceNodeId() const { return sourceNodeId; }
    std::string getTargetNodeId() const { return targetNodeId; }
    RelationshipType getRelationship() const { return type; }
    double getStrength() const { return strength; }
    std::string getDescription() const { return description; }
    const std::vector<std::string>& getSources() const { return sources; }
    std::time_t getTimestamp() const { return timestamp; }

    
    std::string getRelationshipTypeAsString() const {
        switch(type) {
            case RelationshipType::DEPENDS_ON: return "DEPENDS_ON";
            case RelationshipType::IMPLEMENTS: return "IMPLEMENTS";
            case RelationshipType::USES: return "USES";
            case RelationshipType::CONTAINS: return "CONTAINS";
            case RelationshipType::EXTENDS: return "EXTENDS";
            case RelationshipType::RELATED_TO: return "RELATED_TO";
            case RelationshipType::ANY: return "ANY";
            case RelationshipType::Unknown: return "Unknown";
            default: return "Unknown";
        }
    }
    
    // Setters
    void setStrength(double newStrength) { 
        strength = std::max(0.0, std::min(1.0, newStrength)); 
    }
    
    void setDescription(const std::string& desc) { 
        description = desc; 
    }
    
    void addSource(const std::string& source) { 
        if (!source.empty()) {
            sources.push_back(source); 
        }
    }
    
    void removeSource(const std::string& source) {
        sources.erase(std::remove(sources.begin(), sources.end(), source), 
                     sources.end());
    }
    
    // Validation
    bool isValid() const {
        return !id.empty() && 
               //!sourceNodeId.empty() && 
               //!targetNodeId.empty() &&
               sourceNodeId != targetNodeId &&
               type != RelationshipType::Unknown &&
               strength >= 0.0 && strength <= 1.0;
    }
    
    // Check if this edge connects two specific nodes
    bool connectsNodes(const std::string& node1, const std::string& node2) const {
        return (sourceNodeId == node1 && targetNodeId == node2) ||
               (sourceNodeId == node2 && targetNodeId == node1);
    }
    
    // Check if this edge involves a specific node
    bool involvesNode(const std::string& nodeId) const {
        return sourceNodeId == nodeId || targetNodeId == nodeId;
    }
    
    // Get the other node in the edge
    std::string getOtherNode(const std::string& nodeId) const {
        if (sourceNodeId == nodeId) return targetNodeId;
        if (targetNodeId == nodeId) return sourceNodeId;
        return "";
    }
    
    // Serialization
    std::string toYAML(int indent = 0) const {
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
    
    std::string toJSON(int indent = 0) const {
        std::string indentStr(indent, ' ');
        std::stringstream ss;
        
        // Temporary debug log to validate stringstream is working
        std::cout << "Debug: Entering toJSON method, ss type: " << typeid(ss).name() << std::endl;
        
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
    
    std::string toPOML(int indent = 0) const {
        std::string indentStr(indent, ' ');
        std::stringstream ss;
        
        ss << indentStr << "[edge." << id << "]\n";
        ss << indentStr << "source = \"" << sourceNodeId << "\"\n";
        ss << indentStr << "target = \"" << targetNodeId << "\"\n";
        ss << indentStr << "type = \"" << getRelationshipTypeAsString() << "\"\n";
        ss << indentStr << "strength = " << strength << "\n";
        ss << indentStr << "timestamp = " << timestamp << "\n";
        
        if (!description.empty()) {
            ss << indentStr << "description = \"\"\"" << description << "\"\"\"\n";
        }
        
        if (!sources.empty()) {
            ss << indentStr << "sources = [\n";
            for (const auto& source : sources) {
                ss << indentStr << "  \"" << source << "\",\n";
            }
            ss << indentStr << "]\n";
        }
        
        return ss.str();
    }
    
    // Operator overloads
    bool operator==(const KGEdge& other) const {
        return id == other.id;
    }
    
    bool operator!=(const KGEdge& other) const {
        return !(*this == other);
    }
    
    // Friend functions for output
    friend std::ostream& operator<<(std::ostream& os, const KGEdge& edge) {
        os << edge.toYAML();
        return os;
    }
};




class KGEdge {
private:
    // 1. Unique identifier for the edge
    std::string id;
    
    // 2. Source and target node IDs
    std::string sourceNodeId;
    std::string targetNodeId;
    
    // 3. Semantic relationship type
    EdgeRelationshipType type;
    
    // 4. Confidence score (0.0 to 1.0)
    double strength;
    
    // 5. Semantic description
    std::string description;
    
    // 6. List of data sources (e.g., documents, references)
    std::vector<std::string> sources;
    
    // 7. Timestamp for creation/update
    std::time_t timestamp;

public:
    // 8. Constructors
    KGEdge();
    KGEdge(const std::string& sourceId, const std::string& targetId,
           EdgeRelationshipType edgeType, double edgeStrength = 0.5);
    KGEdge(const std::string& id, const std::string& sourceId, const std::string& targetId,
           EdgeRelationshipType edgeType, const std::string& description);

    // 9. Destructor
    ~KGEdge() = default;

    // 10. Getters (inline for performance)
    std::string getId() const { return id; }
    std::string getSourceNodeId() const { return sourceNodeId; }
    std::string getTargetNodeId() const { return targetNodeId; }
    EdgeRelationshipType getRelationship() const { return type; }
    double getStrength() const { return strength; }
    std::string getDescription() const { return description; }
    const std::vector<std::string>& getSources() const { return sources; }
    std::time_t getTimestamp() const { return timestamp; }

    // 11. Convert relationship type to string
    std::string getRelationshipTypeAsString() const;

    // 12. Setters
    void setSourceNodeId(const std::string& sourceId);
    void setTargetNodeId(const std::string& targetId);
    void setRelationship(EdgeRelationshipType edgeType);
    void setStrength(double newStrength);
    void setDescription(const std::string& desc);
    void addSource(const std::string& source);
    void removeSource(const std::string& source);

    // 13. Validation methods
    bool isValid() const;
    bool connectsNodes(const std::string& node1, const std::string& node2) const;
    bool involvesNode(const std::string& nodeId) const;
    std::string getOtherNode(const std::string& nodeId) const;

    // 14. Serialization
    std::string toYAML(int indent = 0) const;
    std::string toJSON(int indent = 0) const;

    // 15. Operators (inline for performance)
    bool operator==(const KGEdge& other) const { return id == other.id; }
    bool operator!=(const KGEdge& other) const { return !(*this == other); }
    friend std::ostream& operator<<(std::ostream& os, const KGEdge& edge) {
        os << edge.toYAML();
        return os;
    }

private:
    // 16. Generate unique ID
    std::string generateId();
};






*/

