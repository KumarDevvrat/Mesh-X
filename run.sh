# Just a handy script to compile and run the program directly
# Don't want to commit empty bin/ ext/ directories on github

#! /bin/bash
if [ ! -d "bin" ]; then
    echo "Creating bin/"
    mkdir bin
fi

if [ ! -d "ext" ]; then
    echo "Creating ext/"
    mkdir ext
fi

if [ ! -f "ext/orders.csv" ]; then
    echo "ERROR: could not find the orders file in orders.csv directory"
fi

make
./bin/mesh-x
