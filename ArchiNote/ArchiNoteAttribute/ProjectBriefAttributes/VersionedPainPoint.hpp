
#ifndef __VERSIONEDPAINPOINT_HPP__
#define __VERSIONEDPAINPOINT_HPP__




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
#include "../../Structs/ProjectBriefContent/PainPointContent.hpp"






// Concrete class for VersionedPainPoints (1.4 - Single instance)
class VersionedPainPoints {
private:
    AttributeMetadata metadata;  // Composed metadata (has-a)
    PainPointContent content;  // Concrete content (stack-allocated, no pointer)

public:
    // Constructor: Use static factory for content
    VersionedPainPoints(const std::string& objId, const std::string& issue, const std::string& impact, const std::string& stakeholders) 
        : metadata(objId, ArchiNoteAttributeType::PAIN_POINTS),
          content(PainPointContent::CreatePainPointsContent(issue, impact, stakeholders)) {
        metadata.history.push_back(new PainPointContent(content));  // Clone for history
    }

    // Destructor: Manual cleanup for history in metadata
    ~VersionedPainPoints() {
        for (void* ptr : metadata.history) {
            delete static_cast<PainPointContent*>(ptr);  // Manual delete
        }
        metadata.history.clear();
    }

    // No copy (to avoid double-delete)
    VersionedPainPoints(const VersionedPainPoints&) = delete;
    VersionedPainPoints& operator=(const VersionedPainPoints&) = delete;

    // Move constructor/assignment
    VersionedPainPoints(VersionedPainPoints&& other) noexcept 
        : metadata(std::move(other.metadata)), content(std::move(other.content)) {}

    VersionedPainPoints& operator=(VersionedPainPoints&& other) noexcept {
        if (this != &other) {
            for (void* ptr : metadata.history) {
                delete static_cast<PainPointContent*>(ptr);
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
    void updateContent(const std::string& newIssue, const std::string& newImpact, const std::string& newStakeholders) {
        // Save old content to history
        metadata.history.push_back(new PainPointContent(content));  // Deep copy
        std::time_t now = std::time(nullptr);
        metadata.changeLog[now] = "Updated pain points content";

        // Update content (direct assignment via factory)
        content = PainPointContent::CreatePainPointsContent(newIssue, newImpact, newStakeholders);

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

    // Get history (cast back to PainPointsContent)
    std::vector<PainPointContent*> getHistory() const {
        std::vector<PainPointContent*> result;
        for (void* ptr : metadata.history) {
            result.push_back(static_cast<PainPointContent*>(ptr));
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














#endif // __VERSIONEDPAINPOINT_HPP