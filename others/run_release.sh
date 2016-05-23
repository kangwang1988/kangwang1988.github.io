#!/bin/sh
###author:KyleWong
###打包工具
#********************打包步骤********************#
#********************1.获取Git获取最新代码********************#
#********************2.基于Xcodebuild打包生成.app文件(签名也在此步骤，使用到的provvision/certificate请提前安装到系统.)********************#
#********************3.基于.app文件生成.ipa文件********************#
#********************4.其他********************#


priorWD=$PWD
configuration="Release"
codeParDir="~/Documents/Codes/$configuration"
prjName="your-project-name"
gitURL="git@git.xxx.com:username/repository.git"
gitBranch="master"
baseDir=$(dirname "$0")
codeDir="$codeParDir/$prjName"
targetName="$prjName" #可执行目标文件
workspaceName="$targetName.xcworkspace"
schemeName="$targetName" #与xcworkspace配合使用
sdkName="iphoneos"
gccMacros=""
provisionUUID="your-provision-uuid"
buildOutputDir="$codeDir/tmpSysRoot$configuration"
ipaOutputDir="~/$targetName$configuration/$targetName$configuration"".ipa"
jsonOutputDir="~/$targetName$configuration/$targetName$configuration"".json"

#获取代码
sh "$baseDir/git_common.sh" "$codeParDir" "$prjName" "$gitURL" "$gitBranch"

#切入实际代码并pod
cd "$codeDir"
/usr/local/bin/pod _0.39.0_ install
cd "$priorWD"

#确保scheme存在且可被xcodebuild正常使用
open "$codeDir/$workspaceName" -a Xcode
sleep 5

#compile
sh "$baseDir/compile_common.sh" "$codeDir" "$workspaceName" "$schemeName" "$sdkName" "$configuration" "$gccMacros" "$provisionUUID" "$buildOutputDir"

#package
sh "$baseDir/package_common.sh" "$buildOutputDir/$targetName.app" "$ipaOutputDir"

#便于网页安装相关数据更新
dateTime=$(date +"%m.%d %H:%M")
echo "{\"time\":\"$dateTime\"}" > "$jsonOutputDir"

#恢复环境
cd "$priorWD"