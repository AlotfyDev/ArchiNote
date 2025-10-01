
#ifndef __REVIEWCHECKLISTREPOAGENT_HPP__
#define __REVIEWCHECKLISTREPOAGENT_HPP__





#include <string>
#include <map>
#include <vector>
#include <functional>
#include <stdexcept>
#include <algorithm>
#include <sstream>
#include <algorithm>


#include "../IAttributeRepoAgent.hpp"
#include "../../ArchiNoteAttribute/ProjectBriefAttributes/VersionedReviewChecklist.hpp"
#include "../../ArchiNoteStorage/IStorageAdapter.hpp"


class ReviewChecklistRepoAgent : public IAttributeRepoAgent {
private:
    std::vector<VersionedReviewChecklist*> checklistItems;  // In-memory storage
    IStorageAdapter* storageAdapter;  // Storage adapter for persistence

    // Helper: Find review checklist item by ID
    VersionedReviewChecklist* findById(const std::string& id) const {
        auto it = std::find_if(checklistItems.begin(), checklistItems.end(),
                               [&id](const VersionedReviewChecklist* item) { return item->getID() == id; });
        return (it != checklistItems.end()) ? *it : nullptr;
    }

    // Helper: Validate params map
    void validateParams(const std::map<std::string, std::string>& params) const {
        if (params.find("item") == params.end() || params.at("item").empty()) {
            throw std::invalid_argument("Missing or empty item");
        }
        if (params.find("status") == params.end() || params.at("status").empty()) {
            throw std::invalid_argument("Missing or empty status");
        }
        if (params.find("isChecked") == params.end() || params.at("isChecked").empty()) {
            throw std::invalid_argument("Missing or empty isChecked");
        }
        // Validate that isChecked is a valid boolean string ("true" or "false")
        std::string isChecked = params.at("isChecked");
        std::transform(isChecked.begin(), isChecked.end(), isChecked.begin(), [](unsigned char c) { return std::tolower(c); });
        if (isChecked != "true" && isChecked != "false") {
            throw std::invalid_argument("isChecked must be 'true' or 'false'");
        }
    }

public:
    // Constructor with IStorageAdapter
    ReviewChecklistRepoAgent(IStorageAdapter* adapter = nullptr) : storageAdapter(adapter) {}

    ~ReviewChecklistRepoAgent() override {
        for (auto* item : checklistItems) {
            delete item;  // Manual memory management
        }
        checklistItems.clear();
    }

    // No copy (to avoid double-delete)
    ReviewChecklistRepoAgent(const ReviewChecklistRepoAgent&) = delete;
    ReviewChecklistRepoAgent& operator=(const ReviewChecklistRepoAgent&) = delete;

    // Move constructor/assignment
    ReviewChecklistRepoAgent(ReviewChecklistRepoAgent&& other) noexcept 
        : checklistItems(std::move(other.checklistItems)), storageAdapter(other.storageAdapter) {
        other.checklistItems.clear();
        other.storageAdapter = nullptr;
    }
    ReviewChecklistRepoAgent& operator=(ReviewChecklistRepoAgent&& other) noexcept {
        if (this != &other) {
            for (auto* item : checklistItems) {
                delete item;
            }
            checklistItems.clear();
            checklistItems = std::move(other.checklistItems);
            storageAdapter = other.storageAdapter;
            other.checklistItems.clear();
            other.storageAdapter = nullptr;
        }
        return *this;
    }

    // CRUD operations
    void create(const std::string& id, const std::map<std::string, std::string>& params) override {
        if (findById(id)) {
            throw std::invalid_argument("Review Checklist item ID already exists: " + id);
        }
        validateParams(params);
        // Convert isChecked from string to bool
        std::string isCheckedStr = params.at("isChecked");
        std::transform(isCheckedStr.begin(), isCheckedStr.end(), isCheckedStr.begin(), [](unsigned char c) { return std::tolower(c); });
        bool isChecked = (isCheckedStr == "true");
        auto* item = new VersionedReviewChecklist(id, params.at("item"),
                                                 isChecked,
                                                 params.at("status"));
        checklistItems.push_back(item);
        if (storageAdapter) {
            storageAdapter->save(id, toJSON(id));  // Persist to storage
        }
    }

    void* read(const std::string& id) const override {
        auto* item = findById(id);
        if (!item && storageAdapter) {
            // Try loading from storage
            std::string data = storageAdapter->load(id);
            if (!data.empty()) {
                // Pseudo-code: Deserialize and add to checklistItems
                // checklistItems.push_back(deserialize(data));
                // item = findById(id);
            }
        }
        if (!item) {
            throw std::invalid_argument("Review Checklist item not found: " + id);
        }
        return item;  // Consumer casts to VersionedReviewChecklist*
    }

    void update(const std::string& id, const std::map<std::string, std::string>& params) override {
        auto* item = findById(id);
        if (!item) {
            throw std::invalid_argument("Review Checklist item not found: " + id);
        }
        validateParams(params);
        // Convert isChecked from string to bool
        std::string isCheckedStr = params.at("isChecked");
        std::transform(isCheckedStr.begin(), isCheckedStr.end(), isCheckedStr.begin(), [](unsigned char c) { return std::tolower(c); });
        bool isChecked = (isCheckedStr == "true");
        item->updateContent(params.at("item"), isChecked, params.at("status"));
        if (storageAdapter) {
            storageAdapter->save(id, toJSON(id));  // Persist to storage
        }
    }

    void remove(const std::string& id) override {
        auto it = std::find_if(checklistItems.begin(), checklistItems.end(),
                               [&id](const VersionedReviewChecklist* item) { return item->getID() == id; });
        if (it == checklistItems.end()) {
            throw std::invalid_argument("Review Checklist item not found: " + id);
        }
        delete *it;
        checklistItems.erase(it);
        if (storageAdapter) {
            storageAdapter->remove(id);  // Remove from storage
        }
    }

    // Serialization
    std::string toJSON(const std::string& id) const override {
        auto* item = findById(id);
        if (!item) {
            throw std::invalid_argument("Review Checklist item not found: " + id);
        }
        return item->toFullJSON();
    }

    std::string toYAML(const std::string& id) const override {
        auto* item = findById(id);
        if (!item) {
            throw std::invalid_argument("Review Checklist item not found: " + id);
        }
        std::stringstream yaml;
        yaml << "id: " << id << "\n";
        yaml << "type: REVIEW_CHECKLIST\n";
        yaml << "content:\n";
        yaml << "  item: \"" << item->toFullJSON() /* simplified: extract item */ << "\"\n";
        yaml << "  isChecked: " << item->toFullJSON() /* simplified: extract isChecked */ << "\n";
        yaml << "  status: \"" << item->toFullJSON() /* simplified: extract status */ << "\"\n";
        yaml << "relations:\n";
        for (const auto& rel : item->getRelations()) {
            yaml << "  - " << static_cast<int>(rel) << "\n";
        }
        return yaml.str();
    }

    // KG export
    void exportToKG(const std::string& id) const override {
        auto* item = findById(id);
        if (!item) {
            throw std::invalid_argument("Review Checklist item not found: " + id);
        }
        // Pseudo-code: Export node and edges to KG
        // KGClient::createNode(id, "REVIEW_CHECKLIST", toJSON(id));
        // for (rel : item->getRelations()) KGClient::createEdge(id, rel.target, rel.type);
    }

    // Async communication
    void createAsync(const std::string& id, const std::map<std::string, std::string>& params, 
                     std::function<void(const std::string&, bool)> callback) override {
        try {
            create(id, params);
            callback(id, true);  // Success
        } catch (const std::exception& e) {
            callback(id, false);  // Failure
        }
    }

    // Storage adapter management
    void setStorageAdapter(IStorageAdapter* adapter) override {
        storageAdapter = adapter;
    }
};















#endif // __REVIEWCHECKLISTREPOAGENT_HPP__