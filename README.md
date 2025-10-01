# ArchiNote - Semantic Document Management System

[![C++](https://img.shields.io/badge/C%2B%2B-17-blue.svg)](https://isocpp.org/)
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
[![Version: 1.0.0](https://img.shields.io/badge/Version-1.0.0-green.svg)]()

## Overview

ArchiNote is a sophisticated C++ system designed to simulate the semantic structure of software development documents. It programmatically converts unstructured text-based documents into structured, semantically-rich representations that can be processed, versioned, and integrated with knowledge graphs.

### Key Features

- **Semantic Document Processing**: Converts unstructured documents into structured formats with rich metadata
- **Version Control Integration**: Built-in versioning system for tracking document evolution
- **Knowledge Graph Ready**: Designed for integration with hierarchical knowledge graph systems
- **Relational Database Compatible**: Optimized for traditional RDBMS systems rather than NoSQL solutions
- **Modular Architecture**: Clean separation between document management, storage, and graph operations
- **Type-Safe Attribute System**: Strongly-typed document attributes with validation

## Architecture

### Core Components

```
┌─────────────────────────────────────────────────────────────────┐
│                        ArchiNote System                         │
├─────────────────────────────────────────────────────────────────┤
│  ┌─────────────┐                                               │
│  │   ArchiNote  │                                               │
│  │   Manager   │                                               │
│  └─────────────┘                                               │
├─────────────────────────────────────────────────────────────────┤
│  ┌─────────────┐  ┌─────────────┐  ┌─────────────┐              │
│  │   Storage   │  │ Repository  │  │  Attributes │              │
│  │  Adapters   │  │   Agents    │  │   System    │              │
│  └─────────────┘  └─────────────┘  └─────────────┘              │
├─────────────────────────────────────────────────────────────────┤
│  ┌─────────────┐  ┌─────────────┐                               │
│  │   KGraph    │  │ Supporting  │                               │
│  │ (External)  │  │   Types     │                               │
│  └─────────────┘  └─────────────┘                               │
└─────────────────────────────────────────────────────────────────┘
```

### Component Relationships

- **ArchiNoteManager**: Central coordinator managing document operations and repository agents
- **KGraph** (External): Independent knowledge graph library for semantic relationships
- **Storage Adapters**: Pluggable storage backends (HDF5, SQLite planned)
- **Repository Agents**: Specialized agents for different document attribute types
- **Attribute System**: Type-safe, structured representation of document content

### External Dependencies

ArchiNote integrates with the following external libraries:

- **[KGraph](https://github.com/AlotfyDev/KGraph)**: Hierarchical knowledge graph system for semantic processing

## Installation

### Prerequisites

- C++17 compatible compiler (GCC 7+, Clang 5+, MSVC 2017+)
- CMake 3.15 or higher
- HDF5 library (optional, for HDF5 storage adapter)
- **[KGraph Library](https://github.com/AlotfyDev/KGraph)**: For knowledge graph integration

### Build Instructions

```bash
# Clone the repository
git clone https://github.com/AlotfyDev/ArchiNote.git
cd ArchiNote

# Clone KGraph dependency (if not installed system-wide)
git clone https://github.com/AlotfyDev/KGraph.git
cd KGraph && mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=/usr/local
cmake --build . --config Release
cmake --install .  # Install system-wide
cd ../../

# Create ArchiNote build directory
mkdir build && cd build

# Configure with CMake (KGraph will be auto-detected if installed)
cmake .. -DCMAKE_BUILD_TYPE=Release

# Alternative: Specify KGraph path if not installed system-wide
cmake .. -DCMAKE_BUILD_TYPE=Release -DKGraph_DIR=/path/to/KGraph/lib/cmake/KGraph

# Build the project
cmake --build . --config Release

# Run tests (if available)
ctest
```

### Dependencies

- **Standard Library**: Full C++17 support required
- **HDF5** (optional): For HDF5StorageAdapter
- **SQLite** (planned): For version control and relational storage
- **KGraph** (external): Hierarchical knowledge graph system for semantic processing

## Usage

### Basic Document Creation

```cpp
#include "ArchiNote/ArchiNoteAdmin/IArchiNoteManager.hpp"
#include "ArchiNote/Structs/ProjectBriefContent/ObjectiveContent.hpp"

// Create a manager instance
auto manager = std::make_unique<ArchiNoteManager>();

// Create an objective attribute
auto objective = ObjectiveContent::CreateObjectiveContent(
    "Improve system performance by 30%",
    "Measured by response time metrics",
    1  // High priority
);

// Add to document structure
manager->addAttribute(ArchiNoteAttributeType::KEY_OBJECTIVES,
                     "OBJ-001", objective.toJSON());
```

### Knowledge Graph Integration

```cpp
#include "KGraph/KGraph.hpp"

// Initialize knowledge graph (from external KGraph library)
KGraph graph;
graph.initializeGraph("archinote.db");

// Ingest document structure
graph.ingestFromArchiNotes(yamlDocumentData);

// Query semantic relationships
auto relatedNodes = graph.getNeighbors("OBJ-001", EdgeRelationshipType::DEPENDS_ON);

// Export for RAG systems
std::string ragData = graph.exportToRAG(documentNodeIds);
```

### Version Control Operations

```cpp
// Create versioned attribute wrapper
VersionedAttribute versionedObj(objective);

// Update with change tracking
auto newObjective = ObjectiveContent::CreateObjectiveContent(
    "Improve system performance by 40%",  // Updated description
    "Measured by response time metrics",
    1
);

versionedObj.update(newObjective, "Increased performance target from 30% to 40%");

// Retrieve version history
auto versions = versionedObj.getVersions();
```

## API Reference

### IArchiNoteManager Interface

#### Core Methods

```cpp
class IArchiNoteManager {
public:
    // Repository agent management
    virtual void createAgent(ArchiNoteAttributeType type) = 0;
    virtual IAttributeRepoAgent* readAgent(ArchiNoteAttributeType type) const = 0;
    virtual void updateAgent(ArchiNoteAttributeType type, IAttributeRepoAgent* agent) = 0;
    virtual void removeAgent(ArchiNoteAttributeType type) = 0;

    // Relationship management
    virtual void addRelation(const std::string& sourceId,
                           ArchiNoteAttributeType sourceType,
                           const std::string& targetId,
                           ArchiNoteAttributeType targetType,
                           ArchiNoteAttributeRelationType relationType) = 0;

    // Serialization
    virtual std::string toJSON(ArchiNoteAttributeType type, const std::string& id) const = 0;
    virtual std::string toYAML(ArchiNoteAttributeType type, const std::string& id) const = 0;
    virtual std::string exportAllToJSON() const = 0;

    // Storage management
    virtual void setStorageAdapter(IStorageAdapter* adapter) = 0;
    virtual void saveAll() = 0;
    virtual void loadAll() = 0;
};
```

### KGraph Class

#### Key Methods

```cpp
class KGraph {
public:
    // Node management
    void addNode(std::shared_ptr<BaseNode> node);
    std::shared_ptr<BaseNode> getNode(const std::string& node_id) const;
    void removeNode(const std::string& node_id);

    // Relationship management
    void addRelationship(std::shared_ptr<BaseNode> source,
                        std::shared_ptr<BaseNode> target,
                        EdgeRelationshipType rel_type,
                        double strength = 1.0);

    // Path finding
    std::vector<KGPath> findAllPaths(const std::string& source_id,
                                   const std::string& target_id,
                                   int max_depth = 10,
                                   double min_strength = 0.1) const;

    // Validation and analysis
    bool validateGraph() const;
    std::vector<std::string> findOrphanedNodes() const;
};
```

## Project Structure

```
ArchiNote/
├── ArchiNote/                          # Main ArchiNote system
│   ├── ObjectsTree.md                  # Project structure documentation
│   ├── OverView.md                     # Version control design document
│   ├── RAIIStylePointer.hpp           # Smart pointer utilities
│   ├── ArchiNoteAdmin/                # Administrative interfaces
│   │   ├── ArchiNoteAdmin.hpp
│   │   └── IArchiNoteManager.hpp
│   ├── ArchiNoteAttribute/            # Attribute type definitions
│   │   ├── ProjcetBriefAttributes.hpp
│   │   └── ProjectBriefAttributes/    # Specific attribute implementations
│   │       ├── VersionedAssumption.hpp
│   │       ├── VersionedConstraint.hpp
│   │       ├── VersionedHighLevelFeature.hpp
│   │       ├── VersionedKeyObjective.hpp
│   │       ├── VersionedPainPoint.hpp
│   │       ├── VersionedProjectOverview.hpp
│   │       ├── VersionedReviewChecklist.hpp
│   │       ├── VersionedRisk.hpp
│   │       ├── VersionedSuccessMetric.hpp
│   │       └── VersionedTargetAudience.hpp
│   ├── ArchiNoteAttributeRepoAgent/   # Repository agent implementations
│   │   ├── IAttributeRepoAgent.hpp
│   │   └── ProjectBriefAttributeRepoAgent/
│   │       ├── AssumptionsRepoAgent.hpp
│   │       ├── ConstraintsRepoAgent.hpp
│   │       ├── HighLevelFeaturesRepoAgent.hpp
│   │       ├── ObjectivesRepoAgent.hpp
│   │       ├── PainPointsRepoAgent.hpp
│   │       ├── ProjectOverviewRepoAgent.hpp
│   │       ├── ReviewChecklistRepoAgent.hpp
│   │       ├── RiskRepoAgent.hpp
│   │       ├── SuccessMetricsRepoAgent.hpp
│   │       └── TargetAudienceRepoAgent.hpp
│   ├── ArchiNoteManager/              # Manager implementations
│   │   ├── IArchiNoteManager.hpp.md
│   │   └── ProjectBriefManager/
│   │       └── ProjectBriefArchiNoteManager.hpp
│   ├── ArchiNoteStorage/              # Storage abstraction layer
│   │   ├── IStorageAdapter.hpp
│   │   └── HDF5StorageAdapter/
│   │       └── HDF5StorageAdapter.hpp
│   └── Enums/                         # Type definitions
│       ├── ArchiNoteAttributeRelationType.hpp
│       ├── ArchiNoteAttributeType.hpp
│       ├── ArchiNoteType.hpp
│       ├── AttributeContentDataType.hpp
│       ├── ObjectiveSubType.hpp
│       └── RiskSubType.hpp
├── CMakeLists.txt                     # Build configuration
├── README.md                          # This file
└── .gitignore                         # Version control exclusions
```

### External Dependencies Structure

**KGraph Library** (https://github.com/AlotfyDev/KGraph):
```
KGraph/
├── KGraph/                           # Core KGraph implementation
│   ├── KGraph.cpp                    # Main graph engine
│   └── KGraph.hpp                    # KGraph class definition
├── SupportingTypes/                  # Supporting type definitions
│   ├── KGBaseNode.cpp               # Base node implementation
│   ├── KGBaseNode.hpp               # Base node interface
│   ├── KGEdge.cpp                   # Edge implementation
│   ├── KGEdge.hpp                   # Edge definitions
│   ├── KGPath.cpp                   # Path implementation
│   ├── KGPath.hpp                   # Path definitions
│   ├── KGStructuredAttribute.cpp    # Structured attribute implementation
│   ├── KGStructuredAttribute.hpp    # Structured attribute interface
│   ├── GraphOrchestrator.cpp        # Graph orchestration utilities
│   ├── GraphOrchestrator.hpp        # Graph orchestration interface
│   ├── NodePipelineManager.cpp      # Node pipeline management
│   ├── NodePipelineManager.hpp      # Node pipeline interface
│   └── [Enum definitions]           # Type definitions
├── CMakeLists.txt                    # KGraph build configuration
└── README.md                         # KGraph documentation
```

## Document Attribute Types

The system supports various document attribute types for comprehensive project documentation:

| Attribute Type | Description | Versioned Class |
|---------------|-------------|-----------------|
| `KEY_OBJECTIVES` | Project objectives and goals | `VersionedKeyObjective` |
| `TARGET_AUDIENCE` | Target user groups | `VersionedTargetAudience` |
| `PAIN_POINTS` | User pain points | `VersionedPainPoint` |
| `HIGH_LEVEL_FEATURES` | Main product features | `VersionedHighLevelFeature` |
| `ASSUMPTIONS` | Project assumptions | `VersionedAssumption` |
| `SUCCESS_METRICS` | Success measurement criteria | `VersionedSuccessMetric` |
| `INITIAL_RISKS` | Identified project risks | `VersionedRisk` |
| `REVIEW_CHECKLIST` | Review criteria | `VersionedReviewChecklist` |
| `CONSTRAINTS` | Project constraints | `VersionedConstraint` |
| `OVERVIEW` | Project overview | `VersionedProjectOverview` |

## Integration Capabilities

### External KGraph Integration

ArchiNote integrates with the external [KGraph](https://github.com/AlotfyDev/KGraph) library for knowledge graph functionality:

```cpp
// Include external KGraph library
#include "KGraph/KGraph.hpp"

// Initialize knowledge graph
KGraph graph;
graph.initializeGraph("archinote.db");

// Ingest ArchiNote documents
graph.ingestFromArchiNotes(yamlDocumentData);

// Export for RAG systems
std::string ragExport = graph.exportToRAG(documentNodeIds);

// Query for document retrieval
std::string queryResults = graph.retrieveForQuery("performance optimization strategies");
```

### Relational Database Compatibility

The structured approach enables efficient relational database storage:

```cpp
// Structured data maps well to relational schemas
// - Documents become tables
// - Attributes become columns
// - Relationships become foreign keys
// - Versions become audit trails
```

### Version Control Design

Two approaches are supported for document versioning:

1. **Self-Cloning Structs**: Each attribute manages its own version history
2. **VersionedAttribute Wrapper**: Centralized version management with change tracking

## Development Guidelines

### Code Style

- Follow C++17 standards
- Use RAII principles for resource management
- Implement proper exception safety
- Document public APIs with clear examples

### Testing Strategy

```cpp
// Example test structure
TEST(ArchiNoteManagerTest, CreateAndRetrieveObjective) {
    auto manager = std::make_unique<ArchiNoteManager>();

    // Test objective creation and retrieval
    auto objective = ObjectiveContent::CreateObjectiveContent(
        "Test objective", "Test metric", 1);

    // Verify behavior
    EXPECT_TRUE(objective.isContentValid());
}
```

### Contributing

1. Fork the repository
2. Create a feature branch
3. Add tests for new functionality
4. Ensure all tests pass
5. Submit a pull request

## Performance Considerations

### Chunking Strategy

The system uses **structure-based chunking** rather than vector-based approaches:

- **Precise**: Uses document structure for accurate segmentation
- **Fast**: No vector similarity calculations required
- **Deterministic**: Consistent results across processing runs

### Storage Optimization

- **HDF5 Integration**: Efficient structured data storage
- **SQLite Planning**: ACID-compliant version control
- **Memory Management**: RAII patterns prevent leaks

## Future Enhancements

### Planned Features

- **Full SQLite Integration**: Complete relational database support
- **Advanced Query Engine**: Complex document relationship queries
- **Collaborative Editing**: Multi-user document editing capabilities
- **REST API**: HTTP interface for web integration
- **Plugin System**: Extensible attribute type system

### ArchiGraph Integration

While designed as independent systems, ArchiNote and ArchiGraph can be integrated:

- **Document Export**: ArchiNote documents become ArchiGraph nodes
- **Semantic Linking**: Document relationships become graph edges
- **Hierarchical RAG**: Multi-level document retrieval systems

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## Support

For questions, issues, or contributions, please:

1. Check existing documentation
2. Review open issues on GitHub
3. Create new issues for bugs or feature requests
4. Submit pull requests for improvements

---

**ArchiNote Version 1.0.0** - Semantic Document Management for Modern Software Development