#ifndef __VERSIONEDCONSTRAINTS_HPP__
#define __VERSIONEDCONSTRAINTS_HPP__



#include "../../Enums/ArchiNoteAttributeRelationType.hpp"
#include "../../Structs/AttributeMetaData.hpp"



#include "../../Structs/ExportEvent.hpp"
#include "../../Structs/UpdateEvent.hpp"
#include "../../Structs/ValidationEvent.hpp"
#include "../../Structs/ProjectBriefContent/ConstraintContent.hpp"



// ArchiNoteAttributeRelationType


// Concrete class for VersionedConstraints (1.6 - Constraints part)
class VersionedConstraint {
private:
    AttributeMetadata metadata;  // Composed metadata (has-a)
    ConstraintContent content;  // Concrete content (stack-allocated, no pointer)

public:
    // Constructor: Use static factory for content
    VersionedConstraint(const std::string& objId, const std::string& constraint, const std::string& impact) 
        : metadata(objId, ArchiNoteAttributeType::CONSTRAINTS),
          content(ConstraintContent::CreateConstraintsContent(constraint, impact)) {
        metadata.history.push_back(new ConstraintContent(content));  // Clone for history
    }

    // Destructor: Manual cleanup for history in metadata
    ~VersionedConstraint() {
        for (void* ptr : metadata.history) {
            delete static_cast<ConstraintContent*>(ptr);  // Manual delete
        }
        metadata.history.clear();
    }

    // No copy (to avoid double-delete)
    VersionedConstraint(const VersionedConstraint&) = delete;
    VersionedConstraint& operator=(const VersionedConstraint&) = delete;

    // Move constructor/assignment
    VersionedConstraint(VersionedConstraint&& other) noexcept 
        : metadata(std::move(other.metadata)), content(std::move(other.content)) {}

    VersionedConstraint& operator=(VersionedConstraint&& other) noexcept {
        if (this != &other) {
            for (void* ptr : metadata.history) {
                delete static_cast<ConstraintContent*>(ptr);
            }
            metadata.history.clear();
            metadata = std::move(other.metadata);
            content = std::move(other.content);
        }
        return *this;
    }

    // Getters for metadata
    std::string getID() const { return metadata.id; }
    ArchiNoteAttributeType getType() const { return metadata.type; }
    const std::vector<ArchiNoteAttributeRelationType>& getRelations() const { return metadata.relations; }

    // Content-specific methods
    void updateContent(const std::string& newConstraint, const std::string& newImpact) {
        // Save old content to history
        metadata.history.push_back(new ConstraintContent(content));  // Deep copy
        std::time_t now = std::time(nullptr);
        metadata.changeLog[now] = "Updated constraints content";

        // Update content (direct assignment via factory)
        content = ConstraintContent::CreateConstraintsContent(newConstraint, newImpact);

        // Update metadata timestamp
        metadata.timestamp = now;
    }

    bool isValid() const {
        bool valid = content.isContentValid();
        // Fire validation event if needed
        return valid;
    }

    std::string toFullJSON() const {
        // Metadata JSON
        std::string json = "{ \"metadata\": { \"id\": \"" + metadata.id + "\", \"type\": " + std::to_string(static_cast<int>(metadata.type)) + 
                           ", \"timestamp\": " + std::to_string(metadata.timestamp) + " }, ";
        // Content JSON
        json += "\"content\": " + content.toJSON() + " }";
        return json;
    }

    // Get history (cast back to ConstraintsContent)
    std::vector<ConstraintContent*> getHistory() const {
        std::vector<ConstraintContent*> result;
        for (void* ptr : metadata.history) {
            result.push_back(static_cast<ConstraintContent*>(ptr));
        }
        return result;
    }

    // Compute diff
    std::string computeDiff(std::time_t fromTimestamp, std::time_t toTimestamp) const {
        std::string diff = "Diff for " + getID() + ":\n";
        auto it = metadata.changeLog.find(fromTimestamp);
        if (it != metadata.changeLog.end()) {
            diff += "- Change: " + it->second + "\n";
        }
        // Content diff (simplified)
        diff += "- Content updated\n";
        return diff;
    }
};





#endif // __VERSIONEDCONSTRAINT_HPP__