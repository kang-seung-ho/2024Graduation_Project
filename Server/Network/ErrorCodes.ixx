export module Iconer.Net.ErrorCode;
import <cstdint>;
import <string>;

export namespace iconer::net
{
	enum class [[nodiscard]] ErrorCode : std::int32_t
	{
		PendedIoOperation = 997,
		IO_INCOMPLETE = 996,
		INVALID_HANDLE = 6,
		INVALID_PARAMETER = 87,
		NOT_ENOUGH_MEMORY = 8,
		OPERATION_ABORTED = 995,
		WAIT_EVENT_0 = 0,
		WAIT_IO_COMPLETION = 192,
		NetBASEERR = 10000,
		BlockingCallsInterrupted = 10004,
		NetErrorAsBADF = 10009,
		NetErrorAsACCES = 10013,
		NetErrorAsFAULT = 10014,
		NetErrorAsINVAL = 10022,
		NetErrorAsMFILE = 10024,
		NonBlockedOperation = 10035, // EWOULDBLOCK
		YetInProgress = 10036,
		NetErrorAsALREADY = 10037,
		NotASocket = 10038,
		NetErrorAsDESTADDRREQ = 10039,
		NetErrorAsMSGSIZE = 10040,
		NetErrorAsPROTOTYPE = 10041,
		NetErrorAsNOPROTOOPT = 10042,
		NetErrorAsPROTONOSUPPORT = 10043,
		NetErrorAsSOCKTNOSUPPORT = 10044,
		NoSupportOperation = 10045,
		NetErrorAsPFNOSUPPORT = 10046,
		NetErrorAsAFNOSUPPORT = 10047,
		CannotUseThisAddress = 10048,
		NetErrorAsADDRNOTAVAIL = 10049,
		NetworkIsDown = 10050,
		NetErrorAsNETUNREACH = 10051,
		NetErrorAsNETRESET = 10052,
		ConnectionAborted = 10053,
		ConnectionStateReset = 10054, // Disconneting
		NoBufferStorage = 10055,
		NetErrorAsISCONN = 10056,
		NotConnected = 10057,
		ClosedSocket = 10058,
		NetErrorAsTOOMANYREFS = 10059,
		NetworkTimeOut = 10060,
		NetErrorAsCONNREFUSED = 10061,
		NetErrorAsLOOP = 10062,
		NetErrorAsNAMETOOLONG = 10063,
		NetErrorAsHOSTDOWN = 10064,
		NetErrorAsHOSTUNREACH = 10065,
		NetErrorAsNOTEMPTY = 10066,
		NetErrorAsPROCLIM = 10067,
		NetErrorAsUSERS = 10068,
		NetErrorAsDQUOT = 10069,
		NetErrorAsSTALE = 10070,
		NetErrorAsREMOTE = 10071,
		NetSYSNOTREADY = 10091,
		NetVERNOTSUPPORTED = 10092,
		NotInitializedSocketSystem = 10093,
		NetErrorAsDISCON = 10101,
		NetErrorAsNOMORE = 10102,
		NetErrorAsCANCELLED = 10103,
		NetErrorAsINVALIDPROCTABLE = 10104,
		NetErrorAsINVALIDPROVIDER = 10105,
		NetErrorAsPROVIDERFAILEDINIT = 10106,
		NetSYSCALLFAILURE = 10107,
		NetSERVICE_NOT_FOUND = 10108,
		NetTYPE_NOT_FOUND = 10109,
		E_NO_MORE = 10110,
		E_CANCELLED = 10111,
		NetErrorAsREFUSED = 10112,
		NetHOST_NOT_FOUND = 11001,
		NetTRY_AGAIN = 11002,
		NetNO_RECOVERY = 11003,
		NetNO_DATA = 11004,
		QOS_RECEIVERS = 11005,
		QOS_SENDERS = 11006,
		QOS_NO_SENDERS = 11007,
		QOS_NO_RECEIVERS = 11008,
		QOS_REQUEST_CONFIRMED = 11009,
		QOS_ADMISSION_FAILURE = 11010,
		QOS_POLICY_FAILURE = 11011,
		QOS_BAD_STYLE = 11012,
		QOS_BAD_OBJECT = 11013,
		QOS_TRAFFIC_CTRL_ERROR = 11014,
		QOS_GENERIC_ERROR = 11015,
		QOS_ESERVICETYPE = 11016,
		QOS_EFLOWSPEC = 11017,
		QOS_EPROVSPECBUF = 11018,
		QOS_EFILTERSTYLE = 11019,
		QOS_EFILTERTYPE = 11020,
		QOS_EFILTERCOUNT = 11021,
		QOS_EOBJLENGTH = 11022,
		QOS_EFLOWCOUNT = 11023,
		QOS_EUNKOWNPSOBJ = 11024,
		QOS_EPOLICYOBJ = 11025,
		QOS_EFLOWDESC = 11026,
		QOS_EPSFLOWSPEC = 11027,
		QOS_EPSFILTERSPEC = 11028,
		QOS_ESDMODEOBJ = 11029,
		QOS_ESHAPERATEOBJ = 11030,
		QOS_RESERVED_PETYPE = 11031,
		SECURE_HOST_NOT_FOUND = 11032,
		IPSEC_NAME_POLICY_ERROR = 11033,
	};

	[[nodiscard]] ErrorCode AcquireNetworkError() noexcept;
	[[nodiscard]] std::int32_t AcquireNetworkErrorByInteger() noexcept;
}

export namespace std
{
	[[nodiscard]]
	constexpr string to_string(iconer::net::ErrorCode code)
	{
		// [0-9a-zA-Z]*( = )([0-9a-zA-Z]*) -> find 'case'
		// ([^a-zA-Z]*)(\s=\s)([_\.]?[0-9a-zA-Z]*),$ -> find word with ':'
		// :$ -> change ':' to ': return "";'
		switch (code)
		{
		case iconer::net::ErrorCode::PendedIoOperation: return "PendedIoOperation";
		case iconer::net::ErrorCode::IO_INCOMPLETE: return "WSA_IO_INCOMPLETE";
		case iconer::net::ErrorCode::INVALID_HANDLE: return "WSA_INVALID_HANDLE";
		case iconer::net::ErrorCode::INVALID_PARAMETER: return "WSA_INVALID_PARAMETER";
		case iconer::net::ErrorCode::NOT_ENOUGH_MEMORY: return "WSA_NOT_ENOUGH_MEMORY";
		case iconer::net::ErrorCode::OPERATION_ABORTED: return "OPERATION_ABORTED";
		case iconer::net::ErrorCode::WAIT_EVENT_0: return "WSA_WAIT_EVENT_0";
		case iconer::net::ErrorCode::WAIT_IO_COMPLETION: return "WSA_WAIT_IO_COMPLETION";
		case iconer::net::ErrorCode::NetBASEERR: return "WSABASEERR";
		case iconer::net::ErrorCode::BlockingCallsInterrupted: return "BlockingCallsInterrupted";
		case iconer::net::ErrorCode::NetErrorAsBADF: return "WSAEBADF";
		case iconer::net::ErrorCode::NetErrorAsACCES: return "WSAEACCES";
		case iconer::net::ErrorCode::NetErrorAsFAULT: return "WSAEFAULT";
		case iconer::net::ErrorCode::NetErrorAsINVAL: return "WSAEINVAL";
		case iconer::net::ErrorCode::NetErrorAsMFILE: return "WSAEMFILE";
		case iconer::net::ErrorCode::NonBlockedOperation: return "NonBlockedOperation";
		case iconer::net::ErrorCode::YetInProgress: return "YetInProgress";
		case iconer::net::ErrorCode::NetErrorAsALREADY: return "WSAEALREADY";
		case iconer::net::ErrorCode::NotASocket: return "NotASocket";
		case iconer::net::ErrorCode::NetErrorAsDESTADDRREQ: return "WSAEDESTADDRREQ";
		case iconer::net::ErrorCode::NetErrorAsMSGSIZE: return "WSAEMSGSIZE";
		case iconer::net::ErrorCode::NetErrorAsPROTOTYPE: return "WSAEPROTOTYPE";
		case iconer::net::ErrorCode::NetErrorAsNOPROTOOPT: return "WSAENOPROTOOPT";
		case iconer::net::ErrorCode::NetErrorAsPROTONOSUPPORT: return "WSAEPROTONOSUPPORT";
		case iconer::net::ErrorCode::NetErrorAsSOCKTNOSUPPORT: return "WSAESOCKTNOSUPPORT";
		case iconer::net::ErrorCode::NoSupportOperation: return "NoSupportOperation";
		case iconer::net::ErrorCode::NetErrorAsPFNOSUPPORT: return "WSAEPFNOSUPPORT";
		case iconer::net::ErrorCode::NetErrorAsAFNOSUPPORT: return "WSAEAFNOSUPPORT";
		case iconer::net::ErrorCode::CannotUseThisAddress: return "CannotUseThisAddress";
		case iconer::net::ErrorCode::NetErrorAsADDRNOTAVAIL: return "WSAEADDRNOTAVAIL";
		case iconer::net::ErrorCode::NetworkIsDown: return "NetworkIsDown";
		case iconer::net::ErrorCode::NetErrorAsNETUNREACH: return "WSAENETUNREACH";
		case iconer::net::ErrorCode::NetErrorAsNETRESET: return "WSAENETRESET";
		case iconer::net::ErrorCode::ConnectionAborted: return "ConnectionAborted";
		case iconer::net::ErrorCode::ConnectionStateReset: return "ConnectionStateReset";
		case iconer::net::ErrorCode::NoBufferStorage: return "NoBufferStorage";
		case iconer::net::ErrorCode::NetErrorAsISCONN: return "WSAEISCONN";
		case iconer::net::ErrorCode::NotConnected: return "NotConnected";
		case iconer::net::ErrorCode::ClosedSocket: return "ClosedSocket";
		case iconer::net::ErrorCode::NetErrorAsTOOMANYREFS: return "WSAETOOMANYREFS";
		case iconer::net::ErrorCode::NetworkTimeOut: return "NetworkTimeOut";
		case iconer::net::ErrorCode::NetErrorAsCONNREFUSED: return "WSAECONNREFUSED";
		case iconer::net::ErrorCode::NetErrorAsLOOP: return "WSAELOOP";
		case iconer::net::ErrorCode::NetErrorAsNAMETOOLONG: return "WSAENAMETOOLONG";
		case iconer::net::ErrorCode::NetErrorAsHOSTDOWN: return "WSAEHOSTDOWN";
		case iconer::net::ErrorCode::NetErrorAsHOSTUNREACH: return "WSAEHOSTUNREACH";
		case iconer::net::ErrorCode::NetErrorAsNOTEMPTY: return "WSAENOTEMPTY";
		case iconer::net::ErrorCode::NetErrorAsPROCLIM: return "WSAEPROCLIM";
		case iconer::net::ErrorCode::NetErrorAsUSERS: return "WSAEUSERS";
		case iconer::net::ErrorCode::NetErrorAsDQUOT: return "WSAEDQUOT";
		case iconer::net::ErrorCode::NetErrorAsSTALE: return "WSAESTALE";
		case iconer::net::ErrorCode::NetErrorAsREMOTE: return "WSAEREMOTE";
		case iconer::net::ErrorCode::NetSYSNOTREADY: return "WSASYSNOTREADY";
		case iconer::net::ErrorCode::NetVERNOTSUPPORTED: return "WSAVERNOTSUPPORTED";
		case iconer::net::ErrorCode::NotInitializedSocketSystem: return "NotInitializedSocketSystem";
		case iconer::net::ErrorCode::NetErrorAsDISCON: return "WSAEDISCON";
		case iconer::net::ErrorCode::NetErrorAsNOMORE: return "WSAENOMORE";
		case iconer::net::ErrorCode::NetErrorAsCANCELLED: return "WSAECANCELLED";
		case iconer::net::ErrorCode::NetErrorAsINVALIDPROCTABLE: return "WSAEINVALIDPROCTABLE";
		case iconer::net::ErrorCode::NetErrorAsINVALIDPROVIDER: return "WSAEINVALIDPROVIDER";
		case iconer::net::ErrorCode::NetErrorAsPROVIDERFAILEDINIT: return "WSAEPROVIDERFAILEDINIT";
		case iconer::net::ErrorCode::NetSYSCALLFAILURE: return "WSASYSCALLFAILURE";
		case iconer::net::ErrorCode::NetSERVICE_NOT_FOUND: return "WSASERVICE_NOT_FOUND";
		case iconer::net::ErrorCode::NetTYPE_NOT_FOUND: return "WSATYPE_NOT_FOUND";
		case iconer::net::ErrorCode::E_NO_MORE: return "WSA_E_NO_MORE";
		case iconer::net::ErrorCode::E_CANCELLED: return "WSA_E_CANCELLED";
		case iconer::net::ErrorCode::NetErrorAsREFUSED: return "WSAEREFUSED";
		case iconer::net::ErrorCode::NetHOST_NOT_FOUND: return "WSAHOST_NOT_FOUND";
		case iconer::net::ErrorCode::NetTRY_AGAIN: return "WSATRY_AGAIN";
		case iconer::net::ErrorCode::NetNO_RECOVERY: return "WSANO_RECOVERY";
		case iconer::net::ErrorCode::NetNO_DATA: return "NetNO_DATA";
		case iconer::net::ErrorCode::QOS_RECEIVERS: return "WSA_QOS_RECEIVERS";
		case iconer::net::ErrorCode::QOS_SENDERS: return "WSA_QOS_SENDERS";
		case iconer::net::ErrorCode::QOS_NO_SENDERS: return "WSA_QOS_NO_SENDERS";
		case iconer::net::ErrorCode::QOS_NO_RECEIVERS: return "WSA_QOS_NO_RECEIVERS";
		case iconer::net::ErrorCode::QOS_REQUEST_CONFIRMED: return "WSA_QOS_REQUEST_CONFIRMED";
		case iconer::net::ErrorCode::QOS_ADMISSION_FAILURE: return "WSA_QOS_ADMISSION_FAILURE";
		case iconer::net::ErrorCode::QOS_POLICY_FAILURE: return "WSA_QOS_POLICY_FAILURE";
		case iconer::net::ErrorCode::QOS_BAD_STYLE: return "WSA_QOS_BAD_STYLE";
		case iconer::net::ErrorCode::QOS_BAD_OBJECT: return "WSA_QOS_BAD_OBJECT";
		case iconer::net::ErrorCode::QOS_TRAFFIC_CTRL_ERROR: return "WSA_QOS_TRAFFIC_CTRL_ERROR";
		case iconer::net::ErrorCode::QOS_GENERIC_ERROR: return "WSA_QOS_GENERIC_ERROR";
		case iconer::net::ErrorCode::QOS_ESERVICETYPE: return "WSA_QOS_ESERVICETYPE";
		case iconer::net::ErrorCode::QOS_EFLOWSPEC: return "WSA_QOS_EFLOWSPEC";
		case iconer::net::ErrorCode::QOS_EPROVSPECBUF: return "WSA_QOS_EPROVSPECBUF";
		case iconer::net::ErrorCode::QOS_EFILTERSTYLE: return "WSA_QOS_EFILTERSTYLE";
		case iconer::net::ErrorCode::QOS_EFILTERTYPE: return "WSA_QOS_EFILTERTYPE";
		case iconer::net::ErrorCode::QOS_EFILTERCOUNT: return "WSA_QOS_EFILTERCOUNT";
		case iconer::net::ErrorCode::QOS_EOBJLENGTH: return "WSA_QOS_EOBJLENGTH";
		case iconer::net::ErrorCode::QOS_EFLOWCOUNT: return "WSA_QOS_EFLOWCOUNT";
		case iconer::net::ErrorCode::QOS_EUNKOWNPSOBJ: return "WSA_QOS_EUNKOWNPSOBJ";
		case iconer::net::ErrorCode::QOS_EPOLICYOBJ: return "WSA_QOS_EPOLICYOBJ";
		case iconer::net::ErrorCode::QOS_EFLOWDESC: return "WSA_QOS_EFLOWDESC";
		case iconer::net::ErrorCode::QOS_EPSFLOWSPEC: return "WSA_QOS_EPSFLOWSPEC";
		case iconer::net::ErrorCode::QOS_EPSFILTERSPEC: return "WSA_QOS_EPSFILTERSPEC";
		case iconer::net::ErrorCode::QOS_ESDMODEOBJ: return "WSA_QOS_ESDMODEOBJ";
		case iconer::net::ErrorCode::QOS_ESHAPERATEOBJ: return "WSA_QOS_ESHAPERATEOBJ";
		case iconer::net::ErrorCode::QOS_RESERVED_PETYPE: return "WSA_QOS_RESERVED_PETYPE";
		case iconer::net::ErrorCode::SECURE_HOST_NOT_FOUND: return "WSA_SECURE_HOST_NOT_FOUND";
		case iconer::net::ErrorCode::IPSEC_NAME_POLICY_ERROR: return "WSA_IPSEC_NAME_POLICY_ERROR";
		default: return "Unknown";
		}
	}

	[[nodiscard]]
	constexpr wstring to_wstring(iconer::net::ErrorCode code)
	{
		switch (code)
		{
		case iconer::net::ErrorCode::PendedIoOperation: return L"PendedIoOperation";
		case iconer::net::ErrorCode::IO_INCOMPLETE: return L"WSA_IO_INCOMPLETE";
		case iconer::net::ErrorCode::INVALID_HANDLE: return L"WSA_INVALID_HANDLE";
		case iconer::net::ErrorCode::INVALID_PARAMETER: return L"WSA_INVALID_PARAMETER";
		case iconer::net::ErrorCode::NOT_ENOUGH_MEMORY: return L"WSA_NOT_ENOUGH_MEMORY";
		case iconer::net::ErrorCode::OPERATION_ABORTED: return L"OPERATION_ABORTED";
		case iconer::net::ErrorCode::WAIT_EVENT_0: return L"WSA_WAIT_EVENT_0";
		case iconer::net::ErrorCode::WAIT_IO_COMPLETION: return L"WSA_WAIT_IO_COMPLETION";
		case iconer::net::ErrorCode::NetBASEERR: return L"WSABASEERR";
		case iconer::net::ErrorCode::BlockingCallsInterrupted: return L"BlockingCallsInterrupted";
		case iconer::net::ErrorCode::NetErrorAsBADF: return L"WSAEBADF";
		case iconer::net::ErrorCode::NetErrorAsACCES: return L"WSAEACCES";
		case iconer::net::ErrorCode::NetErrorAsFAULT: return L"WSAEFAULT";
		case iconer::net::ErrorCode::NetErrorAsINVAL: return L"WSAEINVAL";
		case iconer::net::ErrorCode::NetErrorAsMFILE: return L"WSAEMFILE";
		case iconer::net::ErrorCode::NonBlockedOperation: return L"NonBlockedOperation";
		case iconer::net::ErrorCode::YetInProgress: return L"YetInProgress";
		case iconer::net::ErrorCode::NetErrorAsALREADY: return L"WSAEALREADY";
		case iconer::net::ErrorCode::NotASocket: return L"NotASocket";
		case iconer::net::ErrorCode::NetErrorAsDESTADDRREQ: return L"WSAEDESTADDRREQ";
		case iconer::net::ErrorCode::NetErrorAsMSGSIZE: return L"WSAEMSGSIZE";
		case iconer::net::ErrorCode::NetErrorAsPROTOTYPE: return L"WSAEPROTOTYPE";
		case iconer::net::ErrorCode::NetErrorAsNOPROTOOPT: return L"WSAENOPROTOOPT";
		case iconer::net::ErrorCode::NetErrorAsPROTONOSUPPORT: return L"WSAEPROTONOSUPPORT";
		case iconer::net::ErrorCode::NetErrorAsSOCKTNOSUPPORT: return L"WSAESOCKTNOSUPPORT";
		case iconer::net::ErrorCode::NoSupportOperation: return L"NoSupportOperation";
		case iconer::net::ErrorCode::NetErrorAsPFNOSUPPORT: return L"WSAEPFNOSUPPORT";
		case iconer::net::ErrorCode::NetErrorAsAFNOSUPPORT: return L"WSAEAFNOSUPPORT";
		case iconer::net::ErrorCode::CannotUseThisAddress: return L"CannotUseThisAddress";
		case iconer::net::ErrorCode::NetErrorAsADDRNOTAVAIL: return L"WSAEADDRNOTAVAIL";
		case iconer::net::ErrorCode::NetworkIsDown: return L"NetworkIsDown";
		case iconer::net::ErrorCode::NetErrorAsNETUNREACH: return L"WSAENETUNREACH";
		case iconer::net::ErrorCode::NetErrorAsNETRESET: return L"WSAENETRESET";
		case iconer::net::ErrorCode::ConnectionAborted: return L"ConnectionAborted";
		case iconer::net::ErrorCode::ConnectionStateReset: return L"ConnectionStateReset";
		case iconer::net::ErrorCode::NoBufferStorage: return L"NoBufferStorage";
		case iconer::net::ErrorCode::NetErrorAsISCONN: return L"WSAEISCONN";
		case iconer::net::ErrorCode::NotConnected: return L"NotConnected";
		case iconer::net::ErrorCode::ClosedSocket: return L"ClosedSocket";
		case iconer::net::ErrorCode::NetErrorAsTOOMANYREFS: return L"WSAETOOMANYREFS";
		case iconer::net::ErrorCode::NetworkTimeOut: return L"NetworkTimeOut";
		case iconer::net::ErrorCode::NetErrorAsCONNREFUSED: return L"WSAECONNREFUSED";
		case iconer::net::ErrorCode::NetErrorAsLOOP: return L"WSAELOOP";
		case iconer::net::ErrorCode::NetErrorAsNAMETOOLONG: return L"WSAENAMETOOLONG";
		case iconer::net::ErrorCode::NetErrorAsHOSTDOWN: return L"WSAEHOSTDOWN";
		case iconer::net::ErrorCode::NetErrorAsHOSTUNREACH: return L"WSAEHOSTUNREACH";
		case iconer::net::ErrorCode::NetErrorAsNOTEMPTY: return L"WSAENOTEMPTY";
		case iconer::net::ErrorCode::NetErrorAsPROCLIM: return L"WSAEPROCLIM";
		case iconer::net::ErrorCode::NetErrorAsUSERS: return L"WSAEUSERS";
		case iconer::net::ErrorCode::NetErrorAsDQUOT: return L"WSAEDQUOT";
		case iconer::net::ErrorCode::NetErrorAsSTALE: return L"WSAESTALE";
		case iconer::net::ErrorCode::NetErrorAsREMOTE: return L"WSAEREMOTE";
		case iconer::net::ErrorCode::NetSYSNOTREADY: return L"WSASYSNOTREADY";
		case iconer::net::ErrorCode::NetVERNOTSUPPORTED: return L"WSAVERNOTSUPPORTED";
		case iconer::net::ErrorCode::NotInitializedSocketSystem: return L"NotInitializedSocketSystem";
		case iconer::net::ErrorCode::NetErrorAsDISCON: return L"WSAEDISCON";
		case iconer::net::ErrorCode::NetErrorAsNOMORE: return L"WSAENOMORE";
		case iconer::net::ErrorCode::NetErrorAsCANCELLED: return L"WSAECANCELLED";
		case iconer::net::ErrorCode::NetErrorAsINVALIDPROCTABLE: return L"WSAEINVALIDPROCTABLE";
		case iconer::net::ErrorCode::NetErrorAsINVALIDPROVIDER: return L"WSAEINVALIDPROVIDER";
		case iconer::net::ErrorCode::NetErrorAsPROVIDERFAILEDINIT: return L"WSAEPROVIDERFAILEDINIT";
		case iconer::net::ErrorCode::NetSYSCALLFAILURE: return L"WSASYSCALLFAILURE";
		case iconer::net::ErrorCode::NetSERVICE_NOT_FOUND: return L"WSASERVICE_NOT_FOUND";
		case iconer::net::ErrorCode::NetTYPE_NOT_FOUND: return L"WSATYPE_NOT_FOUND";
		case iconer::net::ErrorCode::E_NO_MORE: return L"WSA_E_NO_MORE";
		case iconer::net::ErrorCode::E_CANCELLED: return L"WSA_E_CANCELLED";
		case iconer::net::ErrorCode::NetErrorAsREFUSED: return L"WSAEREFUSED";
		case iconer::net::ErrorCode::NetHOST_NOT_FOUND: return L"WSAHOST_NOT_FOUND";
		case iconer::net::ErrorCode::NetTRY_AGAIN: return L"WSATRY_AGAIN";
		case iconer::net::ErrorCode::NetNO_RECOVERY: return L"WSANO_RECOVERY";
		case iconer::net::ErrorCode::NetNO_DATA: return L"NetNO_DATA";
		case iconer::net::ErrorCode::QOS_RECEIVERS: return L"WSA_QOS_RECEIVERS";
		case iconer::net::ErrorCode::QOS_SENDERS: return L"WSA_QOS_SENDERS";
		case iconer::net::ErrorCode::QOS_NO_SENDERS: return L"WSA_QOS_NO_SENDERS";
		case iconer::net::ErrorCode::QOS_NO_RECEIVERS: return L"WSA_QOS_NO_RECEIVERS";
		case iconer::net::ErrorCode::QOS_REQUEST_CONFIRMED: return L"WSA_QOS_REQUEST_CONFIRMED";
		case iconer::net::ErrorCode::QOS_ADMISSION_FAILURE: return L"WSA_QOS_ADMISSION_FAILURE";
		case iconer::net::ErrorCode::QOS_POLICY_FAILURE: return L"WSA_QOS_POLICY_FAILURE";
		case iconer::net::ErrorCode::QOS_BAD_STYLE: return L"WSA_QOS_BAD_STYLE";
		case iconer::net::ErrorCode::QOS_BAD_OBJECT: return L"WSA_QOS_BAD_OBJECT";
		case iconer::net::ErrorCode::QOS_TRAFFIC_CTRL_ERROR: return L"WSA_QOS_TRAFFIC_CTRL_ERROR";
		case iconer::net::ErrorCode::QOS_GENERIC_ERROR: return L"WSA_QOS_GENERIC_ERROR";
		case iconer::net::ErrorCode::QOS_ESERVICETYPE: return L"WSA_QOS_ESERVICETYPE";
		case iconer::net::ErrorCode::QOS_EFLOWSPEC: return L"WSA_QOS_EFLOWSPEC";
		case iconer::net::ErrorCode::QOS_EPROVSPECBUF: return L"WSA_QOS_EPROVSPECBUF";
		case iconer::net::ErrorCode::QOS_EFILTERSTYLE: return L"WSA_QOS_EFILTERSTYLE";
		case iconer::net::ErrorCode::QOS_EFILTERTYPE: return L"WSA_QOS_EFILTERTYPE";
		case iconer::net::ErrorCode::QOS_EFILTERCOUNT: return L"WSA_QOS_EFILTERCOUNT";
		case iconer::net::ErrorCode::QOS_EOBJLENGTH: return L"WSA_QOS_EOBJLENGTH";
		case iconer::net::ErrorCode::QOS_EFLOWCOUNT: return L"WSA_QOS_EFLOWCOUNT";
		case iconer::net::ErrorCode::QOS_EUNKOWNPSOBJ: return L"WSA_QOS_EUNKOWNPSOBJ";
		case iconer::net::ErrorCode::QOS_EPOLICYOBJ: return L"WSA_QOS_EPOLICYOBJ";
		case iconer::net::ErrorCode::QOS_EFLOWDESC: return L"WSA_QOS_EFLOWDESC";
		case iconer::net::ErrorCode::QOS_EPSFLOWSPEC: return L"WSA_QOS_EPSFLOWSPEC";
		case iconer::net::ErrorCode::QOS_EPSFILTERSPEC: return L"WSA_QOS_EPSFILTERSPEC";
		case iconer::net::ErrorCode::QOS_ESDMODEOBJ: return L"WSA_QOS_ESDMODEOBJ";
		case iconer::net::ErrorCode::QOS_ESHAPERATEOBJ: return L"WSA_QOS_ESHAPERATEOBJ";
		case iconer::net::ErrorCode::QOS_RESERVED_PETYPE: return L"WSA_QOS_RESERVED_PETYPE";
		case iconer::net::ErrorCode::SECURE_HOST_NOT_FOUND: return L"WSA_SECURE_HOST_NOT_FOUND";
		case iconer::net::ErrorCode::IPSEC_NAME_POLICY_ERROR: return L"WSA_IPSEC_NAME_POLICY_ERROR";
		default: return L"Unknown";
		}
	}
}
