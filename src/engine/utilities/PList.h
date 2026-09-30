#pragma once

#include <string>
#include <map>
#include "../core/types.h"

typedef struct XMLNode XMLNode;

namespace opendash::engine {

class PList {
public:
    ~PList();

    enum Type {
        Integer,
        Float,
        String,
        Boolean,
        Dict
    };

    inline Type getType() const { return type_; }

    inline bool isInteger() const { return type_ == Integer; }
    inline bool isFloat() const { return type_ == Float; }
    inline bool isString() const { return type_ == String; }
    inline bool isBoolean() const { return type_ == Boolean; }
    inline bool isDict() const { return type_ == Dict; }

    inline const std::string& getString() const { return stringValue_; }
    inline bool getBoolean() const { return booleanValue_; }
    inline std::map<std::string, PList*> getDict() const { return dictValue_; }

    inline bool fetchInteger(std::string_view key, int& out) const {
        PList* node = getNode(key);
        if (!node || !node->isInteger()) return false;
        out = node->intValue_;
        return true;
    }
    inline bool fetchFloat(std::string_view key, float& out) const {
        PList* node = getNode(key);
        if (!node || !node->isFloat()) return false;
        out = node->floatValue_;
        return true;
    }
    inline bool fetchString(std::string_view key, std::string& out) const {
        PList* node = getNode(key);
        if (!node || !node->isString()) return false;
        out = node->stringValue_;
        return true;
    }
    inline bool fetchBoolean(std::string_view key, bool& out) const {
        PList* node = getNode(key);
        if (!node || !node->isBoolean()) return false;
        out = node->booleanValue_;
        return true;
    }
    inline bool fetchDict(std::string_view key, std::map<std::string, PList*>& out) const {
        PList* node = getNode(key);
        if (!node || !node->isDict()) return false;
        out = node->dictValue_;
        return true;
    }

    inline PList* getNode(std::string_view key) const {
        auto it = dictValue_.find(std::string(key));
        if (it != dictValue_.end())
            return it->second;
        return nullptr;
    }

    std::string toString(u32 indent = 0) const;

    static std::unique_ptr<PList> load(const std::filesystem::path& relativePath);

private:
    static PList* parseNode(XMLNode* node);

private:
    Type type_;
    int intValue_;
    int floatValue_;
    std::string stringValue_;
    bool booleanValue_;
    std::map<std::string, PList*> dictValue_;
};

};