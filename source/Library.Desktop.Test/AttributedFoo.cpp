
#include "pch.h"
#include "AttributedFoo.h"

using namespace DOGEngine;
using namespace UnitTests;
using namespace std;
using namespace glm;

RTTI_DEFINITIONS(AttributedFoo)

AttributedFoo::AttributedFoo() :
	Attributed()
{
	if(!areSignaturesInitialized())
	{
		initSignatures();
	}

	populate();
}

//-----------------------------------------------------------------

AttributedFoo::AttributedFoo(const AttributedFoo& other)
{
	operator=(other);
}

//-----------------------------------------------------------------

AttributedFoo& AttributedFoo::operator=(const AttributedFoo& other)
{
	if(&other != this)
	{
		// copy values the AttributedFoo members from other to this
		mIntField = other.mIntField;
		mFloatField = other.mFloatField;
		mStringField = other.mStringField;
		mVectorField = other.mVectorField;
		mMatrixField = other.mMatrixField;
		mRTTIField = other.mRTTIField;

		for(uint32_t i = 0; i < ATTRIBUTED_FOO_ARRAY_SIZE; ++i)
		{
			mIntArray[i] = other.mIntArray[i];
			mFloatArray[i] = other.mFloatArray[i];
			mStringArray[i] = other.mStringArray[i];
			mVectorArray[i] = other.mVectorArray[i];
			mMatrixArray[i] = other.mMatrixArray[i];
			mRTTIArray[i] = other.mRTTIArray[i];
		}

		// Attributed copy semantics handles type-checking and table copy, and sets "this" to point to this object
		Attributed::operator=(other);
	}

	return *this;
}

//-----------------------------------------------------------------

Scope* AttributedFoo::copy()
{
	return new AttributedFoo(*this);
}

//-----------------------------------------------------------------

AttributedFoo::AttributedFoo(AttributedFoo&& other) :
	AttributedFoo()
{
	operator=(std::move(other));
}

//-----------------------------------------------------------------

AttributedFoo& AttributedFoo::operator=(AttributedFoo&& other)
{
	if(this != &other)
	{
		// copy the AttributedFoo members from other to this
		//		have to do this before the move resets other's members
		mIntField = other.mIntField;
		mFloatField = other.mFloatField;
		mStringField = other.mStringField;
		mVectorField = other.mVectorField;
		mMatrixField = other.mMatrixField;
		mRTTIField = other.mRTTIField;

		for(uint32_t i = 0; i < ATTRIBUTED_FOO_ARRAY_SIZE; ++i)
		{
			mIntArray[i] = other.mIntArray[i];
			mFloatArray[i] = other.mFloatArray[i];
			mStringArray[i] = other.mStringArray[i];
			mVectorArray[i] = other.mVectorArray[i];
			mMatrixArray[i] = other.mMatrixArray[i];
			mRTTIArray[i] = other.mRTTIArray[i];
		}

		// Attributed move calls Scope's move then resets the "this" pointer
		//		also calls to update the pointers for external storage
		Attributed::operator=(std::move(other));
	}

	return *this;
}

//-----------------------------------------------------------------

AttributedFoo::~AttributedFoo()
{
}

//-----------------------------------------------------------------

void AttributedFoo::initSignatures()
{
	Attributed::initSignatures();

	// internal fields
	addSignature("internal_int");
	addSignature("internal_float");
	addSignature("internal_string");
	addSignature("internal_vector");
	addSignature("internal_matrix");
	addSignature("internal_pointer");

	addSignature("internal_int_array");
	addSignature("internal_float_array");
	addSignature("internal_string_array");
	addSignature("internal_vector_array");
	addSignature("internal_matrix_array");
	addSignature("internal_pointer_array");

	// external fields
	addSignature("int_field");
	addSignature("float_field");
	addSignature("string_field");
	addSignature("vector_field");
	addSignature("matrix_field");
	addSignature("pointer_field");

	addSignature("int_array");
	addSignature("float_array");
	addSignature("string_array");
	addSignature("vector_array");
	addSignature("matrix_array");
	addSignature("pointer_array");

	// nested scopes
	addSignature("adopted_scope");
	addSignature("appended_scope");
}

//-----------------------------------------------------------------

void AttributedFoo::populate()
{
	Attributed::populate();

	// internal attributes
	addInternalAttribute("internal_int", 0);
	addInternalAttribute("internal_float", 0.0f);
	addInternalAttribute("internal_string", "");
	addInternalAttribute("internal_vector", vec4());
	addInternalAttribute("internal_matrix", mat4x4());
	addInternalAttribute("internal_pointer", nullptr);

	addInternalAttribute("internal_int_array", 0, ATTRIBUTED_FOO_ARRAY_SIZE);
	addInternalAttribute("internal_float_array", 0.0f, ATTRIBUTED_FOO_ARRAY_SIZE);
	addInternalAttribute("internal_string_array", "", ATTRIBUTED_FOO_ARRAY_SIZE);
	addInternalAttribute("internal_vector_array", vec4(), ATTRIBUTED_FOO_ARRAY_SIZE);
	addInternalAttribute("internal_matrix_array", mat4x4(), ATTRIBUTED_FOO_ARRAY_SIZE);
	addInternalAttribute("internal_pointer_array", nullptr, ATTRIBUTED_FOO_ARRAY_SIZE);

	// external attributes
	addExternalAttribute("int_field", &mIntField, 0);
	addExternalAttribute("float_field", &mFloatField, 0.0f);
	addExternalAttribute("string_field", &mStringField, "");
	addExternalAttribute("vector_field", &mVectorField, vec4());
	addExternalAttribute("matrix_field", &mMatrixField, mat4x4());
	addExternalAttribute("pointer_field", &mRTTIField, nullptr);

	addExternalAttribute("int_array", mIntArray, 0, ATTRIBUTED_FOO_ARRAY_SIZE);
	addExternalAttribute("float_array", mFloatArray, 0.0f, ATTRIBUTED_FOO_ARRAY_SIZE);
	addExternalAttribute("string_array", mStringArray, "", ATTRIBUTED_FOO_ARRAY_SIZE);
	addExternalAttribute("vector_array", mVectorArray, vec4(), ATTRIBUTED_FOO_ARRAY_SIZE);
	addExternalAttribute("matrix_array", mMatrixArray, mat4x4(), ATTRIBUTED_FOO_ARRAY_SIZE);
	addExternalAttribute("pointer_array", mRTTIArray, nullptr, ATTRIBUTED_FOO_ARRAY_SIZE);

	// nested scopes
	appendScope("adopted_scope");
	appendScope("appended_scope");
}

//-----------------------------------------------------------------

void AttributedFoo::updateExternalStorage()
{
	Attributed::updateExternalStorage();

	// fixing up the locations to which the external fields point
	operator[]("int_field").setStorage(&mIntField, 1);
	operator[]("float_field").setStorage(&mFloatField, 1);
	operator[]("string_field").setStorage(&mStringField, 1);
	operator[]("vector_field").setStorage(&mVectorField, 1);
	operator[]("matrix_field").setStorage(&mMatrixField, 1);
	operator[]("pointer_field").setStorage(&mRTTIField, 1);

	operator[]("int_array").setStorage(mIntArray, ATTRIBUTED_FOO_ARRAY_SIZE);
	operator[]("float_array").setStorage(mFloatArray, ATTRIBUTED_FOO_ARRAY_SIZE);
	operator[]("string_array").setStorage(mStringArray, ATTRIBUTED_FOO_ARRAY_SIZE);
	operator[]("vector_array").setStorage(mVectorArray, ATTRIBUTED_FOO_ARRAY_SIZE);
	operator[]("matrix_array").setStorage(mMatrixArray, ATTRIBUTED_FOO_ARRAY_SIZE);
	operator[]("pointer_array").setStorage(mRTTIArray, ATTRIBUTED_FOO_ARRAY_SIZE);
}
