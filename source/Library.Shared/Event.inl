
namespace DOGEngine
{
	template <typename T>
	Event<T>::Event(const T& message) :
		EventPublisher(sSubscribers, sMutex),
		mMessage(message)
	{
	}

	//-----------------------------------------------------------------

	template <typename T>
	Event<T>::Event(const Event& other) :
		EventPublisher(other),
		mMessage(other.mMessage)
	{
	}

	//-----------------------------------------------------------------

	template <typename T>
	Event<T>& Event<T>::operator=(const Event& other)
	{
		if(this != &other)
		{
			EventPublisher::operator=(other);
			mMessage = other.mMessage;
		}

		return *this;
	}

	//-----------------------------------------------------------------

	template <typename T>
	Event<T>::Event(Event&& other) :
		EventPublisher(std::move(other)),
		mMessage(std::move(other.mMessage))
	{
	}
	
	//-----------------------------------------------------------------

	template <typename T>
	Event<T>& Event<T>::operator=(Event&& other)
	{
		if(this != &other)
		{
			EventPublisher::operator=(std::move(other));
			mMessage = std::move(other.mMessage);
		}

		return *this;
	}

	//-----------------------------------------------------------------

	template <typename T>
	Event<T>::~Event()
	{
	}

	//-----------------------------------------------------------------

	template <typename T>
	void Event<T>::subscribe(IEventSubscriber& subscriber)
	{
		std::lock_guard<std::mutex> lock(sMutex);
		sSubscribers.pushBack(&subscriber);
	}

	//-----------------------------------------------------------------

	template <typename T>
	void Event<T>::unsubscribe(IEventSubscriber& subscriber)
	{
		std::lock_guard<std::mutex> lock(sMutex);
		sSubscribers.remove(&subscriber);
	}

	//-----------------------------------------------------------------

	template <typename T>
	void Event<T>::unsubscribeAll()
	{
		std::lock_guard<std::mutex> lock(sMutex);
		sSubscribers.clear();
	}

	//-----------------------------------------------------------------

	template <typename T>
	std::uint32_t Event<T>::numSubscribers()
	{
		std::lock_guard<std::mutex> lock(sMutex);
		return sSubscribers.size();
	}

	//-----------------------------------------------------------------

	template <typename T>
	const T& Event<T>::message() const
	{
		return mMessage;
	}
}
