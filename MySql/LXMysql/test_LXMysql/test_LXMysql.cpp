// test_LXMysql.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include "LXMysql.h"
#include <string>
using namespace std;
using namespace LX;

int main()
{
    LXMysql my;
    cout << "my.Init() = " << my.Init() << endl;
    

    my.SetConnectTimeout(3);//连接超时秒
    my.SetReconnect(true);//自动重连

    //连接mysql，如果没有调用Init内部会自动调用
    my.Connect("192.168.248.130", "root", "someoneWJL88@", "mydatabase");

    string sql;
#if 1
    //建表
    sql= "CREATE TABLE IF NOT EXISTS `t_video` ( \
        `id` int auto_increment,\
        `name` varchar(1024),\
        `path` varchar(2046),\
        `data` LONGBLOB,\
        `size` int,\
        PRIMARY KEY(`id`)\
        )ENGINE=InnoDB;";
    if (!my.Query(sql.c_str())) {
        cout<<"CREATE TABLE t_video failed !"<<endl;
    }
#endif
  
#if 0
    //测试自动重连 但是好像测试失败了
    for (;;) {
        cout << my.Query(sql.c_str()) << flush;
    }
#endif

#if 0
    cout << my.Query(sql.c_str()) << endl;//没有返回结果集的语句使用获取结果集语句会执行失败
    my.StoreResult();
    my.FreeResult();
#endif
    //cout << my.Query(sql.c_str()) << endl;//没有返回结果集的语句使用获取结果集语句会执行失败

    
#if 0
    //准备测试表中的数据
    //sql = "insert into t_video(`name`) value('qiaobenyoucai')";
    cout << my.Query(sql.c_str()) << endl;
    for (int i = 0; i < 100; i++)
    {
        sql = "insert into `t_video`(`name`,`path`,`size`) values('test" + to_string(i) + ".jpg','d:/img/test.jpg',10240);";
        cout << my.Query(sql.c_str()) << endl;
    }
   
#endif
    XDATA data1;//调用动态连接库中的类型一定要导出
    string table = "t_video";
#if 0
    //插入普通字符数据
  
    data1["name"] = LXData("wwwwwwwwwwww");
    data1["size"] = LXData("123");
    //cout << my.GetInsertSQL(data1, "t_video");
    my.Insert(data1, table);
#endif

#if 0
    //插入二进制数据
    data1["name"] = "test2.png";
    LXData binaryFile;
    binaryFile.LoadFile("E:\\Project\\MySql\\LXMysql\\test_LXMysql\\images\\test.png");
    data1["data"] = binaryFile;
    data1["size"] = &binaryFile.size;
    if (!my.InsertBin(data1, table)) {
        cout << "InsertBin failed!" << endl;
    }
    binaryFile.Drop();
#endif

#if 0
    //修改一般数据
    XDATA update;
    update["name"] = "wangzzzz";
    cout << my.Update(update, table, "where id = 1") << endl;

#endif

#if 0
    //修改二进制数据
    XDATA update2;
    LXData file2;
    file2.LoadFile("E:\\Project\\MySql\\LXMysql\\test_LXMysql\\images\\test2.bmp");
    update2["data"] = file2;
    cout << my.UpdateBin(update2, table, "where id = 2") << endl;
    file2.Drop();
#endif


#if 1
    //测试事务
    //使用事务插入多条数据
    XDATA data3;

    data3["name"] = "trans001";
    data3["size"] = "1";
    my.StartTransaction();
    my.Insert(data3, table);
    my.Insert(data3, table);
    my.Insert(data3, table);
    my.Rollback();//回滚

    data3["name"] = "trans002";
    data3["size"] = "3";
    my.Insert(data3, table);
    data3["name"] = "trans003";
    data3["size"] = "5";
    my.Insert(data3, table);
    my.Commit();
    my.StopTransaction();


#endif

#if 1
    //获取结果集
    sql = "select * from t_video";
    my.Query(sql.c_str());
    my.StoreResult();//结果集存储到本地

    while (1) {
        auto row = my.FetchRow();//从结果集中获取一行文本数据
        if (row.size() == 0) break;

        row[3].saveFile(row[1].data);//保存文件 文件会输出到该项目的工作目录下 目前会保存到E:\Project\Lib\mysql-8.1.0-winx64\lib
        
        for (int i = 0; i < row.size(); i++) {
             if (row[i].data)
                cout << row[i].data << " "; 
        }
        cout << endl;
    }

    my.FreeResult();

    //cout << my.Query(sql.c_str()) << endl;
    //my.UseResult();//开始接收结果集
    //my.FreeResult();
#endif

   

    my.Close();
    
    //std::cout << "Hello World!\n";
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
