/******************************************************************************
 *  Copyright (c) 2025 The CRTC project authors . All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 ******************************************************************************/
 /*****************************************************************************
				   Author: chensong
				   date:  2025-10-18

输赢不重要，答案对你们有什么意义才重要。

光阴者，百代之过客也，唯有奋力奔跑，方能生风起时，是时代造英雄，英雄存在于时代。或许世人道你轻狂，可你本就年少啊。 看护好，自己的理想和激情。

 ******************************************************************************/

#ifndef _GB_MEDIA_SERVER_STATISTICS_BASE_H_
#define _GB_MEDIA_SERVER_STATISTICS_BASE_H_

#include <string>
#include <mutex>
#include <chrono>
#include <memory>

namespace gb_media_server {

/**
 * @author chensong
 * @date 2025-10-18
 * @brief 统计数据基类（Statistics Base）
 * 
 * 所有统计数据类的基类，提供通用的统计功能。
 * 
 * 主要功能：
 * 1. 时间戳管理：记录创建时间和最后更新时间
 * 2. JSON序列化：将统计数据转换为JSON格式
 * 3. 数据重置：重置所有统计数据
 * 4. 线程安全：使用互斥锁保护数据访问
 * 
 * 使用示例：
 * @code
 * class MyStatistics : public StatisticsBase {
 * public:
 *     virtual std::string ToJson() const override {
 *         // 实现JSON序列化
 *     }
 * };
 * @endcode
 */
class StatisticsBase {
public:
    /**
     * @brief 构造函数
     * 
     * 初始化统计数据基类，设置创建时间和最后更新时间为当前时间。
     */
    StatisticsBase();
    
    /**
     * @brief 虚析构函数
     * 
     * 确保派生类的析构函数被正确调用。
     */
    virtual ~StatisticsBase();
    
    /**
     * @brief 获取统计数据的JSON表示
     * 
     * 纯虚函数，派生类必须实现该方法，将统计数据转换为JSON格式。
     * 
     * @return JSON格式的统计数据字符串
     * @note 派生类应该实现完整的JSON序列化逻辑
     */
    virtual std::string ToJson() const = 0;
    
    /**
     * @brief 重置统计数据
     * 
     * 重置所有统计数据到初始状态，包括时间戳。
     * 派生类可以重写该方法以重置特定的统计数据。
     */
    virtual void Reset();
    
    /**
     * @brief 更新时间戳
     * 
     * 更新最后更新时间为当前时间。
     * 该方法应该在统计数据更新时调用。
     */
    void UpdateTimestamp();
    
    /**
     * @brief 获取创建时间
     * 
     * @return 创建时间戳（毫秒）
     */
    int64_t GetCreatedTime() const { return created_time_; }
    
    /**
     * @brief 获取最后更新时间
     * 
     * @return 最后更新时间戳（毫秒）
     */
    int64_t GetLastUpdateTime() const { return last_update_time_; }
    
    /**
     * @brief 获取持续时长
     * 
     * 计算从创建到当前的持续时长。
     * 
     * @return 持续时长（秒）
     */
    int64_t GetDuration() const;
    
public:
    /**
     * @brief 获取当前时间戳（毫秒）
     * 
     * @return 当前时间戳（毫秒）
     */
    static int64_t GetCurrentTimeMs();
    
protected:
    int64_t created_time_;           ///< 创建时间戳（毫秒）
    int64_t last_update_time_;       ///< 最后更新时间戳（毫秒）
    mutable std::mutex mutex_;       ///< 线程安全锁
};

} // namespace gb_media_server

#endif // _GB_MEDIA_SERVER_STATISTICS_BASE_H_
