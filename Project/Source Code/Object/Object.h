/*******************************************************************
* Author	: wwyang
* Date		: 2007.3.23
* Copyright : State key CAD&CG Lab, Group of Animation 
* Head File : 
* Version   : 1.0
*********************************************************************/
// .NAME Object - Super class for object

// .SECTION Description
// It is the base class for representing objects. All other object classes
// are derived from it.

#ifndef _OBJECT_H_
#define _OBJECT_H_

#include <string> // For std::string

using namespace std;
class Object{ // Avoid redefinition error with afx
public:
	// Description:
	// Constructor and destructor
	Object();
	virtual ~Object();

	// Description:
	// New itself
	virtual Object *New() = 0;

	// Description:
	// Set/Get the name of the object
	void SetName(const string& name);
	const string& GetName() const;

	// Description:
	// Set/Get the description of the object
	void SetDescription(const string& description);
	const string& GetDescription() const;

	// Description:
	// Set/Get the status of being activated
	void SetAlive(bool bAlive);
	bool GetAlive() const;

	// Description:
	// Set/Get the visibility
	void SetVisible(bool bVisible);
	bool GetVisible() const;

	// Description:
	// Set/get id 
	void SetID(long iID);
	long GetID() const;

	// Description:
	// Set/get object layer ID
	void SetLayerLevel(long iLayerLevel);
	long GetLayerLevel() const;

	// Description:
	// Set/get parent object
	void SetParentObject(Object *pParentObject);
	Object* GetParentObject();
	const Object* GetParentObject() const;

	// Description:
	// Copy itself from another
	virtual void Copy(const Object *other); 

protected:
	// Object ID
	long _iID;
	// Name of the object
	string _name;
	// Description of the object
	string _description;
	// Parent object
	Object *_pParentObject;
	// Layer level of the object -- to indicate the layer depth of 
	// the object in the scene
	long _iLayerLevel;

	// If alive - Alive means focus on it
	bool _bAlive;
	// If visible
	bool _bVisible;

public:
	// reserved data used for algorithm
	long _iReserved;
};



#endif