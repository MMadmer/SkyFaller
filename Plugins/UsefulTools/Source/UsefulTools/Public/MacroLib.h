#pragma once

/**
 * Mark struct use custom serialization via "bool Serialize(FArchive& Ar)".
 * @param Name Struct full name.
 * @param UseIdenticalViaEquality Mark use overriden "==" for compare operations.
 */
#define MAKE_SERIALIZABLE_STRUCT(Name) \
	inline FArchive& operator<<(FArchive& Ar, Name& Save) \
	{ \
		Save.Serialize(Ar); \
		return Ar; \
	} \
	template<> \
	struct TStructOpsTypeTraits<Name> : public TStructOpsTypeTraitsBase2<Name> \
	{ \
		enum  \
		{ \
			WithSerializer = true \
		}; \
	};
