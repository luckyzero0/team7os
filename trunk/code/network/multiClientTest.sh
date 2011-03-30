#!/bin/bash

nachos -m 1 -x ../test/multiClientTest &
nachos -m 2 -x ../test/multiClientTest &
nachos -m 3 -x ../test/multiClientTest &
nachos -m 4 -x ../test/multiClientTest