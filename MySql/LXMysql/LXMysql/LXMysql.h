#pragma once
#include<vector>
#include "LXData.h"
using std::vector;
using std::string;
#ifdef LXMYSQL_EXPORTS
//动态链接库项目调用
#define LXAPI __declspec(dllexport)
#else
//测试程序调用
#define LXAPI __declspec(dllimport)
#endif // LXMYSQL_EXPORTS
/*
	希望用户拿到该头文件的时候不要知道使用了哪些头文件，所以把头文件放到了cpp中，对于使用到的数据类型同意只做类型声明
*/
struct MYSQL;
struct MYSQL_RES;
//所有函数都是线程非安全的
namespace LX {

	
	class LXAPI LXMysql {
	public:
		//初始化
		bool Init();
		//清理所有占用的资源
		void Close();
		//数据库连接 不考虑线程安全
		bool Connect(const char* host, const char* user, const char* pass, const char* db, const unsigned short port = 3306, const unsigned long flag = 0);

		bool Query(const char* sql, unsigned long sqllen = 0);
		//mysql flag参数设定 要在Connect之前调用
		bool Options(LX_OPT option, const void* arg);
		//设定连接数据库超时时间
		bool SetConnectTimeout(int sec);
		//自动重连 默认不自动
		bool SetReconnect(bool isre = true);

		//结果集获取
		//获取全部结果到本地
		bool StoreResult();
		//通过fetch获取结果
		bool UseResult();
		//释放结果集占用的空间
		void FreeResult();
		//获取一行数据
		vector<LXData> FetchRow();

		//生成insert sql语句
		string GetInsertSQL(XDATA kv,string table);

		//插入非二进程数据
		bool Insert(XDATA& kv, string& table);
		//插入二进制数据
		bool InsertBin(XDATA kv, string& table);
		
		//获取更新数据的sql语句 where语句中，用户要包含where
		string GetUpdateSql(XDATA kv, string table,string where);
		//返回更新数量 失败返回-1(修改非二进制数据)
		int Update(XDATA kv, string table, string where);
		//修改二进制数据
		int UpdateBin(XDATA kv, string table, string where);

		//事务接口
		bool StartTransaction();
		bool StopTransaction();
		bool Commit();
		bool Rollback();

		//简易接口 返回select数据结果，每次调用清理上一次的结果集
		XROWS GetResult(const char* sql);

	protected:
		MYSQL* mysql = 0;
		MYSQL_RES* result = 0;
	};
}