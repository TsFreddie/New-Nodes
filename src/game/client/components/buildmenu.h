#ifndef GAME_CLIENT_COMPONENTS_BUILDMENU_H
#define GAME_CLIENT_COMPONENTS_BUILDMENU_H
#include <base/vmath.h>
#include <game/client/component.h>

class CBuildmenu : public CComponent
{	
	bool m_WasActive;
	bool m_Active;
	
	vec2 m_SelectorMouse;
	int m_SelectedBuilding;

	static void ConKeyBuildmenu(IConsole::IResult *pResult, void *pUserData);
	
public:
	
	CBuildmenu();
	
	virtual void OnReset();
	virtual void OnConsoleInit();
	virtual void OnRender();
	virtual void OnRelease();
	virtual void OnMessage(int MsgType, void *pRawMsg);
	virtual bool OnMouseMove(float x, float y);
	virtual bool IsActive();
	
	void Build(int Building);
};


#endif