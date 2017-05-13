
#pragma once

#include "IXmlParseHelper.h"
#include "XmlParseMaster.h"

#include "SharedDataTable.h"

namespace DOGEngine
{
	/**
	 * Xml parse helper class that handles expat
	 * callbacks for "File" elements.
	 *
	 * File elements represent Xml subfiles, which
	 * define some root element (Sector, Entity, or
	 * Action) to be attached to a surrounding element
	 * in the file above.
	 *
	 * For example:
	 *
	 *		In root.xml -- 
	 *
	 *				<World name="World">
	 *					<File path="sector.xml" />
	 *				</World>

	 *		In sector.xml --
	 *
	 *				<Sector name="Sector">
	 *					...
	 *				</Sector>
	 *
	 * This defines a Sector in the sector.xml that will
	 * be added to the World created in root.xml
	 *
	 * Subfile roots have expectations about to what they
	 * will be attached in the file at the level above.
	 * Actions expect ActionLists or Entities at the level
	 * above. Entities expect Sectors. Sectors expect Worlds.
	 * Subfiles cannot have a World for a root.
	 *
	 * "File" tags have one required attribute: "path". This
	 * is a relative filepath from the current Xml file.
	 *
	 *		<File path="other.xml" />
	 *		<File path="dir/other.xml" />
	 */
	class XmlParseHelperSubfile final : public IXmlParseHelper
	{
		RTTI_DECLARATIONS(XmlParseHelperSubfile, IXmlParseHelper)

	public:

		XmlParseHelperSubfile(const XmlParseHelperSubfile& other) = delete;
		XmlParseHelperSubfile(XmlParseHelperSubfile&& other) = delete;
		XmlParseHelperSubfile& operator=(const XmlParseHelperSubfile& other) = delete;
		XmlParseHelperSubfile& operator=(XmlParseHelperSubfile&& other) = delete;

		/**
		 * @brief Constructor.
		 */
		XmlParseHelperSubfile();

		/**
		 * @brief Destructor.
		 */
		virtual ~XmlParseHelperSubfile();

		/**
		 * @brief Virtual constructor. Creates a new
		 *		  XmlParseHelperSubfile.
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

	private:

		const static std::string sFileElement;
		const static std::string sPathAttribute;
		const static std::string sScopeName;
	};
}
