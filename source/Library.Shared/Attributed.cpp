
#include "pch.h"
#include "Attributed.h"

using namespace std;
using namespace glm;
using namespace DOGEngine;

RTTI_DEFINITIONS(Attributed)

Attributed::SignatureMap Attributed::sSignatureMap;

#pragma region Public Interface

Attributed::Attributed(uint32_t size) :
	Scope(size)
{
	if(!areSignaturesInitialized())
	{
		initSignatures();
	}

	populate();
}

//-----------------------------------------------------------------

Attributed::Attributed(const Attributed& other) :
	Scope(other)
{
	(*this)["this"] = this;
	updateExternalStorage();
}

//-----------------------------------------------------------------

Attributed& Attributed::operator=(const Attributed& other)
{
	if(&other != this)
	{
		Scope::operator=(other);

		(*this)["this"] = this;
		updateExternalStorage();
	}

	return *this;
}

//-----------------------------------------------------------------

Scope* Attributed::copy()
{
	return new Attributed(*this);
}

//-----------------------------------------------------------------

Attributed::Attributed(Attributed&& other) :
	Scope(std::move(other))
{
	(*this)["this"] = this;

	// repopulate other and update our external pointers
	other.populate();
	updateExternalStorage();
}

//-----------------------------------------------------------------

Attributed& Attributed::operator=(Attributed&& other)
{
	if(this != &other)
	{
		Scope::operator=(std::move(other));

		(*this)["this"] = this;

		// repopulate other and update our external pointers
		other.populate();
		updateExternalStorage();
	}

	return *this;
}

//-----------------------------------------------------------------

Attributed::~Attributed()
{
}

//-----------------------------------------------------------------

bool Attributed::Equals(const RTTI* rhs) const
{
	if(rhs == nullptr)
	{
		return false;
	}

	if(this == rhs)
	{
		return true;
	}

	// have to get other as Attributed if we can
	Attributed* other = rhs->As<Attributed>();
	if(other == nullptr)
	{
		return false;
	}

	return operator==(*other);
}

//-----------------------------------------------------------------

bool Attributed::operator==(const Attributed& other) const
{
	bool result = true;
	if(this != &other)
	{
		if(size() != other.size())
		{
			// different sizes, can't be equal
			result = false;
		}
		else
		{
			// we have to manually step through the table so we can ignore the "this" field
			MapIter otherFindIter;
			MapIter otherEndIter = other.mMap.end();

			for(PairType& kvPair : mMap)
			{
				// skip "this" attribute so we don't infinitely recurse
				if(kvPair.first == "this")
				{
					continue;
				}

				// otherwise, we do a comparison between what's in the other's table
				otherFindIter = other.mMap.find(kvPair.first);
				if(otherFindIter == otherEndIter)
				{
					result = false;
					break;
				}
				if(kvPair.second != otherFindIter->second)
				{
					result = false;
					break;
				}
			}
		}
	}

	return result;
}

//-----------------------------------------------------------------

bool Attributed::operator!=(const Attributed& other) const
{
	return !operator==(other);
}

//-----------------------------------------------------------------

Datum& Attributed::addAuxiliaryAttribute(const string& key)
{
	return append(key);
}

//-----------------------------------------------------------------

uint32_t Attributed::prescribedSize() const
{
	// number of prescribed attributes is the size of the static list for this type
	return sSignatureMap[TypeIdInstance()].size();
}

//-----------------------------------------------------------------

uint32_t Attributed::auxiliarySize() const
{
	// number of auxiliary attributes is the total number of attributes minus the number of prescribed attributes
	return size() - prescribedSize();
}

//-----------------------------------------------------------------

uint32_t Attributed::auxiliaryBeginIndex() const
{
	// first index of the auxiliary attributes is the number of prescribed attributes
	return prescribedSize();
}

//-----------------------------------------------------------------

bool Attributed::isAttribute(const string& name) const
{
	return find(name) != nullptr;
}

//-----------------------------------------------------------------

bool Attributed::isPrescribedAttribute(const string& name) const
{
	// prescribed attribute if in the signatures map AND currently in the table
	SignatureList& signatureList = sSignatureMap[TypeIdInstance()];
	return signatureList.find(name) != signatureList.end() && isAttribute(name);
}

//-----------------------------------------------------------------

bool Attributed::isAuxiliaryAttribute(const string& name) const
{
	return isAttribute(name) && !isPrescribedAttribute(name);
}

//-----------------------------------------------------------------

void Attributed::clearAttributeCache()
{
	sSignatureMap.clear();
}

#pragma endregion

//-----------------------------------------------------------------

#pragma region Protected Interface

void Attributed::initSignatures()
{
	addSignature("this");
}

//-----------------------------------------------------------------

void Attributed::populate()
{
	addInternalAttribute("this", this);
}

//-----------------------------------------------------------------

void Attributed::updateExternalStorage()
{
}

//-----------------------------------------------------------------

bool Attributed::areSignaturesInitialized() const
{
	return !sSignatureMap[TypeIdInstance()].isEmpty();
}

//-----------------------------------------------------------------

void Attributed::addSignature(const string& signature)
{
	SignatureList& signatureList = sSignatureMap[TypeIdInstance()];
	if(signatureList.find(signature) == signatureList.end())
	{
		signatureList.pushBack(signature);
	}
}

//-----------------------------------------------------------------

void Attributed::addTableEntry(const string& name)
{
	Datum& datum = append(name);
	datum.setType(Datum::DatumType::Table);
}

//-----------------------------------------------------------------

void Attributed::addInternalAttribute(const string& name, const int32_t defaultValue, const uint32_t size)
{
	addInternalAttributeHelper<int32_t>(name, defaultValue, size, Datum::DatumType::Integer);
}

//-----------------------------------------------------------------

void Attributed::addInternalAttribute(const string& name, const float defaultValue, const uint32_t size)
{
	addInternalAttributeHelper<float>(name, defaultValue, size, Datum::DatumType::Float);
}

//-----------------------------------------------------------------

void Attributed::addInternalAttribute(const string& name, const string defaultValue, const uint32_t size)
{
	addInternalAttributeHelper<string>(name, defaultValue, size, Datum::DatumType::String);
}

//-----------------------------------------------------------------

void Attributed::addInternalAttribute(const string& name, const vec4 defaultValue, const uint32_t size)
{
	addInternalAttributeHelper<vec4>(name, defaultValue, size, Datum::DatumType::Vector);
}

//-----------------------------------------------------------------

void Attributed::addInternalAttribute(const string& name, const mat4x4 defaultValue, const uint32_t size)
{
	addInternalAttributeHelper<mat4x4>(name, defaultValue, size, Datum::DatumType::Matrix);
}

//-----------------------------------------------------------------

void Attributed::addInternalAttribute(const string& name, RTTI* const defaultValue, const uint32_t size)
{
	addInternalAttributeHelper<RTTI*>(name, defaultValue, size, Datum::DatumType::Pointer);
}

//-----------------------------------------------------------------

void Attributed::addExternalAttribute(const string& name, int32_t* const storage, const int32_t defaultValue, const uint32_t size)
{
	addExternalAttributeHelper<int32_t>(name, storage, defaultValue, size);
}

//-----------------------------------------------------------------

void Attributed::addExternalAttribute(const string& name, float* const storage, const float defaultValue, const uint32_t size)
{
	addExternalAttributeHelper<float>(name, storage, defaultValue, size);
}

//-----------------------------------------------------------------

void Attributed::addExternalAttribute(const string& name, string* const storage, const string& defaultValue, const uint32_t size)
{
	addExternalAttributeHelper<string>(name, storage, defaultValue, size);
}

//-----------------------------------------------------------------

void Attributed::addExternalAttribute(const string& name, vec4* const storage, const vec4& defaultValue, const uint32_t size)
{
	addExternalAttributeHelper<vec4>(name, storage, defaultValue, size);
}

//-----------------------------------------------------------------

void Attributed::addExternalAttribute(const string& name, mat4x4* const storage, const mat4x4& defaultValue, const uint32_t size)
{
	addExternalAttributeHelper<mat4x4>(name, storage, defaultValue, size);
}

//-----------------------------------------------------------------

void Attributed::addExternalAttribute(const string& name, RTTI** const storage, RTTI* const defaultValue, const uint32_t size)
{
	addExternalAttributeHelper<RTTI*>(name, storage, defaultValue, size);
}

#pragma endregion

//-----------------------------------------------------------------

#pragma region Private Helpers

template <typename T>
void Attributed::addInternalAttributeHelper(const std::string& name, const T defaultValue, const std::uint32_t size, const Datum::DatumType type)
{
	if(find(name) == nullptr)
	{
		// add the key to the table, reserve space for the array (or scalar value)
		Datum& datum = append(name);
		datum.setType(type);
		datum.reserve(size);

		// fill the datum with the default value
		for(uint32_t i = 0; i < size; ++i)
		{
			datum.pushBack(defaultValue);
		}
	}
}

//-----------------------------------------------------------------

template <typename T>
void Attributed::addExternalAttributeHelper(const string& name, T* const storage, const T defaultValue, const uint32_t size)
{
	if(find(name) == nullptr)
	{
		// add the key to the table, set its storage
		Datum& datum = append(name);
		datum.setStorage(storage, size);

		// fill the array with the default value
		for(uint32_t i = 0; i < size; ++i)
		{
			datum.set(defaultValue, i);
		}
	}
}

#pragma endregion
