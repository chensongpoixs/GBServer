# GbMediaServer独立编译脚本
# 作者: chensong
# 日期: 2025-10-18
#
# 用途: 在GbMediaServer目录独立编译，不依赖顶层CMakeLists.txt

Write-Host "========================================" -ForegroundColor Cyan
Write-Host "GbMediaServer独立编译" -ForegroundColor Cyan
Write-Host "========================================" -ForegroundColor Cyan
Write-Host ""

# 确认当前目录
$currentDir = Get-Location
if ($currentDir.Path -notmatch "GbMediaServer") {
    Write-Host "✗ 请在GbMediaServer目录运行此脚本" -ForegroundColor Red
    Write-Host "   当前目录: $currentDir" -ForegroundColor Gray
    exit 1
}

Write-Host "当前目录: $currentDir" -ForegroundColor Gray
Write-Host ""

# 1. 检查库文件
Write-Host "1. 检查WebSocket库..." -ForegroundColor Yellow

$websocketppFile = "third_party\websocketpp\websocketpp\config\asio_no_tls.hpp"
$jsonFile = "third_party\json\include\nlohmann\json.hpp"

$libsOk = $true

if (Test-Path $websocketppFile) {
    Write-Host "   ✓ websocketpp" -ForegroundColor Green
} else {
    Write-Host "   ✗ websocketpp 缺失" -ForegroundColor Red
    $libsOk = $false
}

if (Test-Path $jsonFile) {
    Write-Host "   ✓ nlohmann/json" -ForegroundColor Green
} else {
    Write-Host "   ✗ nlohmann/json 缺失" -ForegroundColor Red
    $libsOk = $false
}

if (-not $libsOk) {
    Write-Host ""
    Write-Host "库文件缺失，正在下载..." -ForegroundColor Yellow
    .\download_websocket_libs.ps1
    Write-Host ""
}

# 2. 创建build目录
Write-Host ""
Write-Host "2. 准备build目录..." -ForegroundColor Yellow

$buildDir = "build_local"
if (-not (Test-Path $buildDir)) {
    New-Item -ItemType Directory -Path $buildDir | Out-Null
    Write-Host "   ✓ 创建: $buildDir" -ForegroundColor Green
} else {
    Write-Host "   ✓ 使用现有: $buildDir" -ForegroundColor Green
    
    # 清除旧的缓存
    $cacheFile = Join-Path $buildDir "CMakeCache.txt"
    $filesDir = Join-Path $buildDir "CMakeFiles"
    
    if (Test-Path $cacheFile) {
        Remove-Item $cacheFile -Force
        Write-Host "   ✓ 清除CMake缓存" -ForegroundColor Green
    }
    
    if (Test-Path $filesDir) {
        Remove-Item $filesDir -Recurse -Force
        Write-Host "   ✓ 清除CMakeFiles" -ForegroundColor Green
    }
}

# 3. 运行CMake配置
Write-Host ""
Write-Host "3. 运行CMake配置..." -ForegroundColor Yellow
Write-Host ""

Push-Location $buildDir

try {
    # 运行CMake，指向父目录（GbMediaServer）
    $cmakeOutput = cmake .. 2>&1
    
    # 分析输出
    $foundWebsocketpp = $false
    $foundJson = $false
    $addedWebsocketpp = $false
    $addedJson = $false
    $hasError = $false
    $configSuccess = $false
    
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
        elseif ($lineStr -match "Configuring done") {
            $configSuccess = $true
            Write-Host $lineStr -ForegroundColor Green
        }
        elseif ($lineStr -match "Generating done") {
            Write-Host $lineStr -ForegroundColor Green
        }
        elseif ($lineStr -match "Error|错误") {
            $hasError = $true
            Write-Host $lineStr -ForegroundColor Red
        }
        elseif ($lineStr -match "fatal") {
            $hasError = $true
            Write-Host $lineStr -ForegroundColor Red
        }
        elseif ($lineStr -match "Warning|警告") {
            Write-Host $lineStr -ForegroundColor Yellow
        }
        elseif ($lineStr -match "GbMediaServer") {
            Write-Host $lineStr -ForegroundColor Cyan
        }
        elseif ($lineStr -match "Building for|Selecting Windows SDK") {
            Write-Host $lineStr -ForegroundColor Cyan
        }
        else {
            # 其他信息用灰色显示
            if ($lineStr.Trim() -ne "") {
                Write-Host $lineStr -ForegroundColor Gray
            }
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
    
    # 判断是否可以继续编译
    if ($configSuccess -and $foundWebsocketpp -and $foundJson -and $addedWebsocketpp -and $addedJson -and -not $hasError) {
        Write-Host "========================================" -ForegroundColor Green
        Write-Host "✓ CMake配置成功" -ForegroundColor Green
        Write-Host "========================================" -ForegroundColor Green
        Write-Host ""
        Write-Host "4. 开始编译..." -ForegroundColor Yellow
        Write-Host ""
        
        # 编译
        $buildOutput = cmake --build . --config Release 2>&1
        
        $buildSuccess = $false
        $buildError = $false
        
        foreach ($line in $buildOutput) {
            $lineStr = $line.ToString()
            
            if ($lineStr -match "Build succeeded|生成成功") {
                $buildSuccess = $true
                Write-Host $lineStr -ForegroundColor Green
            }
            elseif ($lineStr -match "error C|fatal error") {
                $buildError = $true
                Write-Host $lineStr -ForegroundColor Red
            }
            elseif ($lineStr -match "warning C") {
                Write-Host $lineStr -ForegroundColor Yellow
            }
            elseif ($lineStr -match "Building|Compiling|Linking") {
                Write-Host $lineStr -ForegroundColor Cyan
            }
            else {
                Write-Host $lineStr
            }
        }
        
        Write-Host ""
        
        if ($LASTEXITCODE -eq 0 -and -not $buildError) {
            Write-Host "========================================" -ForegroundColor Green
            Write-Host "✓ 编译成功！" -ForegroundColor Green
            Write-Host "========================================" -ForegroundColor Green
            Write-Host ""
            
            # 查找可执行文件
            $exeFile = "Release\GbMediaServer.exe"
            if (Test-Path $exeFile) {
                $exePath = Resolve-Path $exeFile
                Write-Host "可执行文件: $exePath" -ForegroundColor Green
                
                $exeSize = (Get-Item $exeFile).Length / 1MB
                Write-Host "文件大小: $([Math]::Round($exeSize, 2)) MB" -ForegroundColor Gray
            }
        } else {
            Write-Host "========================================" -ForegroundColor Red
            Write-Host "✗ 编译失败" -ForegroundColor Red
            Write-Host "========================================" -ForegroundColor Red
            Write-Host ""
            Write-Host "请检查上面的错误信息" -ForegroundColor Yellow
        }
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
        Write-Host "⚠ 配置不完整" -ForegroundColor Yellow
        Write-Host "========================================" -ForegroundColor Yellow
        Write-Host ""
        
        if (-not $foundWebsocketpp -or -not $foundJson) {
            Write-Host "WebSocket库未检测到，可能的原因:" -ForegroundColor Yellow
            Write-Host "1. 库文件不存在或路径不正确" -ForegroundColor White
            Write-Host "2. CMakeLists.txt配置有误" -ForegroundColor White
            Write-Host ""
            Write-Host "请运行诊断: .\diagnose_cmake.ps1" -ForegroundColor Yellow
        }
        
        if (-not $addedWebsocketpp -or -not $addedJson) {
            Write-Host "包含路径未添加，可能的原因:" -ForegroundColor Yellow
            Write-Host "1. 变量未正确设置" -ForegroundColor White
            Write-Host "2. 条件判断失败" -ForegroundColor White
        }
    }
}
finally {
    Pop-Location
}

Write-Host ""
