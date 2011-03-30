#!/bin/bash

for (( i=i; i<=4; i++ ))
do
	nachos -m $i -x ../test/multiClientTest
done