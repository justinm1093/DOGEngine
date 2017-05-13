
namespace DOGEngine
{
	template <typename TBaseProduct>
	Factory<TBaseProduct>::~Factory()
	{
	}

	//----------------------------------------------------------------

	template <typename TBaseProduct>
	Factory<TBaseProduct>* Factory<TBaseProduct>::find(const std::string& productName)
	{
		// try to find factory with given name and return a pointer to it (null if not found)
		FactoriesIterator iter = sFactories.find(productName);
		return iter != sFactories.end() ? iter->second : nullptr;
	}

	//-----------------------------------------------------------------

	template <typename TBaseProduct>
	TBaseProduct* Factory<TBaseProduct>::create(const std::string& productName)
	{
		return sFactories.containsKey(productName) ? sFactories[productName]->create() : nullptr;
	}

	//-----------------------------------------------------------------

	template <typename TBaseProduct>
	typename Factory<TBaseProduct>::FactoriesIterator Factory<TBaseProduct>::begin()
	{
		return sFactories.begin();
	}

	//-----------------------------------------------------------------

	template <typename TBaseProduct>
	typename Factory<TBaseProduct>::FactoriesIterator Factory<TBaseProduct>::end()
	{
		return sFactories.end();
	}

	//-----------------------------------------------------------------

	template <typename TBaseProduct>
	bool Factory<TBaseProduct>::isFactoryRegistered(const std::string& productName)
	{
		return sFactories.containsKey(productName);
	}

	//-----------------------------------------------------------------

	template <typename TBaseProduct>
	typename const Factory<TBaseProduct>::ConcreteFactories& Factory<TBaseProduct>::getFactories()
	{
		return sFactories;
	}

	//-----------------------------------------------------------------

	template <typename TBaseProduct>
	std::uint32_t Factory<TBaseProduct>::numFactories()
	{
		return sFactories.size();
	}

	//-----------------------------------------------------------------

	template <typename TBaseProduct>
	bool Factory<TBaseProduct>::addFactory(Factory<TBaseProduct>& factory)
	{
		// attempt insert, return whether or not we actually inserted factory
		//		return sets a flag on the factory that says if it's registered
		//		it is up to the caller to handle what to do with unregistered factories
		bool didInsert = false;
		sFactories.insert(PairType(factory.getProductName(), &factory), &didInsert);

		return didInsert;
	}

	//-----------------------------------------------------------------

	template <typename TBaseProduct>
	void Factory<TBaseProduct>::removeFactory(Factory<TBaseProduct>& factory)
	{
		// we only remove the registered factory since that is the inserted one
		if(factory.isRegistered())
		{
			sFactories.remove(factory.getProductName());
		}
	}
}
