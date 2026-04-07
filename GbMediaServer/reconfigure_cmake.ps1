# CMake重新配置脚本
# 作者: chensong
# 日期: 2025-10-18
#
# 用途: 清除CMake缓存并重新配置

param(
    [string]$BuildDir = "..\..\build",  # 默认为D:\Work\crtc\build
    [string]$SourceDir = ".."            # 默认为上级目录
)

Write-Host "========================================" -ForegroundColor Cyan
Write-Host "CMake重新配置" -ForegroundColor Cyan
Write-Host "========================================" -ForegroundColor Cyan
Write-Host ""

# 获取当前目录
$currentDir = Get-Location
Write-Host "当前目录: $currentDir" -ForegroundColor Gray
Write-Host ""

# 检查build目录
$buildPath = Join-Path $currentDir $BuildDir
if (-not (Test-Path $buildPath)) {
    Write-Host "✗ Build目录不存在: $buildPath" -ForegroundColor Red
    Write-Host ""
    Write-Host "尝试查找build目录..." -ForegroundColor Yellow
    
    # 尝试几个可能的位置
    $possiblePaths = @(
        "..\..\build",
        "..\build",
        "build"
    )
    
    foreach ($path in $possiblePaths) {
        $testPath = Join-Path $currentDir $path
        if (Test-Path $testPath) {
            Write-Host "✓ 找到build目录: $testPath" -ForegroundColor Green
            $buildPath = $testPath
            break
        }
    }
    
    if (-not (Test-Path $buildPath)) {
        Write-Host ""
        Write-Host "请指定正确的build目录:" -ForegroundColor Yellow
        Write-Host "  .\reconfigure_cmake.ps1 -BuildDir <path>" -ForegroundColor White
        Write-Host ""
        Write-Host "或者创建build目录:" -ForegroundColor Yellow
        Write-Host "  mkdir $buildPath" -ForegroundColor White
        exit 1
    }
}

Write-Host "Build目录: $buildPath" -ForegroundColor Cyan
Write-Host ""

# 1. 清除CMake缓存
Write-Host "1. 清除CMake缓存..." -ForegroundColor Yellow

$cacheFile = Join-Path $buildPath "CMakeCache.txt"
$filesDir = Join-Path $buildPath "CMakeFiles"

if (Test-Path $cacheFile) {
    Remove-Item $cacheFile -Force
    Write-Host "   ✓ 删除 CMakeCache.txt" -ForegroundColor Green
}

if (Test-Path $filesDir) {
    Remove-Item $filesDir -Recurse -Force
    Write-Host "   ✓ 删除 CMakeFiles/" -ForegroundColor Green
}

Write-Host ""

# 2. 运行CMake配置
Write-Host "2. 运行CMake配置..." -ForegroundColor Yellow
Write-Host ""

Push-Location $buildPath

try {
    # 确定源代码目录
    $sourceDir = Resolve-Path $SourceDir
    Write-Host "源代码目录: $sourceDir" -ForegroundColor Gray
    Write-Host ""
    
    # 运行CMake
    $output = cmake $sourceDir 2>&1
    
    # 显示输出
    $output | ForEach-Object {
        $line = $_.ToString()
        
        # 高亮重要信息
        if ($line -match "Found local websocketpp") {
            Write-Host $line -ForegroundColor Green
        }
        elseif ($line -match "Found local nlohmann") {
            Write-Host $line -ForegroundColor Green
        }
        elseif ($line -match "添加.*包含路径") {
            Write-Host $line -ForegroundColor Green
        }
        elseif ($line -match "WebSocket依赖库已就绪") {
            Write-Host $line -ForegroundColor Green
        }
        elseif ($line -match "Warning|警告") {
            Write-Host $line -ForegroundColor Yellow
        }
        elseif ($line -match "Error|错误|fatal") {
            Write-Host $line -ForegroundColor Red
        }
        else {
            Write-Host $line
        }
    }
    
    Write-Host ""
    
    # 检查是否成功
    if ($LASTEXITCODE -eq 0) {
        Write-Host "========================================" -ForegroundColor Green
        Write-Host "✓ CMake配置成功" -ForegroundColor Green
        Write-Host "========================================" -ForegroundColor Green
        Write-Host ""
        Write-Host "下一步: 编译项目" -ForegroundColor Yellow
        Write-Host "  cmake --build . --config Release" -ForegroundColor White
        Write-Host ""
        Write-Host "或在Visual Studio中打开解决方案并编译" -ForegroundColor White
    }
    else {
        Write-Host "========================================" -ForegroundColor Red
        Write-Host "✗ CMake配置失败" -ForegroundColor Red
        Write-Host "========================================" -ForegroundColor Red
        Write-Host ""
        Write-Host "请检查上面的错误信息" -ForegroundColor Yellow
    }
}
finally {
    Pop-Location
}

Write-Host ""
