#pragma once
#include<vector>
#include "LXData.h"
using std::vector;
using std::string;
#ifdef LXMYSQL_EXPORTS
//��̬���ӿ���Ŀ����
#define LXAPI __declspec(dllexport)
#else
//���Գ������
#define LXAPI __declspec(dllimport)
#endif // LXMYSQL_EXPORTS
/*
	ϣ���û��õ���ͷ�ļ���ʱ��Ҫ֪��ʹ������Щͷ�ļ������԰�ͷ�ļ��ŵ���cpp�У�����ʹ�õ�����������ͬ��ֻ����������
*/
struct MYSQL;
struct MYSQL_RES;
namespace LX {

	
	class LXAPI LXMysql {
	public:
		//��ʼ��
		bool Init();
		//��������ռ�õ���Դ
		void Close();
		//���ݿ����� �������̰߳�ȫ
		bool Connect(const char* host, const char* user, const char* pass, const char* db, const unsigned short port = 3306, const unsigned long flag = 0);

		bool Query(const char* sql, unsigned long sqllen = 0);
		//mysql flag�����趨 Ҫ��Connect֮ǰ����
		bool Options(LX_OPT option, const void* arg);
		//�趨�������ݿⳬʱʱ��
		bool SetConnectTimeout(int sec);
		//�Զ����� Ĭ�ϲ��Զ�
		bool SetReconnect(bool isre = true);

		//�������ȡ
		//��ȡȫ�����������
		bool StoreResult();
		//ͨ��fetch��ȡ���
		bool UseResult();
		//�ͷŽ����ռ�õĿռ�
		void FreeResult();
		//��ȡһ������
		vector<LXData> FetchRow();

		//����insert sql���
		string GetInsertSQL(XDATA kv,string table);

		//����Ƕ���������
		bool Insert(XDATA& kv, string& table);
		//�������������
		bool InsertBin(XDATA kv, string& table);
		
		//��ȡ�������ݵ�sql��� where����У��û�Ҫ����where
		string GetUpdateSql(XDATA kv, string table,string where);
		//���ظ������� ʧ�ܷ���-1(�޸ķǶ���������)
		int Update(XDATA kv, string table, string where);
	protected:
		MYSQL* mysql = 0;
		MYSQL_RES* result = 0;
	};
}