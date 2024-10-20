#!/bin/bash

# Compile server 
g++ ./server/server.cpp -o ./server/server

if [ $? -eq 0 ]
then
    # Compile client
    g++ ./client/client.cpp -o ./client/client  

    if [ $? -eq 0 ]
    then
        # run server
        ./server/server
    else
        rm ./server/server && rm ./client/client
    fi

else
    rm ./server/server && rm ./client/client
    echo 'Server compile error!'
fi