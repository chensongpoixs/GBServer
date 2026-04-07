# 快速修复编译错误脚本
# 作者: chensong
# 日期: 2025-10-18
#
# 针对目录结构: D:\Work\crtc\
#   ├── build/              (CMake构建目录)
#   └── GBServer/
#       └── GbMediaServer/  (源代码目录，当前位置)

Write-Host "========================================" -ForegroundColor Cyan
Write-Host "快速修复编译错误" -ForegroundColor Cyan
Write-Host "========================================" -ForegroundColor Cyan
Write-Host ""

# 1. 确认当前位置
$currentDir = Get-Location
Write-Host "当前目录: $currentDir" -ForegroundColor Gray

if ($currentDir.Path -notmatch "GbMediaServer") {
    Write-Host ""
    Write-Host "✗ 请在GbMediaServer目录中运行此脚本" -ForegroundColor Red
    exit 1
}

Write-Host ""

# 2. 检查库文件
Write-Host "1. 检查库文件..." -ForegroundColor Yellow

$websocketppFile = "third_party\websocketpp\websocketpp\config\asio_no_tls.hpp"
$jsonFile = "third_party\json\include\nlohmann\json.hpp"

$filesOk = $true

if (Test-Path $websocketppFile) {
    Write-Host "   ✓ websocketpp" -ForegroundColor Green
} else {
    Write-Host "   ✗ websocketpp 缺失" -ForegroundColor Red
    $filesOk = $false
}

if (Test-Path $jsonFile) {
    Write-Host "   ✓ nlohmann/json" -ForegroundColor Green
} else {
    Write-Host "   ✗ nlohmann/json 缺失" -ForegroundColor Red
    $filesOk = $false
}

if (-not $filesOk) {
    Write-Host ""
    Write-Host "库文件缺失，正在下载..." -ForegroundColor Yellow
    .\download_websocket_libs.ps1
    Write-Host ""
}

# 3. 定位build目录
Write-Host ""
Write-Host "2. 定位build目录..." -ForegroundColor Yellow

# 根据目录结构，build应该在../../build
$buildDir = "..\..\build"
$buildPath = Join-Path $currentDir $buildDir

if (Test-Path $buildPath) {
    Write-Host "   ✓ 找到: $buildPath" -ForegroundColor Green
} else {
    Write-Host "   ✗ 未找到: $buildPath" -ForegroundColor Red
    Write-Host ""
    Write-Host "请手动指定build目录位置" -ForegroundColor Yellow
    exit 1
}

# 4. 清除CMake缓存
Write-Host ""
Write-Host "3. 清除CMake缓存..." -ForegroundColor Yellow

$cacheFile = Join-Path $buildPath "CMakeCache.txt"
$filesDir = Join-Path $buildPath "CMakeFiles"

if (Test-Path $cacheFile) {
    Remove-Item $cacheFile -Force
    Write-Host "   ✓ 删除 CMakeCache.txt" -ForegroundColor Green
} else {
    Write-Host "   - CMakeCache.txt 不存在" -ForegroundColor Gray
}

if (Test-Path $filesDir) {
    Remove-Item $filesDir -Recurse -Force
    Write-Host "   ✓ 删除 CMakeFiles/" -ForegroundColor Green
} else {
    Write-Host "   - CMakeFiles/ 不存在" -ForegroundColor Gray
}

# 5. 重新运行CMake
Write-Host ""
Write-Host "4. 重新运行CMake..." -ForegroundColor Yellow
Write-Host ""

Push-Location $buildPath

try {
    # 源代码目录是D:\Work\crtc（build的父目录）
    $sourceDir = Resolve-Path (Join-Path $buildPath "..")
    
    Write-Host "   源代码目录: $sourceDir" -ForegroundColor Gray
    Write-Host "   构建目录: $(Resolve-Path $buildPath)" -ForegroundColor Gray
    Write-Host ""
    
    # 运行CMake
    $cmakeOutput = cmake "$sourceDir" 2>&1
    
    # 分析输出
    $foundWebsocketpp = $false
    $foundJson = $false
    $addedWebsocketpp = $false
    $addedJson = $false
    $hasError = $false
    
    foreach ($line in $cmakeOutput) {
        $lineStr = $line.ToString()
        
        # 检查关键信息
        if ($lineStr -match "Found local websocketpp") {
            $foundWebsocketpp = $true
            Write-Host $lineStr -ForegroundColor Green
        }
        elseif ($lineStr -match "Found local nlohmann") {
            $foundJson = $true
            Write-Host $lineStr -ForegroundColor Green
        }
        elseif ($lineStr -match "添加websocketpp包含路径") {
            $addedWebsocketpp = $true
            Write-Host $lineStr -ForegroundColor Green
        }
        elseif ($lineStr -match "添加nlohmann/json包含路径") {
            $addedJson = $true
            Write-Host $lineStr -ForegroundColor Green
        }
        elseif ($lineStr -match "WebSocket依赖库已就绪") {
            Write-Host $lineStr -ForegroundColor Green
        }
        elseif ($lineStr -match "Error|错误|fatal") {
            $hasError = $true
            Write-Host $lineStr -ForegroundColor Red
        }
        elseif ($lineStr -match "Warning|警告") {
            Write-Host $lineStr -ForegroundColor Yellow
        }
        elseif ($lineStr -match "GbMediaServer") {
            Write-Host $lineStr -ForegroundColor Cyan
        }
        else {
            Write-Host $lineStr -ForegroundColor Gray
        }
    }
    
    Write-Host ""
    Write-Host "========================================" -ForegroundColor Cyan
    Write-Host "配置结果" -ForegroundColor Cyan
    Write-Host "========================================" -ForegroundColor Cyan
    Write-Host ""
    
    # 显示检查结果
    Write-Host "websocketpp检测: " -NoNewline
    if ($foundWebsocketpp) {
        Write-Host "✓" -ForegroundColor Green
    } else {
        Write-Host "✗" -ForegroundColor Red
    }
    
    Write-Host "nlohmann/json检测: " -NoNewline
    if ($foundJson) {
        Write-Host "✓" -ForegroundColor Green
    } else {
        Write-Host "✗" -ForegroundColor Red
    }
    
    Write-Host "websocketpp包含路径: " -NoNewline
    if ($addedWebsocketpp) {
        Write-Host "✓" -ForegroundColor Green
    } else {
        Write-Host "✗" -ForegroundColor Red
    }
    
    Write-Host "nlohmann/json包含路径: " -NoNewline
    if ($addedJson) {
        Write-Host "✓" -ForegroundColor Green
    } else {
        Write-Host "✗" -ForegroundColor Red
    }
    
    Write-Host ""
    
    # 最终结果
    if ($LASTEXITCODE -eq 0 -and $foundWebsocketpp -and $foundJson -and $addedWebsocketpp -and $addedJson) {
        Write-Host "========================================" -ForegroundColor Green
        Write-Host "✓ 修复成功！" -ForegroundColor Green
        Write-Host "========================================" -ForegroundColor Green
        Write-Host ""
        Write-Host "下一步: 编译项目" -ForegroundColor Yellow
        Write-Host "  cmake --build . --config Release" -ForegroundColor White
        Write-Host ""
        Write-Host "或在Visual Studio中打开解决方案并编译" -ForegroundColor White
    }
    elseif ($hasError) {
        Write-Host "========================================" -ForegroundColor Red
        Write-Host "✗ CMake配置失败" -ForegroundColor Red
        Write-Host "========================================" -ForegroundColor Red
        Write-Host ""
        Write-Host "请检查上面的错误信息" -ForegroundColor Yellow
    }
    else {
        Write-Host "========================================" -ForegroundColor Yellow
        Write-Host "⚠ 配置完成但可能有问题" -ForegroundColor Yellow
        Write-Host "========================================" -ForegroundColor Yellow
        Write-Host ""
        
        if (-not $foundWebsocketpp -or -not $foundJson) {
            Write-Host "库文件未被检测到，可能的原因:" -ForegroundColor Yellow
            Write-Host "1. 库文件路径不正确" -ForegroundColor White
            Write-Host "2. CMakeLists.txt配置有误" -ForegroundColor White
            Write-Host ""
            Write-Host "请运行诊断脚本:" -ForegroundColor Yellow
            Write-Host "  cd ..\..\GBServer\GbMediaServer" -ForegroundColor White
            Write-Host "  .\diagnose_cmake.ps1" -ForegroundColor White
        }
        
        if (-not $addedWebsocketpp -or -not $addedJson) {
            Write-Host "包含路径未添加，可能的原因:" -ForegroundColor Yellow
            Write-Host "1. CMakeLists.txt中的条件判断失败" -ForegroundColor White
            Write-Host "2. 变量未正确设置" -ForegroundColor White
        }
    }
}
finally {
    Pop-Location
}

Write-Host ""
