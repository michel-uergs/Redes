#!/bin/bash

# Compile server 
g++ ./src/server.cpp -o ./src/server

if [ $? -eq 0 ]
then
    # Compile client
    g++ ./src/client.cpp -o ./src/client 

    if [ $? -eq 0 ]
    then
        # run server
        ./src/server
    else
        echo 'Client compile error!'
        rm ./src/server && rm ./src/client
    fi

else
    rm ./src/server && rm ./src/client
    echo 'Server compile error!'
fi