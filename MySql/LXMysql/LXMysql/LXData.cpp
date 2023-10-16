#include "LXData.h"
#include<iostream>
#include<fstream>

using std::fstream;
using std::ios;
using std::cerr;
using std::endl;
using std::cout;
using std::string;

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
}