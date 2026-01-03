#!/bin/bash

script_path="/config/run-server.sh"

# 检查文件是否存在
if [ -f "$script_path" ]; then
    echo "找到启动脚本，正在执行..."
    chmod +x "$script_path"
    "$script_path" &
fi

&>/dev/null /bin/bash