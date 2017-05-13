
#include "pch.h"
#include "Datum.h"

#include "Scope.h"

using namespace std;
using namespace glm;

namespace DOGEngine
{

#pragma region Static Function Pointer Array Definitions

	Datum::ComparisonFuncs Datum::sComparisonFuncs[(uint32_t)DatumType::Unknown] =
	{
		&Datum::comparisonHelper<int32_t>,
		&Datum::comparisonHelper<float>,
		&Datum::comparisonHelper<string>,
		&Datum::comparisonHelperRTTI,		// calls implementations of RTTI::Equals
		&Datum::comparisonHelper<mat4x4>,
		&Datum::comparisonHelper<vec4>,
		&Datum::comparisonHelperRTTI		// calls implementations of RTTI::Equals
	};

	//-----------------------------------------------------------------

	Datum::ReserveFuncs Datum::sReserveFuncs[(uint32_t)DatumType::Unknown] =
	{
		&Datum::reserveHelper<int32_t>,
		&Datum::reserveHelper<float>,
		&Datum::reserveHelper<string>,
		&Datum::reserveHelper<RTTI*>,
		&Datum::reserveHelper<mat4x4>,
		&Datum::reserveHelper<vec4>,
		&Datum::reserveHelper<Scope*>
	};

	//-----------------------------------------------------------------

	Datum::ShrinkToFitFuncs Datum::sShrinkToFitFuncs[(uint32_t)DatumType::Unknown] =
	{
		&Datum::shrinkToFitHelper<int32_t>,
		&Datum::shrinkToFitHelper<float>,
		&Datum::shrinkToFitHelper<string>,
		&Datum::shrinkToFitHelper<RTTI*>,
		&Datum::shrinkToFitHelper<mat4x4>,
		&Datum::shrinkToFitHelper<vec4>,
		&Datum::shrinkToFitHelper<Scope*>
	};

	//-----------------------------------------------------------------

	Datum::RemoveAtFuncs Datum::sRemoveAtFuncs[(uint32_t)DatumType::Unknown] =
	{
		&Datum::removeAtHelper<int32_t>,
		&Datum::removeAtHelper<float>,
		&Datum::removeAtHelper<string>,
		&Datum::removeAtHelper<RTTI*>,
		&Datum::removeAtHelper<mat4x4>,
		&Datum::removeAtHelper<vec4>,
		&Datum::removeAtHelper<Scope*>
	};

	//-----------------------------------------------------------------

	Datum::PopBackFuncs Datum::sPopBackFuncs[(uint32_t)DatumType::Unknown] =
	{
		&Datum::popBackHelper<int32_t>,
		&Datum::popBackHelper<float>,
		&Datum::popBackHelper<string>,
		&Datum::popBackHelper<RTTI*>,
		&Datum::popBackHelper<mat4x4>,
		&Datum::popBackHelper<vec4>,
		&Datum::popBackHelper<Scope*>
	};

	//-----------------------------------------------------------------

	Datum::ClearFuncs Datum::sClearFuncs[(uint32_t)DatumType::Unknown] =
	{
		&Datum::clearHelper<int32_t>,
		&Datum::clearHelper<float>,
		&Datum::clearHelper<string>,
		&Datum::clearHelper<RTTI*>,
		&Datum::clearHelper<mat4x4>,
		&Datum::clearHelper<vec4>,
		&Datum::clearHelper<Scope*>
	};

	//-----------------------------------------------------------------

	Datum::PushFromStringFuncs Datum::sPushFromStringFuncs[(std::uint32_t)DatumType::Unknown] =
	{
		&Datum::pushBackFromStringHelper<int32_t>,
		&Datum::pushBackFromStringHelper<float>,
		&Datum::pushBackFromStringHelper<string>,
		nullptr,
		&Datum::pushBackFromStringHelper<mat4x4>,
		&Datum::pushBackFromStringHelper<vec4>,
		nullptr
	};

	//-----------------------------------------------------------------

	Datum::SetFromStringFuncs Datum::sSetFromStringFuncs[(std::uint32_t)DatumType::Unknown] =
	{
		&Datum::setFromStringHelper<int32_t>,
		&Datum::setFromStringHelper<float>,
		&Datum::setFromStringHelper<string>,
		nullptr,
		&Datum::setFromStringHelper<mat4x4>,
		&Datum::setFromStringHelper<vec4>,
		nullptr
	};

	//-----------------------------------------------------------------

	Datum::ToStringFuncs Datum::sToStringFuncs[(std::uint32_t)DatumType::Unknown] =
	{
		&Datum::toStringInt,
		&Datum::toStringFloat,
		&Datum::toStringString,
		&Datum::toStringRTTI,
		&Datum::toStringMatrix,
		&Datum::toStringVector,
		&Datum::toStringRTTI
	};

	//-----------------------------------------------------------------

	Datum::CopyFuncs Datum::sCopyFuncs[(std::uint32_t)DatumType::Unknown] =
	{
		&Datum::performDeepCopyHelper<int32_t>,
		&Datum::performDeepCopyHelper<float>,
		&Datum::performDeepCopyHelper<string>,
		&Datum::performDeepCopyHelperRTTI,
		&Datum::performDeepCopyHelper<mat4x4>,
		&Datum::performDeepCopyHelper<vec4>,
		&Datum::performDeepCopyHelperRTTI
	};

#pragma endregion

	//=================================================================

#pragma region Datum Public Interface

#pragma region Construction, Destruction

	Datum::Datum(DatumType type) :
		mType(type),
		mSize(0),
		mCapacity(0),
		mIsExternal(false)
	{
		mData.v = nullptr;
	}

	//-----------------------------------------------------------------

	Datum::Datum(const Datum& other) :
		Datum(other.mType)
	{
		if(other.mIsExternal)
		{
			// if other is external...
			//		sets size, capacity, external bool, location of data
			mData.v = other.mData.v;
			mSize = other.mSize;
			mCapacity = other.mCapacity;
			mIsExternal = other.mIsExternal;
		}
		else
		{
			// if other is internal...
			//		deep copy -- sets size, capacity, copies data (external bool still false)
			performDeepCopy(other);
		}
	}

	//-----------------------------------------------------------------

	Datum::Datum(Datum&& other) :
		mType(other.mType),
		mSize(other.mSize),
		mCapacity(other.mCapacity),
		mIsExternal(other.mIsExternal)
	{
		mData.v = other.mData.v;

		other.mType = DatumType::Unknown;
		other.mSize = 0;
		other.mCapacity = 0;
		other.mIsExternal = false;
		other.mData.v = nullptr;
	}

	//-----------------------------------------------------------------

	Datum::~Datum()
	{
		// can only clear and free memory that we own and if we have a type
		if(!mIsExternal)
		{
			clear();
			free(mData.v);
		}
	}

#pragma endregion

	//-----------------------------------------------------------------

#pragma region Assignment Operators

	Datum& Datum::operator=(const Datum& other)
	{
		if(this != &other)
		{
			// our original data is internal, blow it away completely
			// if the data is external, we just set the data pointer
			if(!mIsExternal)
			{
				clear();
				free(mData.v);
			}
			mIsExternal = false;
			mData.v = nullptr;

			// setting type so any methods we need to call resolve against the required type
			mType = other.mType;

			if(other.mIsExternal)
			{
				// other is external -- we now point to other's (sets size, capacity, data location)
				mData.v = other.mData.v;
				mSize = other.mSize;
				mCapacity = other.mCapacity;
			}
			else
			{
				// other is internal -- deep copy other's data (sets size, capacity, copies data)
				mCapacity = mSize = 0;
				performDeepCopy(other);
			}

			// setting external bool last now that all needed methods have been called
			mIsExternal = other.mIsExternal;
		}

		return *this;
	}

	//-----------------------------------------------------------------

	Datum& Datum::operator=(Datum&& other)
	{
		if(&other != this)
		{
			// clear data if we have inserted values and we're not external
			if(mType != DatumType::Unknown && !mIsExternal)
			{
				clear();
				free(mData.v);
			}

			mType = other.mType;
			mSize = other.mSize;
			mCapacity = other.mCapacity;
			mIsExternal = other.mIsExternal;
			mData.v = other.mData.v;

			other.mType = DatumType::Unknown;
			other.mSize = 0;
			other.mCapacity = 0;
			other.mIsExternal = false;
			other.mData.v = nullptr;
		}

		return *this;
	}

	//-----------------------------------------------------------------

	Datum& Datum::operator=(const int32_t& rhs)
	{
		assignmentHelper<int32_t>(rhs, DatumType::Integer);
		return *this;
	}

	//-----------------------------------------------------------------

	Datum& Datum::operator=(const float& rhs)
	{
		assignmentHelper<float>(rhs, DatumType::Float);
		return *this;
	}
	
	//-----------------------------------------------------------------
	
	Datum& Datum::operator=(const string& rhs)
	{
		assignmentHelper<string>(rhs, DatumType::String);
		return *this;
	}

	//-----------------------------------------------------------------

	Datum& Datum::operator=(RTTI* const& rhs)
	{
		assignmentHelper<RTTI*>(rhs, DatumType::Pointer);
		return *this;
	}

	//-----------------------------------------------------------------

	Datum& Datum::operator=(const vec4& rhs)
	{
		assignmentHelper<vec4>(rhs, DatumType::Vector);
		return *this;
	}

	//-----------------------------------------------------------------

	Datum& Datum::operator=(const mat4x4& rhs)
	{
		assignmentHelper<mat4x4>(rhs, DatumType::Matrix);
		return *this;
	}

	//-----------------------------------------------------------------

	Datum& Datum::operator=(Scope& rhs)
	{
		assignmentHelper<Scope*>(&rhs, DatumType::Table);
		return *this;
	}

#pragma endregion

	//-----------------------------------------------------------------

#pragma region Comparison Operators

	bool Datum::operator==(const Datum& other) const
	{
		bool result = false;
		if(mType == DatumType::Unknown)
		{
			// no type, so we just see if the other is also no type
			result = other.mType == DatumType::Unknown;
		}
		else
		{
			// typed, so we run a templated helper function
			result = Datum::sComparisonFuncs[static_cast<uint32_t>(mType)](*this, other);
		}

		return result;
	}

	//-----------------------------------------------------------------

	bool Datum::operator==(const int32_t& rhs) const
	{
		return getHelper<int32_t>(0, DatumType::Integer) == rhs;
	}

	//-----------------------------------------------------------------

	bool Datum::operator==(const float& rhs) const
	{
		return getHelper<float>(0, DatumType::Float) == rhs;
	}
	
	//-----------------------------------------------------------------

	bool Datum::operator==(const string& rhs) const
	{
		return getHelper<string>(0, DatumType::String) == rhs;
	}

	//-----------------------------------------------------------------

	bool Datum::operator==(RTTI* const& rhs) const
	{
		return getHelper<RTTI*>(0, DatumType::Pointer)->Equals(rhs);
	}

	//-----------------------------------------------------------------

	bool Datum::operator==(const vec4& rhs) const
	{
		return getHelper<vec4>(0, DatumType::Vector) == rhs;
	}

	//-----------------------------------------------------------------

	bool Datum::operator==(const mat4x4& rhs) const
	{
		return getHelper<mat4x4>(0, DatumType::Matrix) == rhs;
	}

	//-----------------------------------------------------------------

	bool Datum::operator==(const Scope& rhs) const
	{
		// get the 0th scope as an rtti so we can call the equality function
		return getHelper<RTTI*>(0, DatumType::Table)->Equals(reinterpret_cast<const RTTI*>(&rhs));
	}

	//-----------------------------------------------------------------

	bool Datum::operator!=(const Datum& other) const
	{
		return !operator==(other);
	}

	//-----------------------------------------------------------------

	bool Datum::operator!=(const int32_t& rhs) const
	{
		return !operator==(rhs);
	}

	//-----------------------------------------------------------------

	bool Datum::operator!=(const float& rhs) const
	{
		return !operator==(rhs);
	}
	
	//-----------------------------------------------------------------

	bool Datum::operator!=(const string& rhs) const
	{
		return !operator==(rhs);
	}

	//-----------------------------------------------------------------

	bool Datum::operator!=(RTTI* const& rhs) const
	{
		return !operator==(rhs);
	}

	//-----------------------------------------------------------------

	bool Datum::operator!=(const vec4& rhs) const
	{
		return !operator==(rhs);
	}

	//-----------------------------------------------------------------

	bool Datum::operator!=(const mat4x4& rhs) const
	{
		return !operator==(rhs);
	}

	//-----------------------------------------------------------------

	bool Datum::operator!=(const Scope& rhs) const
	{
		return !operator==(rhs);
	}

#pragma endregion

	//-----------------------------------------------------------------

#pragma region Capacity

	void Datum::reserve(uint32_t reserveSize)
	{
		if(mType == DatumType::Unknown)
		{
			throw exception("Error -- cannot reserve space for a Datum that has no type!");
		}

		Datum::sReserveFuncs[static_cast<uint32_t>(mType)](*this, reserveSize);
	}

	//-----------------------------------------------------------------

	void Datum::shrinkToFit()
	{
		if(mType == DatumType::Unknown)
		{
			throw exception("Error -- cannot shrink space for a Datum that has no type!");
		}

		Datum::sShrinkToFitFuncs[static_cast<uint32_t>(mType)](*this);
	}

#pragma endregion

	//-----------------------------------------------------------------

#pragma region SetStorage

	void Datum::setStorage(int32_t* const& data, const uint32_t size)
	{
		setStorageHelper<int32_t>(data, size, DatumType::Integer);
	}

	//-----------------------------------------------------------------

	void Datum::setStorage(float* const& data, const uint32_t size)
	{
		setStorageHelper<float>(data, size, DatumType::Float);
	}

	//-----------------------------------------------------------------

	void Datum::setStorage(string* const& data, const uint32_t size)
	{
		setStorageHelper<string>(data, size, DatumType::String);
	}

	//-----------------------------------------------------------------

	void Datum::setStorage(RTTI** const& data, const uint32_t size)
	{
		setStorageHelper<RTTI*>(data, size, DatumType::Pointer);
	}

	//-----------------------------------------------------------------

	void Datum::setStorage(vec4* const& data, const uint32_t size)
	{
		setStorageHelper<vec4>(data, size, DatumType::Vector);
	}

	//-----------------------------------------------------------------

	void Datum::setStorage(mat4x4* const& data, const uint32_t size)
	{
		setStorageHelper<mat4x4>(data, size, DatumType::Matrix);
	}

#pragma endregion

	//-----------------------------------------------------------------

#pragma region Pushing & Popping

	void Datum::pushBack(const int32_t& data)
	{
		pushBackHelper<int32_t>(data, DatumType::Integer);
	}

	//-----------------------------------------------------------------

	void Datum::pushBack(const float& data)
	{
		pushBackHelper<float>(data, DatumType::Float);
	}

	//-----------------------------------------------------------------

	void Datum::pushBack(const string& data)
	{
		pushBackHelper<string>(data, DatumType::String);
	}

	//-----------------------------------------------------------------

	void Datum::pushBack(RTTI* const& data)
	{
		pushBackHelper<RTTI*>(data, DatumType::Pointer);
	}

	//-----------------------------------------------------------------

	void Datum::pushBack(const vec4& data)
	{
		pushBackHelper<vec4>(data, DatumType::Vector);
	}

	//-----------------------------------------------------------------

	void Datum::pushBack(const mat4x4& data)
	{
		pushBackHelper<mat4x4>(data, DatumType::Matrix);
	}

	//-----------------------------------------------------------------

	void Datum::pushBack(Scope& data)
	{
		pushBackHelper<Scope*>(&data, DatumType::Table);
	}

	//-----------------------------------------------------------------

	bool Datum::remove(const int32_t& data)
	{
		return removeHelper<int32_t>(data, DatumType::Integer);
	}

	//-----------------------------------------------------------------

	bool Datum::remove(const float& data)
	{
		return removeHelper<float>(data, DatumType::Float);
	}

	//-----------------------------------------------------------------

	bool Datum::remove(const string& data)
	{
		return removeHelper<string>(data, DatumType::String);
	}

	//-----------------------------------------------------------------

	bool Datum::remove(RTTI* const& data)
	{
		return removeHelper<RTTI*>(data, DatumType::Pointer);
	}

	//-----------------------------------------------------------------

	bool Datum::remove(const vec4& data)
	{
		return removeHelper<vec4>(data, DatumType::Vector);
	}

	//-----------------------------------------------------------------

	bool Datum::remove(const mat4x4& data)
	{
		return removeHelper<mat4x4>(data, DatumType::Matrix);
	}

	//-----------------------------------------------------------------

	bool Datum::remove(Scope& data)
	{
		return removeHelper<Scope*>(&data, DatumType::Table);
	}

	//-----------------------------------------------------------------

	void Datum::removeAt(uint32_t index)
	{
		// noop for untyped Datum
		if(mType != DatumType::Unknown)
		{
			Datum::sRemoveAtFuncs[static_cast<uint32_t>(mType)](*this, index);
		}
	}

	//-----------------------------------------------------------------

	void Datum::popBack()
	{
		// noop for untyped Datum
		if(mType != DatumType::Unknown)
		{
			Datum::sPopBackFuncs[static_cast<uint32_t>(mType)](*this);
		}
	}

	//-----------------------------------------------------------------

	void Datum::clear()
	{
		// noop for untyped Datum
		if(mType != DatumType::Unknown)
		{
			Datum::sClearFuncs[static_cast<uint32_t>(mType)](*this);
		}
	}

#pragma endregion

	//-----------------------------------------------------------------

#pragma region Value Setters & Getters

	void Datum::set(const int32_t& data, const uint32_t index)
	{
		setHelper<int32_t>(data, index, DatumType::Integer);
	}

	//-----------------------------------------------------------------

	void Datum::set(const float& data, const uint32_t index)
	{
		setHelper<float>(data, index, DatumType::Float);
	}
	
	//-----------------------------------------------------------------

	void Datum::set(const string& data, const uint32_t index)
	{
		setHelper<string>(data, index, DatumType::String);
	}

	//-----------------------------------------------------------------

	void Datum::set(RTTI* const& data, const std::uint32_t index)
	{
		setHelper<RTTI*>(data, index, DatumType::Pointer);
	}

	//-----------------------------------------------------------------

	void Datum::set(const vec4& data, const std::uint32_t index)
	{
		setHelper<vec4>(data, index, DatumType::Vector);
	}

	//-----------------------------------------------------------------

	void Datum::set(const mat4x4& data, const std::uint32_t index)
	{
		setHelper<mat4x4>(data, index, DatumType::Matrix);
	}

	//-----------------------------------------------------------------

	void Datum::set(Scope& data, const std::uint32_t index)
	{
		setHelper<Scope*>(&data, index, DatumType::Table);
	}

	//-----------------------------------------------------------------

	template<>
	int32_t& Datum::get<int32_t>(const uint32_t index)
	{
		return getHelper<int32_t>(index, DatumType::Integer);
	}

	//-----------------------------------------------------------------

	template<>
	float& Datum::get<float>(const uint32_t index)
	{
		return getHelper<float>(index, DatumType::Float);
	}

	//-----------------------------------------------------------------

	template<>
	string& Datum::get<string>(const uint32_t index)
	{
		return getHelper<string>(index, DatumType::String);
	}

	//-----------------------------------------------------------------

	template<>
	RTTI*& Datum::get<RTTI*>(const uint32_t index)
	{
		return getHelper<RTTI*>(index, DatumType::Pointer);
	}

	//-----------------------------------------------------------------

	template<>
	vec4& Datum::get<vec4>(const uint32_t index)
	{
		return getHelper<vec4>(index, DatumType::Vector);
	}

	//-----------------------------------------------------------------

	template<>
	mat4x4& Datum::get<mat4x4>(const uint32_t index)
	{
		return getHelper<mat4x4>(index, DatumType::Matrix);
	}

	//-----------------------------------------------------------------

	template<>
	Scope& Datum::get<Scope>(const uint32_t index)
	{
		return *getHelper<Scope*>(index, DatumType::Table);
	}

	//-----------------------------------------------------------------

	template<>
	const int32_t& Datum::get<int32_t>(const uint32_t index) const
	{
		return getHelper<int32_t>(index, DatumType::Integer);
	}

	//-----------------------------------------------------------------

	template<>
	const float& Datum::get<float>(const uint32_t index) const
	{
		return getHelper<float>(index, DatumType::Float);
	}

	//-----------------------------------------------------------------

	template<>
	const string& Datum::get<string>(const uint32_t index) const
	{
		return getHelper<string>(index, DatumType::String);
	}

	//-----------------------------------------------------------------

	template<>
	RTTI* const& Datum::get<RTTI*>(const uint32_t index) const
	{
		return getHelper<RTTI*>(index, DatumType::Pointer);
	}

	//-----------------------------------------------------------------

	template<>
	const vec4& Datum::get<vec4>(const uint32_t index) const
	{
		return getHelper<vec4>(index, DatumType::Vector);
	}

	//-----------------------------------------------------------------

	template<>
	const mat4x4& Datum::get<mat4x4>(const uint32_t index) const
	{
		return getHelper<mat4x4>(index, DatumType::Matrix);
	}

	//-----------------------------------------------------------------

	template<>
	const Scope& Datum::get<Scope>(const uint32_t index) const
	{
		return *getHelper<Scope*>(index, DatumType::Table);
	}

	//-----------------------------------------------------------------

	Datum& Datum::operator[](const string& name)
	{
		return get<Scope>()[name];
	}

	//-----------------------------------------------------------------

	Scope& Datum::operator[](std::uint32_t index)
	{
		return get<Scope>(index);
	}

	//-----------------------------------------------------------------

	const Scope& Datum::operator[](std::uint32_t index) const
	{
		return get<Scope>(index);
	}

#pragma endregion

	//-----------------------------------------------------------------

#pragma region To & From String

	void Datum::pushBackFromString(const string& str)
	{
		if(mType == DatumType::Unknown)
		{
			throw exception("Error -- cannot set data from string on a Datum with no type!");
		}

		if(mType == DatumType::Pointer)
		{
			throw exception("Error -- cannot set data from string on a Datum with type Pointer!");
		}

		if(mType == DatumType::Table)
		{
			throw exception("Error -- cannot set data from string on a Datum with type Table!");
		}

		Datum::sPushFromStringFuncs[static_cast<uint32_t>(mType)](*this, str);
	}

	//-----------------------------------------------------------------

	void Datum::setFromString(const string& str, const uint32_t index)
	{
		if(mType == DatumType::Unknown)
		{
			throw exception("Error -- cannot set data from string on a Datum with no type!");
		}

		if(mType == DatumType::Pointer)
		{
			throw exception("Error -- cannot set data from string on a Datum with type Pointer!");
		}
		
		if(mType == DatumType::Table)
		{
			throw exception("Error -- cannot set data from string on a Datum with type Table!");
		}

		Datum::sSetFromStringFuncs[static_cast<uint32_t>(mType)](*this, str, index);
	}

	//-----------------------------------------------------------------

	string Datum::toString(const uint32_t index) const
	{
		if(mType == DatumType::Unknown)
		{
			throw exception("Error -- cannot get data as a string from a Datum with no type!");
		}

		return Datum::sToStringFuncs[static_cast<uint32_t>(mType)](*this, index);
	}

#pragma endregion

	//-----------------------------------------------------------------

#pragma region Utility

	void Datum::setType(DatumType type)
	{
		if(mType != DatumType::Unknown && mType != type)
		{
			throw exception("Error -- cannot overwrite the type of a Datum whose type is already set!");
		}

		// only set type if current type is Unknown or the given argument
		mType = type;
	}

	//-----------------------------------------------------------------

	Datum::DatumType Datum::type() const
	{
		return mType;
	}

	//-----------------------------------------------------------------

	uint32_t Datum::size() const
	{
		return mSize;
	}

	//-----------------------------------------------------------------

	uint32_t Datum::capacity() const
	{
		return mCapacity;
	}

	//-----------------------------------------------------------------

	bool Datum::isExternal() const
	{
		return mIsExternal;
	}

	//-----------------------------------------------------------------

	bool Datum::isEmpty() const
	{
		return mSize == 0;
	}

#pragma endregion

#pragma endregion

	//=================================================================

#pragma region Private Templated Helper Functions

#pragma region Copy Helpers

	void Datum::performDeepCopy(const Datum& other)
	{
		if(mType != DatumType::Unknown)
		{
			Datum::sCopyFuncs[static_cast<uint32_t>(mType)](*this, other);
		}
	}

	//-----------------------------------------------------------------

	template <typename T>
	void Datum::performDeepCopyHelper(const Datum& other)
	{
		reserveHelper<T>(other.mCapacity);
		for(uint32_t i = 0; i < other.mSize; ++i)
		{
			pushBackHelper<T>(other.get<T>(i), mType);
		}
	}

	//-----------------------------------------------------------------

	void Datum::performDeepCopyHelperRTTI(const Datum& other)
	{
		reserveHelper<RTTI*>(other.mCapacity);
		for(uint32_t i = 0; i < other.mSize; ++i)
		{
			pushBackHelper<RTTI*>(other.getHelper<RTTI*>(i, other.mType), mType);
		}
	}

#pragma endregion

	//-----------------------------------------------------------------

#pragma region Assignment

	template <typename T>
	void Datum::assignmentHelper(const T& rhs, const DatumType expectedType)
	{
		if(mSize == 0)
		{
			// attempt to push back if there is nothing in the Datum
			pushBackHelper<T>(rhs, expectedType);
		}
		else
		{
			// set the value if it is already there
			setHelper<T>(rhs, 0, expectedType);
		}
	}

#pragma endregion

	//-----------------------------------------------------------------

#pragma region Comparison

	template <typename T>
	bool Datum::comparisonHelper(const Datum& other) const
	{
		// if type or size is not the same, return false immediately
		if(mType != other.mType || mSize != other.mSize)
		{
			return false;
		}

		bool result = true;
		for(uint32_t i = 0; i < mSize; ++i)
		{
			// return false if any elements don't match
			if(getHelper<T>(i, mType) != other.getHelper<T>(i, other.mType))
			{
				result = false;
				break;
			}
		}

		return result;
	}

	//-----------------------------------------------------------------

	bool Datum::comparisonHelperRTTI(const Datum& other) const
	{
		if(mType != other.mType || mSize != other.mSize)
		{
			return false;
		}

		// step through all stored pointers
		bool result = true;
		RTTI* currentPointer;
		RTTI* otherCurrentPointer;
		for(uint32_t i = 0; i < mSize; ++i)
		{
			currentPointer = getHelper<RTTI*>(i, mType);
			otherCurrentPointer = other.getHelper<RTTI*>(i, other.mType);

			// if current pointer is null, result is whether or not the other pointer is null
			if(currentPointer == nullptr)
			{
				result = otherCurrentPointer == nullptr;
			}

			// otherwise, we need to run Equals on other
			else
			{
				result = currentPointer->Equals(otherCurrentPointer);
				if(!result)
				{
					break;
				}
			}
		}

		return result;
	}

#pragma endregion

	//-----------------------------------------------------------------

#pragma region Capacity

	template <typename T>
	void Datum::reserveHelper(const uint32_t reserveSize)
	{
		if(mIsExternal)
		{
			throw exception("Error -- cannot reserve space for a Datum storing external data!");
		}

		// only reallocate if we're requesting more data than we have
		if(reserveSize > mCapacity)
		{
			T* newArray = reinterpret_cast<T*>(malloc(reserveSize * sizeof(T)));
			memcpy(newArray, mData.v, mCapacity * sizeof(T));

			free(mData.v);
			mData.v = newArray;

			mCapacity = reserveSize;
		}
	}

	//-----------------------------------------------------------------

	template <typename T>
	void Datum::shrinkToFitHelper()
	{
		if(mIsExternal)
		{
			throw exception("Error -- cannot shrink allocated space for a Datum storing external data!");
		}

		// only need to shrink capacity if size is smaller than capacity
		if(mSize < mCapacity)
		{
			T* newArray = reinterpret_cast<T*>(malloc(mSize * sizeof(T)));
			memcpy(newArray, mData.v, mSize * sizeof(T));

			free(mData.v);

			mData.v = newArray;
			mCapacity = mSize;
		}
	}

#pragma endregion

	//-----------------------------------------------------------------

#pragma region SetStorage

	template <typename T>
	void Datum::setStorageHelper(T* const& data, const uint32_t size, const DatumType type)
	{
		// if we own allocated space, we need to clear it and throw an exception
		if(!mIsExternal && mCapacity > 0)
		{
			clear();
			free(mData.v);
			mData.v = nullptr;

			throw exception("Error -- cannot make a Datum with an owned allocation external!");
		}

		// set type, array, and size data
		setType(type);
		mData.v = data;
		mSize = size;
		mCapacity = size;

		// we no longer own our data
		mIsExternal = true;
	}

#pragma endregion

	//-----------------------------------------------------------------

#pragma region Push & Pop

	template <typename T>
	void Datum::pushBackHelper(const T& data, const DatumType expectedType)
	{
		if(mIsExternal)
		{
			throw exception("Error -- cannot push back to a Datum that stores external data!");
		}

		// attempt to set the type
		//		either our current type is still Unknown or matches the expected type (type will be set)
		//		or our current type is known and mismatches the expected type (setType throws an overwrite exception)
		setType(expectedType);

		// allocate more space if needed
		if(mSize == mCapacity)
		{
			reserveHelper<T>(std::max<uint32_t>(1, mCapacity * 2));
		}

		// have to treat our void* array as the appropriate type
		T* dataArray = reinterpret_cast<T*>(mData.v);
		new(dataArray + mSize++)T(data);
	}

	//-----------------------------------------------------------------

	template <typename T>
	void Datum::popBackHelper()
	{
		if(mIsExternal)
		{
			throw exception("Error -- cannot remove a value from a Datum that stores external data!");
		}

		if(!isEmpty())
		{
			T* dataArray = reinterpret_cast<T*>(mData.v);
			dataArray[--mSize].~T();
		}
	}

	//-----------------------------------------------------------------

	template <typename T> 
	bool Datum::removeHelper(const T& data, const DatumType expectedType)
	{
		if(mIsExternal)
		{
			throw exception("Error -- cannot remove a value from a Datum that stores external data!");
		}

		if(expectedType != mType)
		{
			throw exception("Error -- cannot remove an element of a type that this Datum does not store!");
		}

		bool result = false;
		T* dataArray = reinterpret_cast<T*>(mData.v);
		
		// find the index of the data
		for(uint32_t i = 0; i < mSize; ++i)
		{
			if(data == dataArray[i])
			{
				removeAtHelper<T>(i);
				result = true;
				break;
			}
		}

		return result;
	}

	//-----------------------------------------------------------------

	template <typename T> 
	void Datum::removeAtHelper(uint32_t index)
	{
		if(mIsExternal)
		{
			throw exception("Error -- cannot remove a value from a Datum that stores external data!");
		}

		// array out of bounds
		if(index >= mSize)
		{
			throw exception("Error -- index is out of bounds for the Datum array!");
		}

		T* dataArray = reinterpret_cast<T*>(mData.v);

		// destruct the remove element
		dataArray[index].~T();

		// shift all data down one at the removal location
		uint32_t moveBlockSize = sizeof(T) * (mSize - index - 1);
		T* moveBlockAddress = dataArray + index;

		memmove_s(moveBlockAddress, moveBlockSize, moveBlockAddress + 1, moveBlockSize);
		--mSize;
	}

	//-----------------------------------------------------------------

	template <typename T>
	void Datum::clearHelper()
	{
		if(mIsExternal)
		{
			throw exception("Error -- cannot clear a Datum that stores external data!");
		}

		while(!isEmpty())
		{
			// call the templated popback helper directly (don't need to switch check for type continuously)
			popBackHelper<T>();
		}
	}

#pragma endregion

	//-----------------------------------------------------------------

#pragma region Set & Get

	template <typename T>
	void Datum::setHelper(const T& data, const uint32_t index, const DatumType expectedType)
	{
		if(expectedType != mType)
		{
			throw exception("Error -- mismatch between the Datum's type and the argument type of the invoked set() method!");
		}

		if(index >= mSize)
		{
			throw exception("Error -- cannot set values outside the bounds of the Datum's data!");
		}

		T* dataArray = reinterpret_cast<T*>(mData.v);
		dataArray[index] = data;
	}

	//-----------------------------------------------------------------

	template <typename T>
	T& Datum::getHelper(const uint32_t index, const DatumType expectedType) const
	{
		if(expectedType != mType)
		{
			throw exception("Error -- mismatch between the Datum's type and the return type of the invoked get() template!");
		}

		if(index >= mSize)
		{
			throw exception("Error -- cannot return values outside the bounds of the Datum's data!");
		}

		T* dataArray = reinterpret_cast<T*>(mData.v);
		return dataArray[index];
	}

#pragma endregion

	//-----------------------------------------------------------------

#pragma region From String

	template <>
	int32_t Datum::dataFromString<int32_t>(const string& str)
	{
		int data;
		sscanf_s(str.c_str(), "%d", &data);
		return data;
	}

	//-----------------------------------------------------------------

	template <>
	float Datum::dataFromString<float>(const string& str)
	{
		float data;
		sscanf_s(str.c_str(), "%f", &data);
		return data;
	}

	//-----------------------------------------------------------------

	template <>
	string Datum::dataFromString<string>(const string& str)
	{
		return str;
	}

	//-----------------------------------------------------------------

	template <>
	vec4 Datum::dataFromString<vec4>(const string& str)
	{
		vec4 data;
		sscanf_s(str.c_str(), "vec4(%f, %f, %f, %f)", &data.x, &data.y, &data.z, &data.w);
		return data;
	}

	//-----------------------------------------------------------------

	template <>
	mat4x4 Datum::dataFromString<mat4x4>(const string& str)
	{
		mat4x4 data;
		sscanf_s(str.c_str(), "mat4x4((%f, %f, %f, %f), (%f, %f, %f, %f), (%f, %f, %f, %f), (%f, %f, %f, %f))",
			&data[0].x, &data[0].y, &data[0].z, &data[0].w,
			&data[1].x, &data[1].y, &data[1].z, &data[1].w,
			&data[2].x, &data[2].y, &data[2].z, &data[2].w,
			&data[3].x, &data[3].y, &data[3].z, &data[3].w);
		return data;
	}

	//-----------------------------------------------------------------

	template <typename T>
	void Datum::pushBackFromStringHelper(const string& str)
	{
		pushBackHelper<T>(dataFromString<T>(str), mType);
	}

	//-----------------------------------------------------------------

	template <typename T>
	void Datum::setFromStringHelper(const string& str, const uint32_t index)
	{
		setHelper<T>(dataFromString<T>(str), index, mType);
	}

#pragma endregion

	//-----------------------------------------------------------------

#pragma region ToString

	string Datum::toStringInt(const uint32_t index) const
	{
		return std::to_string(getHelper<int32_t>(index, mType));
	}
	
	//-----------------------------------------------------------------

	string Datum::toStringFloat(const uint32_t index) const
	{
		return std::to_string(getHelper<float>(index, mType));
	}

	//-----------------------------------------------------------------

	string Datum::toStringString(const uint32_t index) const
	{
		return getHelper<string>(index, mType);
	}
	
	//-----------------------------------------------------------------

	string Datum::toStringVector(const uint32_t index) const
	{
		return glm::to_string(getHelper<vec4>(index, mType));
	}
	
	//-----------------------------------------------------------------

	string Datum::toStringMatrix(const uint32_t index) const
	{
		return glm::to_string(getHelper<mat4x4>(index, mType));
	}
	
	//-----------------------------------------------------------------

	string Datum::toStringRTTI(const uint32_t index) const
	{
		string dataAsString = "";

		RTTI* data = getHelper<RTTI*>(index, mType);
		if(data != nullptr)
		{
			dataAsString = data->ToString();
		}

		return dataAsString;
	}

#pragma endregion

#pragma endregion

}
