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
        // �ļ�ָ���ƶ�����β����ý�β��
        fin.seekg(0, ios::end);
        size = fin.tellg();
        fin.seekg(0, ios::beg); // һ�����ļ�ָ���ƶ����ļ���ͷ������������ļ�����
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
        //ֻҪ�ڶ��ļ�һ���Ƕ�������ʽ����
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

    //staticֻ��Ϊ���ڱ�cpp����Ч
#ifndef _WIN32
    static size_t Convert(char* from_cha, char* to_cha, char* in, size_t inlen, char* out, size_t outlen) {
        iconv_t cd;
        cd = iconv_open(to_cha, from_cha);
        if (cd == 0) return -1;

        memset(out, 0, sizeof out);
        //����ת���ֽ���������
        size_t re = iconv(cd, &in, &inlen, &out, &outlen);

        if (cd != 0) iconv_close(cd);

        return re;
    }
#endif

    //https://blog.csdn.net/hxp1994/article/details/100905140
    //UTF8תGBKһ��������UTF8��תunicode��תGBK����WindowsVS��unicode����һ����utf16
    string LXData::UTF8ToGBK()
    {
        string re;
#ifdef _WIN32
        //utf8תΪunicode ��windows��unicode�ַ���ʹ��utf16���� ����ƽ̨���뷽ʽ���ܲ�һ��    
        //���ַ���ӳ�䵽 UTF-16 (���ַ�) �ַ����� �ַ�����һ�����Զ��ֽ��ַ�����
        //https://learn.microsoft.com/zh-cn/windows/win32/api/stringapiset/nf-stringapiset-multibytetowidechar
        int len = MultiByteToWideChar(CP_UTF8, 0, data, -1, 0, 0);//�����utf16ռ�ö����ֽ�
        if (len <= 0) {
            return re;
        }
        wstring udata;
        udata.resize(len);
        //��Windows�ϣ�wchar_t����ͨ�����ڱ�ʾ���ַ������С��UTF-16�����һ���ַ��Ĵ�С��һ�µģ���ռ��2���ֽڡ�
        // ��ˣ�wchar_t���Է�������ڴ洢�ʹ���UTF-16������ַ�����
        // ����ע�⣬�����������������ƽ̨�϶����ã�����ƽ̨���ܻ��в�ͬ��ʵ��
        MultiByteToWideChar(CP_UTF8, 0, data, -1, (wchar_t*)udata.data(), len);//utf8תUnicode

        //unicodeתGBK

        len = WideCharToMultiByte(CP_ACP, 0, (wchar_t*)udata.data(), -1, 0, 0, 0, 0);//��ȡ���ֽ�תΪ���ֽ���Ҫ�Ŀռ��С
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
        int len = MultiByteToWideChar(CP_ACP, 0, data, -1, 0, 0);//�����utf16ռ�ö����ֽ�
        if (len <= 0) {
            return re;
        }
        wstring udata;
        udata.resize(len);

        MultiByteToWideChar(CP_ACP, 0, data, -1, (wchar_t*)udata.data(), len);

        //unicodeתGBK

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