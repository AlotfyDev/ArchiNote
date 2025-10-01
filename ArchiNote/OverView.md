

ArchiNote Version Control Design (Theoretical Discussion)
Overview
This document outlines the theoretical design for integrating Version Control into ArchiNote, a standalone tool for creating and managing semantic documents (e.g., Project Brief) with a Jupyter-inspired frontend and a C++ backend. The focus is on handling text and ideas with versioning, ensuring each document attribute (struct) supports tracking changes over time. Two approaches are proposed: (1) structs self-cloning for versioning, or (2) wrapping structs in a class with a version container and history.
Version Control Requirements

Purpose: Track changes to document attributes (e.g., Objectives, Risks) at the cell/struct level.
Frontend: Display version history in Jupyter-like cells (e.g., diff view, revert option).
Backend: Store versions efficiently (minimize duplication), support querying (e.g., "show changes to Objective OBJ-001 since yesterday").
Scalability: Handle collaborative editing (multi-user) and large documents.
Integration: Ensure compatibility with ArchiNote’s semantic structure (structs with meta-data) and future ArchiGraph integration.

Approach 1: Structs Self-Cloning for Versioning
Concept
Each DocAttribute struct (e.g., Objective, Risk) includes a mechanism to clone itself, creating a new instance for each change. Versions are stored as a list within the struct or in a separate version table in the backend.
Proposed Design

Struct Definition:struct DocAttribute {
    std::string id;  // e.g., "OBJ-001"
    ArchiNoteAttributeType type;  // Enum link
    std::time_t timestamp;
    std::string author;
    bool hasAmbiguities;
    std::vector<RelationType> relations;
    std::vector<std::shared_ptr<DocAttribute>> versions;  // Self-contained version history
    virtual bool isValid() const = 0;
    virtual std::string toJSON() const = 0;
    void saveVersion() { versions.push_back(std::make_shared<DocAttribute>(*this)); }
};


Example for Objective:struct Objective : DocAttribute {
    std::string description;
    std::string metric;
    ObjectiveSubType subType;
    int priority;
    Objective() : DocAttribute() { type = ArchiNoteAttributeType::KEY_OBJECTIVES; }
    bool isValid() const override { return !description.empty() && priority > 0; }
    std::string toJSON() const override { /* Serialize with versions */ }
};


Backend Workflow:
On edit (e.g., change description in Objective), call saveVersion() before applying changes.
Store versions in versions vector (or external DB table for scalability).
Query: getVersions("OBJ-001") → return vector of historical states.


Frontend Integration:
Render version history as a collapsible cell (e.g., dropdown showing timestamps/authors).
Support diff view (e.g., highlight changes in description using a JS diff library like diff2html).
Revert: Load selected version into editable cell.



Pros

Simplicity: Versioning logic is encapsulated within each struct.
Decentralized: No need for a separate versioning class; each attribute manages its own history.
Lightweight for Small Docs: Suitable for single-user or small-scale documents.

Cons

Memory Overhead: Storing full clones in versions vector increases memory usage (e.g., duplicating large strings like description).
Scalability Issue: Not ideal for collaborative editing (no conflict resolution).
Query Complexity: Querying across all versions (e.g., "show all changes by author X") requires iterating over all structs.

Approach 2: Wrapping Structs in a Version Container Class
Concept
Introduce a VersionedAttribute class that wraps each DocAttribute struct, managing a container for versions and a history log (e.g., timestamp, author, change type). The container lives in the backend, and the frontend queries it for display.
Proposed Design

VersionedAttribute Class:class VersionedAttribute {
public:
    std::shared_ptr<DocAttribute> current;  // Current state
    std::vector<std::pair<std::time_t, std::shared_ptr<DocAttribute>>> history;  // Version history
    std::map<std::string, std::string> changeLog;  // e.g., {"2025-09-13T16:00": "Updated description"}
    
    VersionedAttribute(std::shared_ptr<DocAttribute> attr) : current(attr) {}
    
    void update(std::shared_ptr<DocAttribute> newAttr, const std::string& changeDesc) {
        history.emplace_back(std::time(nullptr), current);  // Save old version
        changeLog[std::to_string(std::time(nullptr))] = changeDesc;
        current = newAttr;  // Update to new state
    }
    
    std::vector<std::shared_ptr<DocAttribute>> getVersions() const {
        std::vector<std::shared_ptr<DocAttribute>> versions;
        for (const auto& [ts, attr] : history) versions.push_back(attr);
        return versions;
    }
    
    std::string toJSON() const { /* Serialize current + history */ }
};


Notebook Integration:struct ArchiNoteNotebook {
    std::string notebookId;
    std::map<std::string, std::shared_ptr<VersionedAttribute>> attributes;  // Keyed by attr ID
    void addAttribute(std::shared_ptr<DocAttribute> attr) {
        attributes[attr->id] = std::make_shared<VersionedAttribute>(attr);
    }
    std::string generateVersionReport(const std::string& attrId) const;  // JSON report
};


Backend Workflow:
On edit: Create new struct instance, pass to VersionedAttribute::update().
Store minimal diffs (e.g., only changed fields) in changeLog to optimize storage.
Query: notebook.generateVersionReport("OBJ-001") → JSON with history.


Frontend Integration:
API endpoint (e.g., GET /notebook/:id/attr/:attrId/versions) returns version history.
Render as interactive timeline (e.g., using Plotly.js for version progression).
Support collaborative features (e.g., merge conflicts via diff3 algorithm).



Pros

Flexibility: Separates versioning logic from struct content, easier to extend (e.g., add merge logic).
Storage Efficiency: Can store diffs instead of full clones (e.g., only changed description).
Collaboration-Friendly: changeLog supports conflict detection and resolution.
Query Power: Centralized history enables complex queries (e.g., "show all changes in Risks by author X").

Cons

Complexity: Adds an extra layer (VersionedAttribute class) to the backend.
Overhead for Simple Docs: May be overkill for single-user, small-scale documents.
Maintenance: Requires careful synchronization between current and history.

Layered Architecture in ArchiNote

Backend (C++):
Storage: Use SQLite for versioning (table: versions(attrId, timestamp, json, changeDesc)).
Querying: SQL queries for version history (e.g., SELECT * FROM versions WHERE attrId='OBJ-001' ORDER BY timestamp).
API: Expose /versions endpoint for frontend (returns JSON with history).


Frontend (Jupyter-inspired):
Cells: Each attribute (e.g., Objective) rendered as a cell, with a "Versions" tab showing history.
Interaction: Click version → load diff view (using JS library like diff2html).
Real-time: WebSockets for multi-user updates (e.g., notify on version change).


Integration with ArchiGraph (Future):
Export notebook as a node with versioned attributes (e.g., ProjectBriefNode with versions field).



Recommendation
Approach 2 (VersionedAttribute Class) is preferred for ArchiNote because:

Scalability: Better for collaborative editing and large documents (e.g., storing diffs, handling conflicts).
Flexibility: Easier to extend for future features (e.g., branching, merging).
Query Efficiency: Centralized changeLog supports complex queries and reporting.
Frontend Fit: Maps well to Jupyter-like UI (e.g., timeline view for versions).

Implementation Notes:

Use nlohmann/json for JSON serialization in C++ backend to handle versioned structs.
Store diffs in changeLog to reduce memory (e.g., {field: "description", old: "old text", new: "new text"}).
Add VersionedAttribute::revert(timestamp) for reverting to a specific version.
Frontend: Use Plotly.js for interactive version timelines, diff2html for diff views.

Next Steps

Define detailed VersionedAttribute API (e.g., methods for diff, merge).
Prototype SQLite schema for versioning.
Design frontend version UI (e.g., collapsible history panel per cell).
Plan for conflict resolution in collaborative mode (e.g., diff3-based merging).
