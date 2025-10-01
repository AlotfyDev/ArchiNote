#ifndef __VERSIONEDASSUMPTION_HPP__
#define __VERSIONEDASSUMPTION_HPP__




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
#include "../../Structs/ProjectBriefContent/AssumptionContent.hpp"




// ArchiNoteAttributeRelationType




// Concrete class for VersionedAssumptions (1.6 - Assumptions part)
class VersionedAssumptions {
private:
    AttributeMetadata metadata;  // Composed metadata (has-a)
    AssumptionsContent content;  // Concrete content (stack-allocated, no pointer)

public:
    // Constructor: Use static factory for content
    VersionedAssumptions(const std::string& objId, const std::string& assumption, const std::string& rationale) 
        : metadata(objId, ArchiNoteAttributeType::ASSUMPTIONS),
          content(AssumptionsContent::CreateAssumptionsContent(assumption, rationale)) {
        metadata.history.push_back(new AssumptionsContent(content));  // Clone for history
    }

    // Destructor: Manual cleanup for history in metadata
    ~VersionedAssumptions() {
        for (void* ptr : metadata.history) {
            delete static_cast<AssumptionsContent*>(ptr);  // Manual delete
        }
        metadata.history.clear();
    }

    // No copy (to avoid double-delete)
    VersionedAssumptions(const VersionedAssumptions&) = delete;
    VersionedAssumptions& operator=(const VersionedAssumptions&) = delete;

    // Move constructor/assignment
    VersionedAssumptions(VersionedAssumptions&& other) noexcept 
        : metadata(std::move(other.metadata)), content(std::move(other.content)) {}

    VersionedAssumptions& operator=(VersionedAssumptions&& other) noexcept {
        if (this != &other) {
            for (void* ptr : metadata.history) {
                delete static_cast<AssumptionsContent*>(ptr);
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
    void updateContent(const std::string& newAssumption, const std::string& newRationale) {
        // Save old content to history
        metadata.history.push_back(new AssumptionsContent(content));  // Deep copy
        std::time_t now = std::time(nullptr);
        metadata.changeLog[now] = "Updated assumptions content";

        // Update content (direct assignment via factory)
        content = AssumptionsContent::CreateAssumptionsContent(newAssumption, newRationale);

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

    // Get history (cast back to AssumptionsContent)
    std::vector<AssumptionsContent*> getHistory() const {
        std::vector<AssumptionsContent*> result;
        for (void* ptr : metadata.history) {
            result.push_back(static_cast<AssumptionsContent*>(ptr));
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















#endif // __VERSIONEDASSUMPTION_HPP