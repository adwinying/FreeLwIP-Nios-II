#!/bin/sh

send_message()
{
for i in `seq 1 100000`
do
	# ランダムな時間スリープ [50ms - 500ms]
	./usleep

	# ランダムな文字列出力
	len=$(( ($(od -vAn -N4 -tu4 < /dev/urandom) % 1472) + 1 ))
	echo `cat /dev/urandom | tr -dc '[:alnum:]' | head -c $len`
done
}

# ECHO クライアントを実行
send_message | socat stdin udp4-sendto:172.24.14.20:7
