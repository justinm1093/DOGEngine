
#pragma once

#include "XmlParseMaster.h"
#include "IXmlParseHelper.h"
#include "RTTI.h"

#include "SharedDataTable.h"

namespace DOGEngine
{
	/**
	 * Xml parse helper class that handles expat
	 * callbacks for single data elements.
	 *
	 * These single-data elements must appear nested
	 * within a Scope object, or they will not be read.
	 *
	 * Supported elements:
	 *
	 *		<Int name="<name>" value="<value>" />
	 *			where "value" is an integral number
	 *
	 *		<Float name="<name>" value="<value>" />
	 *			where "value" is a floating point number
	 *
	 *		<String name="<name>" value="<value>" />
	 *			where "value" is a string
	 *
	 *		<Vector name="<name>" value="<value>" />
	 *			where "value" is a string in this form -- vec4(x, y, z, w)
	 *
	 *		<Matrix name="<name>" value="<value>" />
	 *			where "value" is a string in this form -- mat4x4((x, y, z, w), (x, y, z, w), (x, y, z, w), (x, y, z, w))
	 *
	 * All supported elements may also take an
	 * optional "index" attribute that marks the element
	 * data to be set in place on the containing Scope.
	 *
	 * Without the "index" attribute, the datapoint is
	 * pushed back at its specified name.
	 */
	class XmlParseHelperData : public IXmlParseHelper
	{
		RTTI_DECLARATIONS(XmlParseHelperData, IXmlParseHelper)

	public:

		XmlParseHelperData(const XmlParseHelperData& other) = delete;
		XmlParseHelperData(XmlParseHelperData&& other) = delete;
		XmlParseHelperData& operator=(const XmlParseHelperData& other) = delete;
		XmlParseHelperData& operator=(XmlParseHelperData&& other) = delete;

		/**
		 * @brief Constructor.
		 */
		XmlParseHelperData();

		/**
		 * @brief Destructor.
		 */
		virtual ~XmlParseHelperData();

		/**
		 * @brief Virtual constructor. Creates a new
		 *		  XmlParseHelperData.
		 *
		 * @return Returns the address of a new parse
		 *		   helper as the abstract base class.
		 */
		virtual IXmlParseHelper* clone() override;

		/**
		 * @brief Expat callback for element start tags.
		 *
		 * @param sharedData The shared data for the current parse.
		 * @param name The name of the current element.
		 * @param attributes Map of key-value pairs of attributes
		 *					 as unparsed strings.
		 *
		 * @return Returns true if the start element is handled.
		 *		   Otherwise, returns false.
		 *
		 * @exception If there are any errors during parse.
		 */
		virtual bool startElementHandler(SharedData& sharedData, const std::string& name, const AttributeMap& attributes) override;

		/**
		 * @brief Expat callback for element end tags.
		 *
		 * @param sharedData The shared data for the current parse.
		 * @param name The name of the current element.
		 *
		 * @return Returns true if the end element is handled.
		 *		   Otherwise, returns false.
		 */
		virtual bool endElementHandler(SharedData& sharedData, const std::string& name) override;

		/**
		 * @brief Clears the static map that associates
		 *		  element names with handler functions.
		 */
		static void clearHandlerCache();

	private:

		/**
		 * @brief Converts a string into an integer.
		 *
		 * @param indexStr The string to convert.
		 *
		 * @return Returns the string as an integer.
		 */
		std::int32_t getIndexFromString(const std::string& indexStr);

		// datum type map
		typedef HashMap<std::string, Datum::DatumType> DatumTypeMap;
		static DatumTypeMap sDatumTypeMap;

		// element tag names
		const static std::string sIntElement;
		const static std::string sFloatElement;
		const static std::string sStringElement;
		const static std::string sVectorElement;
		const static std::string sMatrixElement;

		// element attribute names
		const static std::string sNameAttribute;
		const static std::string sValueAttribute;
		const static std::string sIndexAttribute;
	};
}
