
#ifndef __KGBASEGNODE_HPP__
#define __KGBASEGNODE_HPP__



#include<ios>
#include <string>

#include <vector> 
#include <map>

#include "EKGNodeType.hpp"
#include "KGEdge.hpp"
#include "KGStructuredAttribute.hpp"


/*
// الـ Base Node class
class BaseNode {
protected:
    std::string id;                 // UUID
    std::string name;
    std::string description;
    NodeType type;
    double confidence_score;        // درجة الثقة
    std::vector<std::string> sources; // المصادر
    std::vector<KGEdge> Edges;
    std::time_t timestamp;
    std::map<std::string, std::shared_ptr<KGStructuredAttribute>> attributes;

public:
    BaseNode(NodeType nodeType, std::string nodeName);
    virtual ~BaseNode() = default;
    
        // Methods لإدارة الـ Edges
    void addEdge(const KGEdge& edge);
    void removeEdge(const std::string& edgeId);
    const std::vector<KGEdge>& getEdges() const;
    std::vector<KGEdge>& getEdges();
    std::vector<KGEdge> getEdgesByType(RelationshipType type) const;
    
    // البحث عن edges معينة
    bool hasEdgeTo(const std::string& targetNodeId) const;
    bool hasEdgeType(RelationshipType type) const;


    // Getters and setters
    virtual std::string getId() const { return id; }
    virtual std::string getName() const { return name; }
    virtual NodeType getType() const { return type; }
    virtual std::time_t getTimestamp() const { return timestamp; }
    virtual std::string getDescription() const { return description; }
    virtual double getConfidence() const { return confidence_score; }

    
    
    virtual void addSource(std::string source);
    virtual void setConfidence(double confidence);
    virtual void setDescription(const std::string& desc);
    virtual void updateTimestamp();

    // Methods للتعامل مع الـ Attributes

    virtual void addAttribute(const std::shared_ptr<KGStructuredAttribute>& attribute);
    virtual bool removeAttribute(const std::string& attributeName);
    virtual std::shared_ptr<KGStructuredAttribute> getAttribute(const std::string& attributeName) const;
    virtual bool hasAttribute(const std::string& attributeName) const;
    virtual std::vector<std::string> getAttributeNames() const;
    virtual const std::map<std::string, std::shared_ptr<KGStructuredAttribute>>& getAllAttributes() const;

    // Template methods للوصول الآمن إلى قيم Attributes
    template<typename T>
    T getAttributeValue(const std::string& attributeName, const std::string& key, const T& defaultValue = T()) const {
        auto attr = getAttribute(attributeName);
        if (!attr) return defaultValue;
        
        try {
            return std::get<T>(attr->getValue(key));
        } catch (...) {
            return defaultValue;
        }
    }

    template<typename T>
    bool setAttributeValue(const std::string& attributeName, const std::string& key, const T& value) {
        auto attr = getAttribute(attributeName);
        if (!attr) return false;
        
        attr->setValue(key, value);
        updateTimestamp();
        return true;
    }





    // Serialization للتمثيل الخارجي
     // Serialization methods
    virtual std::string toYAML() const;
    virtual std::string toJSON() const;
    virtual std::string toPOML() const;

    // Helper methods للتسلسل
    virtual std::string serializeAttributesToYAML() const;
    virtual std::string serializeAttributesToJSON() const;
    virtual std::string serializeAttributesToPOML() const;
};


*/



class BaseNode {
private:
    // 1. Unique identifier for the node
    std::string id;

    // 2. Node type (e.g., PROJECT, TASK, BACKLOG)
    NodeType type;

    // 3. List of structured attributes
    std::vector<std::shared_ptr<KGStructuredAttribute>> attributes;

    // 4. Semantic description
    std::string description;

    // 5. Timestamp for creation/update
    std::time_t timestamp;

    // 6. Incoming and outgoing edge IDs
    std::vector<std::string> incoming_edge_ids;
    std::vector<std::string> outgoing_edge_ids;

    // 7. Path IDs this node belongs to
    std::vector<std::string> path_ids;

public:
    // 8. Constructors
    BaseNode();
    BaseNode(const std::string& id, NodeType type, const std::string& desc = "");

    // 9. Virtual destructor
    virtual ~BaseNode() = default;

    // 10. Getters (inline for performance)
    std::string getId() const { return id; }
    NodeType getType() const { return type; }
    const std::vector<std::shared_ptr<KGStructuredAttribute>>& getAttributes() const { return attributes; }
    std::string getDescription() const { return description; }
    std::time_t getTimestamp() const { return timestamp; }
    const std::vector<std::string>& getIncomingEdgeIds() const { return incoming_edge_ids; }
    const std::vector<std::string>& getOutgoingEdgeIds() const { return outgoing_edge_ids; }
    const std::vector<std::string>& getPathIds() const { return path_ids; }

    // 11. Attribute management
    void addAttribute(std::shared_ptr<KGStructuredAttribute> attr);
    void removeAttribute(const std::string& attrId);
    std::shared_ptr<KGStructuredAttribute> getAttributeByName(const std::string& name) const;

    // 12. Edge ID management
    void addIncomingEdgeId(const std::string& edgeId);
    void addOutgoingEdgeId(const std::string& edgeId);
    void removeEdgeId(const std::string& edgeId);

    // 13. Path ID management
    void addPathId(const std::string& pathId);
    void removePathId(const std::string& pathId);

    // 14. Setters
    void setType(NodeType type);
    void setDescription(const std::string& desc);

    // 15. Virtual methods for serialization and validation
    virtual std::string toYAML(int indent = 0) const;
    virtual std::string toJSON(int indent = 0) const;
    virtual std::string toString() const;
    virtual bool isValid() const;

    // 16. Clone method
    virtual std::shared_ptr<BaseNode> clone() const;

    // 17. Operators (inline for performance)
    bool operator==(const BaseNode& other) const { return id == other.id; }
    bool operator!=(const BaseNode& other) const { return !(*this == other); }
    friend std::ostream& operator<<(std::ostream& os, const BaseNode& node) {
        os << node.toYAML();
        return os;
    }

private:
    // 18. Generate unique ID
    std::string generateId();
};





#endif // __KGBASENODE_HPP__