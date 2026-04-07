#!/bin/bash
# WebSocket依赖库下载脚本
# 作者: chensong
# 日期: 2025-10-18
# 
# 用途: 手动下载websocketpp和nlohmann/json库
# 使用: bash download_websocket_libs.sh

# 颜色定义
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
CYAN='\033[0;36m'
NC='\033[0m' # No Color

echo -e "${CYAN}========================================${NC}"
echo -e "${CYAN}WebSocket依赖库下载脚本${NC}"
echo -e "${CYAN}========================================${NC}"
echo ""

# 设置目录
THIRD_PARTY_DIR="third_party"
WEBSOCKETPP_DIR="$THIRD_PARTY_DIR/websocketpp"
JSON_DIR="$THIRD_PARTY_DIR/json"

# 创建third_party目录
if [ ! -d "$THIRD_PARTY_DIR" ]; then
    mkdir -p "$THIRD_PARTY_DIR"
    echo -e "${GREEN}[创建]${NC} third_party目录"
fi

# ============================================================================
# 下载websocketpp
# ============================================================================
echo ""
echo -e "${YELLOW}1. 检查websocketpp...${NC}"

if [ -f "$WEBSOCKETPP_DIR/websocketpp/server.hpp" ]; then
    echo -e "${GREEN}[存在]${NC} websocketpp已存在，跳过下载"
else
    echo -e "${YELLOW}[下载]${NC} 正在下载websocketpp..."
    
    # 检查是否有git
    if command -v git &> /dev/null; then
        # 使用git下载
        echo -e "   ${CYAN}使用git克隆...${NC}"
        git clone --depth 1 --branch 0.8.2 https://github.com/zaphoyd/websocketpp.git "$WEBSOCKETPP_DIR"
        
        if [ $? -eq 0 ]; then
            echo -e "${GREEN}[成功]${NC} websocketpp下载完成"
        else
            echo -e "${RED}[失败]${NC} websocketpp下载失败"
            echo -e "   ${YELLOW}请手动下载: https://github.com/zaphoyd/websocketpp/archive/refs/tags/0.8.2.tar.gz${NC}"
            echo -e "   ${YELLOW}解压到: $WEBSOCKETPP_DIR${NC}"
        fi
    else
        # 使用wget或curl下载
        echo -e "   ${CYAN}git未安装，使用wget/curl下载...${NC}"
        
        TARBALL_URL="https://github.com/zaphoyd/websocketpp/archive/refs/tags/0.8.2.tar.gz"
        TARBALL_FILE="$THIRD_PARTY_DIR/websocketpp.tar.gz"
        
        echo -e "   ${CYAN}正在下载: $TARBALL_URL${NC}"
        
        # 尝试使用wget（带进度条）
        if command -v wget &> /dev/null; then
            wget --progress=bar:force -O "$TARBALL_FILE" "$TARBALL_URL" 2>&1 | \
                grep --line-buffered "%" | \
                sed -u -e "s,\.,,g" | \
                awk '{printf("\r   下载进度: %s", $2)}'
            echo ""  # 换行
        # 尝试使用curl（带进度条）
        elif command -v curl &> /dev/null; then
            curl -L --progress-bar -o "$TARBALL_FILE" "$TARBALL_URL"
        else
            echo -e "${RED}[失败]${NC} 未找到wget或curl，无法下载"
            echo -e "   ${YELLOW}请手动下载: $TARBALL_URL${NC}"
            echo -e "   ${YELLOW}解压到: $WEBSOCKETPP_DIR${NC}"
        fi
        
        if [ -f "$TARBALL_FILE" ]; then
            echo -e "   ${CYAN}下载完成，正在解压...${NC}"
            
            # 解压
            tar -xzf "$TARBALL_FILE" -C "$THIRD_PARTY_DIR"
            
            # 重命名目录
            if [ -d "$THIRD_PARTY_DIR/websocketpp-0.8.2" ]; then
                if [ -d "$WEBSOCKETPP_DIR" ]; then
                    rm -rf "$WEBSOCKETPP_DIR"
                fi
                mv "$THIRD_PARTY_DIR/websocketpp-0.8.2" "$WEBSOCKETPP_DIR"
            fi
            
            # 删除tarball
            rm -f "$TARBALL_FILE"
            
            echo -e "${GREEN}[成功]${NC} websocketpp下载并解压完成"
        fi
    fi
fi

# ============================================================================
# 下载nlohmann/json
# ============================================================================
echo ""
echo -e "${YELLOW}2. 检查nlohmann/json...${NC}"

if [ -f "$JSON_DIR/include/nlohmann/json.hpp" ]; then
    echo -e "${GREEN}[存在]${NC} nlohmann/json已存在，跳过下载"
else
    echo -e "${YELLOW}[下载]${NC} 正在下载nlohmann/json..."
    
    # 检查是否有git
    if command -v git &> /dev/null; then
        # 使用git下载
        echo -e "   ${CYAN}使用git克隆...${NC}"
        git clone --depth 1 --branch v3.11.2 https://github.com/nlohmann/json.git "$JSON_DIR"
        
        if [ $? -eq 0 ]; then
            echo -e "${GREEN}[成功]${NC} nlohmann/json下载完成"
        else
            echo -e "${RED}[失败]${NC} nlohmann/json下载失败"
            echo -e "   ${YELLOW}请手动下载: https://github.com/nlohmann/json/archive/refs/tags/v3.11.2.tar.gz${NC}"
            echo -e "   ${YELLOW}解压到: $JSON_DIR${NC}"
        fi
    else
        # 使用wget或curl下载
        echo -e "   ${CYAN}git未安装，使用wget/curl下载...${NC}"
        
        TARBALL_URL="https://github.com/nlohmann/json/archive/refs/tags/v3.11.2.tar.gz"
        TARBALL_FILE="$THIRD_PARTY_DIR/json.tar.gz"
        
        echo -e "   ${CYAN}正在下载: $TARBALL_URL${NC}"
        
        # 尝试使用wget（带进度条）
        if command -v wget &> /dev/null; then
            wget --progress=bar:force -O "$TARBALL_FILE" "$TARBALL_URL" 2>&1 | \
                grep --line-buffered "%" | \
                sed -u -e "s,\.,,g" | \
                awk '{printf("\r   下载进度: %s", $2)}'
            echo ""  # 换行
        # 尝试使用curl（带进度条）
        elif command -v curl &> /dev/null; then
            curl -L --progress-bar -o "$TARBALL_FILE" "$TARBALL_URL"
        else
            echo -e "${RED}[失败]${NC} 未找到wget或curl，无法下载"
            echo -e "   ${YELLOW}请手动下载: $TARBALL_URL${NC}"
            echo -e "   ${YELLOW}解压到: $JSON_DIR${NC}"
        fi
        
        if [ -f "$TARBALL_FILE" ]; then
            echo -e "   ${CYAN}下载完成，正在解压...${NC}"
            
            # 解压
            tar -xzf "$TARBALL_FILE" -C "$THIRD_PARTY_DIR"
            
            # 重命名目录
            if [ -d "$THIRD_PARTY_DIR/json-3.11.2" ]; then
                if [ -d "$JSON_DIR" ]; then
                    rm -rf "$JSON_DIR"
                fi
                mv "$THIRD_PARTY_DIR/json-3.11.2" "$JSON_DIR"
            fi
            
            # 删除tarball
            rm -f "$TARBALL_FILE"
            
            echo -e "${GREEN}[成功]${NC} nlohmann/json下载并解压完成"
        fi
    fi
fi

# ============================================================================
# 验证
# ============================================================================
echo ""
echo -e "${CYAN}========================================${NC}"
echo -e "${CYAN}验证结果${NC}"
echo -e "${CYAN}========================================${NC}"

WEBSOCKETPP_OK=false
JSON_OK=false

if [ -f "$WEBSOCKETPP_DIR/websocketpp/server.hpp" ]; then
    WEBSOCKETPP_OK=true
fi

if [ -f "$JSON_DIR/include/nlohmann/json.hpp" ]; then
    JSON_OK=true
fi

echo ""
echo -n "websocketpp: "
if [ "$WEBSOCKETPP_OK" = true ]; then
    echo -e "${GREEN}✓ 已安装${NC}"
    echo -e "   路径: $WEBSOCKETPP_DIR"
else
    echo -e "${RED}✗ 未安装${NC}"
fi

echo ""
echo -n "nlohmann/json: "
if [ "$JSON_OK" = true ]; then
    echo -e "${GREEN}✓ 已安装${NC}"
    echo -e "   路径: $JSON_DIR/include"
else
    echo -e "${RED}✗ 未安装${NC}"
fi

echo ""
if [ "$WEBSOCKETPP_OK" = true ] && [ "$JSON_OK" = true ]; then
    echo -e "${GREEN}========================================${NC}"
    echo -e "${GREEN}所有依赖库已就绪！${NC}"
    echo -e "${GREEN}========================================${NC}"
    echo ""
    echo -e "${YELLOW}下一步:${NC}"
    echo -e "${NC}1. 修改 server/ws_stats_service.h${NC}"
    echo -e "${NC}   将 #define ENABLE_WEBSOCKET 0 改为 1${NC}"
    echo -e "${NC}2. 重新运行CMake配置${NC}"
    echo -e "${NC}   cd build && cmake ..${NC}"
    echo -e "${NC}3. 编译项目${NC}"
    echo -e "${NC}   make -j4${NC}"
else
    echo -e "${RED}========================================${NC}"
    echo -e "${RED}部分依赖库安装失败${NC}"
    echo -e "${RED}========================================${NC}"
    echo ""
    echo -e "${YELLOW}请检查网络连接或手动下载${NC}"
    echo ""
    echo -e "${YELLOW}或者使用系统包管理器安装:${NC}"
    echo -e "${NC}Ubuntu/Debian:${NC}"
    echo -e "${NC}  sudo apt-get install libwebsocketpp-dev nlohmann-json3-dev${NC}"
    echo ""
    echo -e "${NC}CentOS/RHEL:${NC}"
    echo -e "${NC}  sudo yum install websocketpp-devel json-devel${NC}"
fi

echo ""
