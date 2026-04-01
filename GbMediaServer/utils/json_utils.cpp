/******************************************************************************
 *  Copyright (c) 2025 The CRTC project authors . All Rights Reserved.
 ******************************************************************************/

#include "utils/json_utils.h"
#include <iomanip>

namespace gb_media_server {

// JsonBuilder实现

JsonBuilder::JsonBuilder() : first_field_(true) {
}

JsonBuilder::~JsonBuilder() {
}

void JsonBuilder::AddString(const std::string& key, const std::string& value) {
    std::ostringstream oss;
    oss << "\"" << EscapeString(key) << "\":\"" << EscapeString(value) << "\"";
    fields_[key] = oss.str();
}

void JsonBuilder::AddNumber(const std::string& key, int64_t value) {
    std::ostringstream oss;
    oss << "\"" << EscapeString(key) << "\":" << value;
    fields_[key] = oss.str();
}

void JsonBuilder::AddNumber(const std::string& key, uint64_t value) {
    std::ostringstream oss;
    oss << "\"" << EscapeString(key) << "\":" << value;
    fields_[key] = oss.str();
}

void JsonBuilder::AddNumber(const std::string& key, uint32_t value) {
    std::ostringstream oss;
    oss << "\"" << EscapeString(key) << "\":" << value;
    fields_[key] = oss.str();
}

void JsonBuilder::AddNumber(const std::string& key, double value) {
    std::ostringstream oss;
    oss << "\"" << EscapeString(key) << "\":" << std::fixed << std::setprecision(2) << value;
    fields_[key] = oss.str();
}

void JsonBuilder::AddBool(const std::string& key, bool value) {
    std::ostringstream oss;
    oss << "\"" << EscapeString(key) << "\":" << (value ? "true" : "false");
    fields_[key] = oss.str();
}

void JsonBuilder::AddObject(const std::string& key, const std::string& json_object) {
    std::ostringstream oss;
    oss << "\"" << EscapeString(key) << "\":" << json_object;
    fields_[key] = oss.str();
}

void JsonBuilder::AddArray(const std::string& key, const std::vector<std::string>& items) {
    std::ostringstream oss;
    oss << "\"" << EscapeString(key) << "\":[";
    for (size_t i = 0; i < items.size(); ++i) {
        if (i > 0) oss << ",";
        oss << items[i];
    }
    oss << "]";
    fields_[key] = oss.str();
}

std::string JsonBuilder::Build() const {
    std::ostringstream oss;
    oss << "{";
    bool first = true;
    for (const auto& pair : fields_) {
        if (!first) oss << ",";
        oss << pair.second;
        first = false;
    }
    oss << "}";
    return oss.str();
}

std::string JsonBuilder::EscapeString(const std::string& str) {
    std::ostringstream oss;
    for (char c : str) {
        switch (c) {
            case '"':  oss << "\\\""; break;
            case '\\': oss << "\\\\"; break;
            case '\b': oss << "\\b"; break;
            case '\f': oss << "\\f"; break;
            case '\n': oss << "\\n"; break;
            case '\r': oss << "\\r"; break;
            case '\t': oss << "\\t"; break;
            default:
                if (c < 0x20) {
                    oss << "\\u" << std::hex << std::setw(4) << std::setfill('0') << (int)c;
                } else {
                    oss << c;
                }
        }
    }
    return oss.str();
}

std::string JsonBuilder::BuildResponse(int code, const std::string& msg, const std::string& data) {
    JsonBuilder json;
    json.AddNumber("code", (int64_t)code);
    json.AddString("msg", msg);
    if (!data.empty()) {
        json.AddObject("data", data);
    }
    return json.Build();
}

// JsonArrayBuilder实现

JsonArrayBuilder::JsonArrayBuilder() {
}

JsonArrayBuilder::~JsonArrayBuilder() {
}

void JsonArrayBuilder::AddString(const std::string& value) {
    std::ostringstream oss;
    oss << "\"" << JsonBuilder::EscapeString(value) << "\"";
    items_.push_back(oss.str());
}

void JsonArrayBuilder::AddNumber(int64_t value) {
    std::ostringstream oss;
    oss << value;
    items_.push_back(oss.str());
}

void JsonArrayBuilder::AddNumber(double value) {
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(2) << value;
    items_.push_back(oss.str());
}

void JsonArrayBuilder::AddObject(const std::string& json_object) {
    items_.push_back(json_object);
}

std::string JsonArrayBuilder::Build() const {
    std::ostringstream oss;
    oss << "[";
    for (size_t i = 0; i < items_.size(); ++i) {
        if (i > 0) oss << ",";
        oss << items_[i];
    }
    oss << "]";
    return oss.str();
}

} // namespace gb_media_server
