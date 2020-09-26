/*******************************************************************
* Author	: wwyang
* Date		: 2007.3.30
* Copyright : State key CAD&CG Lab, Group of Animation 
* Version   : 1.0
*********************************************************************/
// .NAME CBaseWindow - It's an abstract class for window 

// .SECTION Description
// This class gives an unified definition of window related to the 
// framework. The functions in this class will be used freely by 
// other classes in the framework without considering system dependence
// of window 

#ifndef _BASEWINDOW_H_
#define _BASEWINDOW_H_

#include <QObject>

#pragma warning(disable:4267)

#include "BaseRender.h" // For window's renderer
#include <list> // For list

class CBaseWindow: public QObject{  
public:
	CBaseWindow(){}
	virtual ~CBaseWindow()
	{
		if(_listRender.size() != 0){
			int i = 0, size = _listRender.size();
			std::list<CBaseRender*>::iterator it = _listRender.begin();
			CBaseRender *pRender;
			
			for(i = 0; i < size; i ++){
				pRender = *it;
				it ++;
				delete pRender;
			}

			_listRender.clear();
		}
	}

	// Description:
	// Add the render in this window
	inline void AddRenderer(CBaseRender *pRender);

	// Description:
	// Delete the render in this window
	inline void DeleteRender(CBaseRender *pRender);

	// Description:
	// Delete all renders
	inline void DeleteRenders();


	// Description:
	// Rendering objects in the window using the attached renderers
	inline virtual void Render();

protected:
	std::list<CBaseRender*> _listRender; // Note: the render is managed by window -- free and storing
};

inline void CBaseWindow::AddRenderer(CBaseRender *pRender)
{
	if(pRender == 0){
		return;
	}

	pRender->SetParentWindow(this);

	// Add to list
	_listRender.push_back(pRender);

	return;
}

inline void CBaseWindow::DeleteRenders()
{
	std::list<CBaseRender*>::iterator it = _listRender.begin();
	while(it != _listRender.end()){
		delete *it;
		it ++;
	}

	_listRender.clear();
}

inline void CBaseWindow::DeleteRender(CBaseRender *pRender)
{
	if(pRender == 0){
		return;
	}
	else{
		// Find the render in list
		std::list<CBaseRender*>::iterator it = _listRender.begin();
		while(it != _listRender.end()){
			if(*it == pRender){
				delete *it;
				_listRender.erase(it);

				return;
			}
			it ++;
		}
	}
	
	return;
}

inline void CBaseWindow::Render()
{
	// Do all the renderers
	std::list<CBaseRender*>::iterator it = _listRender.begin();
	while(it != _listRender.end()){
		(*it)->Render();
		it ++;
	}

	return;
}

#endif