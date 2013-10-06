HashPump
========

A tool to exploit the hash length extension attack in various hashing algorithms

Currently supported algorithms: MD5, SHA1, SHA256, SHA512

Help Menu
=========

    ./HashPump -h
    HashPump [-h help] [-t test] [-s signature] [-d data] [-a additional] [-k keylength]
         HashPump generates strings to exploit signatures vulnerable to the Hash Length Extension Attack.
         -h --help          Display this message.
         -t --test          Run tests to verify each algorithm is operating properly.
         -s --signature     The signature from known message.
         -d --data          The data from the known message.
         -a --additional    The information you would like to add to the known message.
         -k --keylength     The length in bytes of the key being used to sign the original message with.
         Version 1.0 with MD5, SHA1, SHA256 and SHA512 support.
         <Developed by bwall(@bwallHatesTwits)>

Sample Output
=============

    ./HashPump -s 6d5f807e23db210bc254a28be2d6759a0f5f5d99 --data count=10\&lat=37.351\&user_id=1\&long=-119.827\&waffle=eggo -a \&waffle=liege -k 14
    0e41270260895979317fff3898ab85668953aaa2
    count=10&lat=37.351&user_id=1&long=-119.827&waffle=eggo\x80\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x02(&waffle=liege

Compile
=======

    git clone https://github.com/bwall/HashPump.git
    apt-get install eclipse eclipse-cdt

Open eclipse, go to File->import, category General, choose Existing projects into Workspace, and select the folder you just downloaded. Use Ctrl+B to build the project (or see the Project menu to build only this project). The executable will be in the Debug folder of the source code.
