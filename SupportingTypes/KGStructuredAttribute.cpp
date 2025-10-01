



// KGStructuredAttribute.cpp
#include "KGStructuredAttribute.hpp"
#include "EAttributeType.hpp"
#include "EAttributeContentType.hpp"
#include <sstream>
#include <sstream>
#include <iomanip>
#include <Algorithm>




/*
void KGStructuredAttribute::setValue(const std::string& key, const AttributeValue& value) {
    structured_content[key] = value;
    updateTimestamp();
}

AttributeValue KGStructuredAttribute::getValue(const std::string& key) const {
    auto it = structured_content.find(key);
    if (it != structured_content.end()) {
        return it->second;
    }
    return AttributeValue{}; // return empty variant
}

std::string KGStructuredAttribute::toYAML() const {
    std::stringstream ss;
    ss << name << ":\n";
    
    if (content_type == ContentType::KEY_VALUE) {
        for (const auto& [key, value] : structured_content) {
            ss << "  " << key << ": ";
            std::visit([&](auto&& arg) {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<T, std::string>) {
                    ss << "\"" << arg << "\"";
                } else if constexpr (std::is_arithmetic_v<T>) {
                    ss << arg;
                } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                    ss << "[";
                    for (size_t i = 0; i < arg.size(); ++i) {
                        if (i > 0) ss << ", ";
                        ss << "\"" << arg[i] << "\"";
                    }
                    ss << "]";
                }
            }, value);
            ss << "\n";
        }
    }
    
    return ss.str();
}

std::string KGStructuredAttribute::toString() const {
    std::stringstream ss;
    ss << name << ": ";
    
    if (content_type == ContentType::KEY_VALUE) {
        for (const auto& [key, value] : structured_content) {
            ss << key << "=";
            std::visit([&](auto&& arg) {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<T, std::string>) {
                    ss << arg;
                } else if constexpr (std::is_arithmetic_v<T>) {
                    ss << arg;
                }
            }, value);
            ss << "; ";
        }
    }
    
    return ss.str();
}


    std::string KGStructuredAttribute::getTypeAsString() const {
    switch(type) {
        case EAttributeType::STRING: return "string";
        case EAttributeType::INTEGER: return "integer";
        case EAttributeType::FLOAT: return "float";
        case EAttributeType::BOOLEAN: return "boolean";
        case EAttributeType::DATETIME: return "datetime";
        case EAttributeType::ENUM: return "enum";
        case EAttributeType::REFERENCE: return "reference";
        case EAttributeType::COMPOSITE: return "composite";
        default: return "unknown";
    }
}


std::string KGStructuredAttribute::toPOML() const {
    std::stringstream ss;
    ss << "[" << name << "]\n";
    
    if (content_type == ContentType::KEY_VALUE) {
        for (const auto& [key, value] : structured_content) {
            ss << key << " = ";
            std::visit([&](auto&& arg) {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<T, std::string>) {
                    // التحقق إذا كانت القيمة تحتوي على أسطر متعددة
                    if (arg.find('\n') != std::string::npos) {
                        ss << "\"\"\"" << arg << "\"\"\"";
                    } else {
                        ss << "\"" << arg << "\"";
                    }
                } else if constexpr (std::is_same_v<T, int> || 
                                   std::is_same_v<T, double> || 
                                   std::is_same_v<T, bool>) {
                    ss << arg;
                } else if constexpr (std::is_same_v<T, std::time_t>) {
                    char buffer[80];
                    std::strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", std::localtime(&arg));
                    ss << "\"" << buffer << "\"";
                } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                    ss << "[";
                    for (size_t i = 0; i < arg.size(); ++i) {
                        if (i > 0) ss << ", ";
                        ss << "\"" << arg[i] << "\"";
                    }
                    ss << "]";
                } else if constexpr (std::is_same_v<T, std::map<std::string, std::string>>) {
                    ss << "{";
                    bool firstInner = true;
                    for (const auto& [k, v] : arg) {
                        if (!firstInner) ss << ", ";
                        ss << k << " = \"" << v << "\"";
                        firstInner = false;
                    }
                    ss << "}";
                }
            }, value);
            ss << "\n";
        }
    }
    else if (content_type == ContentType::LIST) {
        ss << "items = [\n";
        for (size_t i = 0; i < list_content.size(); ++i) {
            ss << "  ";
            std::visit([&](auto&& arg) {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<T, std::string>) {
                    ss << "\"" << arg << "\"";
                } else if constexpr (std::is_same_v<T, int> || 
                                   std::is_same_v<T, double> || 
                                   std::is_same_v<T, bool>) {
                    ss << arg;
                }
            }, list_content[i]);
            if (i < list_content.size() - 1) {
                ss << ",";
            }
            ss << "\n";
        }
        ss << "]\n";
    }
    else if (content_type == ContentType::NESTED_STRUCT) {
        for (size_t i = 0; i < nested_structures.size(); ++i) {
            ss << "\n[[" << name << "." << nested_structures[i]->getName() << "]]\n";
            std::string nestedPoml = nested_structures[i]->toPOML();
            // إزالة السطر الأول (الاسم) لأنه موجود بالفعل في الـ [[ ]]
            size_t pos = nestedPoml.find('\n');
            if (pos != std::string::npos) {
                nestedPoml = nestedPoml.substr(pos + 1);
            }
            ss << nestedPoml;
        }
    }
    
    return ss.str();
}

*/



// KGStructuredAttribute.cpp
#include "KGStructuredAttribute.hpp"
#include <sstream>
#include <random>
#include <iomanip>

// 1. Constructor
KGStructuredAttribute::KGStructuredAttribute(const std::string& attrName, EAttributeType attrType, EAttributeContentType contentType)
    : id(generateId()), name(attrName), description(""), type(attrType),
      content_type(contentType), confidence_score(0.5), sources(), timestamp(std::time(nullptr)),
      structured_content(), list_content(), nested_structures() {}

// 2. Generate unique ID
std::string KGStructuredAttribute::generateId() {
    std::time_t now = std::time(nullptr);
    std::mt19937 rng(static_cast<unsigned int>(now));
    std::uniform_int_distribution<int> dist(1000, 9999);
    std::stringstream ss;
    ss << "ATTR_" << now << "_" << dist(rng);
    return ss.str();
}

// 3. Setters
void KGStructuredAttribute::setName(const std::string& name) {
    this->name = name;
    timestamp = std::time(nullptr);
}

void KGStructuredAttribute::setDescription(const std::string& desc) {
    description = desc;
    timestamp = std::time(nullptr);
}

void KGStructuredAttribute::setType(EAttributeType attrType) {
    type = attrType;
    timestamp = std::time(nullptr);
}

void KGStructuredAttribute::setContentType(EAttributeContentType contentType) {
    this->content_type = contentType;
    timestamp = std::time(nullptr);
}

void KGStructuredAttribute::setConfidence(double confidence) {
    confidence_score = std::max(0.0, std::min(1.0, confidence));
    timestamp = std::time(nullptr);
}

void KGStructuredAttribute::addSource(const std::string& source) {
    if (!source.empty()) {
        sources.push_back(source);
        timestamp = std::time(nullptr);
    }
}

void KGStructuredAttribute::removeSource(const std::string& source) {
    sources.erase(std::remove(sources.begin(), sources.end(), source), sources.end());
    timestamp = std::time(nullptr);
}

void KGStructuredAttribute::setValue(const std::string& key, const AttributeValue& value) {
    structured_content[key] = value;
    timestamp = std::time(nullptr);
}

void KGStructuredAttribute::addListItem(const AttributeValue& item) {
    list_content.push_back(item);
    timestamp = std::time(nullptr);
}

void KGStructuredAttribute::addNestedStructure(const std::shared_ptr<KGStructuredAttribute>& nestedAttr) {
    if (nestedAttr && nestedAttr->isValid()) {
        nested_structures.push_back(nestedAttr);
        timestamp = std::time(nullptr);
    }
}

// 4. Utility
std::string KGStructuredAttribute::getTypeAsString() const {
    return AttributeTypeUtils::AttributeTypeString(type);

}