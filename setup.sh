#!/bin/bash
action=$1
path=$2

if [ "$action" = "init" ];then
    git remote add bs git@github.com:rexbu/bs.git
    git remote add basic git@github.com:rexbu/basic.git
    git remote add libzip git@github.com:rexbu/libzip.git
    git remote add model git@github.com:rexbu/model.git

    git subtree add --prefix ${path}bs bs master
    git subtree add --prefix ${path}basic basic master
    git subtree add --prefix ${path}model model master
    git subtree add --prefix ${path}3rdparty/libzip libzip master
elif [ "$action" = "push" ];then
    git subtree push --prefix ${path}bs git@github.com:rexbu/bs.git master
    git subtree push --prefix ${path}basic git@github.com:rexbu/basic.git master
    git subtree push --prefix ${path}3rdparty/libzip git@github.com:rexbu/libzip.git  master
    git subtree push --prefix ${path}model git@github.com:rexbu/model.git master
elif [ "$action" = "pull" ];then
    git subtree pull --prefix ${path}bs git@github.com:rexbu/bs.git master
    git subtree pull --prefix ${path}basic git@github.com:rexbu/basic.git master
    git subtree pull --prefix ${path}3rdparty/libzip git@github.com:rexbu/libzip.git  master
    git subtree pull --prefix ${path}model git@github.com:rexbu/model.git master
else
    echo "please use ./setup.sh {action} {path}. action:init|push|pull. when path is current dir, don't use path. eg: ./setup.sh push"
fi
