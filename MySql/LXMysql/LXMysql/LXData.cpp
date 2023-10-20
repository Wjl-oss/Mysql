#include "LXData.h"
#include<iostream>
#include<fstream>

#ifdef _WIN32
#include<Windows.h>
#else
#include<iconv.h>
#endif

using std::fstream;
using std::ios;
using std::cerr;
using std::endl;
using std::cout;
using std::string;
using std::wstring;


namespace LX {

 
 
	LXData::LXData(const char* str)
	{
		if (!str) return;
        this->type = LX_TYPE_STRING;
		this->data = str;
		this->size = strlen(str);
	}
    LXData::LXData(const int* d)
    {
        this->type = LX_TYPE_LONG;
        this->data = (const char*)d;
        this->size = sizeof(int);
    }
	bool LXData::LoadFile(const char* filename)
	{
		if (!filename) return false;
        fstream fin(filename, ios::in | ios::binary);
        
        if (!fin.is_open())
        {
            cerr << "file " << filename << " open failed!" << endl;
        }
        // 文件指针移动到结尾处获得结尾处
        fin.seekg(0, ios::end);
        size = fin.tellg();
        fin.seekg(0, ios::beg); // 一定把文件指针移动到文件开头，否则读不到文件内容
        if (size <= 0) {
            cerr << "file is empty()" << endl;
            return false;
        }
        data = new char[size + 1];
        int readed = 0;
        while (!fin.eof())
        {
            fin.read((char*)data, size-readed);
            if (fin.gcount() <= 0)
            {
                break;
            }
            readed += fin.gcount();
        }
        fin.close();
        //只要在读文件一定是二进制形式传输
        this->type = LX_TYPE_LONG_BLOB;
		return true;
	}
    bool LXData::saveFile(const char* filename)
    {
        if (!data || size <= 0) return false;
        fstream out(filename, ios::out | ios::binary);
        if (!out.is_open()) {
            cout << "saveFile failed!open failed!" << filename << endl;
            return false;
        }

        out.write(data, size);
        out.close();
        return true;
    }
    void LXData::Drop()
    {
        if (!data) return;
        delete[] data;
        data = nullptr;
    }

    //static只是为了在本cpp内有效
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

    //https://blog.csdn.net/hxp1994/article/details/100905140
    //UTF8转GBK一般做法是UTF8先转unicode再转GBK，在WindowsVS中unicode编码一般是utf16
    string LXData::UTF8ToGBK()
    {
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

    string LXData::GBKToUTF8()
    {
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
}