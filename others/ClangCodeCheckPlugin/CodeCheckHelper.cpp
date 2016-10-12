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
string kKeyInterfSelDictRefProtos = "refProtos";
string kKeyInterfSelDictSuperClass = "superClass";
string kKeyInterfSelDictProtos = "protos";
string kKeyInterfSelDictInterfs = "interfs";

CodeCheckHelper *sCodeCheckHelper;

CodeCheckHelper* CodeCheckHelper::sharedInstance(){
    if(!sCodeCheckHelper){
        sCodeCheckHelper = new CodeCheckHelper();
    }
    return sCodeCheckHelper;
}

void CodeCheckHelper::appendObjcClsMethodImpl(bool isInstanceMethod,string cls,string selector,string filename,unsigned rangeFrom,unsigned rangeTo,string sourcecode){
    stringstream ss;
    ss<<rangeFrom<<'-'<<rangeTo;
    string key = string(isInstanceMethod?"-":"+")+"["+cls+" "+selector+"]";
    clsMethodJson[key]={{kKeyInterfSelDictFilename,filename},{kKeyInterfSelDictSourceCode,sourcecode},{kKeyInterfSelDictRange,ss.str()}};
}

void CodeCheckHelper::appendObjcMethodImplCall(bool isInstanceMethod, string cls, string selector, bool calleeIsInstanceMethod, string calleeCls, string calleeSel){
    string key = string(isInstanceMethod?"-":"+")+"["+cls+" "+selector+"]";
    string value = string(calleeIsInstanceMethod?"-":"+")+"["+calleeCls+" "+calleeSel+"]";
    json clsMethodObj = json(clsMethodJson[key]);
    json callees = json(clsMethodObj[kKeyInterfSelDictCallees]);
    vector<string> oldCalleeVec;
    if(callees.is_array())
        oldCalleeVec = callees.get<vector<string>>();
    if(find(oldCalleeVec.begin(),oldCalleeVec.end(),value)==oldCalleeVec.end())
        callees.push_back(value);
    clsMethodObj[kKeyInterfSelDictCallees]=callees;
    clsMethodJson[key]=clsMethodObj;
}

void CodeCheckHelper::appendObjcCls(string cls,string supCls,vector<string> protoVec){
    stringstream ss;
    json clsJson = json(clsInterfHierachy[cls]);
    json protoJson = json(clsJson[kKeyInterfSelDictProtos]);
    vector<string> oldProtoVec;
    if(protoJson.is_array())
        oldProtoVec = protoJson.get<vector<string>>();
    for(vector<string>::iterator it = protoVec.begin();it!=protoVec.end();it++){
        string proto = *it;
        if(find(oldProtoVec.begin(), oldProtoVec.end(), proto) == oldProtoVec.end()) {
            protoJson.push_back(proto);
        }
    }
    clsInterfHierachy[cls]={{kKeyInterfSelDictSuperClass,supCls},{kKeyInterfSelDictProtos,protoJson},{kKeyInterfSelDictInterfs,clsJson[kKeyInterfSelDictInterfs]}};
}

void CodeCheckHelper::appendObjcClsInterf(string cls,bool isInstanceInterf,string selector){
    json interfHierachyObj = json(clsInterfHierachy[cls]);
    json interfs = json(interfHierachyObj[kKeyInterfSelDictInterfs]);
    string value = string(isInstanceInterf?"-":"+")+"["+cls+" "+selector+"]";
    vector<string> oldInterfsVec;
    if(interfs.is_array())
        oldInterfsVec = interfs.get<vector<string>>();
    if(find(oldInterfsVec.begin(),oldInterfsVec.end(),value)==oldInterfsVec.end())
        interfs.push_back(value);
    clsInterfHierachy[cls]={{kKeyInterfSelDictSuperClass,interfHierachyObj[kKeyInterfSelDictSuperClass]},{kKeyInterfSelDictProtos,interfHierachyObj[kKeyInterfSelDictProtos]},{kKeyInterfSelDictInterfs,interfs}};
}

void CodeCheckHelper::appendObjcProto(string proto,vector<string> refProto){
    json protoJson = json(protoInterfHierachy[proto]);
    json protoRefProtosJson = json(protoJson[kKeyInterfSelDictProtos]);
    vector<string> oldProtoRefProtoVec;
    if(protoRefProtosJson.is_array())
        oldProtoRefProtoVec = protoRefProtosJson.get<vector<string>>();
    for(vector<string>::iterator it = refProto.begin();it!=refProto.end();it++){
        string value = *it;
        if(find(oldProtoRefProtoVec.begin(),oldProtoRefProtoVec.end(),value)==oldProtoRefProtoVec.end())
            protoRefProtosJson.push_back(*it);
    }
    protoInterfHierachy[proto]={{kKeyInterfSelDictInterfs,protoJson[kKeyInterfSelDictInterfs]},{kKeyInterfSelDictProtos,protoRefProtosJson}};
}

void CodeCheckHelper::appendObjcProtoInterf(string proto, bool isInstanceInterf, string selector){
    json protoHierachyObj = json(protoInterfHierachy[proto]);
    json interfs = json(protoHierachyObj[kKeyInterfSelDictInterfs]);
    string value = string(isInstanceInterf?"-":"+")+"["+proto+" "+selector+"]";
    vector<string> oldInterfsVec;
    if(interfs.is_array())
        oldInterfsVec = interfs.get<vector<string>>();
    if(find(oldInterfsVec.begin(),oldInterfsVec.end(),value)==oldInterfsVec.end())
        interfs.push_back(value);
    protoInterfHierachy[proto]={{kKeyInterfSelDictInterfs,interfs},{kKeyInterfSelDictProtos,protoHierachyObj[kKeyInterfSelDictProtos]}};
}

void CodeCheckHelper::synchronize(){
    milliseconds time = duration_cast< milliseconds >(
                                                      system_clock::now().time_since_epoch()
                                                      );
    if(!clsMethodJson.is_null()){
        ofstream ofs;
        stringstream ss;
        ss<<gSrcRootPath<<"/Analyzer/"<<time.count()<<".clsMethod.jsonpart";
        ofs.open (ss.str(),ofstream::out | ofstream::trunc);
        ofs<<clsMethodJson<<endl;
        ofs.close();
    }
    if(!clsInterfHierachy.is_null()){
        ofstream ofs;
        stringstream ss;
        ss<<gSrcRootPath<<"/Analyzer/"<<time.count()<<".clsInterfHierachy.jsonpart";
        ofs.open (ss.str(),ofstream::out | ofstream::trunc);
        ofs<<clsInterfHierachy<<endl;
        ofs.close();
    }
    if(!protoInterfHierachy.is_null()){
        ofstream ofs;
        stringstream ss;
        ss<<gSrcRootPath<<"/Analyzer/"<<time.count()<<".protoInterfHierachy.jsonpart";
        ofs.open (ss.str(),ofstream::out | ofstream::trunc);
        ofs<<protoInterfHierachy<<endl;
        ofs.close();
    }
}
