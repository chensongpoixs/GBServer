# 重新编译并重启后端服务
# 日期: 2025-10-18

Write-Host "========================================" -ForegroundColor Cyan
Write-Host "重新编译并重启后端服务" -ForegroundColor Cyan
Write-Host "========================================" -ForegroundColor Cyan
Write-Host ""

# 检查是否在正确的目录
if (-not (Test-Path "server/ws_stats_service.cpp")) {
    Write-Host "错误: 请在项目根目录运行此脚本" -ForegroundColor Red
    Write-Host "当前目录: $(Get-Location)" -ForegroundColor Yellow
    Write-Host "应该在: D:\Work\crtc\GBServer\GbMediaServer" -ForegroundColor Yellow
    exit 1
}

Write-Host "[1/4] 检查构建目录..." -ForegroundColor Yellow
if (-not (Test-Path "build_local")) {
    Write-Host "  创建构建目录..." -ForegroundColor Yellow
    New-Item -ItemType Directory -Path "build_local" | Out-Null
}
Write-Host "  ✓ 构建目录存在" -ForegroundColor Green

Write-Host ""
Write-Host "[2/4] 配置CMake..." -ForegroundColor Yellow
Push-Location build_local
try {
    cmake .. 2>&1 | Out-Null
    if ($LASTEXITCODE -eq 0) {
        Write-Host "  ✓ CMake配置成功" -ForegroundColor Green
    } else {
        Write-Host "  ✗ CMake配置失败" -ForegroundColor Red
        Write-Host "  请手动运行: cd build_local && cmake .." -ForegroundColor Yellow
        exit 1
    }
} finally {
    Pop-Location
}

Write-Host ""
Write-Host "[3/4] 编译项目..." -ForegroundColor Yellow
Write-Host "  这可能需要几分钟，请耐心等待..." -ForegroundColor Cyan

Push-Location build_local
try {
    $startTime = Get-Date
    cmake --build . --config Release -j4 2>&1 | Out-Null
    $endTime = Get-Date
    $duration = ($endTime - $startTime).TotalSeconds
    
    if ($LASTEXITCODE -eq 0) {
        Write-Host "  ✓ 编译成功 (耗时: $([math]::Round($duration, 1))秒)" -ForegroundColor Green
    } else {
        Write-Host "  ✗ 编译失败" -ForegroundColor Red
        Write-Host "  请手动运行: cd build_local && cmake --build . --config Release -j4" -ForegroundColor Yellow
        exit 1
    }
} finally {
    Pop-Location
}

Write-Host ""
Write-Host "[4/4] 检查可执行文件..." -ForegroundColor Yellow
$exePath = "build_local\Release\GbMediaServer.exe"
if (Test-Path $exePath) {
    $fileInfo = Get-Item $exePath
    Write-Host "  ✓ 可执行文件已生成" -ForegroundColor Green
    Write-Host "  路径: $exePath" -ForegroundColor White
    Write-Host "  大小: $([math]::Round($fileInfo.Length / 1MB, 2)) MB" -ForegroundColor White
    Write-Host "  修改时间: $($fileInfo.LastWriteTime)" -ForegroundColor White
} else {
    Write-Host "  ✗ 可执行文件未找到" -ForegroundColor Red
    exit 1
}

Write-Host ""
Write-Host "========================================" -ForegroundColor Cyan
Write-Host "编译完成！" -ForegroundColor Green
Write-Host "========================================" -ForegroundColor Cyan
Write-Host ""
Write-Host "下一步操作:" -ForegroundColor Yellow
Write-Host "1. 如果后端正在运行，请先停止（按 Ctrl+C）" -ForegroundColor White
Write-Host "2. 启动新的后端服务:" -ForegroundColor White
Write-Host "   cd build_local\Release" -ForegroundColor Cyan
Write-Host "   .\GbMediaServer.exe" -ForegroundColor Cyan
Write-Host ""
Write-Host "3. 刷新前端页面:" -ForegroundColor White
Write-Host "   打开 http://localhost:3001/" -ForegroundColor Cyan
Write-Host "   按 F5 刷新" -ForegroundColor Cyan
Write-Host ""
Write-Host "预期结果: 前端应该能收到数据并正常显示" -ForegroundColor Green
Write-Host ""

# 询问是否立即启动
Write-Host "是否立即启动后端服务? (Y/N)" -ForegroundColor Yellow
$response = Read-Host
if ($response -eq 'Y' -or $response -eq 'y') {
    Write-Host ""
    Write-Host "正在启动后端服务..." -ForegroundColor Cyan
    Write-Host "按 Ctrl+C 可以停止服务" -ForegroundColor Yellow
    Write-Host ""
    Push-Location build_local\Release
    try {
        .\GbMediaServer.exe
    } finally {
        Pop-Location
    }
} else {
    Write-Host ""
    Write-Host "请手动启动后端服务" -ForegroundColor Yellow
}
