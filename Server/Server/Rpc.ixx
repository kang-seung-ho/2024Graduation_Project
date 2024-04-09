export module Iconer.Application.Rpc;
export import <cstddef>;
export import <memory>;

export namespace iconer::app
{
	struct [[nodiscard]] SerializedRpcPacket
	{
		std::unique_ptr<std::byte[]> buffer;
	};
}
