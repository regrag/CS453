#!/bin/bash

sleep 10 &
echo sleep 10 &
sleep 10 &
echo sleep 10 &
sleep 10 &
echo sleep 10 &

echo "Each above command has created a sleep process in the background. We will wait for it to finish, then run the jobs command."
sleep 10

jobs
echo "The above command, jobs, should now have printed out 3 finished sleep commands."
