// test_gbk_utf8.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include<string>
#include<string.h>
#ifdef _WIN32
#include<Windows.h>
#else
#include<iconv.h>
#endif

#ifndef _WIN32
static size_t Convert(char* from_cha, char* to_cha, char* in, size_t inlen, char* out, size_t outlen) {
    iconv_t cd;
    cd = iconv_open(to_cha, from_cha);
    if (cd == 0) return -1;

    memset(out, 0, sizeof out);
    //返回转换字节数的数量
    size_t re = iconv(cd, &in, &inlen, &out, &outlen);

    if (cd != 0) iconv_close(cd);

    return re;
}
#endif

using std::string;
using std::cout;
using std::endl;
using std::wstring;

//多字节宽字节
//https://blog.csdn.net/hxp1994/article/details/100905140


//UTF8转GBK一般做法是UTF8先转unicode再转GBK，在WindowsVS中unicode编码一般是utf16
string UTF8ToGBK(const char* data) {
    string re;
#ifdef _WIN32
    //utf8转为unicode 在windows上unicode字符集使用utf16编码 不用平台编码方式可能不一样    
    //将字符串映射到 UTF-16 (宽字符) 字符串。 字符串不一定来自多字节字符集。
    //https://learn.microsoft.com/zh-cn/windows/win32/api/stringapiset/nf-stringapiset-multibytetowidechar
    int len = MultiByteToWideChar(CP_UTF8, 0, data, -1, 0, 0);//计算出utf16占用多少字节
    if (len <= 0) {
        return re;
    }
    wstring udata;
    udata.resize(len);
    //在Windows上，wchar_t类型通常用于表示宽字符，其大小与UTF-16编码的一个字符的大小是一致的，都占用2个字节。
    // 因此，wchar_t可以方便地用于存储和处理UTF-16编码的字符串。
    // 但请注意，这种情况并非在所有平台上都适用，其他平台可能会有不同的实现
    MultiByteToWideChar(CP_UTF8, 0, data, -1, (wchar_t*)udata.data(), len);//utf8转Unicode

    //unicode转GBK

    len = WideCharToMultiByte(CP_ACP, 0, (wchar_t*)udata.data(), -1, 0, 0, 0, 0);//获取宽字节转为多字节需要的空间大小
    if (len <= 0) {
        return re;
    }
    re.resize(len);
    WideCharToMultiByte(CP_ACP, 0, (wchar_t*)udata.data(), -1, (char*)re.data(), len, 0, 0);

#else

    re.resize(1024);
    int inlen = strlen(data);
    Convert((char*)"utf-8", (char*)"gbk", (char*)data, inlen, (char*)re.c_str(), re.size());
    int outlen = strlen(re.data());
    re.resize(outlen);
#endif

    return re;
}

string GBKToUTF8(const char* data) {
    string re;
#ifdef _WIN32
    int len = MultiByteToWideChar(CP_ACP, 0, data, -1, 0, 0);//计算出utf16占用多少字节
    if (len <= 0) {
        return re;
    }
    wstring udata;
    udata.resize(len);

    MultiByteToWideChar(CP_ACP, 0, data, -1, (wchar_t*)udata.data(), len);

    //unicode转GBK

    len = WideCharToMultiByte(CP_UTF8, 0, (wchar_t*)udata.data(), -1, 0, 0, 0, 0);
    if (len <= 0) {
        return re;
    }
    re.resize(len);
    WideCharToMultiByte(CP_UTF8, 0, (wchar_t*)udata.data(), -1, (char*)re.data(), len, 0, 0);

#else
    re.resize(1024);
    int inlen = strlen(data);
    Convert((char*)"gbk", (char*)"utf-8", (char*)data, inlen, (char*)re.c_str(), re.size());
    int outlen = strlen(re.data());
    re.resize(outlen);
#endif
    return re;
}

int main()
{
#ifdef _WIN32
    std::cout << "Hello World! 王家林\n";
    //测试UTF8到GBK的转换
    cout << UTF8ToGBK(u8"测试UTF-8转GBK") << endl;//windows环境下的vs默认采用unicode字符集的utf16编码，也就是宽字符编码

    //vs中的文件一般是uft8-bom编码 所以跑下面这个测试前要先把此cpp转编码格式为GBK
    //windows控制台编码格式一般是GBK
    //测试GBK到UTF8的转换
    string utf8 = GBKToUTF8("测试GBK转为UTF8再转为GBK");//再转为GBK是为了打印在控制台
    cout << "utf8 = " << utf8 << endl;//utf8在采用GBK编码的控制显示应该是乱码
    cout << UTF8ToGBK(utf8.c_str()) << endl;//转为GBK

#else
    //linux环境下的默认字符集编码一般是采用utf8,转换为utf8之后输出是会乱码的
    string in = u8"测试UTF-8转GBK";
    cout << "输入 utf8 = " << in << endl;
    string gbk = UTF8ToGBK(in.c_str());
    cout << "gbk = " << gbk << endl;//linux默认是utf8，打印gbk会乱码
    cout << "gbk转utf8 = " << GBKToUTF8(gbk.c_str()) << endl;
#endif
    return 0;
}
