#ifndef __VERSIONEDREVIEWCHECKLIST_HPP__
#define __VERSIONEDREVIEWCHECKLIST_HPP__






#include "../../Enums/ArchiNoteAttributeRelationType.hpp"
#include "../../Structs/AttributeMetaData.hpp"



#include "../../Structs/ExportEvent.hpp"
#include "../../Structs/UpdateEvent.hpp"
#include "../../Structs/ValidationEvent.hpp"
#include "../../Structs/ProjectBriefContent/ReviewChecklistContent.hpp"



// ArchiNoteAttributeRelationTypeContent.hpp"



// ArchiNoteAttributeRelationType


// Concrete class for VersionedReviewChecklist (1.9 - Single instance)
class VersionedReviewChecklist {
private:
    AttributeMetadata metadata;  // Composed metadata (has-a)
    ReviewChecklistContent content;  // Concrete content (stack-allocated, no pointer)

public:
    // Constructor: Use static factory for content
    VersionedReviewChecklist(const std::string& objId, const std::string& item, bool isChecked, const std::string& status) 
        : metadata(objId, ArchiNoteAttributeType::REVIEW_CHECKLIST),
          content(ReviewChecklistContent::CreateReviewChecklistContent(item, isChecked, status)) {
        metadata.history.push_back(new ReviewChecklistContent(content));  // Clone for history
    }

    // Destructor: Manual cleanup for history in metadata
    ~VersionedReviewChecklist() {
        for (void* ptr : metadata.history) {
            delete static_cast<ReviewChecklistContent*>(ptr);  // Manual delete
        }
        metadata.history.clear();
    }

    // No copy (to avoid double-delete)
    VersionedReviewChecklist(const VersionedReviewChecklist&) = delete;
    VersionedReviewChecklist& operator=(const VersionedReviewChecklist&) = delete;

    // Move constructor/assignment
    VersionedReviewChecklist(VersionedReviewChecklist&& other) noexcept 
        : metadata(std::move(other.metadata)), content(std::move(other.content)) {}

    VersionedReviewChecklist& operator=(VersionedReviewChecklist&& other) noexcept {
        if (this != &other) {
            for (void* ptr : metadata.history) {
                delete static_cast<ReviewChecklistContent*>(ptr);
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
    void updateContent(const std::string& newItem, bool newIsChecked, const std::string& newStatus) {
        // Save old content to history
        metadata.history.push_back(new ReviewChecklistContent(content));  // Deep copy
        std::time_t now = std::time(nullptr);
        metadata.changeLog[now] = "Updated review checklist content";

        // Update content (direct assignment via factory)
        content = ReviewChecklistContent::CreateReviewChecklistContent(newItem, newIsChecked, newStatus);

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

    // Get history (cast back to ReviewChecklistContent)
    std::vector<ReviewChecklistContent*> getHistory() const {
        std::vector<ReviewChecklistContent*> result;
        for (void* ptr : metadata.history) {
            result.push_back(static_cast<ReviewChecklistContent*>(ptr));
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















#endif // __VERSIONEDREVIEWCHECKLIST_HPP__