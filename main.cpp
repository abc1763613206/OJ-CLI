#include <iostream>
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <string>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <map>

//#define CURL_STATICLIB
#include "lib/systemarch.h"
//md5算法实现来自https://github.com/JieweiWei/md5
#include "lib/md5.cpp"

/*
#if defined OS_LINUX
#include <curl/curl.h>
#elif defined OS_WIN
//Windows加
#include <windows.h>
#include "lib/curl/curl.h"
#endif
*/
#define CONFIGFILE "config.ini"
const std::string COOKIEFILE="cookie.txt";

const std::string GithubURL="https://github.com/abc1763613206/OJ-CLI";  //写这个的时候应该还没有建好库，先整个占位符

std::map<std::string, std::string> ConfigInfo;

namespace Info{
	void info(std::string s){std::string out="\n\033[37mInfo: "+s+"\n\033[0m";std::cout<<out;}
	void success(std::string s){std::string out="\n\033[32mSuccess: "+s+"\n\033[0m";std::cout<<out;}
	void error(std::string s){std::string out="\n\033[31mError: "+s+"\n\033[0m";std::cout<<out;}
	void debug(std::string s){std::string out="\n\033[34mDebug: "+s+"\n\033[0m";std::cout<<out;}
	void GotoIssues(){std::string out="\n\033[37m请在这里提Issue反馈： \033[4m"+GithubURL+"\n\033[0m";std::cout<<out;}
	void usage(){printf("OJ-CLI 一个便捷的命令行OJ交题程序\n");printf("Usage: oj-cli [OJ] [problemid] [file]\n");}
}

void UnknownException(){ //没有捕获的错误
    Info::error("没有捕获的错误！");
    Info::GotoIssues();
}

struct SystemInfo{  //暂定
	int Arch; //操作系统类型：0为Linux 1为Windows
}SysInfo;


std::fstream _configfile;
void ReplaceStr(std::string &strBig, const std::string &strsrc, const std::string &strdst){
//https://blog.csdn.net/shaoyiju/article/details/78377060
    std::string::size_type pos = 0;
    std::string::size_type srclen = strsrc.size();
    std::string::size_type dstlen = strdst.size();

    while( (pos=strBig.find(strsrc, pos)) != std::string::npos ){
        strBig.replace( pos, srclen, strdst );
        pos += dstlen;
    }
    return ;
}

//win 下和 linux 下由于CRLF(/r/n)问题会出锅（输出问题和加密问题），因此清洁之以兼容跨平台
std::string CleanStr(std::string str){
	ReplaceStr(str,"\n","");
	ReplaceStr(str,"\r","");
	return str;

}
std::string GetConfig(std::string str){ //从map中获取配置
		std::map<std::string, std::string>::iterator iter_configMap;
    	iter_configMap = ConfigInfo.find(str);
    	std::string strResult = iter_configMap->second;
    	return CleanStr(strResult);
}

void Execute(std::string str){  //Windows下需要处理转义
	#if defined OS_WIN
	ReplaceStr(str,"\'","\"");
	//ReplaceStr(str,"&","^&");
	#endif
	//Info::info(str);  //for debug
	system(str.data());
	return ;
}

void MergeFile(std::string _file1,std::string _file2){//系统级文件合并 将_file1合并入_file2
	std::string command="";
	#if defined OS_LINUX
	command="cat "+_file1+" >> "+_file2;
	system(command.data());
	#elif defined OS_WIN
	command="type "+_file1+" >> "+_file2;
	system(command.data());
	#endif
}
/*
CURLcode ret;
CURL *hnd;
struct curl_slist *slist1;
*/

std::string curlHeader="  --progress-bar -H \'User-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/73.0.3683.103 Safari/537.36 OJ-CLI/1.0\' ";

void TransToLower(std::string &str){
	for(int i=0;i<str.length();i++){
		if(str[i]>='A' && str[i]<='Z') str[i]+=32;
		else if(str[i]>='a' && str[i]<='z') continue;
		else {
			Info::error("请输入正确的OJ！");
			throw 1;
		}
	}
}


namespace Request{
	void flushHeader(){curlHeader="  --progress-bar -H \'User-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/73.0.3683.103 Safari/537.36 OJ-CLI/1.0\' ";}
	/*
	using std::stringstream;using std::endl;
	void AddHeader(std::string str){
		slist1 = curl_slist_append(slist1, str.data());
	}
	size_t write_data(void *ptr, size_t size, size_t nmemb, void *stream) //https://www.cnblogs.com/chechen/p/7238919.html
	{
	    string data((const char*) ptr, (size_t) size * nmemb);
	
	    *((stringstream*) stream) << data << endl;
	
	    return size * nmemb;
	}
	std::string Run(std::string url,std::string data){
		//curl --libcurl
		AddHeader("User-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/73.0.3683.103 Safari/537.36 OJ-CLI/1.0");
		hnd = curl_easy_init();
		std::stringstream out;
		curl_easy_setopt(hnd, CURLOPT_URL, url.data());
 		curl_easy_setopt(hnd, CURLOPT_NOPROGRESS, 1L);
 		curl_easy_setopt(hnd, CURLOPT_POSTFIELDS, data.data());
 		curl_easy_setopt(hnd, CURLOPT_POSTFIELDSIZE_LARGE, (curl_off_t)64);
 		//curl_easy_setopt(hnd, CURLOPT_USERAGENT, "curl/7.47.0");
 		curl_easy_setopt(hnd, CURLOPT_HTTPHEADER, slist1);
 		curl_easy_setopt(hnd, CURLOPT_MAXREDIRS, 50L);
 		curl_easy_setopt(hnd, CURLOPT_ACCEPT_ENCODING, "");
 		curl_easy_setopt(hnd, CURLOPT_TCP_KEEPALIVE, 1L);

 		curl_easy_setopt(hnd, CURLOPT_WRITEFUNCTION, write_data);
        curl_easy_setopt(hnd, CURLOPT_WRITEDATA, &out);
 		ret = curl_easy_perform(hnd);
        if (ret != CURLE_OK) {
            Info::error("curl 请求失败：curl_easy_strerror(ret)");Info::GotoIssues();
        }
        std::string result = out.str();
 		return result;
	}*/
	//Windows 下编译使用CURL的锅太多了，在改好之前先用命令行吧
	void AddHeader(std::string str){
		curlHeader=curlHeader+"-H \'"+str+"\' ";
	}
	std::string command;
	void Run(std::string url,std::string data,bool savecookie){
		//curl --libcurl
		
		if(savecookie==true){ //登录操作时使用：保存cookie
			command="curl -c "+COOKIEFILE+" "+"\'"+url+"\' "+curlHeader+"--data \'"+data+"\' > result.txt";
		}else{ //提交时使用：读取cookie
			command="curl -b @"+COOKIEFILE+" "+"\'"+url+"\' "+curlHeader+"--data \'"+data+"\' ";
		}
		Execute(command);
		flushHeader();
	}
	void RunWithSession(std::string url,std::string data){
		command="curl -b @"+COOKIEFILE+" -c "+COOKIEFILE+" "+"\'"+url+"\' "+curlHeader+"--data \'"+data+"\' > result.txt";
		Execute(command);
		flushHeader();
	}
	void formdataUpload(std::string url,std::string _file,std::string language){  //现代OJ净爱用这种奇葩上传方式
		//每个OJ上传方式都很奇葩，暂不封装
	}/*
	void Upload(std::string url,std::string _file){ //有些OJ允许直接上传文件
		command="curl -b @"+COOKIEFILE+" "+"\'"+url+"\' "+curlHeader+"-F \'file=@"+_file+"\' ";
		Execute(command);
		flushHeader();
	}*/
}

namespace LOJ{  //应该能在syzoj系通用
	void Login(){
		//https://github.com/syzoj/syzoj/blob/07a5335bb0cbbbdd255ae578f6f5b9cc15504892/modules/api.js
		std::string Account=GetConfig("LOJAccount");
		std::string Password=GetConfig("LOJPassword");
		//https://github.com/syzoj/syzoj/blob/07a5335bb0cbbbdd255ae578f6f5b9cc15504892/views/login.ejs#L55  
		//好恶心的拼接字符串啊
		std::string encoder="syzoj2_xxx";
		std::string SumStr=Password+encoder;
		std::string EncodedPassword=MD5(SumStr).toStr();
		Info::info("使用 "+Account+" 账户登录 LOJ");
		std::string data="username="+Account+"&password="+EncodedPassword;
		Request::AddHeader("Accept: */*");
		Request::AddHeader("Referer: https://loj.ac/login");
		Request::AddHeader("Origin: https://loj.ac");
		Request::AddHeader("X-Requested-With: XMLHttpRequest");
		Request::AddHeader("Content-Type: application/x-www-form-urlencoded; charset=UTF-8");
		Request::Run("https://loj.ac/api/login",data,true);
		std::fstream _codefile;
		_codefile.open("result.txt");
		std::string strLine;
        std::getline(_codefile, strLine);
        ReplaceStr(strLine,"{\"error_code\":","");
        ReplaceStr(strLine,"}","");
        ReplaceStr(strLine,"\r","");
        ReplaceStr(strLine,"\n","");
        if(strLine=="1"){
        	Info::success("登录成功！");return;
        }
		else if(strLine=="1001"){
			Info::error("用户不存在");
			throw 1;
		}
		else if(strLine=="1002"){
			Info::error("密码错误");
			throw 1;
		}
		else if(strLine=="1003"){
			Info::error("您尚未设置密码，请通过下方「找回密码」来设置您的密码。");
			throw 1;
		}
		else {Info::error("未知错误");throw 1;}
	}
	void Submit(std::string _id,std::string _file){
		std::string _url="https://loj.ac/problem/"+_id+"/submit?contest_id=";
		//Request::AddHeader("Content-Type: multipart/form-data");
		Request::command="curl -b @"+COOKIEFILE+" "+"\'"+_url+"\' "+curlHeader+" -F language=cpp -F code=   -F answer=@"+_file+"   > result.txt";
		Execute(Request::command);
		std::fstream _codefile;
		_codefile.open("result.txt");
		std::string strLine;
        std::getline(_codefile, strLine);
        _codefile.close();
        ReplaceStr(strLine,"Found. Redirecting to ","https://loj.ac");
        Info::info("成功！请去\033[4m"+strLine+"\033[0m查看结果！");
        #if defined OS_WIN
        Request::command="start "+strLine;
        system(Request::command.data());
        #endif
	}

}

namespace UOJ{  
	std::string csrf_token="";  //curl 请求获取token
	void Login(){
		std::string Account=GetConfig("UOJAccount");
		std::string Password=GetConfig("UOJPassword");
		//傻了吧，你UOJ用HmacMD5加盐
		Info::info("使用 "+Account+" 账户登录 UOJ");
		std::fstream _codefile;
		Request::command="curl -c "+COOKIEFILE+" "+"\'http://uoj.ac/login\' > web.txt";  //UOJ需要提前获取一个SESSID
		Execute(Request::command);
		_codefile.open("web.txt");
		std::string strLine;
		std::string filesum="";
		int i=0;
		while (!_codefile.eof())
        {
           i++;
           std::getline(_codefile, strLine);
           if(i==146){  //TODO：换成更好的读取方式
           		csrf_token = strLine;
           		ReplaceStr(csrf_token,"\n","");
           		ReplaceStr(csrf_token,"		_token : \"","");
           		ReplaceStr(csrf_token,"\",","");
           		break;
           }
        }
        _codefile.close();
        //size_t pos = filesum.find("_token : ");
        //Info::info(csrf_token);
		std::string data="_token="+csrf_token+"&login=&username="+Account+"&password="+Password;
		Request::AddHeader("Accept: */*");
		Request::AddHeader("Origin: http://uoj.ac");
		Request::AddHeader("X-Requested-With: XMLHttpRequest");
		Request::AddHeader("Content-Type: application/x-www-form-urlencoded; charset=UTF-8");
		Request::RunWithSession("http://uoj.ac/login",data);
		_codefile.open("result.txt");
        std::getline(_codefile, strLine);
        ReplaceStr(strLine,"\r","");
        ReplaceStr(strLine,"\n","");
        if(strLine=="ok"){
        	Info::success("登录成功！");return;
        }
		else if(strLine=="banned"){
			Info::error("用户已被禁用");
			throw 1;
		}
		else if(strLine=="expired"){
			Info::error("页面已过期");
			throw 1;
		}
		else {Info::error("用户名或密码错误");throw 1;}
	}
	void Submit(std::string _id,std::string _file){
		//Request::AddHeader("Content-Type: multipart/form-data");
		std::string _url="http://uoj.ac/problem/"+_id;
		Request::RunWithSession(_url,"check-answer=");  //不知道这个请求有啥作用
		Request::command="curl -b @"+COOKIEFILE+" "+"\'"+_url+"\' "+curlHeader+"-F _token="+csrf_token+"  -F answer_answer_language=C++  -F answer_answer_editor=  -F answer_answer_upload_type=file -F answer_answer_file=@"+_file+"   -F submit-answer=answer -w %{http_code} > result.txt";
		Execute(Request::command);
		std::fstream _codefile;
		_codefile.open("result.txt");
		std::string strLine;
        std::getline(_codefile, strLine);
        _codefile.close();
		
		if(strLine=="302")Info::info("提交成功！请去\033[4mhttp://uoj.ac/submissions\033[0m查看结果！");
		else {Info::error("提交失败！");throw 1;}
		#if defined OS_WIN
        Request::command="start http://uoj.ac/submissions";
        system(Request::command.data());
        #endif
	}	

}

namespace BZOJ{ //过于古老且无权限号 暂时咕掉

}

namespace DarkBZOJ{ //截至coding时darkbzoj还是死亡状态，所以我们先咕咕咕着
	std::string csrf_token="";  //curl 请求获取token
	void Login(){
		std::string Account=GetConfig("DarkBZOJAccount");
		std::string Password=GetConfig("DarkBZOJPassword");
		//傻了吧，你UOJ用HmacMD5加盐
		Info::info("使用 "+Account+" 账户登录 DarkBZOJ");
		std::fstream _codefile;
		Request::command="curl -c "+COOKIEFILE+" "+"\'https://darkbzoj.tk/login\' > web.txt";  //UOJ需要提前获取一个SESSID
		Execute(Request::command);
		_codefile.open("web.txt");
		std::string strLine;
		std::string filesum="";
		int i=0;
		while (!_codefile.eof())
        {
           i++;
           std::getline(_codefile, strLine);
           if(i==144){  //TODO：换成更好的读取方式
           		csrf_token = strLine;
           		ReplaceStr(csrf_token,"\n","");
           		ReplaceStr(csrf_token,"		_token : \"","");
           		ReplaceStr(csrf_token,"\",","");
           		break;
           }
        }
        _codefile.close();
        //size_t pos = filesum.find("_token : ");
        //Info::info(csrf_token);
		std::string data="_token="+csrf_token+"&login=&username="+Account+"&password="+Password;
		Request::AddHeader("Accept: */*");
		Request::AddHeader("Origin: https://darkbzoj.tk");
		Request::AddHeader("X-Requested-With: XMLHttpRequest");
		Request::AddHeader("Content-Type: application/x-www-form-urlencoded; charset=UTF-8");
		Request::RunWithSession("https://darkbzoj.tk/login",data);
		_codefile.open("result.txt");
        std::getline(_codefile, strLine);
        ReplaceStr(strLine,"\r","");
        ReplaceStr(strLine,"\n","");
        if(strLine=="ok"){
        	Info::success("登录成功！");return;
        }
		else if(strLine=="banned"){
			Info::error("用户已被禁用");
			throw 1;
		}
		else if(strLine=="expired"){
			Info::error("页面已过期");
			throw 1;
		}
		else {Info::error("用户名或密码错误");throw 1;}
	}
	void Submit(std::string _id,std::string _file){
		//Request::AddHeader("Content-Type: multipart/form-data");
		std::string _url="https://darkbzoj.tk/problem/"+_id;
		Request::RunWithSession(_url,"check-answer=");  //不知道这个请求有啥作用
		Request::command="curl -b @"+COOKIEFILE+" "+"\'"+_url+"\' "+curlHeader+"-F _token="+csrf_token+"  -F answer_answer_language=C++  -F answer_answer_editor=  -F answer_answer_upload_type=file -F answer_answer_file=@"+_file+"   -F submit-answer=answer -w %{http_code} > result.txt";
		Execute(Request::command);
		std::fstream _codefile;
		_codefile.open("result.txt");
		std::string strLine;
        std::getline(_codefile, strLine);
        _codefile.close();
		
		if(strLine=="302")Info::info("提交成功！请去\033[4mhttps://darkbzoj.tk/submissions\033[0m查看结果！");
		else {Info::error("提交失败！");throw 1;}
		#if defined OS_WIN
        Request::command="start https://darkbzoj.tk/submissions";
        system(Request::command.data());
        #endif
	}	
}

namespace Public{
	void CreateConfig();void ReadConfigFromFile();
	void Init(){ //程序初始化相关
		#if defined OS_WIN
		SysInfo.Arch=1;
		system("chcp 65001>nul"); //9102年了还是要处理UTF-8乱码问题
		#elif defined OS_LINUX
		SysInfo.Arch=0;
		#elif defined OS_UNKNOWN
		Info::error("未知操作系统！");Info::GotoIssues();
		#endif
		//slist1 = NULL;
		_configfile.open(CONFIGFILE);
		//此处实现感谢ouuan
		bool isopen = _configfile.is_open();
		_configfile.close();
		if(!isopen){   //不存在配置文件，创建新配置文件
			Info::error("未发现配置文件！");
			CreateConfig();
		}
		//有配置文件 从配置文件中读取数值到map
		ReadConfigFromFile();
	}
	void CreateConfig(){ //重新配置
		std::fstream file(CONFIGFILE, std::ios::out);  //清空文件
		Info::info("正在创建配置文件");
		_configfile.open(CONFIGFILE);
		printf("接下来需要提供您的账户信息，请坐和放宽（不配置请输入null\n");
		Info::info("UOJ 和 DarkBZOJ 的密钥请您拼接好了再填入！\n拼接方式请见\033[4mhttps://git.io/fjZdT\n\033[0m");
		#if defined OS_WIN
        Request::command="start https://git.io/fjZdT";
        system(Request::command.data());
        #endif
		std::string str,in;
		printf("您的 UOJ 登录账户:"); std::cin>>str;
		in="UOJAccount="+str+"\n";
		_configfile<<in;
		printf("您的 UOJ 登录密钥\033[31m(加密拼接后)\033[0m:"); std::cin>>str;
		in="UOJPassword="+str+"\n";
		_configfile<<in;
		printf("您的 LOJ 登录账户:"); std::cin>>str;
		in="LOJAccount="+str+"\n";
		_configfile<<in;
		printf("您的 LOJ 登录密码:"); std::cin>>str;
		in="LOJPassword="+str+"\n";
		_configfile<<in;
		//printf("您的 BZOJ 登录账户:"); std::cin>>str;
		//in="BZOJAccount="+str+"\n";
		//_configfile<<in;
		//printf("您的 BZOJ 登录密码:"); std::cin>>str;
		//in="BZOJPassword="+str+"\n";
		//_configfile<<in;
		printf("您的 DarkBZOJ 登录账户:"); std::cin>>str;
		in="DarkBZOJAccount="+str+"\n";
		_configfile<<in;
		printf("您的 DarkBZOJ 登录密钥\033[31m(加密拼接后)\033[0m:"); std::cin>>str;
		in="DarkBZOJPassword="+str+"\n";
		_configfile<<in;
		_configfile.close();
	}
	void ReadConfigFromFile(){
		_configfile.open(CONFIGFILE);
		std::string strLine;
		while (!_configfile.eof())
        {
            std::getline(_configfile, strLine);
            if (strLine.compare(0, 1, "#") == 0) {continue;}  //跳过#开头的注释信息（大概不会把含有#密码的项目过滤掉？
            size_t pos = strLine.find('=');
            std::string str_key = strLine.substr(0, pos);           
            std::string str_value = strLine.substr(pos + 1) ;
            ConfigInfo.insert(std::pair<std::string,std::string>(str_key,str_value));
        }
        _configfile.close();
	}
	void ProcessFunc(std::string oj,std::string _id,std::string _file){
		TransToLower(oj);
		if(oj=="loj"){LOJ::Login();LOJ::Submit(_id,_file);} 
		else if(oj=="uoj"){UOJ::Login();UOJ::Submit(_id,_file);} 
		else if(oj=="darkbzoj"){DarkBZOJ::Login();DarkBZOJ::Submit(_id,_file);}
		else {Info::error("未知OJ！");}
	}
	void End(){
		//程序退出相关 TODO
		
		//清理curl相关
		//curl_easy_cleanup(hnd);
  		//hnd = NULL;
  		//curl_slist_free_all(slist1);
  		//slist1 = NULL;
  		
  		//清理临时文件
  		#if defined OS_LINUX
		std::string command="rm -rf "+COOKIEFILE+" result.txt";
		system(command.data());
		#elif defined OS_WIN
		std::string command="del /F /S "+COOKIEFILE+" result.txt";
		system(command.data());
		#endif
		return ;
	}
}




namespace Debug{ //调试相关函数
	void printMap(){ //遍历map，检查config是否正常配置
		Info::debug("打印map内容");
		std::map<std::string, std::string>::iterator iter_configMap;
		iter_configMap = ConfigInfo.begin();
		iter_configMap++;
		while(iter_configMap != ConfigInfo.end()){
			std::string key=iter_configMap->first;
			std::string value=iter_configMap->second;
			std::string out=key+"="+value+"\n";
			std::cout<<out;
			iter_configMap++;
		}

	}
}

int main(int argc, char* argv[]){
	std::set_terminate(UnknownException);
	Public::Init();
	if (argc < 4 ){
		Info::error("请传入正确参数！");Info::usage(); //TODO:没有参数传入时做成交互
		return -1;
	}
	try {
		std::string  oj = argv[1], _id = argv[2], _file = argv[3];
		Public::ProcessFunc(oj,_id,_file);
	} catch(...){
		Info::error("程序运行时出现错误！");
		Info::GotoIssues();
	}
	Public::End();
	return 0;
}