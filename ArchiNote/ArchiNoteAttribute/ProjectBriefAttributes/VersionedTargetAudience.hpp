#ifndef __VERSIONEDTARGETAUDIENCE_HPP__
#define __VERSIONEDTARGETAUDIENCE_HPP__



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
#include "../../Structs/ProjectBriefContent/TargetAudienceContent.hpp"




// Concrete class for VersionedTargetAudience (1.3 - Single instance)
class VersionedTargetAudience {
private:
    AttributeMetadata metadata;  // Composed metadata (has-a)
    TargetAudienceContent content;  // Concrete content (stack-allocated, no pointer)

public:
    // Constructor: Use static factory for content
    VersionedTargetAudience(const std::string& objId, const std::string& segment, const std::string& needs, const std::string& characteristics) 
        : metadata(objId, ArchiNoteAttributeType::TARGET_AUDIENCE),
          content(TargetAudienceContent::CreateTargetAudienceContent(segment, needs, characteristics)) {
        metadata.history.push_back(new TargetAudienceContent(content));  // Clone for history
    }

    // Destructor: Manual cleanup for history in metadata
    ~VersionedTargetAudience() {
        for (void* ptr : metadata.history) {
            delete static_cast<TargetAudienceContent*>(ptr);  // Manual delete
        }
        metadata.history.clear();
    }

    // No copy (to avoid double-delete)
    VersionedTargetAudience(const VersionedTargetAudience&) = delete;
    VersionedTargetAudience& operator=(const VersionedTargetAudience&) = delete;

    // Move constructor/assignment
    VersionedTargetAudience(VersionedTargetAudience&& other) noexcept 
        : metadata(std::move(other.metadata)), content(std::move(other.content)) {}

    VersionedTargetAudience& operator=(VersionedTargetAudience&& other) noexcept {
        if (this != &other) {
            for (void* ptr : metadata.history) {
                delete static_cast<TargetAudienceContent*>(ptr);
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
    void updateContent(const std::string& newSegment, const std::string& newNeeds, const std::string& newCharacteristics) {
        // Save old content to history
        metadata.history.push_back(new TargetAudienceContent(content));  // Deep copy
        std::time_t now = std::time(nullptr);
        metadata.changeLog[now] = "Updated target audience content";

        // Update content (direct assignment via factory)
        content = TargetAudienceContent::CreateTargetAudienceContent(newSegment, newNeeds, newCharacteristics);

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

    // Get history (cast back to TargetAudienceContent)
    std::vector<TargetAudienceContent*> getHistory() const {
        std::vector<TargetAudienceContent*> result;
        for (void* ptr : metadata.history) {
            result.push_back(static_cast<TargetAudienceContent*>(ptr));
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



















#endif // __VERSIONEDTARGETAUDIENCE_HPP__