#ifndef __VERSIONEDPROJECTOVERVIEW_HPP__
#define __VERSIONEDPROJECTOVERVIEW_HPP__




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
#include "../../Structs/ProjectBriefContent/ProjectOverviewContent.hpp"





// Concrete class for VersionedProjectOverview (1.0 - Single instance)
class VersionedProjectOverview {
private:
    AttributeMetadata metadata;  // Composed metadata (has-a)
    ProjectOverviewContent content;  // Concrete content (stack-allocated, no pointer)

public:
    // Constructor: Use static factory for content
    VersionedProjectOverview(const std::string& objId, const std::string& title, const std::string& brief, const std::string& date) 
        : metadata(objId, ArchiNoteAttributeType::OVERVIEW),
          content(ProjectOverviewContent::CreateProjectOverviewContent(title, brief, date)) {
        metadata.history.push_back(new ProjectOverviewContent(content));  // Clone for history
    }

    // Destructor: Manual cleanup for history in metadata
    ~VersionedProjectOverview() {
        for (void* ptr : metadata.history) {
            delete static_cast<ProjectOverviewContent*>(ptr);  // Manual delete
        }
        metadata.history.clear();
    }

    // No copy (to avoid double-delete)
    VersionedProjectOverview(const VersionedProjectOverview&) = delete;
    VersionedProjectOverview& operator=(const VersionedProjectOverview&) = delete;

    // Move constructor/assignment
    VersionedProjectOverview(VersionedProjectOverview&& other) noexcept 
        : metadata(std::move(other.metadata)), content(std::move(other.content)) {}

    VersionedProjectOverview& operator=(VersionedProjectOverview&& other) noexcept {
        if (this != &other) {
            for (void* ptr : metadata.history) {
                delete static_cast<ProjectOverviewContent*>(ptr);
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
    void updateContent(const std::string& newTitle, const std::string& newBrief, const std::string& newDate) {
        // Save old content to history
        metadata.history.push_back(new ProjectOverviewContent(content));  // Deep copy
        std::time_t now = std::time(nullptr);
        metadata.changeLog[now] = "Updated project overview content";

        // Update content (direct assignment via factory)
        content = ProjectOverviewContent::CreateProjectOverviewContent(newTitle, newBrief, newDate);

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

    // Get history (cast back to ProjectOverviewContent)
    std::vector<ProjectOverviewContent*> getHistory() const {
        std::vector<ProjectOverviewContent*> result;
        for (void* ptr : metadata.history) {
            result.push_back(static_cast<ProjectOverviewContent*>(ptr));
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













#endif // __VERSIONEDPROJECTOVERVIEW_HPP__