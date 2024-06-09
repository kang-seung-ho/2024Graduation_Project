#pragma once
#include <sql.h>
#include <sqlext.h>
#include <afxdb.h>

import <cstddef>;
import <cstdint>;
import <memory>;
import <optional>;
import <expected>;
import <array>;
import <vector>;
import <thread>;
import <stop_token>;
import <mutex>;
import <latch>;

#define UNLIKELY [[unlikely]]

struct sockaddr_in;

namespace auth
{
	static inline constexpr std::uint16_t serverPort = 40000;
	static inline constexpr std::size_t iocpWorkerCount = 4;
	static inline constexpr std::uintptr_t iocpMasterKey = 0;
	static inline constexpr std::uint32_t maxRecvSize = 512;
	static inline constexpr std::uintptr_t invalidSocket = static_cast<std::uintptr_t>(-1);

	class Server
	{
	public:
		using Result = std::expected<void, int>;

		void* iocpHandle{ nullptr };

		Server() = default;

		Result Initialize();
		Result Startup();
		void Cleanup();

		Result InitializeNetwork();
		Result InitializeSockets();
		Result InitializeIocp();
		bool InitializeBuffers();
		Result InitializeWorkers();
		bool InitializeExitHandlers();
		void ArriveWorkersIntialized() noexcept;

		Result BeginReceive();
		void EndReceive();
		Result BeginSend();
		void EndSend(class SendContext* context);

		void OnReceived(unsigned long bytes);
		void OnSent(class SendContext* context);

		Result RegisterSocket(std::uintptr_t socket, std::uintptr_t key) noexcept;

		friend void DatabaseHandler(std::stop_token&& canceller, auth::Server& instance);
		friend void ExitHandler() noexcept;

	private:
		void PrintReceivedData() const;

		std::uintptr_t serverSocket{ invalidSocket };
		std::uintptr_t fileIoSocket{ invalidSocket };
		std::uintptr_t databaseSocket{ invalidSocket };

		std::array<std::jthread, iocpWorkerCount> myThreads;
		static std::latch workerInitializationSynchronizer;

		class IoContext* recvContext;
		std::unique_ptr<char[]> recvBuffer;
		unsigned long recvBytes;
		struct ::sockaddr_in* recvAddress;
	};

	void DatabaseHandler(std::stop_token&& canceller, auth::Server& instance);
	void ExitHandler() noexcept;
};

class CQuery
{
public:
	// 최대 컬럼수, BLOB 입출력 단위, NULL 필드값
	enum {
		MAXCOL = 100, BLOBBATCH = 10000, CQUERYNULL = -100, CQUERYEOF = -101,

		CQUERYNOCOL = -102, CQUERYERROR = -103
	};

private:
	SQLHENV hEnv;                       // 환경 핸들
	SQLHDBC hDbc;                      // 연결 핸들
	SQLRETURN ret;                      // 최후 실행한 SQL문의 결과값
	TCHAR Col[MAXCOL][255];       // 바인딩될 컬럼 정보

	int FindCol(TCHAR* name);       // 컬럼의 이름으로부터 번호를 찾아준다.



public:
	SQLINTEGER AffectCount;                // 영향받은 레코드 개수
	SQLHSTMT hStmt;                          // 명령 핸들. 직접 사용 가능
	SQLSMALLINT nCol;                       // 컬럼 개수
	SQLCHAR ColName[MAXCOL][50];   // 컬럼의 이름들
	SQLINTEGER lCol[MAXCOL];           // 컬럼의 길이/상태 정보

	void PrintDiag();                            // 진단 정보 출력
	CQuery();                                    // 생성자
	~CQuery();                                  // 파괴자:연결 핸들을 해제한다.


	// 데이터 소스에 연결한다.
	BOOL Connect(int Type, TCHAR* ConStr, TCHAR* UID = NULL, TCHAR* PWD = NULL);
	BOOL Exec(LPCTSTR szSQL);      // SQL문을 실행한다.
	int ExecGetInt(LPCTSTR szSQL);   // SQL문을 실행하고 첫번째 컬럼의 정수 읽음


	// SQL문을 실행하고 첫번째 컬럼의 문자열 읽음
	void ExecGetStr(LPCTSTR szSQL, TCHAR* buf);
	SQLRETURN Fetch();                            // 한 행 가져오기
	void Clear();                                       // 커서 해제 및 언 바인딩
	int GetInt(int nCol);                              // 정수형 컬럼 읽기
	int GetInt(TCHAR* sCol);                       // 정수형 컬럼 읽기
	void GetStr(int nCol, TCHAR* buf);          // 문자열형 컬럼 읽기
	void GetStr(TCHAR* sCol, TCHAR* buf);   // 문자열형 컬럼 읽기
	int ReadBlob(LPCTSTR szSQL, void* buf);
	void WriteBlob(LPCTSTR szSQL, void* buf, int size)

};