kill -9 $(ps aux | grep asile | grep -v "grep" | cut -d " " -f10)
