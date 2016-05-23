#!/bin/sh
###author:KyleWong
###基于Xcodebuild打包生成.app文件(签名也在此步骤，使用到的provision/certificate请提前安装到系统.)

priorWD=$PWD
codeDir=$1
workspaceName=$2
schemeName=$3
sdkName=$4
configuration=$5
gccMacros=$6
provisionUUID=$7
tmpSysRoot=$8

cd "$codeDir"

if [ -d "$tmpSysRoot" ];then
    rm -rf "$tmpSysRoot"
    else
    mkdir "$tmpSysRoot"
fi
xcodebuild clean
xcodebuild -workspace "$workspaceName" -scheme "$schemeName" -sdk "$sdkName" -configuration "$configuration" GCC_PREPROCESSOR_DEFINITIONS="$gccMacros" PROVISIONING_PROFILE="$provisionUUID" CONFIGURATION_BUILD_DIR="$tmpSysRoot" BUILT_PRODUCTS_DIR="$tmpSysRoot"

#恢复环境
cd "$priorWD"