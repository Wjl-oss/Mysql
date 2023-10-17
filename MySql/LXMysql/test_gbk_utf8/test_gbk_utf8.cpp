// test_gbk_utf8.cpp : ���ļ����� "main" ����������ִ�н��ڴ˴���ʼ��������
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
    //����ת���ֽ���������
    size_t re = iconv(cd, &in, &inlen, &out, &outlen);

    if (cd != 0) iconv_close(cd);

    return re;
}
#endif

using std::string;
using std::cout;
using std::endl;
using std::wstring;

//���ֽڿ��ֽ�
//https://blog.csdn.net/hxp1994/article/details/100905140


//UTF8תGBKһ��������UTF8��תunicode��תGBK����WindowsVS��unicode����һ����utf16
string UTF8ToGBK(const char* data) {
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

string GBKToUTF8(const char* data) {
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

int main()
{
#ifdef _WIN32
    std::cout << "Hello World! ������\n";
    //����UTF8��GBK��ת��
    cout << UTF8ToGBK(u8"����UTF-8תGBK") << endl;//windows�����µ�vsĬ�ϲ���unicode�ַ�����utf16���룬Ҳ���ǿ��ַ�����

    //vs�е��ļ�һ����uft8-bom���� �����������������ǰҪ�ȰѴ�cppת�����ʽΪGBK
    //windows����̨�����ʽһ����GBK
    //����GBK��UTF8��ת��
    string utf8 = GBKToUTF8("����GBKתΪUTF8��תΪGBK");//��תΪGBK��Ϊ�˴�ӡ�ڿ���̨
    cout << "utf8 = " << utf8 << endl;//utf8�ڲ���GBK����Ŀ�����ʾӦ��������
    cout << UTF8ToGBK(utf8.c_str()) << endl;//תΪGBK

#else
    //linux�����µ�Ĭ���ַ�������һ���ǲ���utf8,ת��Ϊutf8֮������ǻ������
    string in = u8"����UTF-8תGBK";
    cout << "���� utf8 = " << in << endl;
    string gbk = UTF8ToGBK(in.c_str());
    cout << "gbk = " << gbk << endl;//linuxĬ����utf8����ӡgbk������
    cout << "gbkתutf8 = " << GBKToUTF8(gbk.c_str()) << endl;
#endif
    return 0;
}
