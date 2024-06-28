export module Iconer.Net.IoResult;
export import Iconer.Utility.Expected;
export import Iconer.Net.ErrorCode;

export namespace iconer::net
{
	using IoResult = iconer::util::Expected<void, iconer::net::ErrorCode>;
}
