
#pragma once

#include "XmlParseMaster.h"
#include "IXmlParseHelper.h"
#include "RTTI.h"

#include "SharedDataTable.h"

namespace DOGEngine
{
	/**
	 * Xml parse helper class that handles expat
	 * callbacks for table types, including 
	 * Worlds, Sectors, Entities, and Actions.
	 *
	 * Worlds are root elements. They expect the following syntax:
	 *
	 *		<World name="<name>">
	 *			...
	 *		</World>
	 *
	 * Sectors are children of Worlds. They expect the following syntax:
	 *
	 *		<Sector name="<name>">
	 *			...
	 *		</Sector>
	 *
	 * Entities are children of Sectors. They expect the following syntax:
	 *
	 *		<Entity class="<class>" name="<name>">
	 *			...
	 *		</Entity>
	 *
	 * Actions are children of Worlds, Sectors, Entities, and ActionLists. 
	 * They expect the following syntax:
	 *
	 *		<Action class="<class>" name="<name>">
	 *			...
	 *		</Action>
	 *
	 * Reactions are children of Worlds, Sectors, Entities, and Actions.
	 * They expect the following syntax:
	 *
	 *		<Reaction name="<name>" subtype="<subtype">
	 *			<String name="subtype" value="<value>" />			-- optional, child strings with name "subtype" define additional event subtypes for the Reaction
	 *			...													-- optional, Reactions expect Actions to execute as children
	 *		</Reaction>
	 *
	 * "If" is a special Action that expects some special syntax:
	 *
	 *		<If name="<name>">
	 *			<Condition value="<integer>" />						-- optional, default value is 0
	 *			
	 *			<Then class="<class"> name="<name>">				-- optional, otherwise defines an Action to execute if Condition is true
	 *			</Then>
	 *
	 *			<Else class="<class"> name="<name>">				-- optional, otherwise defines an Action to execute if Condition is false
	 *			</Else>
	 *		</If>
	 */
	class XmlParseHelperTable final : public IXmlParseHelper
	{
		RTTI_DECLARATIONS(XmlParseHelperTable, IXmlParseHelper)

	public:

		XmlParseHelperTable(const XmlParseHelperTable& other) = delete;
		XmlParseHelperTable(XmlParseHelperTable&& other) = delete;
		XmlParseHelperTable& operator=(const XmlParseHelperTable& other) = delete;
		XmlParseHelperTable& operator=(XmlParseHelperTable&& other) = delete;

		/**
		 * @brief Constructor.
		 */
		XmlParseHelperTable();

		/**
		 * @brief Destructor.
		 */
		virtual ~XmlParseHelperTable();

		/**
		 * @brief Virtual constructor. Returns a new
		 *		  XmlParseHelperTable.
		 *
		 * @return Returns a pointer to a new
		 *		  XmlParseHelperTable.
		 */
		virtual XmlParseHelperTable* clone() override;

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
		 * @brief Expat callback for Scope element start tags.
		 *
		 * @param sharedData The shared data for the current parse.
		 * @param attributes Map of key-value pairs of attributes
		 *					 as unparsed strings.
		 *
		 * @exception No "name" attribute or the value of "name" is
		 *			  an empty string ""
		 */
		void elementHandlerScope(SharedDataTable& sharedData, const AttributeMap& attributes);

		/**
		 * @brief Expat callback for World element start tags.
		 *
		 * @param sharedData The shared data for the current parse.
		 * @param attributes Map of key-value pairs of attributes
		 *					 as unparsed strings.
		 *
		 * @exception No "name" attribute or the value of "name" is
		 *			  an empty string ""
		 */
		void elementHandlerWorld(SharedDataTable& sharedData, const AttributeMap& attributes);

		/**
		 * @brief Expat callback for Sector element start tags.
		 *
		 * @param sharedData The shared data for the current parse.
		 * @param attributes Map of key-value pairs of attributes
		 *					 as unparsed strings.
		 *
		 * @exception No "name" attribute or the value of "name" is
		 *			  an empty string ""
		 */
		void elementHandlerSector(SharedDataTable& sharedData, const AttributeMap& attributes);

		/**
		 * @brief Expat callback for Entity element start tags.
		 *
		 * @param sharedData The shared data for the current parse.
		 * @param attributes Map of key-value pairs of attributes
		 *					 as unparsed strings.
		 *
		 * @exception No "class" attribute or the value of "class" is
		 *			  an empty string ""
		 * @exception No "name" attribute or the value of "name" is
		 *			  an empty string ""
		 */
		void elementHandlerEntity(SharedDataTable& sharedData, const AttributeMap& attributes);

		/**
		 * @brief Expat callback for Action element start tags.
		 *
		 * @param sharedData The shared data for the current parse.
		 * @param attributes Map of key-value pairs of attributes
		 *					 as unparsed strings.
		 *
		 * @exception No "class" attribute or the value of "class" is
		 *			  an empty string ""
		 * @exception No "name" attribute or the value of "name" is
		 *			  an empty string ""
		 */
		void elementHandlerAction(SharedDataTable& sharedData, const AttributeMap& attributes);

		/**
		 * @brief Expat callback for Reaction element start tags.
		 *
		 * @param sharedData The shared data for the current parse.
		 * @param attributes Map of key-value pairs of attributes
		 *					 as unparsed strings.
		 *
		 * @exception No "name" attribute or the value of "name" is
		 *			  an empty string ""
		 */
		void elementHandlerReaction(SharedDataTable& sharedData, const AttributeMap& attributes);

		/**
		 * @brief Expat callback for If element start tags.
		 *
		 * @param sharedData The shared data for the current parse.
		 * @param attributes Map of key-value pairs of attributes
		 *					 as unparsed strings.
		 *
		 * @exception No "name" attribute or the value of "name" is
		 *			  an empty string ""
		 */
		void elementHandlerIf(SharedDataTable& sharedData, const AttributeMap& attributes);

		/**
		 * @brief Expat callback for Then element start tags.
		 *
		 * @param sharedData The shared data for the current parse.
		 * @param attributes Map of key-value pairs of attributes
		 *					 as unparsed strings.
		 *
		 * @exception No "class" attribute or the value of "class" is
		 *			  an empty string ""
		 * @exception No "name" attribute or the value of "name" is
		 *			  an empty string ""
		 */
		void elementHandlerThen(SharedDataTable& sharedData, const AttributeMap& attributes);

		/**
		 * @brief Expat callback for Else element start tags.
		 *
		 * @param sharedData The shared data for the current parse.
		 * @param attributes Map of key-value pairs of attributes
		 *					 as unparsed strings.
		 *
		 * @exception No "class" attribute or the value of "class" is
		 *			  an empty string ""
		 * @exception No "name" attribute or the value of "name" is
		 *			  an empty string ""
		 */
		void elementHandlerElse(SharedDataTable& sharedData, const AttributeMap& attributes);

		/**
		 * @brief Expat callback for Condition element start tags.
		 *
		 * @param sharedData The shared data for the current parse.
		 * @param attributes Map of key-value pairs of attributes
		 *					 as unparsed strings.
		 *
		 * @exception No "value" attribute or the value of "value" is
		 *			  an empty string ""
		 */
		void elementHandlerCondition(SharedDataTable& sharedData, const AttributeMap& attributes);

		/**
		 * @brief Adds attribute key-value pairs to the current
		 *		  Table as strings.
		 *
		 * @param sharedData The shared data for the current parse.
		 * @param attributes Map of key-value pairs of attributes
		 *					 as unparsed strings.
		 *
		 * @note Ignores "name" and "class" attributes.
		 */
		void addAdditionalAttributes(SharedDataTable& sharedData, const AttributeMap& attributes);


		typedef std::function<void(XmlParseHelperTable&, SharedDataTable&, const AttributeMap&)> HandlerFunctions;
		typedef HashMap<std::string, HandlerFunctions> HandlerMap;

		// hash map holding handler functions
		static HandlerMap sHandlerMap;
		
		// xml element tag names
		const static std::string sScopeElement;
		const static std::string sWorldElement;
		const static std::string sSectorElement;
		const static std::string sEntityElement;
		const static std::string sActionElement;
		const static std::string sReactionElement;

		const static std::string sIfElement;
		const static std::string sThenElement;
		const static std::string sElseElement;
		const static std::string sConditionElement;

		// xml tag attribute names
		const static std::string sNameAttribute;
		const static std::string sClassAttribute;
		const static std::string sValueAttribute;
		const static std::string sSubtypeAttribute;
	};
}
