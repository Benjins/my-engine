#!/bin/bash

cd build
zip my-engine_$CXX.zip my-engine my-engine-editor
ssh-keygen -R login.ccs.neu.edu
sshpass -e scp -o 'StrictHostKeyChecking=no' my-engine_$CXX.zip $FTP_USER@login.ccs.neu.edu:~/.www/personal-projects/deployment/my-engine/
sshpass -e scp -o 'StrictHostKeyChecking=no' -r docs $FTP_USER@login.ccs.neu.edu:~/.www/personal-projects/deployment/my-engine/