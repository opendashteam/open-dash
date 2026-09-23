#include "PList.h"
#include "../AssetManager.h"

#define XML_H_IMPLEMENTATION
#include "../../third_party/xml.h"

#include <string.h>

namespace opendash::engine {

PList::~PList() {
    if (isDict()) {
        for (const auto& [_, value] : this->dictValue_)
            delete value;
    }
}

std::string PList::toString(u32 indent) const {
    switch (type_) {
    case String: return std::string("\"") + stringValue_ + std::string("\"");
    case Boolean: return booleanValue_ ? "true" : "false";
    case Dict:
    }

    indent += 4;

    std::string string = "<DICT>\n";
    for (const auto& [key, value] : dictValue_)
        string += std::string(indent, ' ') + key + ": " + value->toString(indent) + "\n";

    return string;
}

std::unique_ptr<PList> PList::load(const std::filesystem::path& relativePath) {
    std::string rawString;

    if (!AssetManager::get()->readFileAsString(relativePath, rawString))
        return nullptr;

    XMLNode* root = xml_parse_string(rawString.c_str());

    if (root->children->len == 0)
        return nullptr;

    XMLNode* plistNode = xml_node_child_at(root, root->children->len - 1);
    if (strcmp(plistNode->tag, "plist") != 0 || plistNode->children->len == 0)
        return nullptr;

    XMLNode* plistRoot = xml_node_child_at(plistNode, 0);

    return std::unique_ptr<PList>(parseNode(plistRoot));
}

PList* PList::parseNode(XMLNode* node) {
    PList* ret = new PList;

    if (strcmp(node->tag, "string") == 0) {
        ret->type_ = String;
        ret->stringValue_ = node->text != nullptr ? node->text : "";
    } else if (strcmp(node->tag, "true") == 0) {
        ret->type_ = Boolean;
        ret->booleanValue_ = true;
    } else if (strcmp(node->tag, "false") == 0) {
        ret->type_ = Boolean;
        ret->booleanValue_ = false;
    } else if (strcmp(node->tag, "dict") == 0) {
        ret->type_ = Dict;
        int count = node->children->len / 2 * 2;

        for (int i = 0; i < count; i += 2) {
            XMLNode* key = xml_node_child_at(node, i + 0);
            PList* value = parseNode(xml_node_child_at(node, i + 1));

            if (value == nullptr) {
                delete ret;
                return nullptr;
            }

            if (strcmp(key->tag, "key") != 0) {
                delete ret;
                delete value;
                return nullptr;
            }

            ret->dictValue_[key->text] = value;
        }
    }

    return ret;
}

}