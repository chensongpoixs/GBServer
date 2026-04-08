# WebSocket修复验证脚本
# 日期: 2025-10-18

Write-Host "========================================" -ForegroundColor Cyan
Write-Host "WebSocket修复验证脚本" -ForegroundColor Cyan
Write-Host "========================================" -ForegroundColor Cyan
Write-Host ""

# 1. 检查文件是否存在
Write-Host "[1/5] 检查文件..." -ForegroundColor Yellow
if (Test-Path "server/ws_stats_service.cpp") {
    Write-Host "  ✓ server/ws_stats_service.cpp 存在" -ForegroundColor Green
} else {
    Write-Host "  ✗ server/ws_stats_service.cpp 不存在" -ForegroundColor Red
    exit 1
}

# 2. 检查SendClientMessage定义数量
Write-Host "[2/5] 检查SendClientMessage定义数量..." -ForegroundColor Yellow
$content = Get-Content "server/ws_stats_service.cpp" -Raw
$matches = [regex]::Matches($content, "void WebSocketStatsService::SendClientMessage")
$count = $matches.Count
if ($count -eq 1) {
    Write-Host "  ✓ SendClientMessage有 $count 个定义（正确）" -ForegroundColor Green
} else {
    Write-Host "  ✗ SendClientMessage有 $count 个定义（应该只有1个）" -ForegroundColor Red
    exit 1
}

# 3. 检查是否包含ws_server_->send调用
Write-Host "[3/5] 检查ws_server_->send调用..." -ForegroundColor Yellow
if ($content -match "ws_server_->send") {
    Write-Host "  ✓ 包含 ws_server_->send 调用" -ForegroundColor Green
} else {
    Write-Host "  ✗ 未找到 ws_server_->send 调用" -ForegroundColor Red
    exit 1
}

# 4. 检查是否还有TODO
Write-Host "[4/5] 检查TODO..." -ForegroundColor Yellow
$todoMatches = [regex]::Matches($content, "// TODO:")
$todoCount = $todoMatches.Count
if ($todoCount -eq 0) {
    Write-Host "  ✓ 没有TODO（所有功能已实现）" -ForegroundColor Green
} else {
    Write-Host "  ⚠ 发现 $todoCount 个TODO" -ForegroundColor Yellow
}

# 5. 检查WebSocket库
Write-Host "[5/5] 检查WebSocket依赖库..." -ForegroundColor Yellow
$allLibsExist = $true

if (Test-Path "third_party/websocketpp/websocketpp/server.hpp") {
    Write-Host "  ✓ websocketpp 已安装" -ForegroundColor Green
} else {
    Write-Host "  ✗ websocketpp 未安装" -ForegroundColor Red
    $allLibsExist = $false
}

if (Test-Path "third_party/json/include/nlohmann/json.hpp") {
    Write-Host "  ✓ nlohmann/json 已安装" -ForegroundColor Green
} else {
    Write-Host "  ✗ nlohmann/json 未安装" -ForegroundColor Red
    $allLibsExist = $false
}

if (Test-Path "third_party/asio/asio/include/asio.hpp") {
    Write-Host "  ✓ ASIO 已安装" -ForegroundColor Green
} else {
    Write-Host "  ✗ ASIO 未安装" -ForegroundColor Red
    $allLibsExist = $false
}

Write-Host ""
Write-Host "========================================" -ForegroundColor Cyan
Write-Host "验证结果" -ForegroundColor Cyan
Write-Host "========================================" -ForegroundColor Cyan

if ($allLibsExist) {
    Write-Host "✓ 所有检查通过！" -ForegroundColor Green
    Write-Host ""
    Write-Host "下一步操作:" -ForegroundColor Yellow
    Write-Host "1. 编译项目: cd build_local && cmake .. && make -j4" -ForegroundColor White
    Write-Host "2. 启动后端: ./GbMediaServer" -ForegroundColor White
    Write-Host "3. 启动前端: cd monitor-dashboard && npm run dev" -ForegroundColor White
    Write-Host "4. 打开浏览器: http://localhost:3001/" -ForegroundColor White
    Write-Host ""
    Write-Host "预期结果: 前端应该能正常显示实时统计数据" -ForegroundColor Green
} else {
    Write-Host "⚠ 部分检查未通过" -ForegroundColor Yellow
    Write-Host ""
    Write-Host "请先安装WebSocket依赖库:" -ForegroundColor Yellow
    Write-Host "  ./download_websocket_libs.ps1" -ForegroundColor White
}

Write-Host ""
