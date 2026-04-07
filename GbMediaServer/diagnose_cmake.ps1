# CMake配置诊断脚本
# 作者: chensong
# 日期: 2025-10-18

Write-Host "========================================" -ForegroundColor Cyan
Write-Host "CMake WebSocket配置诊断" -ForegroundColor Cyan
Write-Host "========================================" -ForegroundColor Cyan
Write-Host ""

# 1. 检查库文件
Write-Host "1. 检查库文件..." -ForegroundColor Yellow
Write-Host ""

$websocketppHeader = "third_party/websocketpp/websocketpp/server.hpp"
$websocketppConfig = "third_party/websocketpp/websocketpp/config/asio_no_tls.hpp"
$jsonHeader = "third_party/json/include/nlohmann/json.hpp"

Write-Host "websocketpp/server.hpp: " -NoNewline
if (Test-Path $websocketppHeader) {
    Write-Host "✓" -ForegroundColor Green
} else {
    Write-Host "✗" -ForegroundColor Red
}

Write-Host "websocketpp/config/asio_no_tls.hpp: " -NoNewline
if (Test-Path $websocketppConfig) {
    Write-Host "✓" -ForegroundColor Green
    $fullPath = Resolve-Path $websocketppConfig
    Write-Host "   完整路径: $fullPath" -ForegroundColor Gray
} else {
    Write-Host "✗" -ForegroundColor Red
}

Write-Host "nlohmann/json.hpp: " -NoNewline
if (Test-Path $jsonHeader) {
    Write-Host "✓" -ForegroundColor Green
} else {
    Write-Host "✗" -ForegroundColor Red
}

# 2. 检查CMakeLists.txt配置
Write-Host ""
Write-Host "========================================" -ForegroundColor Cyan
Write-Host "2. 检查CMakeLists.txt..." -ForegroundColor Yellow
Write-Host ""

$cmakeContent = Get-Content "CMakeLists.txt" -Raw

# 检查路径检测
if ($cmakeContent -match 'CMAKE_CURRENT_SOURCE_DIR.*THIRD_PARTY_DIR.*websocketpp') {
    Write-Host "✓ 使用绝对路径检测" -ForegroundColor Green
} else {
    Write-Host "✗ 可能使用相对路径" -ForegroundColor Red
}

# 检查include_directories
if ($cmakeContent -match 'include_directories\(\$\{WEBSOCKETPP_INCLUDE_DIR\}\)') {
    Write-Host "✓ 条件添加websocketpp包含路径" -ForegroundColor Green
} elseif ($cmakeContent -match '\$\{WEBSOCKETPP_INCLUDE_DIR\}') {
    Write-Host "⚠ 使用变量但可能未条件检查" -ForegroundColor Yellow
} else {
    Write-Host "✗ 未添加websocketpp包含路径" -ForegroundColor Red
}

# 3. 检查ENABLE_WEBSOCKET
Write-Host ""
Write-Host "========================================" -ForegroundColor Cyan
Write-Host "3. 检查ENABLE_WEBSOCKET..." -ForegroundColor Yellow
Write-Host ""

$wsHeader = Get-Content "server/ws_stats_service.h" -Raw

if ($wsHeader -match '#define\s+ENABLE_WEBSOCKET\s+1') {
    Write-Host "✓ ENABLE_WEBSOCKET = 1" -ForegroundColor Green
} elseif ($wsHeader -match '#define\s+ENABLE_WEBSOCKET\s+0') {
    Write-Host "✗ ENABLE_WEBSOCKET = 0 (已禁用)" -ForegroundColor Red
    Write-Host "   需要修改为 1" -ForegroundColor Yellow
} else {
    Write-Host "? 未找到ENABLE_WEBSOCKET定义" -ForegroundColor Yellow
}

# 4. 生成CMake测试命令
Write-Host ""
Write-Host "========================================" -ForegroundColor Cyan
Write-Host "4. CMake变量测试" -ForegroundColor Yellow
Write-Host ""

$currentDir = (Get-Location).Path
$websocketppPath = Join-Path $currentDir "third_party\websocketpp"
$jsonPath = Join-Path $currentDir "third_party\json\include"

Write-Host "预期的CMake变量值:" -ForegroundColor Cyan
Write-Host "WEBSOCKETPP_INCLUDE_DIR = $websocketppPath" -ForegroundColor Gray
Write-Host "NLOHMANN_JSON_INCLUDE_DIR = $jsonPath" -ForegroundColor Gray

# 5. 建议的修复步骤
Write-Host ""
Write-Host "========================================" -ForegroundColor Cyan
Write-Host "修复步骤" -ForegroundColor Cyan
Write-Host "========================================" -ForegroundColor Cyan
Write-Host ""

$needFix = $false

if (-not (Test-Path $websocketppConfig)) {
    Write-Host "1. 库文件缺失，运行下载脚本:" -ForegroundColor Yellow
    Write-Host "   .\download_websocket_libs.ps1" -ForegroundColor White
    $needFix = $true
}

if ($wsHeader -match '#define\s+ENABLE_WEBSOCKET\s+0') {
    Write-Host "2. 启用WebSocket功能:" -ForegroundColor Yellow
    Write-Host "   修改 server/ws_stats_service.h" -ForegroundColor White
    Write-Host "   将 #define ENABLE_WEBSOCKET 0 改为 1" -ForegroundColor White
    $needFix = $true
}

Write-Host "3. 清除CMake缓存并重新配置:" -ForegroundColor Yellow
Write-Host "   cd ..\build" -ForegroundColor White
Write-Host "   Remove-Item CMakeCache.txt, CMakeFiles -Recurse -Force" -ForegroundColor White
Write-Host "   cmake .." -ForegroundColor White

Write-Host ""
Write-Host "4. 查看CMake输出，确认包含路径:" -ForegroundColor Yellow
Write-Host "   应该看到:" -ForegroundColor White
Write-Host "   -- Found local websocketpp: ..." -ForegroundColor Gray
Write-Host "   -- Found local nlohmann/json: ..." -ForegroundColor Gray
Write-Host "   -- 添加websocketpp包含路径: ..." -ForegroundColor Gray
Write-Host "   -- 添加nlohmann/json包含路径: ..." -ForegroundColor Gray

Write-Host ""
if (-not $needFix) {
    Write-Host "✓ 配置看起来正常，可以尝试重新运行CMake" -ForegroundColor Green
} else {
    Write-Host "⚠ 请先完成上述修复步骤" -ForegroundColor Yellow
}

Write-Host ""
