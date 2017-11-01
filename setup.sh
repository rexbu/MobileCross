#!/bin/bash
action=$2
path=$1

if [ "$action" = "init" ];then
    git remote add $path/bs git@github.com:rexbu/bs.git
    git remote add $path/basic git@github.com:rexbu/basic.git
    git remote add $path/libzip git@github.com:rexbu/libzip.git
    git remote add $path/model git@github.com:rexbu/model.git

    git subtree add --prefix $path/bs bs master
    git subtree add --prefix $path/basic basic master
    git subtree add --prefix $path/model model master
    git subtree add --prefix $path/3rdparty/libzip libzip master
elif [ "$action" = "push" ];then
    git subtree push --prefix $path/bs git@github.com:rexbu/bs.git master
    git subtree push --prefix $path/basic git@github.com:rexbu/basic.git master
    git subtree push --prefix $path/3rdparty/libzip git@github.com:rexbu/libzip.git  master
    git subtree push --prefix $path/model git@github.com:rexbu/model.git master
elif [ "$action" = "pull" ];then
    git subtree pull --prefix $path/bs git@github.com:rexbu/bs.git master
    git subtree pull --prefix $path/basic git@github.com:rexbu/basic.git master
    git subtree pull --prefix $path/3rdparty/libzip git@github.com:rexbu/libzip.git  master
    git subtree pull --prefix $path/model git@github.com:rexbu/model.git master
else
    echo 'please use ./setup.sh {path} {action}. action:init|push|pull. eg: ./setup.sh ./ push'
fi
