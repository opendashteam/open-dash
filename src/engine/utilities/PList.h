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
        String,
        Boolean,
        Dict
    };

    inline Type getType() const { return type_; }

    inline bool isString() const { return type_ == String; }
    inline bool isBoolean() const { return type_ == Boolean; }
    inline bool isDict() const { return type_ == Dict; }

    inline const std::string& getString() const { return stringValue_; }
    inline bool getBoolean() const { return booleanValue_; }
    inline std::map<std::string, PList*> getDict() const { return dictValue_; }

    inline PList* getNode(const std::string& key) const {
        auto it = dictValue_.find(key);
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
    std::string stringValue_;
    bool booleanValue_;
    std::map<std::string, PList*> dictValue_;
};

};