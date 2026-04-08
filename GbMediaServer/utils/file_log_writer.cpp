/*
 *  Copyright (c) 2025 The CRTC project authors . All Rights Reserved.
 *
 *  Please visit https://chensongpoixs.github.io for detail
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */
 /*****************************************************************************
                   Author: chensong
                   date:  2025-10-14

输赢不重要，答案对你们有什么意义才重要。

光阴者，百代之过客也，唯有奋力奔跑，方能生风起时，是时代造英雄，英雄存在于时代。或许世人道你轻狂，可你本就年少啊。 看护好，自己的理想和激情。


我可能会遇到很多的人，听他们讲好2多的故事，我来写成故事或编成歌，用我学来的各种乐器演奏它。
然后还可能在一个国家遇到一个心仪我的姑娘，她可能会被我帅气的外表捕获，又会被我深邃的内涵吸引，在某个下雨的夜晚，她会全身淋透然后要在我狭小的住处换身上的湿衣服。
3小时候后她告诉我她其实是这个国家的公主，她愿意向父皇求婚。我不得已告诉她我是穿越而来的男主角，我始终要回到自己的世界。
然后我的身影慢慢消失，我看到她眼里的泪水，心里却没有任何痛苦，我才知道，原来我的心被丢掉了，我游历全世界的原因，就是要找回自己的本心。
于是我开始有意寻找各种各样失去心的人，我变成一块砖头，一颗树，一滴水，一朵白云，去听大家为什么会失去自己的本心。
我发现，刚出生的宝宝，本心还在，慢慢的，他们的本心就会消失，收到了各种黑暗之光的侵蚀。
从一次争论，到嫉妒和悲愤，还有委屈和痛苦，我看到一只只无形的手，把他们的本心扯碎，蒙蔽，偷走，再也回不到主人都身边。
我叫他本心猎手。他可能是和宇宙同在的级别 但是我并不害怕，我仔细回忆自己平淡的一生 寻找本心猎手的痕迹。
沿着自己的回忆，一个个的场景忽闪而过，最后发现，我的本心，在我写代码的时候，会回来。
安静，淡然，代码就是我的一切，写代码就是我本心回归的最好方式，我还没找到本心猎手，但我相信，顺着这个线索，我一定能顺藤摸瓜，把他揪出来。


 ******************************************************************************/

#include "utils/file_log_writer.h"

#include <cstdio>
#include <cstring>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <vector>

#if (defined(_MSVC_LANG) && _MSVC_LANG >= 201703L) || \
    (!defined(_MSVC_LANG) && defined(__cplusplus) && __cplusplus >= 201703L)
#include <filesystem>
namespace fs = std::filesystem;
#define GB_HAS_FILESYSTEM 1
#else
#define GB_HAS_FILESYSTEM 0
#ifdef _WIN32
#include <direct.h>
#else
#include <sys/stat.h>
#include <sys/types.h>
#endif
#endif

namespace gb_media_server {

namespace {

void PrintErr(const char* msg) {
  std::fprintf(stderr, "%s\n", msg);
  std::fflush(stderr);
}

bool ParseYmd(const std::string& s, int* y, int* mo, int* d) {
  if (s.size() != 10) {
    return false;
  }
#if defined(_WIN32)
  return sscanf_s(s.c_str(), "%d-%d-%d", y, mo, d) == 3;
#else
  return std::sscanf(s.c_str(), "%d-%d-%d", y, mo, d) == 3;
#endif
}

std::time_t MidnightLocal(int year, int mon, int day) {
  std::tm tm {};
  tm.tm_year = year - 1900;
  tm.tm_mon = mon - 1;
  tm.tm_mday = day;
  tm.tm_hour = 0;
  tm.tm_min = 0;
  tm.tm_sec = 0;
  return std::mktime(&tm);
}

}  // namespace

FileLogWriter::FileLogWriter()
    : worker_([this] { WorkerLoop(); }) {}

FileLogWriter::~FileLogWriter() {
  {
    std::lock_guard<std::mutex> qlk(queue_mutex_);
    worker_shutdown_ = true;
  }
  queue_cv_.notify_all();
  if (worker_.joinable()) {
    worker_.join();
  }
  std::lock_guard<std::mutex> flk(file_mutex_);
  CloseFileLocked();
}

void FileLogWriter::Configure(const FileLogConfig& cfg) {
  {
    std::lock_guard<std::mutex> flk(file_mutex_);
    CloseFileLocked();
    cfg_ = cfg;
    if (cfg_.max_lines_per_file < 1) {
      cfg_.max_lines_per_file = 1;
    }
    if (cfg_.retention_days < 0) {
      cfg_.retention_days = 0;
    }
    current_time_key_.clear();
    part_index_ = 1;
    lines_in_current_file_ = 0;
  }
  enabled_.store(cfg_.enabled, std::memory_order_relaxed);
  echo_stdout_.store(cfg_.echo_to_stdout, std::memory_order_relaxed);
  max_queued_.store(cfg_.max_queued_messages, std::memory_order_relaxed);
}

void FileLogWriter::WorkerLoop() {
  while (true) {
    std::vector<std::string> batch;
    {
      std::unique_lock<std::mutex> qlk(queue_mutex_);
      queue_cv_.wait(qlk, [this] {
        return worker_shutdown_ || !queue_.empty();
      });
      if (worker_shutdown_ && queue_.empty()) {
        break;
      }
      while (!queue_.empty()) {
        batch.push_back(std::move(queue_.front()));
        queue_.pop_front();
      }
    }

    for (const auto& s : batch) {
      const bool en = enabled_.load(std::memory_order_relaxed);
      if (en) {
        std::lock_guard<std::mutex> flk(file_mutex_);
        WritePayloadLocked(s);
      } else if (echo_stdout_.load(std::memory_order_relaxed)) {
        std::fwrite(s.data(), 1, s.size(), stdout);
        std::fflush(stdout);
      }
    }
  }
}

void FileLogWriter::WritePayloadLocked(const std::string& payload) {
  if (cfg_.enabled) {
    EnsureCurrentFileLocked();
    if (file_) {
      const size_t line_delta = CountLinesInMessage(payload.c_str());
      std::fwrite(payload.data(), 1, payload.size(), file_);
      std::fflush(file_);
      if (line_delta > 0) {
        MaybeRotateByLinesLocked(line_delta);
      }
    }
  }
  if (cfg_.echo_to_stdout) {
    std::fwrite(payload.data(), 1, payload.size(), stdout);
    std::fflush(stdout);
  }
}

std::string FileLogWriter::TodayLocalDateString() {
  const std::time_t t = std::time(nullptr);
#if defined(_WIN32)
  struct tm lt {};
  localtime_s(&lt, &t);
#else
  struct tm lt {};
  localtime_r(&t, &lt);
#endif
  std::ostringstream oss;
  oss << std::setfill('0') << std::setw(4) << (lt.tm_year + 1900)  
      << std::setw(2) << (lt.tm_mon + 1)   << std::setw(2) << lt.tm_mday;
  return oss.str();
}

std::string FileLogWriter::NowLocalDateTimeKeyString() {
  const std::time_t t = std::time(nullptr);
#if defined(_WIN32)
  struct tm lt {};
  localtime_s(&lt, &t);
#else
  struct tm lt {};
  localtime_r(&t, &lt);
#endif
  std::ostringstream oss;
  oss << std::setfill('0') << std::setw(4) << (lt.tm_year + 1900)
      << std::setw(2) << (lt.tm_mon + 1)  << std::setw(2) << lt.tm_mday
      << std::setw(2) /*<< lt.tm_hour  << std::setw(2) << lt.tm_min 
      << std::setw(2) << lt.tm_sec*/;
  return oss.str();
}

size_t FileLogWriter::CountLinesInMessage(const char* message) {
  if (!message || !message[0]) {
    return 0;
  }
  size_t nl = 0;
  for (const char* p = message; *p; ++p) {
    if (*p == '\n') {
      ++nl;
    }
  }
  const size_t len = std::strlen(message);
  const bool ends_nl = len > 0 && message[len - 1] == '\n';
  return nl + (ends_nl ? 0 : 1);
}

std::string FileLogWriter::MakeFilePathLocked() const {
  std::ostringstream name;
  name << "gbmedia_server_" << current_time_key_ << '_'
       << std::setfill('0') << std::setw(2) << part_index_ << ".log";
#if GB_HAS_FILESYSTEM
  fs::path full = fs::path(cfg_.directory) / name.str();
  return full.string();
#else
  std::string base = cfg_.directory;
  if (!base.empty() && base.back() != '/' && base.back() != '\\') {
    base += '/';
  }
  return base + name.str();
#endif
}

void FileLogWriter::CloseFileLocked() {
  if (file_) {
    std::fclose(file_);
    file_ = nullptr;
  }
}

void FileLogWriter::PurgeExpiredLogsLocked() {
#if GB_HAS_FILESYSTEM
  if (cfg_.retention_days <= 0) {
    return;
  }
  fs::path dir(cfg_.directory);
  std::error_code ec;
  if (!fs::exists(dir, ec)) {
    return;
  }
  const std::string today = TodayLocalDateString();
  int ty = 0;
  int tmo = 0;
  int td = 0;
  if (!ParseYmd(today, &ty, &tmo, &td)) {
    return;
  }
  std::tm tm_cut {};
  tm_cut.tm_year = ty - 1900;
  tm_cut.tm_mon = tmo - 1;
  tm_cut.tm_mday = td;
  tm_cut.tm_hour = 0;
  tm_cut.tm_min = 0;
  tm_cut.tm_sec = 0;
  tm_cut.tm_mday -= cfg_.retention_days;
  const std::time_t cutoff = std::mktime(&tm_cut);
  if (cutoff == static_cast<std::time_t>(-1)) {
    return;
  }

  const std::string prefix = "gbmedia_server_";
  const std::string suffix = ".log";
  const size_t min_old = prefix.size() + 10 + 1 + 2 + suffix.size();
  for (const auto& ent : fs::directory_iterator(dir, ec)) {
    if (ec) {
      break;
    }
    if (!ent.is_regular_file(ec)) {
      continue;
    }
    const std::string fname = ent.path().filename().string();
    if (fname.size() < min_old || fname.compare(0, prefix.size(), prefix) != 0) {
      continue;
    }
    if (fname.compare(fname.size() - suffix.size(), suffix.size(), suffix) != 0) {
      continue;
    }
    const std::string date_part = fname.substr(prefix.size(), 10);
    int fy = 0;
    int fmo = 0;
    int fd = 0;
    if (!ParseYmd(date_part, &fy, &fmo, &fd)) {
      continue;
    }
    const std::time_t file_midnight = MidnightLocal(fy, fmo, fd);
    if (file_midnight == static_cast<std::time_t>(-1)) {
      continue;
    }
    if (file_midnight < cutoff) {
      fs::remove(ent.path(), ec);
    }
  }
#endif
}

void FileLogWriter::OpenNewFileLocked() {
  CloseFileLocked();
#if GB_HAS_FILESYSTEM
  std::error_code ec;
  fs::create_directories(cfg_.directory, ec);
  if (ec) {
    PrintErr(("file_log: create_directories failed: " + ec.message()).c_str());
    return;
  }
#else
#ifdef _WIN32
  _mkdir(cfg_.directory.c_str());
#else
  mkdir(cfg_.directory.c_str(), 0755);
#endif
#endif
  const std::string path = MakeFilePathLocked();
  file_ = std::fopen(path.c_str(), "ab");
  if (!file_) {
    PrintErr(("file_log: fopen failed: " + path).c_str());
    return;
  }
  lines_in_current_file_ = 0;
}

void FileLogWriter::MaybeRotateByDateLocked() {
  const std::string today = TodayLocalDateString();
  if (current_time_key_.empty()) {
    current_time_key_ = NowLocalDateTimeKeyString();
    PurgeExpiredLogsLocked();
    OpenNewFileLocked();
    return;
  }
  const std::string segment_date =
      current_time_key_.size() >= 10 ? current_time_key_.substr(0, 10) : std::string();
  if (today == segment_date) {
    return;
  }
  CloseFileLocked();
  current_time_key_ = NowLocalDateTimeKeyString();
  part_index_ = 1;
  PurgeExpiredLogsLocked();
  OpenNewFileLocked();
}

void FileLogWriter::MaybeRotateByLinesLocked(size_t lines_added) {
  if (!file_) {
    return;
  }
  lines_in_current_file_ += static_cast<int64_t>(lines_added);
  if (lines_in_current_file_ < cfg_.max_lines_per_file) {
    return;
  }
  ++part_index_;
  OpenNewFileLocked();
}

void FileLogWriter::EnsureCurrentFileLocked() {
  if (!cfg_.enabled) {
    CloseFileLocked();
    return;
  }
  MaybeRotateByDateLocked();
  if (!cfg_.enabled) {
    return;
  }
  if (!file_) {
    if (current_time_key_.empty()) {
      current_time_key_ = NowLocalDateTimeKeyString();
      PurgeExpiredLogsLocked();
    }
    OpenNewFileLocked();
  }
}

void FileLogWriter::Write(const char* message) {
  if (!message || !message[0]) {
    return;
  }
  const bool en = enabled_.load(std::memory_order_relaxed);
  const bool echo = echo_stdout_.load(std::memory_order_relaxed);
  if (!en && !echo) {
    return;
  }
  std::string s(message);
  {
    std::lock_guard<std::mutex> qlk(queue_mutex_);
    const size_t cap = max_queued_.load(std::memory_order_relaxed);
    if (cap > 0) {
      while (queue_.size() >= cap) {
        queue_.pop_front();
      }
    }
    queue_.push_back(std::move(s));
  }
  queue_cv_.notify_one();
}

}  // namespace gb_media_server
