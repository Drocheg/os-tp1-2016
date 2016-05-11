ps aux | grep dummyClient | awk '{print $1}' | xargs kill -9
