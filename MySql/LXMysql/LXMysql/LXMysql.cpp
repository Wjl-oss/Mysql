
#include"LXMysql.h"
#include<iostream>
#include<mysql.h>
using std::cout;
using std::endl;
using std::cerr;
namespace LX {
	bool LXMysql::Init()
	{
		Close();//清理上一次的连接资源
		cout << "LXMysql Init()" << endl;
		mysql = mysql_init(0);
		if (!mysql) {
			cerr << "mysql_init failed!" << endl;
		}
		return true;
	}

	void LXMysql::Close()
	{
		FreeResult();
		if (mysql) {
			mysql_close(mysql);
			mysql = nullptr;
		}
	}

	bool LXMysql::Connect(const char* host, const char* user, const char* pass, const char* db, const unsigned short port, const unsigned long flag)
	{
		//用户可以直接调用Connect，不用再调用Init
		if (!mysql && !Init()) {
			cerr << "Connect failed !Mysql is not init!" << endl;
			return false;
		}
		if (!mysql_real_connect(mysql, host, user, pass, db, port, 0, flag)) {
			cerr << "Mysql Connect failed!" << mysql_error(mysql) << endl;
		}
		cout << "mysql connect success!" << endl;
		return true;
	}

	bool LXMysql::Query(const char* sql, unsigned long sqllen)
	{
		if (!mysql) {
			cerr << "Query failed! mysql is NULL" << endl;
			return false;
		}
		if (!sql) {
			cerr << "sql is NULL" << endl;
			return false;
		}
		if (sqllen <= 0) sqllen = (unsigned long)strlen(sql);
		if (sqllen <= 0) {
			cerr << "Query sql is empty or wrong format" << endl;
			return false;
		}
		int re = mysql_real_query(mysql, sql, sqllen);
		if (re != 0) {
			cerr << "mysql_real_query failed" << mysql_error(mysql) << endl;
			return false;
		}
		//mysql_options()
		return true;
	}

	bool LXMysql::Options(LX_OPT opt, const void* arg)
	{
		if (!mysql) {
			cerr << "Option failed! mysql is NULL" << endl;
			return false;
		}
		int re = mysql_options(mysql, (mysql_option)opt, arg);
		if (re != 0) {
			cerr << "mysql_option failed!" << mysql_error(mysql) << endl;
			return false;
		}
		return true;
	}

	bool LXMysql::SetConnectTimeout(int sec)
	{

		return Options(LX_OPT_CONNECT_TIMEOUT, &sec);
	}

	bool LXMysql::SetReconnect(bool isre)
	{
		return Options(LX_OPT_RECONNECT, &isre);
	}
	bool LXMysql::StoreResult()
	{
		if (!mysql) {
			cerr << "Option failed! mysql is NULL" << endl;
			return false;
		}
		FreeResult();
		result = mysql_store_result(mysql);
		if (!result) {
			cerr << "mysql_store_result failed!" << mysql_error(mysql) << endl;
		}
		return true;
	}
	bool LXMysql::UseResult()
	{
		if (!mysql) {
			cerr << "Option failed! mysql is NULL" << endl;
			return false;
		}
		FreeResult();
		result = mysql_use_result(mysql);
		if (!result) {
			cerr << "mysql_use_result failed!" << mysql_error(mysql) << endl;
		}
		return true;
	}
	void LXMysql::FreeResult()
	{
		if (result) {
			mysql_free_result(result);
			result = nullptr;
		}
	}
	
	vector<LXData> LXMysql::FetchRow()
	{
		vector<LXData> re;
		if (!result) {
			return re;
		}
		MYSQL_ROW row = mysql_fetch_row(result);//获取每行数据的长度
		if (!row) {
			return re;
		}
		//获取列的数量
		unsigned long* lens = mysql_fetch_lengths(result);
		int num = mysql_num_fields(result);
		for (int i = 0; i < num; i++) {
			LXData data;
			data.data = row[i];//数据是由mysql开辟的空间 所以获取结果集后需要释放结果集
			data.size = lens[i];
			re.push_back(data);
		}
		return re;
	}
	string LXMysql::GetInsertSQL(XDATA kv, string table)
	{
		string sql;
		
		if (kv.empty() || table.empty())
			return "";
		sql = "insert into `";
		sql += table;
		sql += "`";

		string keys = "";
		string vals = "";
		
		for (auto ptr = kv.begin(); ptr != kv.end(); ptr++) {
			keys += "`";
			keys += ptr->first;
			keys += "`,";

			vals += "'";
			vals += ptr->second.data;
			vals += "',";
		}
		//去除结尾多余的逗号
		keys[keys.size() - 1] = ' ';
		vals[vals.size() - 1] = ' ';

		//
		sql += "(";
		sql += keys;
		sql += ") values(";
		sql += vals;
		sql += ")";
		return  sql;
	}
	bool LXMysql::Insert(XDATA& kv, string& table)
	{
		if (!mysql) {
			cerr << "Insert failed! mysql is NULL" << endl;
			return false;
		}
		string sql = GetInsertSQL(kv, table);
		if (sql.empty())
			return false;
		if (!Query(sql.c_str()))
			return false;

		int num = mysql_affected_rows(mysql);
		if (num <= 0)
			return false;

		return true;
	}
	bool LXMysql::InsertBin(XDATA kv, string& table)
	{
		string sql;

		if (kv.empty() || table.empty()||!mysql)
			return false;
		sql = "insert into `";
		sql += table;
		sql += "`";

		string keys = "";
		string vals = "";
		int i = 0;
		//绑定字段
		MYSQL_BIND bind[256] = { 0 };
		for (auto ptr = kv.begin(); ptr != kv.end(); ptr++) {
			keys += "`";
			keys += ptr->first;

			vals += "`=?,";
			bind[i].buffer = (char*)(ptr->second.data);
			bind[i].buffer_type = (enum_field_types)ptr->second.type;
			bind[i].buffer_length = ptr->second.size;
			i++;
		}
		//去除结尾多余的逗号
		keys[keys.size() - 1] = ' ';
		vals[vals.size() - 1] = ' ';

		sql += "(";
		sql += keys;
		sql += ") values(";
		sql += vals;
		sql += ")";


		//语句预处理
		MYSQL_STMT* stmt = mysql_stmt_init(mysql);
		if (!stmt) {
			mysql_stmt_close(stmt);
			cerr << "mysql_stmt_init failed!" << mysql_error(mysql);
			return false;
		}
		if (mysql_stmt_prepare(stmt, sql.c_str(), sql.length()) != 0) {
			mysql_stmt_close(stmt);
			cerr << "mysql_stmt_prepare" << mysql_error(mysql);
			return false;
		}
		if (mysql_stmt_bind_param(stmt, bind) != 0) {
			mysql_stmt_close(stmt);
			cerr << "mysql_stmt_bind_param" << mysql_stmt_error(stmt);
			return false;
		}
		if (mysql_stmt_execute(stmt) != 0) {
			mysql_stmt_close(stmt);
			cerr << "mysql_stmt_execute" << mysql_stmt_error(stmt);
			return false;
		}
		//清理stmt空间
		mysql_stmt_close(stmt);
		
		
		return true;
	}
	string LXMysql::GetUpdateSql(XDATA kv, string table, string where)
	{
		
		if (kv.empty() || table.empty())
			return "";

		string sql;
		sql = "update `";
		sql += table;
		sql += "` set ";



		for (auto ptr = kv.begin(); ptr != kv.end(); ptr++) {
			sql += "`";
			sql += ptr->first;
			sql += "`= '";
			sql += ptr->second.data;
			sql += "',";
			
		}
		//去除结尾多余的逗号
		sql[sql.size() - 1] = ' ';
		sql += " ";
		sql += where;
		return  sql;
	}
	int LXMysql::Update(XDATA kv, string table, string where)
	{

		if (!mysql) return -1;
		string sql=GetUpdateSql(kv, table, where);
		//cout << mysql << endl;
		if (sql.empty()) return false;

		if (!Query(sql.c_str())) {
			return -1;
		}
		return  mysql_affected_rows(mysql);
		
	}
	int LXMysql::UpdateBin(XDATA kv, string table, string where)
	{
		if (!mysql||kv.empty()||table.empty()) return -1;

		string sql;

		sql = "update `";
		sql += table;
		sql += "` set ";
		
		int i = 0;
		//绑定字段
		MYSQL_BIND bind[256] = { 0 };
		for (auto ptr = kv.begin(); ptr != kv.end(); ptr++) {
			sql += "`";
			sql += ptr->first;
			sql += "` =";

			sql += "?,";
			bind[i].buffer = (char*)(ptr->second.data);
			bind[i].buffer_type = (enum_field_types)ptr->second.type;
			bind[i].buffer_length = ptr->second.size;
			i++;
		}
		sql[sql.size() - 1] = ' ';
		sql += " ";
		sql += where;


		MYSQL_STMT* stmt = mysql_stmt_init(mysql);
		if (!stmt) {
			mysql_stmt_close(stmt);
			cerr << "mysql_stmt_init failed!" << mysql_error(mysql);
			return -1;
		}
		if (mysql_stmt_prepare(stmt, sql.c_str(), sql.length()) != 0) {
			mysql_stmt_close(stmt);
			cerr << "mysql_stmt_prepare" << mysql_error(mysql);
			return -1;
		}
		if (mysql_stmt_bind_param(stmt, bind) != 0) {
			mysql_stmt_close(stmt);
			cerr << "mysql_stmt_bind_param" << mysql_stmt_error(stmt);
			return -1;
		}
		if (mysql_stmt_execute(stmt) != 0) {
			mysql_stmt_close(stmt);
			cerr << "mysql_stmt_execute" << mysql_stmt_error(stmt);
			return -1;
		}
		//清理stmt空间
		mysql_stmt_close(stmt);

		return  mysql_affected_rows(mysql);
	}
	bool LXMysql::StartTransaction()
	{
		return Query("set autocommit=0");
	}
	bool LXMysql::StopTransaction()
	{
		return Query("set autocommit=1");
	}
	bool LXMysql::Commit()
	{
		return Query("commit");
	}
	bool LXMysql::Rollback()
	{
		return Query("rollback");
	}
}
