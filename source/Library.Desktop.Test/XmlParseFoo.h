
#pragma once

#include "Factory.h"
#include "XmlParseMaster.h"
#include "IXmlParseHelper.h"
#include "RTTI.h"

#include "HashMap.h"
#include "Scope.h"

namespace UnitTests
{
	/**
	 * Test class that simulates uses for Xml parse
	 * helpers that XmlParseMaster may use.
	 *
	 * Parses some types of elements in Xml and saves
	 * the parsed data in a table for later verification.
	 */
	class SharedDataFoo;
	class XmlParseFoo : public DOGEngine::IXmlParseHelper
	{
		RTTI_DECLARATIONS(XmlParseFoo, DOGEngine::IXmlParseHelper)
		FACTORY_DECLARATION(XmlParseFoo, DOGEngine::RTTI)

	private:

		typedef DOGEngine::HashMap<std::string, std::string> AttributeMap;

		typedef std::function<void(XmlParseFoo&, SharedDataFoo&, const AttributeMap&)> StartElementHandler;
		typedef std::function<void(XmlParseFoo&, SharedDataFoo&)> EndElementHandler;
		typedef DOGEngine::HashMap<std::string, StartElementHandler> StartHandlers;
		typedef DOGEngine::HashMap<std::string, EndElementHandler> EndHandlers;

	public:

		XmlParseFoo(const XmlParseFoo& other) = delete;
		XmlParseFoo(XmlParseFoo&& other) = delete;
		XmlParseFoo& operator=(const XmlParseFoo& other) = delete;
		XmlParseFoo& operator=(XmlParseFoo&& other) = delete;

		/**
		 * @brief Constructor. Registers Xml element names with
		 *		  handler functions on this object.
		 */
		XmlParseFoo();

		/**
		 * @brief Destructor.
		 */
		virtual ~XmlParseFoo();

		/**
		 * @brief Virtual constructor. Creates a new default
		 *		  XmlParseFoo.
		 *
		 * @return Returns a pointer to a new XmlParseFoo object.
		 */
		virtual XmlParseFoo* clone() override;

		/**
		 * @brief Receive requests by the XmlParseMaster to handle Xml 
		 *		  start-element tags.
		 *
		 * @param userData Reference to the SharedData (or derived type)
		 *				   of the XmlParseMaster that started the parse.
		 * @param name The name of the tag.
		 * @param attributes Map of attribute key-value pairs as unparsed
		 *					 strings.
		 *
		 * @return Returns true if the parse request is handled. Otherwise, 
		 *		   returns false.
		 */
		virtual bool startElementHandler(DOGEngine::SharedData& sharedData, const std::string& name, const AttributeMap& attributes) override;

		/**
		 * @brief Receives requests by the XmlParseMaster to handle Xml 
		 *		  end-element tags.
		 *
		 * @param userData Reference to the SharedData (or derived tyep)
		 *				   of the XmlParseMaster that started the parse.
		 * @param name The name of the tag.
		 *
		 * @return Returns true if the parse request is handled. Otherwise, 
		 *		   returns false.
		 */
		virtual bool endElementHandler(DOGEngine::SharedData& sharedData, const std::string& name) override;

		/**
		 * @brief Clears map that associates Xml element names
		 *		  with XmlParseFoo handler methods.
		 */
		static void clearHandlerCaches();

	private:

		void startElementHandlerPerson(SharedDataFoo& sharedData, const AttributeMap& attributes);
		void startElementHandlerString(SharedDataFoo& sharedData, const AttributeMap& attributes);
		void startElementHandlerInt(SharedDataFoo& sharedData, const AttributeMap& attributes);

		void endElementHandlerPerson(SharedDataFoo& sharedData);
		void endElementHandlerString(SharedDataFoo& sharedData);
		void endElementHandlerInt(SharedDataFoo& sharedData);

		static StartHandlers sStartHandlers;
		static EndHandlers sEndHandlers;
	};

	/**
	 * Derived class of XmlParseMaster::SharedData
	 * that simulates uses for SharedData objects that
	 * must do more than just track the current parse depth.
	 *
	 * This class also counts the number of elements that
	 * have been parsed in the current document.
	 */
	class SharedDataFoo : public DOGEngine::SharedData
	{
		friend class XmlParseFoo;
		RTTI_DECLARATIONS(SharedDataFoo, DOGEngine::SharedData)
		FACTORY_DECLARATION(SharedDataFoo, DOGEngine::RTTI)

	public:

		SharedDataFoo(const SharedDataFoo& other) = delete;
		SharedDataFoo(SharedDataFoo&& other) = delete;
		SharedDataFoo& operator=(const SharedDataFoo& other) = delete;
		SharedDataFoo& operator=(SharedDataFoo&& other) = delete;

		/**
		 * @brief Constructor.
		 */
		SharedDataFoo();

		/**
		 * @brief Destructor.
		 */
		virtual ~SharedDataFoo();

		/**
		 * @brief Virtual constructor. Creates a new default
		 *		  SharedDataFoo object.
		 *
		 * @return Returns a pointer to a new SharedDataFoo
		 *		   object.
		 */
		virtual SharedDataFoo* clone() override;

		/**
		 * @brief Resets the state on this object.
		 */
		virtual void initialize() override;

		/**
		 * @brief Increments the counter keeping track of the
		 *		  number of Xml elements handled during this
		 *		  parse.
		 */
		void incrementNumElementsParsed();
	
		/**
		 * @brief Retrieves the table of parsed data.
		 *
		 * @return Returns a reference to the table.
		 */
		DOGEngine::Scope& getTable();

		/**
		 * @brief Retrieves the table of parsed data.
		 *
		 * @return Returns a const reference to the table of
		 *		   parsed data for verification.
		 */
		const DOGEngine::Scope& getTable() const;

		/**
		 * @brief Retrieves the number of Xml elements handled
		 *		  during this parse.
		 *
		 * @return Returns the number of Xml elements handled
		 *		   so far during the current parse.
		 */
		std::uint32_t numElementsParsed() const;

	private:

		DOGEngine::Scope mTable;
		DOGEngine::Scope* mCurrentTable;

		std::uint32_t mNumElementsParsed;
	};
}
