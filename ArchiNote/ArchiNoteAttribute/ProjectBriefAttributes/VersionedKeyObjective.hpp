#ifndef __VERSIONEDKEYOBJECTIVE_HPP__
#define __VERSIONEDKEYOBJECTIVE_HPP__



#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <memory>
#include <sstream>


#include "../../Enums/ArchiNoteAttributeRelationType.hpp"
#include "../../Structs/AttributeMetaData.hpp"



#include "../../Structs/ExportEvent.hpp"
#include "../../Structs/UpdateEvent.hpp"
#include "../../Structs/ValidationEvent.hpp"
#include "../../Structs/ProjectBriefContent/ObjectiveContent.hpp"








// Concrete class for VersionedKeyObjective (1.1 - Single instance)
class VersionedKeyObjective {
private:
    AttributeMetadata metadata;  // Composed metadata (has-a)
    ObjectiveContent content;  // Concrete content (stack-allocated, no pointer)

public:
    // Constructor: Use static factory for content
    VersionedKeyObjective(const std::string& objId, const std::string& desc, const std::string& met, int pri) 
        : metadata(objId, ArchiNoteAttributeType::KEY_OBJECTIVES),
          content(ObjectiveContent::CreateObjectiveContent(desc, met, pri)) {
        metadata.history.push_back(new ObjectiveContent(content));  // Clone for history
    }

    // Destructor: Manual cleanup for history in metadata
    ~VersionedKeyObjective() {
        for (void* ptr : metadata.history) {
            delete static_cast<ObjectiveContent*>(ptr);  // Manual delete
        }
        metadata.history.clear();
    }

    // No copy (to avoid double-delete)
    VersionedKeyObjective(const VersionedKeyObjective&) = delete;
    VersionedKeyObjective& operator=(const VersionedKeyObjective&) = delete;

    // Move constructor/assignment
    VersionedKeyObjective(VersionedKeyObjective&& other) noexcept 
        : metadata(std::move(other.metadata)), content(std::move(other.content)) {}

    VersionedKeyObjective& operator=(VersionedKeyObjective&& other) noexcept {
        if (this != &other) {
            for (void* ptr : metadata.history) {
                delete static_cast<ObjectiveContent*>(ptr);
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
    void updateContent(const std::string& newDesc, const std::string& newMet, int newPri) {
        // Save old content to history
        metadata.history.push_back(new ObjectiveContent(content));  // Deep copy
        std::time_t now = std::time(nullptr);
        metadata.changeLog[now] = "Updated objective content";

        // Update content (direct assignment via factory)
        content = ObjectiveContent::CreateObjectiveContent(newDesc, newMet, newPri);

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

    // Get history (cast back to ObjectiveContent)
    std::vector<ObjectiveContent*> getHistory() const {
        std::vector<ObjectiveContent*> result;
        for (void* ptr : metadata.history) {
            result.push_back(static_cast<ObjectiveContent*>(ptr));
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














#endif // __VERSIONEDKEYOBJECTIVE_HPP