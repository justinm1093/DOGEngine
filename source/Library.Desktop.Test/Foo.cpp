
#include "pch.h"
#include "Foo.h"

using namespace DOGEngine;
using namespace UnitTests;
using namespace std;

RTTI_DEFINITIONS(Foo)

Foo::Foo() :
	mPointer(new int(0))
{
}

Foo::Foo(int32_t value) :
	mPointer(new int(value))
{
}

Foo::Foo(const Foo& other) :
	mPointer(new int(other.getValue()))
{
}

Foo::Foo(Foo&& other) :
	mPointer(other.mPointer)
{
	other.mPointer = new std::int32_t();
}

Foo::~Foo()
{
	delete mPointer;
}

int32_t Foo::getValue() const
{
	return *mPointer;
}

Foo& Foo::operator=(const Foo& other)
{
	if(this != &other)
	{
		// if we are not self-assigning, delete the old pointer and create a new one
		delete mPointer;
		mPointer = new int(other.getValue());
	}

	return *this;
}

Foo& Foo::operator=(Foo&& other)
{
	if(this != &other)
	{
		delete mPointer;
		mPointer = other.mPointer;

		other.mPointer = new std::int32_t();
	}

	return *this;
}

bool Foo::operator==(const Foo& other) const
{
	return getValue() == other.getValue();
}

bool Foo::operator!=(const Foo& other) const
{
	return getValue() != other.getValue();
}

bool Foo::operator==(const int32_t rhs) const
{
	return getValue() == rhs;
}

bool Foo::operator!=(const int32_t rhs) const
{
	return getValue() != rhs;
}

string Foo::ToString() const
{
	return to_string(*mPointer);
}
