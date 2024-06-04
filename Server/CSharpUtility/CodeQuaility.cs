using System.Diagnostics.CodeAnalysis;

namespace Iconer.Utiliy
{
	static class CodeQuaility
	{
		static internal void Assert(bool condition, in string msg = "Error")
		{
			if (!condition)
			{
				throw new Exception(msg);
			}
		}
	}
}
