// test_gbk_utf8.cpp : ���ļ����� "main" ����������ִ�н��ڴ˴���ʼ��������
//

#include <iostream>
#include<string>
#include<Windows.h>
using std::string;
using std::cout;
using std::endl;
using std::wstring;

//���ֽڿ��ֽ�
//https://blog.csdn.net/hxp1994/article/details/100905140


//UTF8תGBKһ��������UTF8��תunicode��תGBK����WindowsVS��unicode����һ����utf16
string UTF8ToGBK(const char* data) {
    string utf8;

    //utf8תΪunicode ��windows��unicode�ַ���ʹ��utf16���� ����ƽ̨���뷽ʽ���ܲ�һ��
    
    //���ַ���ӳ�䵽 UTF-16 (���ַ�) �ַ����� �ַ�����һ�����Զ��ֽ��ַ�����
    //https://learn.microsoft.com/zh-cn/windows/win32/api/stringapiset/nf-stringapiset-multibytetowidechar
    int len=MultiByteToWideChar(CP_UTF8,0,data,-1,0,0);//�����utf16ռ�ö����ֽ�
    if (len <= 0) {
        return utf8;
    }
    wstring udata;
    udata.resize(len);
    //��Windows�ϣ�wchar_t����ͨ�����ڱ�ʾ���ַ������С��UTF-16�����һ���ַ��Ĵ�С��һ�µģ���ռ��2���ֽڡ�
    // ��ˣ�wchar_t���Է�������ڴ洢�ʹ���UTF-16������ַ�����
    // ����ע�⣬�����������������ƽ̨�϶����ã�����ƽ̨���ܻ��в�ͬ��ʵ��
    MultiByteToWideChar(CP_UTF8, 0, data, -1, (wchar_t*)udata.data(), len);//utf8תUnicode

    //unicodeתGBK

    len=WideCharToMultiByte(CP_ACP, 0, (wchar_t*)udata.data(), -1, 0, 0, 0, 0);//��ȡ���ֽ�תΪ���ֽ���Ҫ�Ŀռ��С
    if (len <= 0) {
        return utf8;
    }
    utf8.resize(len);
    WideCharToMultiByte(CP_ACP, 0, (wchar_t*)udata.data(), -1, (char*)utf8.data(), len, 0, 0);

    return utf8;
}



string GBKToUTF8(const char* data) {
    string gbk;

    int len = MultiByteToWideChar(CP_ACP, 0, data, -1, 0, 0);//�����utf16ռ�ö����ֽ�
    if (len <= 0) {
        return gbk;
    }
    wstring udata;
    udata.resize(len);
  
    MultiByteToWideChar(CP_ACP, 0, data, -1, (wchar_t*)udata.data(), len);

    //unicodeתGBK

    len = WideCharToMultiByte(CP_UTF8, 0, (wchar_t*)udata.data(), -1, 0, 0, 0, 0);
    if (len <= 0) {
        return gbk;
    }
    gbk.resize(len);
    WideCharToMultiByte(CP_UTF8, 0, (wchar_t*)udata.data(), -1, (char*)gbk.data(), len, 0, 0);

    return gbk;
}
int main()
{
    std::cout << "Hello World! ������\n";
    //����UTF8��GBK��ת��
    cout << UTF8ToGBK(u8"����UTF-8תGBK") << endl;//windows�����µ�vsĬ�ϲ���unicode�ַ�����utf16���룬Ҳ���ǿ��ַ�����

    //vs�е��ļ�һ����uft8-bom���� �����������������ǰҪ�ȰѴ�cppת�����ʽΪGBK
    //windows����̨�����ʽһ����GBK
    //����GBK��UTF8��ת��
    string utf8 = GBKToUTF8("����GBKתΪUTF8��תΪGBK");//��תΪGBK��Ϊ�˴�ӡ�ڿ���̨
    cout << "utf8 = " << utf8 << endl;//utf8�ڲ���GBK����Ŀ�����ʾӦ��������
    cout << UTF8ToGBK(utf8.c_str()) << endl;//תΪGBK

    

}

// ���г���: Ctrl + F5 ����� >����ʼִ��(������)���˵�
// ���Գ���: F5 ����� >����ʼ���ԡ��˵�

// ����ʹ�ü���: 
//   1. ʹ�ý��������Դ�������������/�����ļ�
//   2. ʹ���Ŷ���Դ�������������ӵ�Դ�������
//   3. ʹ��������ڲ鿴���������������Ϣ
//   4. ʹ�ô����б��ڲ鿴����
//   5. ת������Ŀ��>���������Դ����µĴ����ļ�����ת������Ŀ��>�����������Խ����д����ļ���ӵ���Ŀ
//   6. ��������Ҫ�ٴδ򿪴���Ŀ����ת�����ļ���>���򿪡�>����Ŀ����ѡ�� .sln �ļ�
