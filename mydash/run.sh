#!/bin/bash

export DASH_PROMPT="mybadassdash>"
export SVN_VERSION=$(svnversion)
make
./mydash
