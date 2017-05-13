
#pragma once

#include "IEventSubscriber.h"

namespace UnitTests
{
	class EventSubscriberFoo final : public DOGEngine::IEventSubscriber
	{
	public:

		enum class SubscriberAction
		{
			SUBSCRIBE,
			UNSUBSCRIBE,
			CLEAR_SUBSCRIBERS,
			NONE
		};

		EventSubscriberFoo(const EventSubscriberFoo& other) = delete;
		EventSubscriberFoo(EventSubscriberFoo&& other) = delete;
		EventSubscriberFoo& operator=(const EventSubscriberFoo& other) = delete;
		EventSubscriberFoo& operator=(EventSubscriberFoo&& other) = delete;

		explicit EventSubscriberFoo(SubscriberAction action = SubscriberAction::NONE);
		~EventSubscriberFoo();

		virtual void notify(const DOGEngine::EventPublisher& e) override;

		void resetNumNotifies();

		std::int32_t getInt() const;
		std::uint32_t getNumNotifies() const;
		std::uint32_t getChildNumNotifies() const;

	private:

		std::int32_t mInt;
		std::uint32_t mNumNotifies;

		SubscriberAction mAction;
		EventSubscriberFoo* mChildFoo;

		std::mutex mMutex;
	};
}
