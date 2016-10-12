//
//  CodeCheckHelper.h
//  ClangCodeCheckPlugin
//
//  Created by KyleWong on 11/10/2016.
//
//

#ifndef CodeCheckHelper_h
#define CodeCheckHelper_h
#include <string>
#include <fstream>
#include <vector>
#include <map>
#include "json.hpp"
using namespace std;
using namespace nlohmann;

class CodeCheckHelper{
public:
   static CodeCheckHelper *sharedInstance();
   void appendObjcClsMethodImpl(bool isInstanceMethod,string cls,string selector,string filename,unsigned rangeFrom,unsigned rangeTo,string sourcecode);
   void appendObjcMethodImplCall(bool isInstanceMethod,string cls,string selector,bool calleeIsInstanceMethod,string calleeCls,string calleeSel);
   void appendObjcCls(string cls,string supCls,vector<string> protoVec);
   void appendObjcClsInterf(string cls,bool isInstanceInterf,string selector);
   void appendObjcProto(string proto,vector<string> refProto);
   void appendObjcProtoInterf(string proto,bool isInstanceInterf,string selector);
   void synchronize();
private:
    json clsMethodJson;
    json clsInterfHierachy;
    json protoInterfHierachy;
};
#endif /* CodeCheckHelper_h */
