export module Iconer.Utility.Property;
import Iconer.Utility.Constraints;
import :IProperty;

export namespace iconer::util
{
	template<notvoids T>
	using Property = IProperty<T, void>;

	template<notvoids T, notvoids Predicate>
	using CustomProperty = IProperty<T, Predicate>;

	template<typename T, typename Predicate>
	IProperty(T, Predicate) -> IProperty<T, Predicate>;
	template<typename T>
	IProperty(T) -> IProperty<T, void>;
}
