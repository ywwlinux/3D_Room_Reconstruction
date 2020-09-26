/*******************************************************************
* Author	: wwyang
* Date		: 2008.2.25
* Copyright : State key CAD&CG Lab, Group of Animation 
* Head File : 
* Version   : 1.0
*********************************************************************/
// .NAME CBaseRender - super class of renderer for a given object 

// .SECTION Description
// This class is for rendering of a given object. The object can be
// any data such as graphical object, visualization data, or living command.
// Remember to create a derived class for a specified rendering effect or object.
// Also the renderers can be composited to form a new render. 

// Note: a render can only be used by one window. It is necessary to create several renders
// for the same object rendering in several windows.

#ifndef _BASERENDER_H_
#define _BASERENDER_H_

class CBaseWindow;

class CBaseRender{

	friend class CBaseWindow;
public:
	CBaseRender() : _pParentWnd(0){}

	// Description:
	// Set the attached window
	void SetParentWindow(CBaseWindow* pParentWnd)
	{
		_pParentWnd = pParentWnd;
	}

	// Description:
	CBaseWindow* GetParentWindow()
	{
		return _pParentWnd;
	}

	// Description:
	// Render for the parent window with content of application for the window
	void Render()
	{
		RenderImplement();

		return;
	}

protected:
	// Description:
	// Actual rendering of the specified object(s)
	virtual void RenderImplement() = 0;

protected:
	CBaseWindow *_pParentWnd; // Used when rendering objects; It should be converted to a QPaintDevice if using
	                          // QPainter to render 
};


#endif