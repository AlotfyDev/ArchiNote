
#ifndef __ATTRIBUTEMETADATA_HPP__
#define __ATTRIBUTEMETADATA_HPP__


#include <string>
#include <vector>
#include <ctime>
#include <map>
#include "../Enums/ArchiNoteAttributeType.hpp"
#include "../Enums/ArchiNoteAttributeRelationType.hpp"




// Flat struct for metadata (no virtuals, pure data)
struct AttributeMetadata {
    std::string id;  // Unique ID (e.g., "OBJ-001")
    ArchiNoteAttributeType type;  // Semantic category
    std::time_t timestamp;  // Creation/update time
    std::string author;  // Who modified
    bool hasAmbiguities = false;  // Ambiguity flag
    std::vector<ArchiNoteAttributeRelationType> relations;  // Semantic links
    std::vector<void*> history;  // Generic history (raw pointers, type-erased)
    std::map<std::time_t, std::string> changeLog;  // Diff descriptions

    // Constructor for initialization
    AttributeMetadata(const std::string& attrId, ArchiNoteAttributeType attrType)
        : id(attrId), type(attrType), timestamp(std::time(nullptr)), author("Unknown") {}

    // Copy constructor (deep copy for history)
    AttributeMetadata(const AttributeMetadata& other)
        : id(other.id), type(other.type), timestamp(other.timestamp), author(other.author),
          hasAmbiguities(other.hasAmbiguities), relations(other.relations),
          changeLog(other.changeLog) {
        // Note: history requires derived class to handle specific type copying
        history = other.history;  // Shallow copy for now, managed by derived
    }

    // Move constructor
    AttributeMetadata(AttributeMetadata&& other) noexcept
        : id(std::move(other.id)), type(other.type), timestamp(other.timestamp),
          author(std::move(other.author)), hasAmbiguities(other.hasAmbiguities),
          relations(std::move(other.relations)), history(std::move(other.history)),
          changeLog(std::move(other.changeLog)) {
        other.history.clear();
    }

    // Assignment operators
    AttributeMetadata& operator=(const AttributeMetadata& other) {
        if (this != &other) {
            id = other.id;
            type = other.type;
            timestamp = other.timestamp;
            author = other.author;
            hasAmbiguities = other.hasAmbiguities;
            relations = other.relations;
            changeLog = other.changeLog;
            history = other.history;  // Shallow copy, managed by derived
        }
        return *this;
    }

    AttributeMetadata& operator=(AttributeMetadata&& other) noexcept {
        if (this != &other) {
            id = std::move(other.id);
            type = other.type;
            timestamp = other.timestamp;
            author = std::move(other.author);
            hasAmbiguities = other.hasAmbiguities;
            relations = std::move(other.relations);
            history = std::move(other.history);
            changeLog = std::move(other.changeLog);
            other.history.clear();
        }
        return *this;
    }

    // Destructor (history cleanup by derived classes)
    ~AttributeMetadata() {
        // No delete here; derived classes manage specific types in history
    }
};











#endif // __DOCATTRIBUTE_HPP__