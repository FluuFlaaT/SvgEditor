# 获取当前目录下所有文件（递归），排除 build 目录
$files = Get-ChildItem -Path . -Recurse | Where-Object { 
    $_.FullName -notmatch "build" -and 
    ($_.Extension -eq ".cpp" -or $_.Extension -eq ".h" -or $_.Extension -eq ".md" -or $_.Extension -eq ".txt")
}

foreach ($file in $files) {
    Write-Host "正在转换: $($file.FullName)"
    
    # 读取文件内容
    # 使用 -Raw 确保保留原始换行符
    $content = Get-Content -Path $file.FullName -Raw
    
    # 将内容写回文件，使用 UTF-8 BOM 编码
    # 在 PowerShell 5.1 中，-Encoding utf8 默认带 BOM
    # 在 PowerShell 7+ 中，需要显式指定 utf8BOM
    if ($PSVersionTable.PSVersion.Major -ge 6) {
        $content | Set-Content -Path $file.FullName -Encoding utf8BOM
    } else {
        $content | Set-Content -Path $file.FullName -Encoding utf8
    }
}

Write-Host "转换完成！"
