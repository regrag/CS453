#!/bin/bash

sleep 10 &
sleep 10 &
sleep 10 &

echo "Each above command should have printed out the command after each execution."
sleep 10
jobs

echo "The above command, jobs, should now have printed out 3 finished sleep commands."
