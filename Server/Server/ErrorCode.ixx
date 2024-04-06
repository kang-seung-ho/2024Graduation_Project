export module Iconer.Net.ErrorCode;
import <cstdint>;
import <string>;
import <format>;

export namespace iconer::net
{
	enum class [[nodiscard]] ErrorCode : std::int32_t
	{
		PendedIoOperation = 997,
		WSA_IO_INCOMPLETE = 996,
		WSA_INVALID_HANDLE = 6,
		WSA_INVALID_PARAMETER = 87,
		WSA_NOT_ENOUGH_MEMORY = 8,
		OPERATION_ABORTED = 995,
		WSA_WAIT_EVENT_0 = 0,
		WSA_WAIT_IO_COMPLETION = 192,
		WSABASEERR = 10000,
		BlockingCallsInterrupted = 10004,
		WSAEBADF = 10009,
		WSAEACCES = 10013,
		WSAEFAULT = 10014,
		WSAEINVAL = 10022,
		WSAEMFILE = 10024,
		NonBlockedOperation = 10035, // EWOULDBLOCK
		YetInProgress = 10036,
		WSAEALREADY = 10037,
		NotASocket = 10038,
		WSAEDESTADDRREQ = 10039,
		WSAEMSGSIZE = 10040,
		WSAEPROTOTYPE = 10041,
		WSAENOPROTOOPT = 10042,
		WSAEPROTONOSUPPORT = 10043,
		WSAESOCKTNOSUPPORT = 10044,
		NoSupportOperation = 10045,
		WSAEPFNOSUPPORT = 10046,
		WSAEAFNOSUPPORT = 10047,
		CannotUseThisAddress = 10048,
		WSAEADDRNOTAVAIL = 10049,
		NetworkIsDown = 10050,
		WSAENETUNREACH = 10051,
		WSAENETRESET = 10052,
		ConnectionAborted = 10053,
		ConnectionStateReset = 10054, // Disconneting
		NoBufferStorage = 10055,
		WSAEISCONN = 10056,
		NotConnected = 10057,
		ClosedSocket = 10058,
		WSAETOOMANYREFS = 10059,
		NetworkTimeOut = 10060,
		WSAECONNREFUSED = 10061,
		WSAELOOP = 10062,
		WSAENAMETOOLONG = 10063,
		WSAEHOSTDOWN = 10064,
		WSAEHOSTUNREACH = 10065,
		WSAENOTEMPTY = 10066,
		WSAEPROCLIM = 10067,
		WSAEUSERS = 10068,
		WSAEDQUOT = 10069,
		WSAESTALE = 10070,
		WSAEREMOTE = 10071,
		WSASYSNOTREADY = 10091,
		WSAVERNOTSUPPORTED = 10092,
		NotInitializedSocketSystem = 10093,
		WSAEDISCON = 10101,
		WSAENOMORE = 10102,
		WSAECANCELLED = 10103,
		WSAEINVALIDPROCTABLE = 10104,
		WSAEINVALIDPROVIDER = 10105,
		WSAEPROVIDERFAILEDINIT = 10106,
		WSASYSCALLFAILURE = 10107,
		WSASERVICE_NOT_FOUND = 10108,
		WSATYPE_NOT_FOUND = 10109,
		WSA_E_NO_MORE = 10110,
		WSA_E_CANCELLED = 10111,
		WSAEREFUSED = 10112,
		WSAHOST_NOT_FOUND = 11001,
		WSATRY_AGAIN = 11002,
		WSANO_RECOVERY = 11003,
		WSANO_DATA = 11004,
		WSA_QOS_RECEIVERS = 11005,
		WSA_QOS_SENDERS = 11006,
		WSA_QOS_NO_SENDERS = 11007,
		WSA_QOS_NO_RECEIVERS = 11008,
		WSA_QOS_REQUEST_CONFIRMED = 11009,
		WSA_QOS_ADMISSION_FAILURE = 11010,
		WSA_QOS_POLICY_FAILURE = 11011,
		WSA_QOS_BAD_STYLE = 11012,
		WSA_QOS_BAD_OBJECT = 11013,
		WSA_QOS_TRAFFIC_CTRL_ERROR = 11014,
		WSA_QOS_GENERIC_ERROR = 11015,
		WSA_QOS_ESERVICETYPE = 11016,
		WSA_QOS_EFLOWSPEC = 11017,
		WSA_QOS_EPROVSPECBUF = 11018,
		WSA_QOS_EFILTERSTYLE = 11019,
		WSA_QOS_EFILTERTYPE = 11020,
		WSA_QOS_EFILTERCOUNT = 11021,
		WSA_QOS_EOBJLENGTH = 11022,
		WSA_QOS_EFLOWCOUNT = 11023,
		WSA_QOS_EUNKOWNPSOBJ = 11024,
		WSA_QOS_EPOLICYOBJ = 11025,
		WSA_QOS_EFLOWDESC = 11026,
		WSA_QOS_EPSFLOWSPEC = 11027,
		WSA_QOS_EPSFILTERSPEC = 11028,
		WSA_QOS_ESDMODEOBJ = 11029,
		WSA_QOS_ESHAPERATEOBJ = 11030,
		WSA_QOS_RESERVED_PETYPE = 11031,
		WSA_SECURE_HOST_NOT_FOUND = 11032,
		WSA_IPSEC_NAME_POLICY_ERROR = 11033,
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
			case iconer::net::ErrorCode::WSA_IO_INCOMPLETE: return "WSA_IO_INCOMPLETE";
			case iconer::net::ErrorCode::WSA_INVALID_HANDLE: return "WSA_INVALID_HANDLE";
			case iconer::net::ErrorCode::WSA_INVALID_PARAMETER: return "WSA_INVALID_PARAMETER";
			case iconer::net::ErrorCode::WSA_NOT_ENOUGH_MEMORY: return "WSA_NOT_ENOUGH_MEMORY";
			case iconer::net::ErrorCode::OPERATION_ABORTED: return "OPERATION_ABORTED";
			case iconer::net::ErrorCode::WSA_WAIT_EVENT_0: return "WSA_WAIT_EVENT_0";
			case iconer::net::ErrorCode::WSA_WAIT_IO_COMPLETION: return "WSA_WAIT_IO_COMPLETION";
			case iconer::net::ErrorCode::WSABASEERR: return "WSABASEERR";
			case iconer::net::ErrorCode::BlockingCallsInterrupted: return "BlockingCallsInterrupted";
			case iconer::net::ErrorCode::WSAEBADF: return "WSAEBADF";
			case iconer::net::ErrorCode::WSAEACCES: return "WSAEACCES";
			case iconer::net::ErrorCode::WSAEFAULT: return "WSAEFAULT";
			case iconer::net::ErrorCode::WSAEINVAL: return "WSAEINVAL";
			case iconer::net::ErrorCode::WSAEMFILE: return "WSAEMFILE";
			case iconer::net::ErrorCode::NonBlockedOperation: return "NonBlockedOperation";
			case iconer::net::ErrorCode::YetInProgress: return "YetInProgress";
			case iconer::net::ErrorCode::WSAEALREADY: return "WSAEALREADY";
			case iconer::net::ErrorCode::NotASocket: return "NotASocket";
			case iconer::net::ErrorCode::WSAEDESTADDRREQ: return "WSAEDESTADDRREQ";
			case iconer::net::ErrorCode::WSAEMSGSIZE: return "WSAEMSGSIZE";
			case iconer::net::ErrorCode::WSAEPROTOTYPE: return "WSAEPROTOTYPE";
			case iconer::net::ErrorCode::WSAENOPROTOOPT: return "WSAENOPROTOOPT";
			case iconer::net::ErrorCode::WSAEPROTONOSUPPORT: return "WSAEPROTONOSUPPORT";
			case iconer::net::ErrorCode::WSAESOCKTNOSUPPORT: return "WSAESOCKTNOSUPPORT";
			case iconer::net::ErrorCode::NoSupportOperation: return "NoSupportOperation";
			case iconer::net::ErrorCode::WSAEPFNOSUPPORT: return "WSAEPFNOSUPPORT";
			case iconer::net::ErrorCode::WSAEAFNOSUPPORT: return "WSAEAFNOSUPPORT";
			case iconer::net::ErrorCode::CannotUseThisAddress: return "CannotUseThisAddress";
			case iconer::net::ErrorCode::WSAEADDRNOTAVAIL: return "WSAEADDRNOTAVAIL";
			case iconer::net::ErrorCode::NetworkIsDown: return "NetworkIsDown";
			case iconer::net::ErrorCode::WSAENETUNREACH: return "WSAENETUNREACH";
			case iconer::net::ErrorCode::WSAENETRESET: return "WSAENETRESET";
			case iconer::net::ErrorCode::ConnectionAborted: return "ConnectionAborted";
			case iconer::net::ErrorCode::ConnectionStateReset: return "ConnectionStateReset";
			case iconer::net::ErrorCode::NoBufferStorage: return "NoBufferStorage";
			case iconer::net::ErrorCode::WSAEISCONN: return "WSAEISCONN";
			case iconer::net::ErrorCode::NotConnected: return "NotConnected";
			case iconer::net::ErrorCode::ClosedSocket: return "ClosedSocket";
			case iconer::net::ErrorCode::WSAETOOMANYREFS: return "WSAETOOMANYREFS";
			case iconer::net::ErrorCode::NetworkTimeOut: return "NetworkTimeOut";
			case iconer::net::ErrorCode::WSAECONNREFUSED: return "WSAECONNREFUSED";
			case iconer::net::ErrorCode::WSAELOOP: return "WSAELOOP";
			case iconer::net::ErrorCode::WSAENAMETOOLONG: return "WSAENAMETOOLONG";
			case iconer::net::ErrorCode::WSAEHOSTDOWN: return "WSAEHOSTDOWN";
			case iconer::net::ErrorCode::WSAEHOSTUNREACH: return "WSAEHOSTUNREACH";
			case iconer::net::ErrorCode::WSAENOTEMPTY: return "WSAENOTEMPTY";
			case iconer::net::ErrorCode::WSAEPROCLIM: return "WSAEPROCLIM";
			case iconer::net::ErrorCode::WSAEUSERS: return "WSAEUSERS";
			case iconer::net::ErrorCode::WSAEDQUOT: return "WSAEDQUOT";
			case iconer::net::ErrorCode::WSAESTALE: return "WSAESTALE";
			case iconer::net::ErrorCode::WSAEREMOTE: return "WSAEREMOTE";
			case iconer::net::ErrorCode::WSASYSNOTREADY: return "WSASYSNOTREADY";
			case iconer::net::ErrorCode::WSAVERNOTSUPPORTED: return "WSAVERNOTSUPPORTED";
			case iconer::net::ErrorCode::NotInitializedSocketSystem: return "NotInitializedSocketSystem";
			case iconer::net::ErrorCode::WSAEDISCON: return "WSAEDISCON";
			case iconer::net::ErrorCode::WSAENOMORE: return "WSAENOMORE";
			case iconer::net::ErrorCode::WSAECANCELLED: return "WSAECANCELLED";
			case iconer::net::ErrorCode::WSAEINVALIDPROCTABLE: return "WSAEINVALIDPROCTABLE";
			case iconer::net::ErrorCode::WSAEINVALIDPROVIDER: return "WSAEINVALIDPROVIDER";
			case iconer::net::ErrorCode::WSAEPROVIDERFAILEDINIT: return "WSAEPROVIDERFAILEDINIT";
			case iconer::net::ErrorCode::WSASYSCALLFAILURE: return "WSASYSCALLFAILURE";
			case iconer::net::ErrorCode::WSASERVICE_NOT_FOUND: return "WSASERVICE_NOT_FOUND";
			case iconer::net::ErrorCode::WSATYPE_NOT_FOUND: return "WSATYPE_NOT_FOUND";
			case iconer::net::ErrorCode::WSA_E_NO_MORE: return "WSA_E_NO_MORE";
			case iconer::net::ErrorCode::WSA_E_CANCELLED: return "WSA_E_CANCELLED";
			case iconer::net::ErrorCode::WSAEREFUSED: return "WSAEREFUSED";
			case iconer::net::ErrorCode::WSAHOST_NOT_FOUND: return "WSAHOST_NOT_FOUND";
			case iconer::net::ErrorCode::WSATRY_AGAIN: return "WSATRY_AGAIN";
			case iconer::net::ErrorCode::WSANO_RECOVERY: return "WSANO_RECOVERY";
			case iconer::net::ErrorCode::WSANO_DATA: return "WSANO_DATA";
			case iconer::net::ErrorCode::WSA_QOS_RECEIVERS: return "WSA_QOS_RECEIVERS";
			case iconer::net::ErrorCode::WSA_QOS_SENDERS: return "WSA_QOS_SENDERS";
			case iconer::net::ErrorCode::WSA_QOS_NO_SENDERS: return "WSA_QOS_NO_SENDERS";
			case iconer::net::ErrorCode::WSA_QOS_NO_RECEIVERS: return "WSA_QOS_NO_RECEIVERS";
			case iconer::net::ErrorCode::WSA_QOS_REQUEST_CONFIRMED: return "WSA_QOS_REQUEST_CONFIRMED";
			case iconer::net::ErrorCode::WSA_QOS_ADMISSION_FAILURE: return "WSA_QOS_ADMISSION_FAILURE";
			case iconer::net::ErrorCode::WSA_QOS_POLICY_FAILURE: return "WSA_QOS_POLICY_FAILURE";
			case iconer::net::ErrorCode::WSA_QOS_BAD_STYLE: return "WSA_QOS_BAD_STYLE";
			case iconer::net::ErrorCode::WSA_QOS_BAD_OBJECT: return "WSA_QOS_BAD_OBJECT";
			case iconer::net::ErrorCode::WSA_QOS_TRAFFIC_CTRL_ERROR: return "WSA_QOS_TRAFFIC_CTRL_ERROR";
			case iconer::net::ErrorCode::WSA_QOS_GENERIC_ERROR: return "WSA_QOS_GENERIC_ERROR";
			case iconer::net::ErrorCode::WSA_QOS_ESERVICETYPE: return "WSA_QOS_ESERVICETYPE";
			case iconer::net::ErrorCode::WSA_QOS_EFLOWSPEC: return "WSA_QOS_EFLOWSPEC";
			case iconer::net::ErrorCode::WSA_QOS_EPROVSPECBUF: return "WSA_QOS_EPROVSPECBUF";
			case iconer::net::ErrorCode::WSA_QOS_EFILTERSTYLE: return "WSA_QOS_EFILTERSTYLE";
			case iconer::net::ErrorCode::WSA_QOS_EFILTERTYPE: return "WSA_QOS_EFILTERTYPE";
			case iconer::net::ErrorCode::WSA_QOS_EFILTERCOUNT: return "WSA_QOS_EFILTERCOUNT";
			case iconer::net::ErrorCode::WSA_QOS_EOBJLENGTH: return "WSA_QOS_EOBJLENGTH";
			case iconer::net::ErrorCode::WSA_QOS_EFLOWCOUNT: return "WSA_QOS_EFLOWCOUNT";
			case iconer::net::ErrorCode::WSA_QOS_EUNKOWNPSOBJ: return "WSA_QOS_EUNKOWNPSOBJ";
			case iconer::net::ErrorCode::WSA_QOS_EPOLICYOBJ: return "WSA_QOS_EPOLICYOBJ";
			case iconer::net::ErrorCode::WSA_QOS_EFLOWDESC: return "WSA_QOS_EFLOWDESC";
			case iconer::net::ErrorCode::WSA_QOS_EPSFLOWSPEC: return "WSA_QOS_EPSFLOWSPEC";
			case iconer::net::ErrorCode::WSA_QOS_EPSFILTERSPEC: return "WSA_QOS_EPSFILTERSPEC";
			case iconer::net::ErrorCode::WSA_QOS_ESDMODEOBJ: return "WSA_QOS_ESDMODEOBJ";
			case iconer::net::ErrorCode::WSA_QOS_ESHAPERATEOBJ: return "WSA_QOS_ESHAPERATEOBJ";
			case iconer::net::ErrorCode::WSA_QOS_RESERVED_PETYPE: return "WSA_QOS_RESERVED_PETYPE";
			case iconer::net::ErrorCode::WSA_SECURE_HOST_NOT_FOUND: return "WSA_SECURE_HOST_NOT_FOUND";
			case iconer::net::ErrorCode::WSA_IPSEC_NAME_POLICY_ERROR: return "WSA_IPSEC_NAME_POLICY_ERROR";
			default: return "Unknown";
		}
	}

	[[nodiscard]]
	constexpr wstring to_wstring(iconer::net::ErrorCode code)
	{
		switch (code)
		{
			case iconer::net::ErrorCode::PendedIoOperation: return L"PendedIoOperation";
			case iconer::net::ErrorCode::WSA_IO_INCOMPLETE: return L"WSA_IO_INCOMPLETE";
			case iconer::net::ErrorCode::WSA_INVALID_HANDLE: return L"WSA_INVALID_HANDLE";
			case iconer::net::ErrorCode::WSA_INVALID_PARAMETER: return L"WSA_INVALID_PARAMETER";
			case iconer::net::ErrorCode::WSA_NOT_ENOUGH_MEMORY: return L"WSA_NOT_ENOUGH_MEMORY";
			case iconer::net::ErrorCode::OPERATION_ABORTED: return L"OPERATION_ABORTED";
			case iconer::net::ErrorCode::WSA_WAIT_EVENT_0: return L"WSA_WAIT_EVENT_0";
			case iconer::net::ErrorCode::WSA_WAIT_IO_COMPLETION: return L"WSA_WAIT_IO_COMPLETION";
			case iconer::net::ErrorCode::WSABASEERR: return L"WSABASEERR";
			case iconer::net::ErrorCode::BlockingCallsInterrupted: return L"BlockingCallsInterrupted";
			case iconer::net::ErrorCode::WSAEBADF: return L"WSAEBADF";
			case iconer::net::ErrorCode::WSAEACCES: return L"WSAEACCES";
			case iconer::net::ErrorCode::WSAEFAULT: return L"WSAEFAULT";
			case iconer::net::ErrorCode::WSAEINVAL: return L"WSAEINVAL";
			case iconer::net::ErrorCode::WSAEMFILE: return L"WSAEMFILE";
			case iconer::net::ErrorCode::NonBlockedOperation: return L"NonBlockedOperation";
			case iconer::net::ErrorCode::YetInProgress: return L"YetInProgress";
			case iconer::net::ErrorCode::WSAEALREADY: return L"WSAEALREADY";
			case iconer::net::ErrorCode::NotASocket: return L"NotASocket";
			case iconer::net::ErrorCode::WSAEDESTADDRREQ: return L"WSAEDESTADDRREQ";
			case iconer::net::ErrorCode::WSAEMSGSIZE: return L"WSAEMSGSIZE";
			case iconer::net::ErrorCode::WSAEPROTOTYPE: return L"WSAEPROTOTYPE";
			case iconer::net::ErrorCode::WSAENOPROTOOPT: return L"WSAENOPROTOOPT";
			case iconer::net::ErrorCode::WSAEPROTONOSUPPORT: return L"WSAEPROTONOSUPPORT";
			case iconer::net::ErrorCode::WSAESOCKTNOSUPPORT: return L"WSAESOCKTNOSUPPORT";
			case iconer::net::ErrorCode::NoSupportOperation: return L"NoSupportOperation";
			case iconer::net::ErrorCode::WSAEPFNOSUPPORT: return L"WSAEPFNOSUPPORT";
			case iconer::net::ErrorCode::WSAEAFNOSUPPORT: return L"WSAEAFNOSUPPORT";
			case iconer::net::ErrorCode::CannotUseThisAddress: return L"CannotUseThisAddress";
			case iconer::net::ErrorCode::WSAEADDRNOTAVAIL: return L"WSAEADDRNOTAVAIL";
			case iconer::net::ErrorCode::NetworkIsDown: return L"NetworkIsDown";
			case iconer::net::ErrorCode::WSAENETUNREACH: return L"WSAENETUNREACH";
			case iconer::net::ErrorCode::WSAENETRESET: return L"WSAENETRESET";
			case iconer::net::ErrorCode::ConnectionAborted: return L"ConnectionAborted";
			case iconer::net::ErrorCode::ConnectionStateReset: return L"ConnectionStateReset";
			case iconer::net::ErrorCode::NoBufferStorage: return L"NoBufferStorage";
			case iconer::net::ErrorCode::WSAEISCONN: return L"WSAEISCONN";
			case iconer::net::ErrorCode::NotConnected: return L"NotConnected";
			case iconer::net::ErrorCode::ClosedSocket: return L"ClosedSocket";
			case iconer::net::ErrorCode::WSAETOOMANYREFS: return L"WSAETOOMANYREFS";
			case iconer::net::ErrorCode::NetworkTimeOut: return L"NetworkTimeOut";
			case iconer::net::ErrorCode::WSAECONNREFUSED: return L"WSAECONNREFUSED";
			case iconer::net::ErrorCode::WSAELOOP: return L"WSAELOOP";
			case iconer::net::ErrorCode::WSAENAMETOOLONG: return L"WSAENAMETOOLONG";
			case iconer::net::ErrorCode::WSAEHOSTDOWN: return L"WSAEHOSTDOWN";
			case iconer::net::ErrorCode::WSAEHOSTUNREACH: return L"WSAEHOSTUNREACH";
			case iconer::net::ErrorCode::WSAENOTEMPTY: return L"WSAENOTEMPTY";
			case iconer::net::ErrorCode::WSAEPROCLIM: return L"WSAEPROCLIM";
			case iconer::net::ErrorCode::WSAEUSERS: return L"WSAEUSERS";
			case iconer::net::ErrorCode::WSAEDQUOT: return L"WSAEDQUOT";
			case iconer::net::ErrorCode::WSAESTALE: return L"WSAESTALE";
			case iconer::net::ErrorCode::WSAEREMOTE: return L"WSAEREMOTE";
			case iconer::net::ErrorCode::WSASYSNOTREADY: return L"WSASYSNOTREADY";
			case iconer::net::ErrorCode::WSAVERNOTSUPPORTED: return L"WSAVERNOTSUPPORTED";
			case iconer::net::ErrorCode::NotInitializedSocketSystem: return L"NotInitializedSocketSystem";
			case iconer::net::ErrorCode::WSAEDISCON: return L"WSAEDISCON";
			case iconer::net::ErrorCode::WSAENOMORE: return L"WSAENOMORE";
			case iconer::net::ErrorCode::WSAECANCELLED: return L"WSAECANCELLED";
			case iconer::net::ErrorCode::WSAEINVALIDPROCTABLE: return L"WSAEINVALIDPROCTABLE";
			case iconer::net::ErrorCode::WSAEINVALIDPROVIDER: return L"WSAEINVALIDPROVIDER";
			case iconer::net::ErrorCode::WSAEPROVIDERFAILEDINIT: return L"WSAEPROVIDERFAILEDINIT";
			case iconer::net::ErrorCode::WSASYSCALLFAILURE: return L"WSASYSCALLFAILURE";
			case iconer::net::ErrorCode::WSASERVICE_NOT_FOUND: return L"WSASERVICE_NOT_FOUND";
			case iconer::net::ErrorCode::WSATYPE_NOT_FOUND: return L"WSATYPE_NOT_FOUND";
			case iconer::net::ErrorCode::WSA_E_NO_MORE: return L"WSA_E_NO_MORE";
			case iconer::net::ErrorCode::WSA_E_CANCELLED: return L"WSA_E_CANCELLED";
			case iconer::net::ErrorCode::WSAEREFUSED: return L"WSAEREFUSED";
			case iconer::net::ErrorCode::WSAHOST_NOT_FOUND: return L"WSAHOST_NOT_FOUND";
			case iconer::net::ErrorCode::WSATRY_AGAIN: return L"WSATRY_AGAIN";
			case iconer::net::ErrorCode::WSANO_RECOVERY: return L"WSANO_RECOVERY";
			case iconer::net::ErrorCode::WSANO_DATA: return L"WSANO_DATA";
			case iconer::net::ErrorCode::WSA_QOS_RECEIVERS: return L"WSA_QOS_RECEIVERS";
			case iconer::net::ErrorCode::WSA_QOS_SENDERS: return L"WSA_QOS_SENDERS";
			case iconer::net::ErrorCode::WSA_QOS_NO_SENDERS: return L"WSA_QOS_NO_SENDERS";
			case iconer::net::ErrorCode::WSA_QOS_NO_RECEIVERS: return L"WSA_QOS_NO_RECEIVERS";
			case iconer::net::ErrorCode::WSA_QOS_REQUEST_CONFIRMED: return L"WSA_QOS_REQUEST_CONFIRMED";
			case iconer::net::ErrorCode::WSA_QOS_ADMISSION_FAILURE: return L"WSA_QOS_ADMISSION_FAILURE";
			case iconer::net::ErrorCode::WSA_QOS_POLICY_FAILURE: return L"WSA_QOS_POLICY_FAILURE";
			case iconer::net::ErrorCode::WSA_QOS_BAD_STYLE: return L"WSA_QOS_BAD_STYLE";
			case iconer::net::ErrorCode::WSA_QOS_BAD_OBJECT: return L"WSA_QOS_BAD_OBJECT";
			case iconer::net::ErrorCode::WSA_QOS_TRAFFIC_CTRL_ERROR: return L"WSA_QOS_TRAFFIC_CTRL_ERROR";
			case iconer::net::ErrorCode::WSA_QOS_GENERIC_ERROR: return L"WSA_QOS_GENERIC_ERROR";
			case iconer::net::ErrorCode::WSA_QOS_ESERVICETYPE: return L"WSA_QOS_ESERVICETYPE";
			case iconer::net::ErrorCode::WSA_QOS_EFLOWSPEC: return L"WSA_QOS_EFLOWSPEC";
			case iconer::net::ErrorCode::WSA_QOS_EPROVSPECBUF: return L"WSA_QOS_EPROVSPECBUF";
			case iconer::net::ErrorCode::WSA_QOS_EFILTERSTYLE: return L"WSA_QOS_EFILTERSTYLE";
			case iconer::net::ErrorCode::WSA_QOS_EFILTERTYPE: return L"WSA_QOS_EFILTERTYPE";
			case iconer::net::ErrorCode::WSA_QOS_EFILTERCOUNT: return L"WSA_QOS_EFILTERCOUNT";
			case iconer::net::ErrorCode::WSA_QOS_EOBJLENGTH: return L"WSA_QOS_EOBJLENGTH";
			case iconer::net::ErrorCode::WSA_QOS_EFLOWCOUNT: return L"WSA_QOS_EFLOWCOUNT";
			case iconer::net::ErrorCode::WSA_QOS_EUNKOWNPSOBJ: return L"WSA_QOS_EUNKOWNPSOBJ";
			case iconer::net::ErrorCode::WSA_QOS_EPOLICYOBJ: return L"WSA_QOS_EPOLICYOBJ";
			case iconer::net::ErrorCode::WSA_QOS_EFLOWDESC: return L"WSA_QOS_EFLOWDESC";
			case iconer::net::ErrorCode::WSA_QOS_EPSFLOWSPEC: return L"WSA_QOS_EPSFLOWSPEC";
			case iconer::net::ErrorCode::WSA_QOS_EPSFILTERSPEC: return L"WSA_QOS_EPSFILTERSPEC";
			case iconer::net::ErrorCode::WSA_QOS_ESDMODEOBJ: return L"WSA_QOS_ESDMODEOBJ";
			case iconer::net::ErrorCode::WSA_QOS_ESHAPERATEOBJ: return L"WSA_QOS_ESHAPERATEOBJ";
			case iconer::net::ErrorCode::WSA_QOS_RESERVED_PETYPE: return L"WSA_QOS_RESERVED_PETYPE";
			case iconer::net::ErrorCode::WSA_SECURE_HOST_NOT_FOUND: return L"WSA_SECURE_HOST_NOT_FOUND";
			case iconer::net::ErrorCode::WSA_IPSEC_NAME_POLICY_ERROR: return L"WSA_IPSEC_NAME_POLICY_ERROR";
			default: return L"Unknown";
		}
	}
}

export template<>
struct std::formatter<iconer::net::ErrorCode, char>
{
	static constexpr
		format_parse_context::iterator
		parse(format_parse_context& context)
	{
		return context.begin();
	}

	static format_context::iterator
		format(iconer::net::ErrorCode code, format_context& context)
	{
		return std::format_to(context.out(), "{}", to_string(code));
	}
};

export template<>
struct std::formatter<iconer::net::ErrorCode, wchar_t>
{
	static constexpr
		wformat_parse_context::iterator
		parse(wformat_parse_context& context)
	{
		return context.begin();
	}

	static wformat_context::iterator
		format(iconer::net::ErrorCode code, wformat_context& context)
	{
		return std::format_to(context.out(), L"{}", to_wstring(code));
	}
};
