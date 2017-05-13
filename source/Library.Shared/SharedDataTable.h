
#pragma once

#include "RTTI.h"
#include "XmlParseMaster.h"
#include "Scope.h"

namespace DOGEngine
{
	/**
	 * SharedData derived class that supports
	 * building a Scope populated using parsed
	 * Xml grammar.
	 *
	 * See these classes for information on the
	 * supported grammar:
	 *
	 *		XmlParseHelperScope.h
	 *		XmlParseHelperInt.h
	 *		XmlParseHelperFloat.h
	 *		XmlParseHelperString.h
	 *		XmlParseHelperVector.h
	 *		XmlParseHelperMatrix.h
	 */
	class SharedDataTable : public SharedData
	{
		RTTI_DECLARATIONS(SharedDataTable, SharedData)

	public:

		SharedDataTable(const SharedDataTable& other) = delete;
		SharedDataTable(SharedDataTable&& other) = delete;
		SharedDataTable& operator=(const SharedDataTable& other) = delete;
		SharedDataTable& operator=(SharedDataTable&& other) = delete;

		/**
		 * @brief Constructor.
		 */
		SharedDataTable();

		/**
		 * @brief Destructor.
		 */
		virtual ~SharedDataTable();

		/**
		 * @brief Virtual constructor. Creates a new
		 *		  SharedDataTable object.
		 *
		 * @return Returns a pointer to a new
		 *		   SharedDataTable object.
		 */
		virtual SharedDataTable* clone() override;

		/**
		 * @brief Resets the parse depth and table
		 *		  kept on this object.
		 */
		virtual void initialize() override;

		/**
		 * @brief Adopts a new Scope to this object.
		 *
		 * @param name The name of the child Scope.
		 * @param child A reference to the new child.
		 *
		 * @exception The name is an empty string ""
		 */
		void setScope(const std::string& name, Scope& child);

		/**
		 * @brief Moves the current Scope one level up, to its
		 *		  parent.
		 */
		void setScopeToParent();

		/**
		 * @brief Gives the stored Scope to the caller.
		 *
		 * @return Returns a pointer to the Scope holding
		 *		   parsed data.
		 *
		 * @note The responsibility of deleting the returned
		 *		 Scope then belongs to the caller.
		 */
		Scope* extractScope();

		/**
		 * @brief Retrieves the stored Scope.
		 *
		 * @retunr Returns a pointer to the Scope holding
		 *		   parsed data.
		 *
		 * @note The responsibility of deleting the returned
		 *		 Scope still belongs to this object.
		 */
		Scope* getScope() const;

	protected:

		Scope* mScope;
	};
}
