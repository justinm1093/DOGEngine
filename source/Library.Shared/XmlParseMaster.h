
#pragma once

#include "Vector.h"
#include "SList.h"

#include "IXmlParseHelper.h"
#include "RTTI.h"

#include "expat.h"

namespace DOGEngine
{
	/**
	 * Class that wraps Expat functionality to parse
	 * Xml files using a chain of responsibility of
	 * registered IXmlParseHelper objects.
	 *
	 * The registered IXmlParseHelpers are uniquely
	 * typed.
	 */
	class SharedData;
	class XmlParseMaster final : public RTTI
	{
		friend class SharedData;
		RTTI_DECLARATIONS(XmlParseMaster, RTTI)

	public:

		XmlParseMaster(const XmlParseMaster& other) = delete;
		XmlParseMaster(XmlParseMaster&& other) = delete;
		XmlParseMaster& operator=(const XmlParseMaster& other) = delete;
		XmlParseMaster& operator=(XmlParseMaster&& other) = delete;

		/**
		 * @brief Constructor.
		 *
		 * @param data The SharedData object that this object adopts.
		 *
		 * @note XmlParseMasters 
		 */
		explicit XmlParseMaster(SharedData& data);

		/**
		 * @brief Destructor.
		 *
		 * @note XmlParseMasters are not responsible for deleting their
		 *		 helpers EXCEPT in the case where the XmlParseMaster is
		 *		 a clone.
		 */
		~XmlParseMaster();

		/**
		 * @brief Virtual constructor. Creates a new XmlParseMaster in a
		 *		  default state. Also clones the original SharedData and
		 *		  parse helpers to the new object.
		 *
		 * @return Returns a pointer to a new XmlParseMaster with cloned
		 *		   SharedData and parse helpers.
		 */
		XmlParseMaster* clone();

		/**
		 * @brief Initializes this object and its SharedData and helpers
		 *		  to states ready to parse.
		 */
		void initialize();

		/**
		 * @brief Says whether this object already has a registered
		 *		  parse helper of the given type.
		 *
		 * @param typeID The RTTI type ID to check.
		 *
		 * @return Returns true if this object already has a parse
		 *		   helper of the given type in its chain of
		 *		   responsibility. Otherwise, returns false.
		 */
		bool hasHelper(std::uint64_t typeID) const;

		/**
		 * @brief Registers a parse helper with the chain of responsibility
		 *		  managed by this object.
		 *
		 * @param helper A reference to the parse helper being added.
		 *
		 * @return Returns true if the helper requesting add was not in the
		 *		   chain and was added. Returns false if the chain already
		 *		   had a helper of that type and the given helper was not
		 *		   added.
		 */
		bool addHelper(IXmlParseHelper& helper);

		/**
		 * @brief Removes the given helper from the chain of responsibility.
		 *
		 * @param helper A reference to the parse helper being remvoed.
		 *
		 * @return Returns true if the helper was in the chain and was
		 *		   removed. Otherwise, returns false.
		 */
		bool removeHelper(IXmlParseHelper& helper);

		/**
		 * @brief Parses the given buffer using expat.
		 *
		 * @param buffer The buffer containing the Xml document to parse.
		 *				 This buffer is not null-terminated.
		 * @param length The length in bytes of the buffer.
		 * @param isFinal A boolean flag that says whether the buffer
		 *				  represents the final chunk of data.
		 * @param isFirstChunk A boolean flag that says whether the buffer
		 *					   represents the first chunk of data. Used to
		 *					   handle parse initialization. Defaults to 'true'
		 *
		 * @return Returns true if the Xml parse finished with no errors
		 *		   reported. Otherwise, returns false.
		 *
		 * @exception This object's SharedData is null.
		 * @exception Buffer is null.
		 * @exception Xml parsing errors throw exceptions.
		 */
		bool parse(char* buffer, std::uint32_t length, bool isFinal, bool isFirstChunk = true);

		/**
		 * @brief Parses the Xml document at the given filepath using expat.
		 *
		 * @param fileName The relative path of the Xml document being
		 *				   parsed.
		 *
		 * @return Returns true if the Xml parse finished with no errors
		 *		   reported. Returns false if the Xml parse could not
		 *		   be completed or reported any errors.
		 *
		 * @exception This object's SharedData is null.
		 * @exception Xml parsing errors throw exceptions.
		 * 
		 * @note No exception is thrown if the given filepath is not valid.
		 *		 This method will simply return false without parsing.
		 */
		bool parseFromFile(const std::string& fileName);

		/**
		 * @brief Sets the SharedData for this object.
		 *
		 * @param data A pointer to the SharedData object being adopted.
		 *			   This may be null.
		 *
		 * @note If the incoming SharedData already has an XmlParseMaster,
		 *		 the old master is disconnected.
		 * @note If this object already has a SharedData, that SharedData 
		 *		 is disconnected.
		 */
		void setSharedData(SharedData& data);

		/**
		* @brief Sets the name of the file currently being parsed.
		*
		* @param fileName The relative path to the file being
		*				   parsed.
		*/
		void setFileName(const std::string& fileName);

		/**
		* @brief Retrieves the name of the file currently being
		*		  parsed.
		*
		* @return Returns the relative path to the file being
		*		   parsed.
		*/
		const std::string& getFileName() const;

		/**
		 * @brief Retrieves this object's SharedData.
		 *
		 * @return Returns a pointer to this object's SharedData.
		 */
		SharedData* getSharedData() const;

		/**
		 * @brief Retrieves this object's list of parse helpers.
		 *
		 * @return Returns a const reference to the list of
		 *		   parse helpers.
		 */
		const Vector<IXmlParseHelper*>& getHelpers() const;

		/**
		 * @brief Retrieves the number of parse helpers registered with this
		 *		  object.
		 *
		 * @return Returns the number of IXmlParseHelpers in this object's
		 *		   chain of responsibility.
		 */
		std::uint32_t numHelpers() const;

		/**
		 * @brief Says whether this object is the result of cloning another
		 *		   XmlParseMaster.
		 *
		 * @return Returns true if this object is a clone. Otherwise,
		 *		   returns false.
		 */
		bool isClone() const;

	private:

		/**
		 * @brief Expat callback function that starts the chain of
		 *		  responsibility for handling Xml start-element tags.
		 *
		 * @param userData Pointer to the SharedData belonging to the
		 *				   XmlParseMaster object that started the parse.
		 * @param name The name of the tag.
		 * @param attributes An array of attribute name-value pairs.
		 *					 Even-numbered entries are attribute names.
		 *					 Odd-numbered entries are attribute values.
		 */
		static void startElementHandler(void* userData, const XML_Char* name, const XML_Char** attributes);

		/**
		 * @brief Expat callback function that starts the chain of
		 *		  responsibility for handling Xml end-element tags.
		 *
		 * @param userData Pointer to the SharedData belonging to the
		 *				   XmlParseMaster object that started the parse.
		 * @param name The name of the tag.
		 */
		static void endElementHandler(void* userData, const XML_Char* name);

		/**
		 * @brief Expat callback function that starts the chain of
		 *		  responsibility for handling Xml character data that
		 *		  appears between start and end element tags.
		 *
		 * @param userData Pointer to the SharedData belonging to the
		 *				   XmlParseMaster object that started the parse.
		 * @param buffer The character data being handled.
		 * @param length The length in bytes of the character data being handled.
		 *
		 * @note Note that the buffer is not null-terminated and requires the
		 *		 length argument for proper handling.
		 */
		static void charDataHandler(void* userData, const XML_Char* buffer, int32_t length);

		XML_Parser mParser;

		std::string mFileName;

		Vector<IXmlParseHelper*> mHelpers;
		SharedData* mSharedData;

		bool mIsClone;

		const static std::uint32_t sBufferSize;
	};

	/**
	 * Abstract base class representing data shared between
	 * the XmlParseMaster and its registered helpers.
	 *
	 * Expat passes around a pointer to an object of
	 * this type  to its registered callback methods.
	 */
	class SharedData : public RTTI
	{
		friend class XmlParseMaster;
		RTTI_DECLARATIONS(SharedData, RTTI)

	public:

		SharedData(const SharedData& other) = delete;
		SharedData(SharedData&& other) = delete;
		SharedData& operator=(const SharedData& other) = delete;
		SharedData& operator=(SharedData&& other) = delete;

		/**
		 * @brief Constructor.
		 */
		SharedData();

		/**
		 * @brief Destructor.
		 */
		virtual ~SharedData();

		/**
		 * @brief Abstract virtual constructor. Implement to create
		 *		  a new SharedData derived class.
		 */
		virtual SharedData* clone() = 0;

		/**
		 * @brief Resets this object to a default state. Derived
		 *		  classes should call upon the base class
		 *		  implementation.
		 */
		virtual void initialize();

		/**
		 * @brief Increments the saved depth of the current Xml
		 *		  parse.
		 */
		void incrementDepth();

		/**
		 * @brief Decrements the saved depth of the current Xml
		 *		  parse.
		 */
		void decrementDepth();

		/**
		 * @brief Pushes a new element name to the stack. This
		 *		  tracks the current nested element in the Xml.
		 *
		 * @param name The element being pushed.
		 */
		void pushNewElement(const std::string& name);

		/**
		 * @brief Validates that the end element on top of the
		 *		  stack matches the end element just read.
		 *
		 * @param name The element that has just been read.
		 *
		 * @exception The read element "name" does not match
		 *			  the expected element in the stack.
		 */
		void popElement(const std::string& name);

		/**
		 * @brief Retrieves the XmlParseMaster that owns this object.
		 *
		 * @return Returns a pointer to the XmlParseMaster that owns
		 *		   this object.
		 */
		XmlParseMaster* getXmlParseMaster() const;

		/**
		 * @brief Retrieves the currently depth of the Xml parse.
		 *
		 * @return Returns the current tag depth in the Xml parse.
		 */
		std::uint32_t depth() const;

	private:

		/**
		 * @brief Sets the XmlParseMaster for this object.
		 *
		 * @param master The XmlParseMaster adopting this object.
		 *
		 * @note If this object already has an XmlParseMaster, the
		 *		 old master is disconnected.
		 * @note Disconnects the incoming XmlParseMaster from any
		 *		 SharedData that it may have.
		 */
		void setXmlParseMaster(XmlParseMaster& master);

		XmlParseMaster* mMaster;
		std::uint32_t mDepth;

		SList<std::string> mElementStack;
	};
}
