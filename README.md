# GbMediaServer 流媒体服务文档


## 目录

- [项目概述](#项目概述)
- [架构设计](#架构设计)
- [核心组件](#核心组件)
- [功能特性](#功能特性)
- [配置说明](#配置说明)
- [API接口](#api接口)
- [使用示例](#使用示例)
- [构建和部署](#构建和部署)
- [技术栈](#技术栈)
- [常见问题](#常见问题)
- [性能优化](#性能优化)
- [开发指南](#开发指南)
- [架构详解](#架构详解)
- [许可证](#许可证)
- [贡献](#贡献)
- [联系方式](#联系方式)
- [更新日志](#更新日志)
- [参考资源](#参考资源)


![WebRTC播放gb28181视频流效果](img/gb28181_play.gif)


## 项目概述

GbMediaServer 是一个高性能的流媒体服务器，支持多种流媒体协议和格式。它主要用于接收GB28181设备的视频流，并提供多种输出格式（WebRTC、FLV、HLS等）供客户端播放。

### 主要功能

- **多协议支持**：支持GB28181、WebRTC、RTSP、HTTP-FLV、HLS等协议
- **流媒体转换**：支持不同协议之间的流媒体格式转换
- **实时播放**：支持低延迟的实时流媒体播放
- **多路分发**：支持一个输入流分发给多个客户端
- **HLS录制**：支持HLS格式的流媒体录制和播放

## 架构设计

### 整体架构

```
┌─────────────────────────────────────────────────────────┐
│                    GbMediaService                        │
│  ┌──────────────┐  ┌──────────────┐  ┌──────────────┐  │
│  │  RtcService  │  │  WebService  │  │   Session    │  │
│  └──────────────┘  └──────────────┘  └──────────────┘  │
│                                                           │
│  ┌──────────────┐  ┌──────────────┐  ┌──────────────┐  │
│  │   Producer   │  │   Consumer   │  │    Stream    │  │
│  └──────────────┘  └──────────────┘  └──────────────┘  │
└─────────────────────────────────────────────────────────┘
```

### 核心概念

#### Session（会话）
- 会话是流媒体服务的基本管理单元
- 每个会话对应一个媒体流（格式：`app/stream`）
- 一个会话可以有一个Producer和多个Consumer

#### Producer（生产者）
- 负责接收外部媒体流（推流）
- 支持的类型：
  - `kProducerTypeGB28181`：GB28181协议推流
  - `kProducerTypeRtc`：WebRTC协议推流
  - `kProducerTypeRtsp`：RTSP协议推流

#### Consumer（消费者）
- 负责向客户端分发媒体流（拉流）
- 支持的类型：
  - `kConsumerTypeRtc`：WebRTC协议拉流
  - `kConsumerTypeFlv`：HTTP-FLV协议拉流
  - `kConsumerTypeHls`：HLS协议拉流
  - `kConsumerTypeTs`：TS分片拉流

#### Stream（流）
- 管理媒体流的存储和分发
- 负责HLS播放列表和分片文件的管理
- 处理音视频帧的缓存和分发

## 核心组件

### 1. GbMediaService

流媒体服务的核心管理类，采用单例模式。

**主要功能：**
- 管理所有会话（Session）的生命周期
- 初始化RTC服务器和Web服务器
- 管理RTP服务器（用于GB28181协议）

**关键方法：**
```cpp
// 创建会话
std::shared_ptr<Session> CreateSession(const std::string &session_name, bool split=true);

// 查找会话
std::shared_ptr<Session> FindSession(const std::string &session_name);

// 关闭会话
bool CloseSession(const std::string &session_name);

// 初始化服务
bool Init(const char * config_file);

// 启动服务
void Start();
```

### 2. RtcService

RTC服务类，负责管理所有WebRTC相关的连接和数据包路由。

**主要功能：**
- 管理RTC接口（RtcProducer/RtcConsumer）的注册和注销
- 接收来自RTC服务器的数据包（STUN、DTLS、RTP、RTCP）
- 根据远程地址或会话名称路由数据包到对应的RTC接口
- 提供任务队列工厂供RTC接口使用

**关键方法：**
```cpp
// 添加RTC消费者
void AddConsumer(std::shared_ptr<RtcInterface> rtc_interface);

// 移除RTC消费者
void RemoveConsumer(std::shared_ptr<RtcInterface> rtc_interface);

// 处理STUN数据包
void OnStun(rtc::AsyncPacketSocket* socket, const uint8_t * data, size_t len, 
            const rtc::SocketAddress& addr, const int64_t& ms);

// 处理DTLS数据包
void OnDtls(rtc::AsyncPacketSocket* socket, const uint8_t* data, size_t len,
            const rtc::SocketAddress& addr, const int64_t& ms);

// 处理RTP数据包
void OnRtp(rtc::AsyncPacketSocket* socket, const uint8_t* data, size_t len,
           const rtc::SocketAddress& addr, const int64_t& ms);

// 处理RTCP数据包
void OnRtcp(rtc::AsyncPacketSocket* socket, const uint8_t* data, size_t len,
            const rtc::SocketAddress& addr, const int64_t& ms);
```

### 3. WebService

Web服务类，提供HTTP接口用于流媒体的推拉流操作。

**支持的HTTP接口：**

| 接口路径 | 方法 | 说明 |
|---------|------|------|
| `/rtc/push` | POST | WebRTC推流 |
| `/rtc/play` | POST | WebRTC拉流 |
| `*.flv` | GET | HTTP-FLV拉流 |
| `*.m3u8` | GET | HLS播放列表 |
| `*.ts` | GET | TS分片文件 |
| `/api/openRtpServer` | POST | 打开RTP服务器 |
| `/api/closeRtpServer` | POST | 关闭RTP服务器 |

### 4. Session

会话管理类，管理一个媒体流会话的生命周期。

**主要功能：**
- 创建和管理Producer（生产者）
- 创建和管理Consumer（消费者）
- 管理媒体流的输入和输出
- 处理音视频帧的分发

**关键方法：**
```cpp
// 创建生产者
std::shared_ptr<Producer> CreateProducer(const std::string &session_name,
                                          const std::string &param,
                                          ShareResourceType type);

// 创建消费者
std::shared_ptr<Consumer> CreateConsumer(libmedia_transfer_protocol::libnetwork::Connection* conn,
                                         const std::string &session_name,
                                         const std::string &param,
                                         ShareResourceType type);

// 添加视频帧
void AddVideoFrame(libmedia_codec::EncodedImage&& frame);

// 添加音频帧
void AddAudioFrame(rtc::CopyOnWriteBuffer&& frame, int64_t pts);
```

### 5. Stream

流管理类，负责媒体流的存储和HLS分片管理。

**主要功能：**
- 管理HLS播放列表（M3U8）
- 管理TS分片文件
- 处理音视频帧的缓存

**关键方法：**
```cpp
// 获取播放列表
std::string GetPlayList();

// 获取分片文件
std::shared_ptr<libmedia_transfer_protocol::libhls::Fragment> GetFragement(const std::string &name);
```

## 功能特性

### 1. GB28181协议支持

- 接收GB28181设备的RTP流
- 支持PS（Program Stream）格式解析
- 自动解码PS流为H264视频和AAC音频
- 支持TCP和UDP两种传输方式

### 2. WebRTC协议支持

- 完整的WebRTC协议栈实现
- 支持SDP（Session Description Protocol）协商
- 支持DTLS握手和SRTP加密
- 支持ICE（Interactive Connectivity Establishment）
- 支持SCTP数据通道
- 支持TWCC（Transport-CC）带宽估计

### 3. HTTP-FLV协议支持

- 支持HTTP-FLV实时流播放
- 低延迟的流媒体传输
- 自动封装音视频帧为FLV格式

### 4. HLS协议支持

- 支持HLS（HTTP Live Streaming）播放
- 自动生成M3U8播放列表
- 自动生成TS分片文件
- 支持HLS录制

### 5. 多路分发

- 一个输入流可以分发给多个客户端
- 支持不同协议同时输出（如同时支持WebRTC和FLV）
- 自动管理消费者的生命周期

## 配置说明

### 配置文件格式

配置文件采用YAML格式，默认文件名为 `gbmedia_server.yaml`。

### 配置示例

```yaml
# HTTP服务器配置
http:
  port: 8001
  # [可选] HTTPS配置，默认不开启
  # ssl:
  #   enabled: false
  #   key-store: classpath:test.monitor.89iot.cn.jks
  #   key-store-password: gpf64qmw
  #   key-store-type: JKS

# RTC服务器配置
rtc:
  ips:
    - 192.168.9.174
  udp:
    port: 10001
  tcp:
    port: 20001
  cert: fullchain.pem
  key: privkey.pem

# RTP端口配置
rtp:
  tcp:
    min_port: 20000
    max_port: 30000
  udp:
    min_port: 20000
    max_port: 30000
```

### 配置项说明

#### HTTP服务器配置
- `port`：HTTP服务器监听端口，默认8001

#### RTC服务器配置
- `ips`：RTC服务器绑定的IP地址列表
- `udp.port`：UDP端口，用于STUN、DTLS、RTP/RTCP
- `tcp.port`：TCP端口，用于TCP模式的RTC连接
- `cert`：DTLS证书文件路径（公钥）
- `key`：DTLS证书文件路径（私钥）

#### RTP端口配置
- `tcp.min_port` / `tcp.max_port`：TCP RTP端口范围
- `udp.min_port` / `udp.max_port`：UDP RTP端口范围

## API接口

### 1. WebRTC推流

**接口：** `POST /rtc/push`

**请求参数：**
- `streamurl`：流地址，格式为 `app/stream`（如：`live/stream1`）
- `sdp`：SDP Offer（JSON格式）

**请求示例：**
```json
POST /rtc/push?streamurl=live/stream1
Content-Type: application/json

{
  "sdp": "v=0\r\no=- 123456789 123456789 IN IP4 127.0.0.1\r\n...",
  "type": "offer"
}
```

**响应示例：**
```json
{
  "sdp": "v=0\r\no=- 987654321 987654321 IN IP4 127.0.0.1\r\n...",
  "type": "answer"
}
```

### 2. WebRTC拉流

**接口：** `POST /rtc/play`

**请求参数：**
- `streamurl`：流地址，格式为 `app/stream`
- `sdp`：SDP Offer（JSON格式）

**请求和响应格式与推流相同**

### 3. HTTP-FLV拉流

**接口：** `GET /{app}/{stream}.flv`

**请求示例：**
```
GET /live/stream1.flv
```

**响应：**
- 返回HTTP 200状态码
- Content-Type: `video/x-flv`
- 以流式方式返回FLV数据

### 4. HLS播放列表

**接口：** `GET /{app}/{stream}.m3u8`

**请求示例：**
```
GET /live/stream1.m3u8
```

**响应：**
- 返回M3U8播放列表内容

### 5. TS分片文件

**接口：** `GET /{app}/{stream}/{fragment}.ts`

**请求示例：**
```
GET /live/stream1/segment_001.ts
```

**响应：**
- 返回TS分片文件内容

### 6. 打开RTP服务器

**接口：** `POST /api/openRtpServer`

**功能说明：**
- 打开一个RTP服务器，用于接收GB28181设备的RTP流
- 自动创建会话和GB28181生产者
- 返回分配的RTP端口信息

**请求参数（JSON）：**
```json
{
  "port": 5060,
  "tcpmode": 1,
  "streamid": "34020000001320000001"
}
```

**参数说明：**
- `port`：RTP端口（可选，实际使用配置的端口范围）
- `tcpmode`：传输模式（0=UDP, 1=TCP）
- `streamid`：流ID，用于标识GB28181设备

**响应示例：**
```json
{
  "code": 0,
  "tcpmode": 1,
  "streamid": "34020000001320000001",
  "port": 20000
}
```

### 7. 关闭RTP服务器

**接口：** `POST /api/closeRtpServer`

**功能说明：**
- 关闭指定的RTP服务器
- 释放相关会话和资源

**请求参数（JSON）：**
```json
{
  "streamid": "34020000001320000001"
}
```

**响应示例：**
```json
{
  "code": 0,
  "streamid": "34020000001320000001"
}
```

## 使用示例

### 1. GB28181设备推流流程

```bash
# 1. 打开RTP服务器，接收GB28181设备的RTP流
curl -X POST http://localhost:8001/api/openRtpServer \
  -H "Content-Type: application/json" \
  -d '{
    "port": 5060,
    "tcpmode": 1,
    "streamid": "34020000001320000001"
  }'

# 响应：
# {
#   "code": 0,
#   "tcpmode": 1,
#   "streamid": "34020000001320000001",
#   "port": 20000
# }

# 2. 通过WebRTC播放流
# 客户端连接到 ws://localhost:8001/rtc/play?streamurl=live/34020000001320000001

# 3. 通过HTTP-FLV播放流
# 访问 http://localhost:8001/live/34020000001320000001.flv

# 4. 通过HLS播放流
# 访问 http://localhost:8001/live/34020000001320000001.m3u8
```

### 2. WebRTC推流示例

```javascript
// 客户端代码示例（伪代码）
const pc = new RTCPeerConnection({
  iceServers: [{ urls: 'stun:localhost:10001' }]
});

// 添加本地媒体流
navigator.mediaDevices.getUserMedia({ video: true, audio: true })
  .then(stream => {
    stream.getTracks().forEach(track => pc.addTrack(track, stream));
    return pc.createOffer();
  })
  .then(offer => {
    return pc.setLocalDescription(offer);
  })
  .then(() => {
    // 发送SDP Offer到服务器
    return fetch('http://localhost:8001/rtc/push?streamurl=live/stream1', {
      method: 'POST',
      headers: { 'Content-Type': 'application/json' },
      body: JSON.stringify({
        streamurl: 'live/stream1',
        sdp: pc.localDescription.sdp,
        type: 'offer'
      })
    });
  })
  .then(response => response.json())
  .then(answer => {
    // 设置远程SDP Answer
    return pc.setRemoteDescription(new RTCSessionDescription(answer));
  });
```

### 3. WebRTC拉流示例

```javascript
// 客户端代码示例（伪代码）
const pc = new RTCPeerConnection({
  iceServers: [{ urls: 'stun:localhost:10001' }]
});

// 接收远程流
pc.ontrack = (event) => {
  const video = document.getElementById('video');
  video.srcObject = event.streams[0];
};

// 创建Offer
pc.createOffer()
  .then(offer => {
    return pc.setLocalDescription(offer);
  })
  .then(() => {
    // 发送SDP Offer到服务器
    return fetch('http://localhost:8001/rtc/play?streamurl=live/stream1', {
      method: 'POST',
      headers: { 'Content-Type': 'application/json' },
      body: JSON.stringify({
        streamurl: 'live/stream1',
        sdp: pc.localDescription.sdp,
        type: 'offer'
      })
    });
  })
  .then(response => response.json())
  .then(answer => {
    // 设置远程SDP Answer
    return pc.setRemoteDescription(new RTCSessionDescription(answer));
  });
```

### 4. HTTP-FLV播放示例

```html
<!-- 使用flv.js播放HTTP-FLV流 -->
<script src="https://cdn.jsdelivr.net/npm/flv.js/dist/flv.min.js"></script>
<video id="videoElement" controls></video>
<script>
  if (flvjs.isSupported()) {
    const videoElement = document.getElementById('videoElement');
    const flvPlayer = flvjs.createPlayer({
      type: 'flv',
      url: 'http://localhost:8001/live/stream1.flv'
    });
    flvPlayer.attachMediaElement(videoElement);
    flvPlayer.load();
    flvPlayer.play();
  }
</script>
```

### 5. HLS播放示例

```html
<!-- 使用hls.js播放HLS流 -->
<script src="https://cdn.jsdelivr.net/npm/hls.js@latest"></script>
<video id="video" controls></video>
<script>
  const video = document.getElementById('video');
  const videoSrc = 'http://localhost:8001/live/stream1.m3u8';
  
  if (Hls.isSupported()) {
    const hls = new Hls();
    hls.loadSource(videoSrc);
    hls.attachMedia(video);
  } else if (video.canPlayType('application/vnd.apple.mpegurl')) {
    video.src = videoSrc;
  }
</script>
```

## 构建和部署

### 依赖要求

- **CMake** >= 2.8
- **C++编译器**：支持C++11或更高版本
- **WebRTC库**：libwebrtc
- **OpenSSL**：用于DTLS和SRTP
- **yaml-cpp**：用于配置文件解析
- **其他依赖**：见CMakeLists.txt

### 编译步骤

```bash
# 1. 创建构建目录
mkdir build
cd build

# 2. 配置CMake
cmake ..

# 3. 编译
cmake --build .

# 4. 运行
./GbMediaServer
```

### 配置文件

将配置文件 `gbmedia_server.yaml` 放置在可执行文件同目录下，或通过命令行参数指定：

```bash
./GbMediaServer /path/to/gbmedia_server.yaml
```

### 部署建议

1. **端口配置**：确保HTTP端口、RTC端口和RTP端口范围未被占用
2. **防火墙设置**：开放必要的端口（HTTP、UDP RTC、RTP端口范围）
3. **证书配置**：如需HTTPS或DTLS，配置相应的证书文件
4. **日志管理**：日志文件会自动生成，建议定期清理

## 技术栈

### 核心库

- **WebRTC**：实时通信协议栈
- **libmedia_transfer_protocol**：媒体传输协议库
- **libmedia_codec**：媒体编解码库 

### 协议支持

- **GB28181**：国标视频监控协议
- **WebRTC**：Web实时通信协议
- **RTP/RTCP**：实时传输协议
- **SRTP/SRTCP**：安全实时传输协议
- **DTLS**：数据报传输层安全协议
- **SCTP**：流控制传输协议
- **HTTP-FLV**：基于HTTP的FLV流媒体
- **HLS**：HTTP Live Streaming

### 编码格式

- **视频**：H.264/AVC
- **音频**：AAC、Opus
- **容器**：FLV、MPEG-TS、PS

## 常见问题

### 1. 端口被占用

**问题**：启动时提示端口被占用

**解决方案**：
- 检查配置文件中的端口设置
- 使用 `netstat` 或 `lsof` 查看端口占用情况
- 修改配置文件使用其他端口

### 2. DTLS握手失败

**问题**：WebRTC连接时DTLS握手失败

**解决方案**：
- 检查证书文件路径是否正确
- 确保证书文件格式正确（PEM格式）
- 检查证书文件权限

### 3. GB28181流无法接收

**问题**：GB28181设备推流后无法播放

**解决方案**：
- 检查RTP服务器是否成功打开
- 检查网络连接和防火墙设置
- 查看日志文件确认是否有错误信息
- 验证设备配置的RTP端口是否正确
- 检查PS流解析是否正常

### 4. FLV播放延迟高

**问题**：HTTP-FLV播放延迟较高

**解决方案**：
- 检查网络带宽是否充足
- 优化编码参数（降低码率、分辨率）
- 使用WebRTC协议获得更低延迟
- 检查服务器负载情况

### 5. HLS播放列表不更新

**问题**：HLS播放列表（M3U8）不更新或分片文件缺失

**解决方案**：
- 检查流是否正常输入
- 确认HLS分片生成是否正常
- 检查磁盘空间是否充足
- 查看Stream对象的日志输出

## 性能优化

### 1. 服务器性能优化

- **线程池配置**：合理配置工作线程数量
- **内存管理**：控制音视频帧缓存大小
- **网络优化**：使用TCP_NODELAY减少延迟
- **CPU优化**：合理使用硬件加速（如H.264硬件编码）

### 2. 网络优化

- **带宽控制**：根据网络情况调整码率
- **拥塞控制**：使用TWCC（Transport-CC）进行带宽估计
- **NAT穿透**：配置STUN/TURN服务器
- **多路复用**：合理使用端口范围

### 3. 编码优化

- **关键帧间隔**：合理设置GOP大小
- **码率控制**：使用CBR或VBR模式
- **分辨率适配**：根据客户端需求动态调整
- **帧率控制**：平衡流畅度和带宽

## 开发指南

### 1. 添加新的Producer类型

```cpp
// 1. 继承Producer基类
class MyProducer : public Producer {
public:
    MyProducer(const std::shared_ptr<Stream>& stream, 
               const std::shared_ptr<Session>& session);
    virtual ~MyProducer();
    
    // 实现纯虚函数
    virtual ShareResourceType ShareResouceType() const override;
    virtual void OnVideoFrame(const libmedia_codec::EncodedImage& frame) override;
    virtual void OnAudioFrame(const rtc::CopyOnWriteBuffer& frame, int64_t pts) override;
};

// 2. 在Session::CreateProducer中添加新类型
std::shared_ptr<Producer> Session::CreateProducer(...) {
    switch(type) {
        case ShareResourceType::kProducerTypeMy:
            return std::make_shared<MyProducer>(stream_, shared_from_this());
        // ...
    }
}
```

### 2. 添加新的Consumer类型

```cpp
// 1. 继承Consumer基类
class MyConsumer : public Consumer {
public:
    MyConsumer(libmedia_transfer_protocol::libnetwork::Connection* conn,
               const std::shared_ptr<Stream>& stream,
               const std::shared_ptr<Session>& session);
    virtual ~MyConsumer();
    
    // 实现纯虚函数
    virtual ShareResourceType ShareResouceType() const override;
    virtual void OnVideoFrame(const libmedia_codec::EncodedImage& frame) override;
    virtual void OnAudioFrame(const rtc::CopyOnWriteBuffer& frame, int64_t pts) override;
};

// 2. 在WebService中添加HTTP处理器
void WebService::HandlerMyConsumer(...) {
    // 创建MyConsumer并添加到Session
}
```

### 3. 日志系统

```cpp
// 使用GBMEDIASERVER_LOG宏记录日志
GBMEDIASERVER_LOG(LS_INFO) << "Session created: " << session_name;
GBMEDIASERVER_LOG(LS_WARNING) << "Failed to create producer";
GBMEDIASERVER_LOG(LS_ERROR) << "Critical error occurred";
```

日志级别：
- `LS_VERBOSE`：详细调试信息
- `LS_INFO`：一般信息
- `LS_WARNING`：警告信息
- `LS_ERROR`：错误信息
- `LS_NONE`：不输出日志

## 架构详解

### 数据流图

```
GB28181设备
    |
    | RTP/PS流
    v
RTP服务器 (TcpServer)
    |
    | 原始RTP包
    v
Gb28181Producer
    |
    | PS解析 -> H.264/AAC
    v
Session
    |
    | 音视频帧
    v
Stream (HLS Muxer)
    |
    +---> RtcConsumer (WebRTC)
    |         |
    |         v
    |     客户端 (浏览器)
    |
    +---> FlvConsumer (HTTP-FLV)
    |         |
    |         v
    |     客户端 (flv.js)
    |
    +---> HLS (M3U8/TS)
              |
              v
          客户端 (HLS播放器)
```

### 线程模型

- **主线程**：程序入口，初始化服务
- **网络线程**：处理网络I/O（RTP、HTTP、WebRTC）
- **工作线程**：处理媒体数据处理（编解码、封装）
- **信令线程**：处理WebRTC信令（SDP、ICE）

### 内存管理

- 使用智能指针（`std::shared_ptr`、`std::unique_ptr`）管理对象生命周期
- 音视频帧使用移动语义（`std::move`）减少拷贝
- 使用`rtc::CopyOnWriteBuffer`优化缓冲区管理
- 及时释放不再使用的Session和Consumer

## 许可证

本项目采用BSD许可证，详见LICENSE文件。

## 贡献

欢迎提交Issue和Pull Request来改进项目。

## 联系方式

- **项目主页**：https://chensongpoixs.github.io
- **作者**：chensong

## 更新日志

### v1.0.0 (2025-10-13)
- 初始版本发布
- 支持GB28181协议推流
- 支持WebRTC推拉流
- 支持HTTP-FLV播放
- 支持HLS播放
- 支持RTP服务器管理

## 参考资源

- [GB/T 28181-2016 标准](http://www.gb688.cn/bzgk/gb/newGbInfo?hcno=469659DC56F9B8187DD9EEE50D0A1428)
- [WebRTC官方文档](https://webrtc.org/)
- [RFC 3550 - RTP](https://tools.ietf.org/html/rfc3550)
- [RFC 3640 - RTP Payload Format for MPEG-4](https://tools.ietf.org/html/rfc3640)
- [HTTP Live Streaming (HLS) 规范](https://tools.ietf.org/html/rfc8216)

---

**注意**：本文档会持续更新，请关注项目最新版本。