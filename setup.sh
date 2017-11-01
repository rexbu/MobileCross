#!/bin/bash
git remote add bs git@github.com:rexbu/bs.git
git remote add basic git@github.com:rexbu/basic.git
git remote add libzip git@github.com:rexbu/libzip.git
git remote add model git@github.com:rexbu/model.git

git subtree add --prefix bs bs master
git subtree add --prefix basic basic master
git subtree add --prefix model model master
git subtree add --prefix 3rdparty/libzip libzip master
