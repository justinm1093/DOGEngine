
#pragma once

#include "RTTI.h"
#include "Factory.h"

namespace UnitTests
{
	class Foo : public DOGEngine::RTTI
	{
		RTTI_DECLARATIONS(Foo, DOGEngine::RTTI)
		FACTORY_DECLARATION(Foo, DOGEngine::RTTI)

	public:

		Foo();
		explicit Foo(std::int32_t value);

		Foo(const Foo& other);
		Foo(Foo&& other);

		virtual ~Foo();

		std::int32_t getValue() const;

		Foo& operator=(const Foo& other);
		Foo& operator=(Foo&& other);

		bool operator==(const Foo& other) const;
		bool operator!=(const Foo& other) const;
		bool operator==(const std::int32_t rhs) const;
		bool operator!=(const std::int32_t rhs) const;

		virtual std::string ToString() const override;

	private:

		std::int32_t* mPointer;
	};
}
