# 获取当前目录下所有文件（递归），排除 build 目录
$files = Get-ChildItem -Path . -Recurse | Where-Object { 
    $_.FullName -notmatch "build" -and 
    ($_.Extension -eq ".cpp" -or $_.Extension -eq ".h" -or $_.Extension -eq ".md" -or $_.Extension -eq ".txt")
}

foreach ($file in $files) {
    Write-Host "正在转换: $($file.FullName)"
    
    # 读取文件内容（使用 -Raw 确保保留原始换行符）
    $content = Get-Content -Path $file.FullName -Raw
    
    # 使用 .NET 的 WriteAllText 以确保不会额外添加空行
    # $true 表示包含 BOM
    $utf8bom = New-Object System.Text.UTF8Encoding($true)
    [System.IO.File]::WriteAllText($file.FullName, $content, $utf8bom)
}

Write-Host "转换完成！"
