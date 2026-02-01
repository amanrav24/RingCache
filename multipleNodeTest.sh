#!/bin/bash

PORT=8080
NUMSERVERS=1
NODEID="NODE"

# 1. Start Servers
for ((i = 0; i < NUMSERVERS; i++)) 
do
    CURRENT_ID="${NODEID}$i"
    CURRENT_PORT=$((PORT + i))
    echo "Starting $CURRENT_ID on port $CURRENT_PORT..."
    
    ./bin/testSingleNode "$CURRENT_ID" "$CURRENT_PORT" &
    # Store the Server PIDs so we can kill them later
    SERVER_PIDS="$SERVER_PIDS $!"
done

# 2. Start Clients
NUMCLIENTS=1
CLIENT_PIDS=""
for ((i = 0; i < NUMCLIENTS; i++)) 
do 
    ./bin/testCache $NUMSERVERS &
    # Store the Client PIDs specifically
    CLIENT_PIDS="$CLIENT_PIDS $!"
done

# 3. Wait ONLY for the Clients
echo "Waiting for $NUMCLIENTS clients to finish..."
wait $CLIENT_PIDS

# 4. Cleanup: Kill the servers
echo "Clients finished. Shutting down servers..."
kill $SERVER_PIDS

echo "Test complete."