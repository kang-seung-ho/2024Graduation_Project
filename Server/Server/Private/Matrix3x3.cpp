module;
#include <DirectXMath.h>
module Iconer.Utility.Matrix3x3;

float
iconer::util::Matrix3x3::GetDeterminantImplementation()
const noexcept
{
	const DirectX::XMFLOAT4X4 mat
	{
		scalar[0], scalar[1], scalar[2], 0,
		scalar[3], scalar[4], scalar[5], 0,
		scalar[6], scalar[7], scalar[8], 0,
				0,         0,         0, 0,
	};

	DirectX::XMFLOAT3 result{};
	auto concurrent_mat = DirectX::XMLoadFloat4x4(std::addressof(mat));
	DirectX::XMStoreFloat3(std::addressof(result), DirectX::XMMatrixDeterminant(concurrent_mat));

	return result.x;
}
