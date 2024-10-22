#!/bin/bash


# Compile Client 
g++ ./src/client.cpp -o ./src/client

if [ $? -eq 0 ]
then
    echo 'Client compile!'
else
    rm ./src/server && rm ./src/client
    echo 'Client compile error!'
fi


# Compile server 
g++ ./src/server.cpp -o ./src/server

if [ $? -eq 0 ]
then
    
    # run server
    ./src/server

else
    rm ./src/server && rm ./src/client
    echo 'Server compile error!'
fi