// test_gbk_utf8.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include<string>
#include<Windows.h>
using std::string;
using std::cout;
using std::endl;
using std::wstring;

//多字节宽字节
//https://blog.csdn.net/hxp1994/article/details/100905140


//UTF8转GBK一般做法是UTF8先转unicode再转GBK，在WindowsVS中unicode编码一般是utf16
string UTF8ToGBK(const char* data) {
    string utf8;

    //utf8转为unicode 在windows上unicode字符集使用utf16编码 不用平台编码方式可能不一样
    
    //将字符串映射到 UTF-16 (宽字符) 字符串。 字符串不一定来自多字节字符集。
    //https://learn.microsoft.com/zh-cn/windows/win32/api/stringapiset/nf-stringapiset-multibytetowidechar
    int len=MultiByteToWideChar(CP_UTF8,0,data,-1,0,0);//计算出utf16占用多少字节
    if (len <= 0) {
        return utf8;
    }
    wstring udata;
    udata.resize(len);
    //在Windows上，wchar_t类型通常用于表示宽字符，其大小与UTF-16编码的一个字符的大小是一致的，都占用2个字节。
    // 因此，wchar_t可以方便地用于存储和处理UTF-16编码的字符串。
    // 但请注意，这种情况并非在所有平台上都适用，其他平台可能会有不同的实现
    MultiByteToWideChar(CP_UTF8, 0, data, -1, (wchar_t*)udata.data(), len);//utf8转Unicode

    //unicode转GBK

    len=WideCharToMultiByte(CP_ACP, 0, (wchar_t*)udata.data(), -1, 0, 0, 0, 0);//获取宽字节转为多字节需要的空间大小
    if (len <= 0) {
        return utf8;
    }
    utf8.resize(len);
    WideCharToMultiByte(CP_ACP, 0, (wchar_t*)udata.data(), -1, (char*)utf8.data(), len, 0, 0);

    return utf8;
}



string GBKToUTF8(const char* data) {
    string gbk;

    int len = MultiByteToWideChar(CP_ACP, 0, data, -1, 0, 0);//计算出utf16占用多少字节
    if (len <= 0) {
        return gbk;
    }
    wstring udata;
    udata.resize(len);
  
    MultiByteToWideChar(CP_ACP, 0, data, -1, (wchar_t*)udata.data(), len);

    //unicode转GBK

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
    std::cout << "Hello World! 王家林\n";
    //测试UTF8到GBK的转换
    cout << UTF8ToGBK(u8"测试UTF-8转GBK") << endl;//windows环境下的vs默认采用unicode字符集的utf16编码，也就是宽字符编码

    //vs中的文件一般是uft8-bom编码 所以跑下面这个测试前要先把此cpp转编码格式为GBK
    //windows控制台编码格式一般是GBK
    //测试GBK到UTF8的转换
    string utf8 = GBKToUTF8("测试GBK转为UTF8再转为GBK");//再转为GBK是为了打印在控制台
    cout << "utf8 = " << utf8 << endl;//utf8在采用GBK编码的控制显示应该是乱码
    cout << UTF8ToGBK(utf8.c_str()) << endl;//转为GBK

    

}

// 运行程序: Ctrl + F5 或调试 >“开始执行(不调试)”菜单
// 调试程序: F5 或调试 >“开始调试”菜单

// 入门使用技巧: 
//   1. 使用解决方案资源管理器窗口添加/管理文件
//   2. 使用团队资源管理器窗口连接到源代码管理
//   3. 使用输出窗口查看生成输出和其他消息
//   4. 使用错误列表窗口查看错误
//   5. 转到“项目”>“添加新项”以创建新的代码文件，或转到“项目”>“添加现有项”以将现有代码文件添加到项目
//   6. 将来，若要再次打开此项目，请转到“文件”>“打开”>“项目”并选择 .sln 文件
