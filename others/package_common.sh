#!/bin/sh
###author:KyleWong
###基于.app文件生成.ipa文件

priorWD=$PWD
appPath=$1
ipaPath=$2
dir=$(dirname "$appPath")
basename=$(basename "$appPath" ".app")
ipadir=$(dirname "$ipaPath")
ipabasename=$(basename "$ipaPath" ".ipa")

cd $dir

payloadDir="$dir/Payload"
if [ ! -d "$payloadDir" ];then
	mkdir "$payloadDir"
fi
rm -rf "$payloadDir/*"
cp -rf "$appPath" "$payloadDir"
zip -qry "$dir/$basename.ipa" "Payload"
mv "$dir/$basename.ipa" "$ipaPath"
mv "$dir/"$basename".app.dsym" "$ipadir/"$ipabasename".app.dsym"
#恢复环境
cd "$priorWD"
