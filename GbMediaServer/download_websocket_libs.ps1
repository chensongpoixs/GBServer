# WebSocket依赖库下载脚本
# 作者: chensong
# 日期: 2025-10-18
# 
# 用途: 手动下载websocketpp和nlohmann/json库
# 使用: 在PowerShell中运行此脚本

Write-Host "========================================" -ForegroundColor Cyan
Write-Host "WebSocket依赖库下载脚本" -ForegroundColor Cyan
Write-Host "========================================" -ForegroundColor Cyan
Write-Host ""

# 设置目录
$thirdPartyDir = "third_party"
$websocketppDir = "$thirdPartyDir/websocketpp"
$jsonDir = "$thirdPartyDir/json"
$asioDir = "$thirdPartyDir/asio"

# 创建third_party目录
if (-not (Test-Path $thirdPartyDir)) {
    New-Item -ItemType Directory -Path $thirdPartyDir | Out-Null
    Write-Host "[创建] third_party目录" -ForegroundColor Green
}

# ============================================================================
# 下载websocketpp
# ============================================================================
Write-Host ""
Write-Host "1. 检查websocketpp..." -ForegroundColor Yellow

if (Test-Path "$websocketppDir/websocketpp/server.hpp") {
    Write-Host "[存在] websocketpp已存在，跳过下载" -ForegroundColor Green
} else {
    Write-Host "[下载] 正在下载websocketpp..." -ForegroundColor Yellow
    
    # 检查是否有git
    $gitExists = Get-Command git -ErrorAction SilentlyContinue
    
    if ($gitExists) {
        # 使用git下载
        Write-Host "   使用git克隆..." -ForegroundColor Cyan
        git clone --depth 1 --branch 0.8.2 https://github.com/zaphoyd/websocketpp.git $websocketppDir
        
        if ($LASTEXITCODE -eq 0) {
            Write-Host "[成功] websocketpp下载完成" -ForegroundColor Green
        } else {
            Write-Host "[失败] websocketpp下载失败" -ForegroundColor Red
            Write-Host "   请手动下载: https://github.com/zaphoyd/websocketpp/archive/refs/tags/0.8.2.zip" -ForegroundColor Yellow
            Write-Host "   解压到: $websocketppDir" -ForegroundColor Yellow
        }
    } else {
        # 使用PowerShell下载zip
        Write-Host "   git未安装，使用PowerShell下载zip..." -ForegroundColor Cyan
        $zipUrl = "https://github.com/zaphoyd/websocketpp/archive/refs/tags/0.8.2.zip"
        $zipFile = "$thirdPartyDir/websocketpp.zip"
        
        try {
            Write-Host "   正在下载: $zipUrl" -ForegroundColor Cyan
            
            # 使用WebClient显示下载进度
            $webClient = New-Object System.Net.WebClient
            
            # 注册进度事件
            Register-ObjectEvent -InputObject $webClient -EventName DownloadProgressChanged -SourceIdentifier WebClient.DownloadProgressChanged -Action {
                $percent = $EventArgs.ProgressPercentage
                $received = [Math]::Round($EventArgs.BytesReceived / 1MB, 2)
                $total = [Math]::Round($EventArgs.TotalBytesToReceive / 1MB, 2)
                Write-Progress -Activity "下载websocketpp" -Status "$received MB / $total MB" -PercentComplete $percent
            } | Out-Null
            
            # 开始下载
            $webClient.DownloadFileAsync($zipUrl, $zipFile)
            
            # 等待下载完成
            while ($webClient.IsBusy) {
                Start-Sleep -Milliseconds 100
            }
            
            # 清理事件
            Unregister-Event -SourceIdentifier WebClient.DownloadProgressChanged -ErrorAction SilentlyContinue
            $webClient.Dispose()
            
            Write-Progress -Activity "下载websocketpp" -Completed
            Write-Host "   下载完成，正在解压..." -ForegroundColor Cyan
            
            Expand-Archive -Path $zipFile -DestinationPath $thirdPartyDir -Force
            
            # 重命名目录
            if (Test-Path "$thirdPartyDir/websocketpp-0.8.2") {
                if (Test-Path $websocketppDir) {
                    Remove-Item -Path $websocketppDir -Recurse -Force
                }
                Rename-Item -Path "$thirdPartyDir/websocketpp-0.8.2" -NewName "websocketpp"
            }
            
            # 删除zip文件
            Remove-Item -Path $zipFile -Force
            
            Write-Host "[成功] websocketpp下载并解压完成" -ForegroundColor Green
        } catch {
            Write-Host "[失败] 下载失败: $_" -ForegroundColor Red
            Write-Host "   请手动下载: $zipUrl" -ForegroundColor Yellow
            Write-Host "   解压到: $websocketppDir" -ForegroundColor Yellow
        }
    }
}

# ============================================================================
# 下载nlohmann/json
# ============================================================================
Write-Host ""
Write-Host "2. 检查nlohmann/json..." -ForegroundColor Yellow

if (Test-Path "$jsonDir/include/nlohmann/json.hpp") {
    Write-Host "[存在] nlohmann/json已存在，跳过下载" -ForegroundColor Green
} else {
    Write-Host "[下载] 正在下载nlohmann/json..." -ForegroundColor Yellow
    
    # 检查是否有git
    $gitExists = Get-Command git -ErrorAction SilentlyContinue
    
    if ($gitExists) {
        # 使用git下载
        Write-Host "   使用git克隆..." -ForegroundColor Cyan
        git clone --depth 1 --branch v3.11.2 https://github.com/nlohmann/json.git $jsonDir
        
        if ($LASTEXITCODE -eq 0) {
            Write-Host "[成功] nlohmann/json下载完成" -ForegroundColor Green
        } else {
            Write-Host "[失败] nlohmann/json下载失败" -ForegroundColor Red
            Write-Host "   请手动下载: https://github.com/nlohmann/json/archive/refs/tags/v3.11.2.zip" -ForegroundColor Yellow
            Write-Host "   解压到: $jsonDir" -ForegroundColor Yellow
        }
    } else {
        # 使用PowerShell下载zip
        Write-Host "   git未安装，使用PowerShell下载zip..." -ForegroundColor Cyan
        $zipUrl = "https://github.com/nlohmann/json/archive/refs/tags/v3.11.2.zip"
        $zipFile = "$thirdPartyDir/json.zip"
        
        try {
            Write-Host "   正在下载: $zipUrl" -ForegroundColor Cyan
            
            # 使用WebClient显示下载进度
            $webClient = New-Object System.Net.WebClient
            
            # 注册进度事件
            Register-ObjectEvent -InputObject $webClient -EventName DownloadProgressChanged -SourceIdentifier WebClient.DownloadProgressChanged2 -Action {
                $percent = $EventArgs.ProgressPercentage
                $received = [Math]::Round($EventArgs.BytesReceived / 1MB, 2)
                $total = [Math]::Round($EventArgs.TotalBytesToReceive / 1MB, 2)
                Write-Progress -Activity "下载nlohmann/json" -Status "$received MB / $total MB" -PercentComplete $percent
            } | Out-Null
            
            # 开始下载
            $webClient.DownloadFileAsync($zipUrl, $zipFile)
            
            # 等待下载完成
            while ($webClient.IsBusy) {
                Start-Sleep -Milliseconds 100
            }
            
            # 清理事件
            Unregister-Event -SourceIdentifier WebClient.DownloadProgressChanged2 -ErrorAction SilentlyContinue
            $webClient.Dispose()
            
            Write-Progress -Activity "下载nlohmann/json" -Completed
            Write-Host "   下载完成，正在解压..." -ForegroundColor Cyan
            
            Expand-Archive -Path $zipFile -DestinationPath $thirdPartyDir -Force
            
            # 重命名目录
            if (Test-Path "$thirdPartyDir/json-3.11.2") {
                if (Test-Path $jsonDir) {
                    Remove-Item -Path $jsonDir -Recurse -Force
                }
                Rename-Item -Path "$thirdPartyDir/json-3.11.2" -NewName "json"
            }
            
            # 删除zip文件
            Remove-Item -Path $zipFile -Force
            
            Write-Host "[成功] nlohmann/json下载并解压完成" -ForegroundColor Green
        } catch {
            Write-Host "[失败] 下载失败: $_" -ForegroundColor Red
            Write-Host "   请手动下载: $zipUrl" -ForegroundColor Yellow
            Write-Host "   解压到: $jsonDir" -ForegroundColor Yellow
        }
    }
}

# ============================================================================
# 下载ASIO (standalone)
# ============================================================================
Write-Host ""
Write-Host "3. 检查ASIO (standalone)..." -ForegroundColor Yellow

if (Test-Path "$asioDir/asio/include/asio.hpp") {
    Write-Host "[存在] ASIO已存在，跳过下载" -ForegroundColor Green
} else {
    Write-Host "[下载] 正在下载ASIO..." -ForegroundColor Yellow
    
    # 检查是否有git
    $gitExists = Get-Command git -ErrorAction SilentlyContinue
    
    if ($gitExists) {
        # 使用git下载
        Write-Host "   使用git克隆..." -ForegroundColor Cyan
        git clone --depth 1 --branch asio-1-28-0 https://github.com/chriskohlhoff/asio.git $asioDir
        
        if ($LASTEXITCODE -eq 0) {
            Write-Host "[成功] ASIO下载完成" -ForegroundColor Green
        } else {
            Write-Host "[失败] ASIO下载失败" -ForegroundColor Red
            Write-Host "   请手动下载: https://github.com/chriskohlhoff/asio/archive/refs/tags/asio-1-28-0.zip" -ForegroundColor Yellow
            Write-Host "   解压到: $asioDir" -ForegroundColor Yellow
        }
    } else {
        # 使用PowerShell下载zip
        Write-Host "   git未安装，使用PowerShell下载zip..." -ForegroundColor Cyan
        $zipUrl = "https://github.com/chriskohlhoff/asio/archive/refs/tags/asio-1-28-0.zip"
        $zipFile = "$thirdPartyDir/asio.zip"
        
        try {
            Write-Host "   正在下载: $zipUrl" -ForegroundColor Cyan
            
            # 使用WebClient显示下载进度
            $webClient = New-Object System.Net.WebClient
            
            # 注册进度事件
            Register-ObjectEvent -InputObject $webClient -EventName DownloadProgressChanged -SourceIdentifier WebClient.DownloadProgressChanged3 -Action {
                $percent = $EventArgs.ProgressPercentage
                $received = [Math]::Round($EventArgs.BytesReceived / 1MB, 2)
                $total = [Math]::Round($EventArgs.TotalBytesToReceive / 1MB, 2)
                Write-Progress -Activity "下载ASIO" -Status "$received MB / $total MB" -PercentComplete $percent
            } | Out-Null
            
            # 开始下载
            $webClient.DownloadFileAsync($zipUrl, $zipFile)
            
            # 等待下载完成
            while ($webClient.IsBusy) {
                Start-Sleep -Milliseconds 100
            }
            
            # 清理事件
            Unregister-Event -SourceIdentifier WebClient.DownloadProgressChanged3 -ErrorAction SilentlyContinue
            $webClient.Dispose()
            
            Write-Progress -Activity "下载ASIO" -Completed
            Write-Host "   下载完成，正在解压..." -ForegroundColor Cyan
            
            Expand-Archive -Path $zipFile -DestinationPath $thirdPartyDir -Force
            
            # 重命名目录
            if (Test-Path "$thirdPartyDir/asio-asio-1-28-0") {
                if (Test-Path $asioDir) {
                    Remove-Item -Path $asioDir -Recurse -Force
                }
                Rename-Item -Path "$thirdPartyDir/asio-asio-1-28-0" -NewName "asio"
            }
            
            # 删除zip文件
            Remove-Item -Path $zipFile -Force
            
            Write-Host "[成功] ASIO下载并解压完成" -ForegroundColor Green
        } catch {
            Write-Host "[失败] 下载失败: $_" -ForegroundColor Red
            Write-Host "   请手动下载: $zipUrl" -ForegroundColor Yellow
            Write-Host "   解压到: $asioDir" -ForegroundColor Yellow
        }
    }
}

# ============================================================================
# 验证
# ============================================================================
Write-Host ""
Write-Host "========================================" -ForegroundColor Cyan
Write-Host "验证结果" -ForegroundColor Cyan
Write-Host "========================================" -ForegroundColor Cyan

$websocketppOk = Test-Path "$websocketppDir/websocketpp/server.hpp"
$jsonOk = Test-Path "$jsonDir/include/nlohmann/json.hpp"
$asioOk = Test-Path "$asioDir/asio/include/asio.hpp"

Write-Host ""
Write-Host "websocketpp: " -NoNewline
if ($websocketppOk) {
    Write-Host "✓ 已安装" -ForegroundColor Green
    Write-Host "   路径: $websocketppDir" -ForegroundColor Gray
} else {
    Write-Host "✗ 未安装" -ForegroundColor Red
}

Write-Host ""
Write-Host "nlohmann/json: " -NoNewline
if ($jsonOk) {
    Write-Host "✓ 已安装" -ForegroundColor Green
    Write-Host "   路径: $jsonDir/include" -ForegroundColor Gray
} else {
    Write-Host "✗ 未安装" -ForegroundColor Red
}

Write-Host ""
Write-Host "ASIO: " -NoNewline
if ($asioOk) {
    Write-Host "✓ 已安装" -ForegroundColor Green
    Write-Host "   路径: $asioDir/asio/include" -ForegroundColor Gray
} else {
    Write-Host "✗ 未安装" -ForegroundColor Red
}

Write-Host ""
if ($websocketppOk -and $jsonOk -and $asioOk) {
    Write-Host "========================================" -ForegroundColor Green
    Write-Host "所有依赖库已就绪！" -ForegroundColor Green
    Write-Host "========================================" -ForegroundColor Green
    Write-Host ""
    Write-Host "下一步:" -ForegroundColor Yellow
    Write-Host "1. 修改 server/ws_stats_service.h" -ForegroundColor White
    Write-Host "   将 #define ENABLE_WEBSOCKET 0 改为 1" -ForegroundColor White
    Write-Host "2. 重新运行CMake配置" -ForegroundColor White
    Write-Host "3. 编译项目" -ForegroundColor White
} else {
    Write-Host "========================================" -ForegroundColor Red
    Write-Host "部分依赖库安装失败" -ForegroundColor Red
    Write-Host "========================================" -ForegroundColor Red
    Write-Host ""
    Write-Host "请检查网络连接或手动下载" -ForegroundColor Yellow
}

Write-Host ""
