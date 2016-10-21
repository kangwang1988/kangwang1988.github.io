#!/bin/sh
###author:KyleWong
###获取Git获取最新代码

priorWD=$PWD
codeParDir=$1
prjName=$2
gitURL=$3
gitBranch=$4
codeDir="$codeParDir/$prjName"
gitParams="--quiet"

#检出代码
if [ ! -d "$codeDir" ];then
	git clone "$gitURL" "$gitParams" -b "$gitBranch" "$codeDir"	
fi

#丢弃所有更改,并更新。
cd "$codeDir"
git reset --hard "$gitParams"
git pull origin "$gitBranch" "$gitParams"

#恢复环境
cd "$priorWD"
