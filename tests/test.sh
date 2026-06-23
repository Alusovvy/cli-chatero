#!/bin/bash

SERVER="127.0.0.1"
PORT="9090"
PASS=0
FAIL=0

normalize(){
    sed -E 's/[0-9]{4}-[0-9]{2}-[0-9]{2}[ :T]?[0-9]{2}:[0-9]{2}:[0-9]{2}//g' |
    tr -d ' \t\r\n'
}

send() { printf "%s\n" "$1" >&3; }
recv() { 
    local line
    while read -t0.5 -r line <&3; do
        echo "$line"
    done
}

run_test() {
    rm -f /tmp/got.txt /tmp/expected.txt
    local isLoggedIn=$1
    local name="$2"
    local input="$3"
    local expected="$4"    
    local commands="${5:-}"
    
    exec 3<>/dev/tcp/$SERVER/$PORT

    if [ "$isLoggedIn" == true ]; then
        send "/login alex"
        recv > /dev/null
    fi

    send "$input"
    recv >> /tmp/got.txt
    echo "$expected" > /tmp/expected.txt

    if [ -n "$commands" ]; then
        local -n ref="$commands"
        for a in "${ref[@]}"; do
            send "$a"
            recv >> /tmp/got.txt
        done
    fi

    exec 3>&- 3<&-
    
    normalize < /tmp/got.txt > /tmp/got_norm.txt
    normalize < /tmp/expected.txt > /tmp/expected_norm.txt

    if diff -q /tmp/expected_norm.txt /tmp/got_norm.txt > /dev/null; then
        echo "PASS: $name"
        PASS=$((PASS + 1))
    else 
    echo "FAIL: $name"
    echo " expected: 
        $(cat /tmp/expected_norm.txt)
        "
    echo " got: 
        $(cat /tmp/got_norm.txt)
        "
    FAIL=$((FAIL + 1))
    fi
}

    args_test_join_and_send_msg=("/login steven" "/create dupa" "/join dupa" "dupa")

    run_test false "Login" "/login aleks" "Type /login to login with your username User was logged in with username: aleks"
    run_test true "Get Details" "/details" "alex"
    run_test true "Create Chatroom" "/create room" "Chatroom created with name: room"
    run_test true "Join Chatroom" "/join room" "You have joined the chatroom: room"
    run_test false "Send Messages" "" "Type/logintologinwithyourusernameUserwasloggedinwithusername:stevenChatroomcreatedwithname:dupaYouhavejoinedthechatroom:dupa2026-06-2319:09:53steven:dupa" args_test_join_and_send_msg

    echo "Failed: $FAIL Passed: $PASS"
