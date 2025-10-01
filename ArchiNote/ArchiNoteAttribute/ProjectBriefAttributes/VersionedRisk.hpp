#ifndef __VERSIONEDRISKCONTENT_HPP__
#define __VERSIONEDRISKCONTENT_HPP__








#include "../../Enums/ArchiNoteAttributeRelationType.hpp"
#include "../../Structs/AttributeMetaData.hpp"



#include "../../Structs/ExportEvent.hpp"
#include "../../Structs/UpdateEvent.hpp"
#include "../../Structs/ValidationEvent.hpp"
#include "../../Structs/ProjectBriefContent/RiskContent.hpp"



// ArchiNoteAttributeRelationType


// Concrete class for VersionedRisk (1.7 - Single instance)
class VersionedRisk {
private:
    AttributeMetadata metadata;  // Composed metadata (has-a)
    RiskContent content;  // Concrete content (stack-allocated, no pointer)

public:
    // Constructor: Use static factory for content
    VersionedRisk(const std::string& objId, const std::string& riskDescription, const std::string& impact, const std::string& mitigation) 
        : metadata(objId, ArchiNoteAttributeType::INITIAL_RISKS),
          content(RiskContent::CreateRiskContent(riskDescription, impact, mitigation)) {
        metadata.history.push_back(new RiskContent(content));  // Clone for history
    }

    // Destructor: Manual cleanup for history in metadata
    ~VersionedRisk() {
        for (void* ptr : metadata.history) {
            delete static_cast<RiskContent*>(ptr);  // Manual delete
        }
        metadata.history.clear();
    }

    // No copy (to avoid double-delete)
    VersionedRisk(const VersionedRisk&) = delete;
    VersionedRisk& operator=(const VersionedRisk&) = delete;

    // Move constructor/assignment
    VersionedRisk(VersionedRisk&& other) noexcept 
        : metadata(std::move(other.metadata)), content(std::move(other.content)) {}

    VersionedRisk& operator=(VersionedRisk&& other) noexcept {
        if (this != &other) {
            for (void* ptr : metadata.history) {
                delete static_cast<RiskContent*>(ptr);
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
    void updateContent(const std::string& newRiskDescription, const std::string& newImpact, const std::string& newMitigation) {
        // Save old content to history
        metadata.history.push_back(new RiskContent(content));  // Deep copy
        std::time_t now = std::time(nullptr);
        metadata.changeLog[now] = "Updated risk content";

        // Update content (direct assignment via factory)
        content = RiskContent::CreateRiskContent(newRiskDescription, newImpact, newMitigation);

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

    // Get history (cast back to RiskContent)
    std::vector<RiskContent*> getHistory() const {
        std::vector<RiskContent*> result;
        for (void* ptr : metadata.history) {
            result.push_back(static_cast<RiskContent*>(ptr));
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



#endif // __VERSIONEDRISKCONTENT_HPP__