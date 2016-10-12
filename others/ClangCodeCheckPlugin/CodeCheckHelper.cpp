//
//  CodeCheckHelper.cpp
//  ClangCodeCheckPlugin
//
//  Created by KyleWong on 11/10/2016.
//
//

#include "CodeCheckHelper.h"
#include <streambuf>
#include <chrono>
using namespace std::chrono;

string gSrcRootPath = "";
string kTokenSeperator="B6D0013E3994E4DA54531E783F795B";
string kKeyInterfSelDictFilename = "filename";
string kKeyInterfSelDictSourceCode = "sourceCode";
string kKeyInterfSelDictRange = "range";
string kKeyInterfSelDictCallees = "callee";

CodeCheckHelper *sCodeCheckHelper;

CodeCheckHelper* CodeCheckHelper::sharedInstance(){
    if(!sCodeCheckHelper){
        sCodeCheckHelper = new CodeCheckHelper();
    }
    return sCodeCheckHelper;
}

void CodeCheckHelper::appendObjcClsMethod(bool isInstanceMethod,string cls,string selector,string filename,unsigned rangeFrom,unsigned rangeTo,string sourcecode){
    stringstream ss;
    ss<<rangeFrom<<'-'<<rangeTo;
    string key = string(isInstanceMethod?"-":"+")+"["+cls+" "+selector+"]";
    clsMethodJson[key]={{kKeyInterfSelDictFilename,filename},{kKeyInterfSelDictSourceCode,sourcecode},{kKeyInterfSelDictRange,ss.str()}};
}

void CodeCheckHelper::appendObjcMethodCall(bool isInstanceMethod, string cls, string selector, bool calleeIsInstanceMethod, string calleeCls, string calleeSel){
    string key = string(isInstanceMethod?"-":"+")+"["+cls+" "+selector+"]";
    string value = string(calleeIsInstanceMethod?"-":"+")+"["+calleeCls+" "+calleeSel+"]";
    json clsMethodObj = json(clsMethodJson[key]);
    json callees = json(clsMethodObj[kKeyInterfSelDictCallees]);
    bool valueExists = false;
    for(json::iterator it=callees.begin();it!=callees.end();it++){
        if(!value.compare(*it)){
            valueExists = true;
            break;
        }
    }
    if(!valueExists)
        callees.push_back(value);
    clsMethodObj[kKeyInterfSelDictCallees]=callees;
    clsMethodJson[key]=clsMethodObj;
}

void CodeCheckHelper::synchronize(){
    if(clsMethodJson.is_null())
        return;
    ofstream ofs;
    stringstream ss;
    milliseconds time = duration_cast< milliseconds >(
                                                    system_clock::now().time_since_epoch()
                                                    );
    ss<<gSrcRootPath<<"/Analyzer/"<<time.count()<<".jsonpart";
    ofs.open (ss.str(),ofstream::out | ofstream::trunc);
    ofs<<clsMethodJson<<endl;
    ofs.close();
}
