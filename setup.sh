#!/bin/bash
action=$1
path=$2


modules=("bs" "basic" "libzip" "model")
remotes=("git@github.com:rexbu/bs.git" "git@github.com:rexbu/basic.git" "git@github.com:rexbu/libzip.git" "git@github.com:rexbu/model.git")
localdirs=("bs" "basic" "3rdparty/libzip" "model")

echo "---- start: ${BASH_SOURCE}"

if [ "$action" = "init" ];then
    for ((i=0; i<${#modules[@]};i++)); do
        git remote add ${modules[i]} ${remotes[i]}
        git subtree add --prefix ${path}${localdirs[i]} ${modules[i]} master
        setup=${path}${localdirs[i]}/setup.sh
        if [ -f "$setup" ];then
            $setup $action ${path}${localdirs[i]}/ 
        fi
    done
elif [ "$action" = "push" ];then
    for ((i=0; i<${#modules[@]};i++)); do
        setup=${path}${localdirs[i]}/setup.sh
        if [ -f "$setup" ];then
            $setup $action ${path}${localdirs[i]}/ 
        fi
        git subtree push --prefix ${path}${localdirs[i]} ${remotes[i]} master
    done
elif [ "$action" = "pull" ];then
    for ((i=0; i<${#modules[@]};i++)); do
        setup=${path}${localdirs[i]}/setup.sh
        if [ -f "$setup" ];then
            $setup $action ${path}${localdirs[i]}/ 
        fi
        git subtree pull --prefix ${path}${localdirs[i]} ${remotes[i]} master
    done
else
    echo "please use ./setup.sh {action} {path}. action:init|push|pull. when path is current dir, don't use path. eg: ./setup.sh push"
fi

echo "#### end: ${BASH_SOURCE}"
