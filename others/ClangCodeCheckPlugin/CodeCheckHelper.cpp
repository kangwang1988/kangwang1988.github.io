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
string kKeyInterfSelDictSource = "source";
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
    clsMethodJson[key]={{kKeyInterfSelDictFilename,filename},{kKeyInterfSelDictSource,sourcecode},{kKeyInterfSelDictRange,ss.str()}};
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
