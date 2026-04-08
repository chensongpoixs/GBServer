# CMake配置测试脚本
# 作者: chensong
# 日期: 2025-10-18

Write-Host "========================================" -ForegroundColor Cyan
Write-Host "CMake配置测试" -ForegroundColor Cyan
Write-Host "========================================" -ForegroundColor Cyan
Write-Host ""

# 检查库文件是否存在
Write-Host "1. 检查库文件..." -ForegroundColor Yellow
Write-Host ""

$websocketppFile = "third_party/websocketpp/websocketpp/server.hpp"
$jsonFile = "third_party/json/include/nlohmann/json.hpp"

Write-Host "websocketpp: " -NoNewline
if (Test-Path $websocketppFile) {
    Write-Host "✓ 存在" -ForegroundColor Green
    Write-Host "   路径: $(Resolve-Path $websocketppFile)" -ForegroundColor Gray
} else {
    Write-Host "✗ 不存在" -ForegroundColor Red
}

Write-Host ""
Write-Host "nlohmann/json: " -NoNewline
if (Test-Path $jsonFile) {
    Write-Host "✓ 存在" -ForegroundColor Green
    Write-Host "   路径: $(Resolve-Path $jsonFile)" -ForegroundColor Gray
} else {
    Write-Host "✗ 不存在" -ForegroundColor Red
}

Write-Host ""
Write-Host "========================================" -ForegroundColor Cyan
Write-Host "2. 检查CMakeLists.txt配置..." -ForegroundColor Yellow
Write-Host ""

# 检查CMakeLists.txt中的路径配置
$cmakeContent = Get-Content "CMakeLists.txt" -Raw

if ($cmakeContent -match 'CMAKE_CURRENT_SOURCE_DIR.*THIRD_PARTY_DIR.*websocketpp') {
    Write-Host "✓ CMakeLists.txt使用了正确的绝对路径" -ForegroundColor Green
} else {
    Write-Host "✗ CMakeLists.txt可能使用了错误的相对路径" -ForegroundColor Red
}

Write-Host ""
Write-Host "========================================" -ForegroundColor Cyan
Write-Host "3. 检查ENABLE_WEBSOCKET宏..." -ForegroundColor Yellow
Write-Host ""

$wsServiceHeader = Get-Content "server/ws_stats_service.h" -Raw

if ($wsServiceHeader -match '#define\s+ENABLE_WEBSOCKET\s+1') {
    Write-Host "✓ ENABLE_WEBSOCKET = 1 (已启用)" -ForegroundColor Green
} elseif ($wsServiceHeader -match '#define\s+ENABLE_WEBSOCKET\s+0') {
    Write-Host "✗ ENABLE_WEBSOCKET = 0 (已禁用)" -ForegroundColor Red
    Write-Host "   需要修改 server/ws_stats_service.h" -ForegroundColor Yellow
} else {
    Write-Host "? 未找到ENABLE_WEBSOCKET宏定义" -ForegroundColor Yellow
}

Write-Host ""
Write-Host "========================================" -ForegroundColor Cyan
Write-Host "总结" -ForegroundColor Cyan
Write-Host "========================================" -ForegroundColor Cyan
Write-Host ""

$websocketppOk = Test-Path $websocketppFile
$jsonOk = Test-Path $jsonFile
$enableOk = $wsServiceHeader -match '#define\s+ENABLE_WEBSOCKET\s+1'

if ($websocketppOk -and $jsonOk -and $enableOk) {
    Write-Host "✓ 所有检查通过，可以重新运行CMake配置" -ForegroundColor Green
    Write-Host ""
    Write-Host "下一步:" -ForegroundColor Yellow
    Write-Host "1. 进入build目录" -ForegroundColor White
    Write-Host "2. 运行: cmake .." -ForegroundColor White
    Write-Host "3. 编译项目" -ForegroundColor White
} else {
    Write-Host "✗ 部分检查失败，请先解决上述问题" -ForegroundColor Red
}

Write-Host ""
