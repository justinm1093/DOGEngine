
#pragma once

#include "RTTI.h"
#include "HashMap.h"

namespace DOGEngine
{
	/**
	 * Abstract base class for objects that
	 * can participate in the chain of responsibility
	 * set up by XmlParseMaster to parse Xml documents.
	 *
	 * SharedData is forward declared here because we
	 * cannot #include XmlParseMaster.h here.
	 */
	class SharedData;
	class IXmlParseHelper : public RTTI
	{
		RTTI_DECLARATIONS(IXmlParseHelper, RTTI)

	protected:

		typedef DOGEngine::HashMap<std::string, std::string> AttributeMap;

	public:

		IXmlParseHelper(const IXmlParseHelper& other) = delete;
		IXmlParseHelper(IXmlParseHelper&& other) = delete;
		IXmlParseHelper& operator=(const IXmlParseHelper& other) = delete;
		IXmlParseHelper& operator=(IXmlParseHelper&& other) = delete;

		/**
		 * @brief Constructor.
		 */
		IXmlParseHelper() = default;

		/**
		 * @brief Destructor.
		 */
		virtual ~IXmlParseHelper() = default;

		/**
		 * @brief Abstract virtual constructor. Implement to 
		 *		  create a new derived helper.
		 */
		virtual IXmlParseHelper* clone() = 0;

		/**
		 * @brief Implement to reset the state of a derived helper.
		 */
		virtual void initialize();

		/**
		 * @brief Abstract method. Implement to receive requests
		 *		  by the XmlParseMaster to handle Xml start-element
		 *		  tags.
		 *
		 * @param userData Reference to the SharedData (or derived type)
		 *				   of the XmlParseMaster that started the parse.
		 * @param name The name of the tag.
		 * @param attributes Map of attribute key-value pairs as unparsed
		 *					 strings.
		 *
		 * @return Should return true if the parse request is handled.
		 *		   Otherwise, should return false.
		 */
		virtual bool startElementHandler(SharedData& sharedData, const std::string& name, const AttributeMap& attributes) = 0;

		/**
		 * @brief Abstract method. Implement to receive requests
		 *		  by the XmlParseMaster to handle Xml end-element
		 *		  tags.
		 *
		 * @param userData Reference to the SharedData (or derived tyep)
		 *				   of the XmlParseMaster that started the parse.
		 * @param name The name of the tag.
		 *
		 * @return Should return true if the parse request is handled.
		 *		   Otherwise, should return false.
		 */
		virtual bool endElementHandler(SharedData& sharedData, const std::string& name) = 0;

		/**
		 * @brief Implement to receive requests by the XmlParseMaster to hadnle 
		 *		  Xml character data.
		 *
		 * @param userData Reference to the SharedData (or derived type)
		 *				   of the XmlParseMaster that started the parse.
		 * @param buffer The character data being handled.
		 * @param length The length in bytes of the character data being handled.
		 *
		 * @return Returns false by default.
		 *
		 * @note Note that the buffer is not null-terminated and requires the
		 *		 length argument for proper handling.
		 */
		virtual bool charDataHandler(SharedData& sharedData, const char* buffer, int32_t length);

	protected:

		/**
		 * @brief Verifies the attribute validity of Xml elements
		 *		  during parse. Call upon this method from derived
		 *		  XmlParseHelpers to enforce that a start-element
		 *		  tag must have an attribute of the given name.
		 *
		 * @param attributes The attributes on the current element.
		 * @param attributeName The name of the attribute that the
		 *						current element must have.
		 * @param elementName The name of the current element, used
		 *					  for printing error strings.
		 * @param fileName The name of the Xml file being parsed,
		 *				   used for printing error strings.
		 * @param requiresValue Says whether the given attribute must
		 *						also specify a non-empty string value.
		 *						Defaulted to 'false'
		 *
		 * @exception The attributes map does not contain the given
		 *			  attribute name.
		 * @exceptoin The attribute is required to have a value set
		 *			  (requiresValue is 'true'), but the attribute 
		 *			  value is an empty string ("").
		 */
		void requiresAttribute(const AttributeMap& attributes, const std::string& attributeName, const std::string& elementName, const std::string& fileName, bool requiresValue = false);
	};
}
