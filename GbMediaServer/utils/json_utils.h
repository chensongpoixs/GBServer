/******************************************************************************
 *  Copyright (c) 2025 The CRTC project authors . All Rights Reserved.
 ******************************************************************************/
 /*****************************************************************************
				   Author: chensong
				   date:  2025-10-18
 ******************************************************************************/

#ifndef _GB_MEDIA_SERVER_JSON_UTILS_H_
#define _GB_MEDIA_SERVER_JSON_UTILS_H_

#include <string>
#include <sstream>
#include <vector>
#include <map>

namespace gb_media_server {

/**
 * @author chensong
 * @date 2025-10-18
 * @brief JSON工具类（JSON Utilities）
 * 
 * 提供简单的JSON构建和序列化功能，用于统计数据的JSON格式化。
 * 
 * 主要功能：
 * 1. JSON对象构建
 * 2. JSON数组构建
 * 3. 基本类型转换
 * 4. 字符串转义
 * 
 * 使用示例：
 * @code
 * JsonBuilder json;
 * json.AddString("name", "test");
 * json.AddNumber("value", 123);
 * json.AddBool("enabled", true);
 * std::string result = json.Build();
 * @endcode
 */
class JsonBuilder {
public:
    JsonBuilder();
    ~JsonBuilder();
    
    // 添加字符串字段
    void AddString(const std::string& key, const std::string& value);
    
    // 添加数字字段
    void AddNumber(const std::string& key, int64_t value);
    void AddNumber(const std::string& key, uint64_t value);
    void AddNumber(const std::string& key, uint32_t value);
    void AddNumber(const std::string& key, double value);
    
    // 添加布尔字段
    void AddBool(const std::string& key, bool value);
    
    // 添加对象字段
    void AddObject(const std::string& key, const std::string& json_object);
    
    // 添加数组字段
    void AddArray(const std::string& key, const std::vector<std::string>& items);
    
    // 构建JSON字符串
    std::string Build() const;
    
    // 静态工具方法
    static std::string EscapeString(const std::string& str);
    static std::string BuildResponse(int code, const std::string& msg, const std::string& data = "");
    
private:
    std::map<std::string, std::string> fields_;
    bool first_field_;
};

/**
 * @brief JSON数组构建器
 */
class JsonArrayBuilder {
public:
    JsonArrayBuilder();
    ~JsonArrayBuilder();
    
    // 添加字符串元素
    void AddString(const std::string& value);
    
    // 添加数字元素
    void AddNumber(int64_t value);
    void AddNumber(double value);
    
    // 添加对象元素
    void AddObject(const std::string& json_object);
    
    // 构建JSON数组字符串
    std::string Build() const;
    
private:
    std::vector<std::string> items_;
};

} // namespace gb_media_server

#endif // _GB_MEDIA_SERVER_JSON_UTILS_H_
