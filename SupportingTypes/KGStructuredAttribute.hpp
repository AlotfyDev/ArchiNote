#ifndef __KGSTRUCTUREDATTRIBUTE_HPP__
#define __KGSTRUCTUREDATTRIBUTE_HPP__



#include <string>
#include <map>
#include <vector>
#include <ctime>
#include <variant>
#include <memory>
#include <atomic>
#include <cstdint>

using std::ctime;

using std::time_t;





#include "EAttributeType.hpp"
#include "EAttributeContentType.hpp"




// 2. Attribute value variant (supports multiple types)
using AttributeValue = std::variant<
    std::string,
    int,
    double,
    bool,
    std::time_t,
    std::vector<std::string>,
    std::map<std::string, std::string>
>;


class KGStructuredAttribute {
protected:
    // 3. Unique identifier
    std::string id;

    // 4. Attribute name (e.g., "goals", "audience")
    std::string name;

    // 5. Semantic description
    std::string description;

    // 6. Attribute type (EAttributeType)
    EAttributeType type;

    // 7. Content type (KEY_VALUE, LIST, etc.)
    EAttributeContentType content_type;

    // 8. Confidence score (0.0 to 1.0)
    double confidence_score;

    // 9. List of data sources
    std::vector<std::string> sources;

    // 10. Thread-safe timestamp
    
    std::atomic<std::int64_t> timestamp; 

    // 11. Structured content (key-value pairs)
    std::map<std::string, AttributeValue> structured_content;

    // 12. List content
    std::vector<AttributeValue> list_content;

    // 13. Nested structures
    std::vector<std::shared_ptr<KGStructuredAttribute>> nested_structures;

public:
    // 14. Constructor
    KGStructuredAttribute(const std::string& attrName, EAttributeType attrType, 
                         EAttributeContentType contentType = EAttributeContentType::KEY_VALUE);

    // 15. Virtual destructor
    virtual ~KGStructuredAttribute() = default;

    // 16. Getters (inline)
    std::string getId() const { return id; }
    std::string getName() const { return name; }
    std::string getDescription() const { return description; }
    EAttributeType getType() const { return type; }
    EAttributeContentType getContentType() const { return content_type; }
    double getConfidence() const { return confidence_score; }
    const std::vector<std::string>& getSources() const { return sources; }
    std::time_t getTimestamp() const { return static_cast<std::time_t>(timestamp.load()); }
    const std::map<std::string, AttributeValue>& getKeyValueContent() const { return structured_content; }
    const std::vector<AttributeValue>& getListContent() const { return list_content; }
    const std::vector<std::shared_ptr<KGStructuredAttribute>>& getNestedStructures() const { return nested_structures; }
    std::string getValue() const { return getName(); }

    // 17. Setters
    void setName(const std::string& name);
    void setDescription(const std::string& desc);
    void setType(EAttributeType attrType);
    void setContentType(EAttributeContentType contentType);
    void setConfidence(double confidence);
    void addSource(const std::string& source);
    void removeSource(const std::string& source);
    void setValue(const std::string& key, const AttributeValue& value);
    void addListItem(const AttributeValue& item);
    void addNestedStructure(const std::shared_ptr<KGStructuredAttribute>& nestedAttr);

    // 18. Pure virtual methods for contract
    virtual std::string toYAML(int indent = 0) const = 0;
    virtual std::string toJSON(int indent = 0) const = 0;
    virtual std::string toString() const = 0;
    virtual bool isValid() const = 0;
    virtual std::shared_ptr<KGStructuredAttribute> clone() const = 0;

    // 19. Utility
    std::string getTypeAsString() const;

protected:
    // 20. Generate unique ID
    std::string generateId();
};














#endif // __KGSTRUCTUREDATTRIBUTE_HPP__