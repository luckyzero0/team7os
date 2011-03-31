#!/usr/bin/env bash

nachos -m 1 -x ../test/mc_wait &
sleep 2
nachos -m 2 -x ../test/mc_signal