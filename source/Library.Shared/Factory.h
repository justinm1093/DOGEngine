
#pragma once

#include "pch.h"
#include "HashMap.h"

namespace DOGEngine
{
	/**
	 * Defines a system by which objects may be created
	 * by name alone. The templated Factory class defines
	 * both a static interface used to call upon other
	 * Factories, as well as an abstract interface that
	 * derived classes use to create objects.
	 *
	 * In order to create a concrete Factory and register
	 * it with the abstract Factory, you must use the
	 * FACTORY_DECLARATIONS in a .h file to declare an
	 * embedded class that can create instances of the
	 * containing class.
	 *
	 * See the documentation of the macro below for more
	 * details.
	 *
	 * When concrete Factories are created, they attempt to
	 * register themselves with their parent abstract Factory.
	 * A boolean flag on the concrete Factories says whether
	 * that Factory is registered with the abstract Factory.
	 * It is up to the caller to decide what to do with
	 * unregistered concrete Factories.
	 *
	 * Factories unregister themselves from their abstract
	 * Factory when they are destroyed.
	 * 
	 * Ideally, concrete Factories are heap allocated. This
	 * way, Factories can persist outside of the stack frames
	 * in which they were declared. It is up to some outside
	 * caller to delete the registered Factories in this case.
	 */
	template <typename TBaseProduct>
	class Factory
	{

	/**
	 * Abstract interface for concrete Factories.
	 */
	public:

		/**
		 * @brief Destructor.
		 */
		virtual ~Factory();

		/**
		 * @brief Abstract method. Creates a new
		 *		  object of the Factory's derived
		 *		  type.
		 *
		 * @return Returns a pointer to the new object
		 *		   as the Factory's abstract base type.
		 */
		virtual TBaseProduct* create(void) const = 0;

		/**
		 * @brief Abstract method. Retrieves the name
		 *		  of the Factory's derived product.
		 *
		 * @return Returns the Factory's derived product
		 *		   type as a string.
		 */
		virtual std::string getProductName(void) const = 0;

		/**
		 * @brief Abstract method. Says whether the Factory
		 *		  is registered with its parent abstract
		 *		  Factory.
		 *
		 * @return Returns true if the Factory is registered.
		 *		   Otherwise, returns false.
		 */
		virtual bool isRegistered() const = 0;

	/**
	 * Static interface for interacting with the
	 * registered concrete Factories.
	 */
	private:

		typedef HashMap<std::string, Factory<TBaseProduct>*> ConcreteFactories;
		typedef std::pair<std::string, Factory<TBaseProduct>*> PairType;
		typedef typename ConcreteFactories::Iterator FactoriesIterator;

	public:

		/**
		 * @brief Retrieves a registered Factory that
		 *		  produces concrete products of the
		 *		  given name.
		 *
		 * @param productName The concrete product type as
		 *					  a string.
		 *
		 * @return Returns a pointer to the registered
		 *		   concrete Factory. The pointer is
		 *		   null if no Factory is registered with
		 *		   the given name.
		 */
		static Factory<TBaseProduct>* find(const std::string& productName);

		/**
		 * @brief Creates a concrete product with the
		 *		  given name.
		 *
		 * @param productName The concrete product type as
		 *					  a string.
		 *
		 * @return Returns a pointer to a new object of the
		 *		   concrete product type if a Factory that
		 *		   creates it is registered. If no Factory
		 *		   for that product is registered, returns
		 *		   null.
		 */
		static TBaseProduct* create(const std::string& productName);

		/**
		 * @brief Gets the beginning Iterator for the
		 *		  HashMap that manages registered concrete
		 *		  Factories.
		 *
		 * @return Returns a HashMap Iterator pointing at
		 *		   the first concrete Factory.
		 */
		static FactoriesIterator begin();

		/**
		 * @brief Gets the end Iterator for the HashMap
		 *		  that manages registered concrete Factories.
		 *
		 * @return Returns a HashMap Iterator pointing beyond
		 *		   the end of the concrete Factories HashMap.
		 */
		static FactoriesIterator end();

		/**
		 * @brief Says whether a concrete Factory is registered
		 *		  with the given product name.
		 *
		 * @param productName The concrete product type as a
		 *					  string.
		 *
		 * @return Returns true if a concrete Factory is
		 *		   registered with the given name. Otherwise,
		 *		   returns false.
		 */
		static bool isFactoryRegistered(const std::string& productName);

		/**
		 * @brief Getter for the HashMap of concrete Factories
		 *		  that this abstract Factory manages.
		 *
		 * @return Returns a const reference to the static HashMap
		 *		   of concrete Factories.
		 */
		static const ConcreteFactories& getFactories();

		/**
		 * @brief Says how many concrete Factories are registered
		 *		  with this abstract Factory.
		 *
		 * @return Returns the number of concrete Factories
		 *		   currently in this abstract Factory's HashMap.
		 */
		static std::uint32_t numFactories();

	protected:

		/**
		 * @brief Attempts to register a concrete Factory with
		 *		  this object's HashMap. Called from concrete
		 *		  Factory constructors.
		 *
		 * @param factory A reference to the new concrete
		 *				  Factory.
		 *
		 * @return Returns true no Factory of that concrete type
		 *		   is registered and the incoming Factory is added.
		 *		   Otherwise, returns false.
		 */
		static bool addFactory(Factory<TBaseProduct>& factory);

		/**
		 * @brief Attempts to unregister the concrete Factory
		 *		  from this object's HashMap. Called from
		 *		  concrete Factory destructors.
		 *
		 * @param factory A reference to the outgoing concrete
		 *				  Factory.
		 */
		static void removeFactory(Factory<TBaseProduct>& factory);

		bool mIsRegistered;

	private:

		static ConcreteFactories sFactories;
	};

	/**
	 * This macro declares an embedded class that derives from the
	 * abstract Factory. This embedded class attempts to register
	 * itself with its parent abstract Factory on creation, and
	 * unregisters itself upon destruction.
	 *
	 * The concrete Factory class creates products of a specific
	 * type, which are called upon by name via the static create
	 * method on the parent abstract Factory.
	 *
	 * It takes two arguments:
	 *
	 *		TDerivedProduct -- The object type that this Factory
	 *						   produces.
	 *		TBaseProduct -- The abstract object type that TDerivedProduct
	 *						ultimately derives.
	 *
	 * TBaseProduct defines a family of concrete Factories that
	 * are all managed by a common abstract Factory. For example,
	 * any Factories declared with TBaseProduct of RTTI are all
	 * managed by Factory<RTTI>.
	 *
	 * Objects are then created with Factory<RTTI>::create("<type name>")
	 */
#define FACTORY_DECLARATION(TDerivedProduct, TBaseProduct)							\
	public:																			\
		class TDerivedProduct ## Factory final : DOGEngine::Factory<TBaseProduct>	\
		{																			\
		public:																		\
			TDerivedProduct ## Factory() 											\
			{ 																		\
				mIsRegistered = addFactory(*this); 									\
			}																		\
																					\
			virtual ~TDerivedProduct ## Factory()									\
			{ 																		\
				removeFactory(*this);												\
			}																		\
																					\
			virtual TBaseProduct* create() const override							\
			{																		\
				TBaseProduct* product = new TDerivedProduct();						\
				assert(product != nullptr);											\
				return product;														\
			}																		\
																					\
			virtual std::string getProductName() const override						\
			{																		\
				return std::string(#TDerivedProduct);								\
			}																		\
																					\
			virtual bool isRegistered() const override								\
			{																		\
				return mIsRegistered;												\
			}																		\
		};																			\

	/**
	 * Definition for abstract Factory Hashmaps.
	 */
	template <typename TBaseProduct> HashMap<std::string, Factory<TBaseProduct>*> Factory<TBaseProduct>::sFactories;
}

#include "Factory.inl"
