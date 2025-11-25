# GbMediaServer 流媒体服务文档
 
![WebRTC播放gb28181视频流效果](img/gb28181_play.gif)
 
## 📖 项目简介
 

GbMediaServer 是一个高性能的流媒体服务器，支持多种流媒体协议和格式。它主要用于接收GB28181设备的视频流，并提供多种输出格式（WebRTC、FLV、HLS等）供客户端播放。该系统实现了低延迟的实时流媒体传输、多协议转换和多路分发功能。


## ✨ 核心功能

### 1. 多协议支持
- **GB28181协议**：接收国标视频监控设备的RTP流
- **WebRTC协议**：支持WebRTC推流和拉流
- **RTSP协议**：支持RTSP推流和拉流
- **RTMP协议**：支持RTMP推流和拉流
- **HTTP-FLV协议**：支持HTTP-FLV实时流播放
- **HLS协议**：支持HLS（HTTP Live Streaming）播放和录制

### 2. 流媒体转换
- 支持不同协议之间的流媒体格式转换
- 自动解析PS流为H.264视频和AAC音频
- 支持音视频分离和合并
- 自动封装为FLV、TS等格式

### 3. 实时播放
- 基于WebRTC的低延迟实时流媒体播放
- 支持HTTP-FLV低延迟播放
- 支持HLS自适应码率播放
- 音视频同步处理

### 4. 多路分发
- 一个输入流可以分发给多个客户端
- 支持不同协议同时输出（如同时支持WebRTC和FLV）
- 自动管理消费者的生命周期
- 支持动态添加和移除消费者

### 5. HLS录制
- 自动生成M3U8播放列表
- 自动生成TS分片文件
- 支持HLS录制和回放
- 自动清理过期分片

## 🏗️ 系统架构

### 整体架构

```
┌─────────────────────────────────────────────────────────────┐
│                    GbMediaServer 系统架构                      │
└─────────────────────────────────────────────────────────────┘

┌──────────────────┐         ┌──────────────────┐
│  GB28181设备      │         │  WebRTC客户端     │
│                  │         │                  │
│  RTP/PS流        │         │  推流/拉流       │
└────────┬─────────┘         └────────┬─────────┘
         │                            │
         │                            │
         ▼                            ▼
┌──────────────────────────────────────────────┐
│         GbMediaService (核心服务)             │
│                                              │
│  ┌──────────────┐  ┌──────────────┐        │
│  │ RtcService   │  │ WebService   │        │
│  │ (WebRTC管理) │  │ (HTTP API)   │        │
│  └──────────────┘  └──────────────┘        │
│                                              │
│  ┌──────────────┐  ┌──────────────┐        │
│  │  Session     │  │   Stream     │        │
│  │  (会话管理)   │  │  (流管理)    │        │
│  └──────────────┘  └──────────────┘        │
└──────────────────────────────────────────────┘
         │                            │
         │                            │
         ▼                            ▼
┌──────────────────┐         ┌──────────────────┐
│   Producer       │         │   Consumer       │
│   (生产者)       │         │   (消费者)       │
│                  │         │                  │
│ - GB28181        │         │ - WebRTC         │
│ - WebRTC         │         │ - HTTP-FLV      │
│ - RTSP           │         │ - HLS            │
│ - RTMP           │         │ - RTSP           │
└──────────────────┘         └──────────────────┘
```

### 模块组成

- **GbMediaService**：核心服务类，采用单例模式，管理整个服务器的生命周期
- **RtcService**：RTC服务类，负责管理所有WebRTC相关的连接和数据包路由
- **WebService**：Web服务类，提供HTTP接口用于流媒体的推拉流操作
- **Session**：会话管理类，管理一个媒体流会话的生命周期
- **Stream**：流管理类，负责媒体流的存储和HLS分片管理
- **Producer**：生产者基类，负责接收外部媒体流（推流）
- **Consumer**：消费者基类，负责向客户端分发媒体流（拉流）

## 📦 项目结构

```
GBServer/
├── GbMediaServer/              # 主程序模块
│   ├── main.cpp               # 程序入口
│   ├── server/                # 服务器模块
│   │   ├── gb_media_service.h/cpp    # 核心服务类
│   │   ├── rtc_service.h/cpp         # RTC服务
│   │   ├── web_service.h/cpp          # Web服务
│   │   ├── session.h/cpp              # 会话管理
│   │   └── stream.h/cpp               # 流管理
│   ├── producer/              # 生产者模块
│   │   ├── producer.h/cpp             # 生产者基类
│   │   ├── gb28181_producer.h/cpp     # GB28181生产者
│   │   ├── rtc_producer.h/cpp         # WebRTC生产者
│   │   ├── crtsp_producer.h/cpp       # RTSP生产者
│   │   └── crtmp_producer.h/cpp       # RTMP生产者
│   ├── consumer/              # 消费者模块
│   │   ├── consumer.h/cpp             # 消费者基类
│   │   ├── rtc_consumer.h/cpp          # WebRTC消费者
│   │   ├── flv_consumer.h/cpp          # FLV消费者
│   │   ├── crtsp_consumer.h/cpp       # RTSP消费者
│   │   └── crtmp_consumer.h/cpp        # RTMP消费者
│   ├── share/                 # 共享资源模块
│   │   ├── rtc_interface.h/cpp         # RTC接口
│   │   └── share_resource.h/cpp       # 共享资源
│   └── utils/                 # 工具模块
│       ├── yaml_config.h/cpp          # YAML配置解析
│       └── string_utils.h/cpp          # 字符串工具
└── README.md                  # 项目文档
```

## 🚀 快速开始

### 环境要求

- **操作系统**：Windows 10/11 或 Linux
- **开发环境**：Visual Studio 2017+ 或 GCC 7+
- **CMake**：3.8 或更高版本
- **WebRTC库**：libwebrtc（需要自行编译）
- **OpenSSL**：用于DTLS和SRTP
- **yaml-cpp**：用于配置文件解析

### 编译步骤

1. **克隆项目**
   ```bash
   git clone https://github.com/chensongpoixs/GBServer.git
   cd GBServer
   ```

2. **准备依赖**
   - 下载并编译 Google WebRTC 源码
   - 安装 OpenSSL 开发库
   - 安装 yaml-cpp 库

3. **配置CMake**
   ```bash
   mkdir build
   cd build
   cmake .. -DWebRTC_ROOT=/path/to/webrtc
   ```

4. **编译项目**
   ```bash
   cmake --build . --config Release
   ```

5. **运行程序**
   ```bash
   ./GbMediaServer/gbmedia_server.exe gbmedia_server.yaml
   ```

### 配置文件

创建配置文件 `gbmedia_server.yaml`：

```yaml
# HTTP服务器配置
http:
  port: 8001

# RTC服务器配置
rtc:
  ips:
    - 192.168.1.100
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

## 📖 使用说明

### GB28181设备推流流程

1. **打开RTP服务器**
   ```bash
   curl -X POST http://localhost:8001/api/openRtpServer \
     -H "Content-Type: application/json" \
     -d '{
       "port": 5060,
       "tcpmode": 1,
       "streamid": "34020000001320000001"
     }'
   ```

2. **配置GB28181设备**
   - 将设备的RTP服务器地址设置为服务器IP
   - 将RTP端口设置为API返回的端口
   - 启动设备推流

3. **播放流**
   - WebRTC播放：`http://localhost:8001/rtc/play?streamurl=live/34020000001320000001`
   - HTTP-FLV播放：`http://localhost:8001/live/34020000001320000001.flv`
   - HLS播放：`http://localhost:8001/live/34020000001320000001.m3u8`

### WebRTC推流示例

```javascript
// 客户端代码示例
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
    return pc.setRemoteDescription(new RTCSessionDescription(answer));
  });
```

### HTTP-FLV播放示例

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

## 🔧 配置说明

### 配置文件格式

配置文件采用YAML格式，默认文件名为 `gbmedia_server.yaml`。

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

## 📝 API 文档

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

## 🏛️ 架构详解

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
  - `kProducerTypeRtmp`：RTMP协议推流

#### Consumer（消费者）
- 负责向客户端分发媒体流（拉流）
- 支持的类型：
  - `kConsumerTypeRtc`：WebRTC协议拉流
  - `kConsumerTypeFlv`：HTTP-FLV协议拉流
  - `kConsumerTypeHls`：HLS协议拉流
  - `kConsumerTypeRtsp`：RTSP协议拉流
  - `kConsumerTypeRtmp`：RTMP协议拉流

#### Stream（流）
- 管理媒体流的存储和分发
- 负责HLS播放列表和分片文件的管理
- 处理音视频帧的缓存和分发

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

## 🔍 故障排查

### 常见问题

#### 1. 端口被占用

**问题**：启动时提示端口被占用

**解决方案：**
- 检查配置文件中的端口设置
- 使用 `netstat` 或 `lsof` 查看端口占用情况
- 修改配置文件使用其他端口

#### 2. DTLS握手失败

**问题**：WebRTC连接时DTLS握手失败

**解决方案：**
- 检查证书文件路径是否正确
- 确保证书文件格式正确（PEM格式）
- 检查证书文件权限

#### 3. GB28181流无法接收

**问题**：GB28181设备推流后无法播放

**解决方案：**
- 检查RTP服务器是否成功打开
- 检查网络连接和防火墙设置
- 查看日志文件确认是否有错误信息
- 验证设备配置的RTP端口是否正确
- 检查PS流解析是否正常

#### 4. FLV播放延迟高

**问题**：HTTP-FLV播放延迟较高

**解决方案：**
- 检查网络带宽是否充足
- 优化编码参数（降低码率、分辨率）
- 使用WebRTC协议获得更低延迟
- 检查服务器负载情况

#### 5. HLS播放列表不更新

**问题**：HLS播放列表（M3U8）不更新或分片文件缺失

**解决方案：**
- 检查流是否正常输入
- 确认HLS分片生成是否正常
- 检查磁盘空间是否充足
- 查看Stream对象的日志输出

### 调试工具

- **日志输出**：程序会输出详细的日志信息到文件
- **WebRTC日志**：可以启用 WebRTC 内部日志
- **网络抓包**：使用 Wireshark 分析网络流量

## 🔒 安全建议

### 认证和授权

- 实现用户认证机制
- 添加访问控制列表（ACL）
- 使用 TLS 加密 HTTP 连接

### 数据加密

- **SRTP**：媒体流加密（已内置）
- **DTLS**：数据通道加密（已内置）
- **TLS**：HTTP通道加密（建议实现）

### 输入验证

- 验证流地址格式
- 限制RTP端口范围
- 实现流访问频率限制

## 🚀 性能优化

### 服务器性能优化

- **线程池配置**：合理配置工作线程数量
- **内存管理**：控制音视频帧缓存大小
- **网络优化**：使用TCP_NODELAY减少延迟
- **CPU优化**：合理使用硬件加速（如H.264硬件编码）

### 网络优化

- **带宽控制**：根据网络情况调整码率
- **拥塞控制**：使用TWCC（Transport-CC）进行带宽估计
- **NAT穿透**：配置STUN/TURN服务器
- **多路复用**：合理使用端口范围

### 编码优化

- **关键帧间隔**：合理设置GOP大小
- **码率控制**：使用CBR或VBR模式
- **分辨率适配**：根据客户端需求动态调整
- **帧率控制**：平衡流畅度和带宽

## 💻 开发指南

### 添加新的Producer类型

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

### 添加新的Consumer类型

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

### 日志系统

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

## 📚 技术栈

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

## 🤝 贡献指南

### 代码规范

- 使用 C++11 或更高版本标准
- 遵循项目现有的代码风格
- 添加详细的注释（Doxygen 格式）
- 确保代码通过编译和测试

### 提交流程

1. Fork 项目
2. 创建功能分支
3. 提交更改
4. 创建 Pull Request

## 📄 许可证

本项目采用BSD许可证，详见LICENSE文件。

## 👥 作者

- **chensong** - 项目创建者和维护者

## 🙏 致谢

- Google WebRTC 团队
- 所有贡献者和用户

## 🔮 未来计划

### 功能增强
- [ ] 支持多用户同时连接
- [ ] 实现流录制功能
- [ ] 支持流转码
- [ ] 实现流统计和监控

### 平台扩展
- [ ] 支持更多编码格式（H.265、VP9等）
- [ ] 支持音频转码
- [ ] 实现流质量自适应

### 性能优化
- [ ] GPU加速编码
- [ ] 自适应码率算法优化
- [ ] 网络传输优化

## 📚 参考资源

- [GB/T 28181-2016 标准](http://www.gb688.cn/bzgk/gb/newGbInfo?hcno=469659DC56F9B8187DD9EEE50D0A1428)
- [WebRTC官方文档](https://webrtc.org/)
- [RFC 3550 - RTP](https://tools.ietf.org/html/rfc3550)
- [RFC 3640 - RTP Payload Format for MPEG-4](https://tools.ietf.org/html/rfc3640)
- [HTTP Live Streaming (HLS) 规范](https://tools.ietf.org/html/rfc8216)

## 🐛 问题反馈

如果遇到问题或有建议，请通过以下方式反馈：

- 提交 Issue
- 发送邮件
- 创建 Pull Request

 

**注意**：本文档会持续更新，请关注项目最新版本。
 
**最后更新**：2025-10-14  
**版本**：1.0.0
 

