#include <engine/graphics.h>
#include <engine/textrender.h>
#include <engine/shared/config.h>
#include <game/generated/protocol.h>
#include <game/generated/client_data.h>

#include <game/gamecore.h> // get_angle
#include <game/client/ui.h>
#include <game/client/render.h>
#include "buildmenu.h"
#include <game/buildings_info.h>

#include <game/localization.h>

CBuildmenu::CBuildmenu()
{
	OnReset();
}

void CBuildmenu::ConKeyBuildmenu(IConsole::IResult *pResult, void *pUserData)
{
	CBuildmenu *pSelf = (CBuildmenu *)pUserData;
	if(!pSelf->m_pClient->m_Snap.m_SpecInfo.m_Active && pSelf->Client()->State() != IClient::STATE_DEMOPLAYBACK)
		pSelf->m_Active = pResult->GetInteger(0) != 0;
}

void CBuildmenu::OnConsoleInit()
{
	Console()->Register("+buildmenu", "", CFGFLAG_CLIENT, ConKeyBuildmenu, this, "Open the build menu");
}

void CBuildmenu::OnReset()
{
	m_WasActive = false;
	m_Active = false;
	m_SelectedBuilding = -1;
}

void CBuildmenu::OnRelease()
{
	m_Active = false;
}

//NODESTODO : Find out why there is emptyed
/*void CBuildmenu::on_message(int msgtype, void *rawmsg)
{
	if(msgtype == NETMSGTYPE_SV_EMOTICON)
	{
		NETMSG_SV_EMOTICON *msg = (NETMSG_SV_EMOTICON *)rawmsg;
		gameclient.clients[msg->cid].emoticon = msg->emoticon;
		gameclient.clients[msg->cid].emoticon_start = client_tick();
	}	
} */

void CBuildmenu::OnMessage(int MsgType, void *pRawMsg)
{
}

bool CBuildmenu::OnMouseMove(float x, float y)
{
	if(!m_Active)
		return false;

	UI()->ConvertMouseMove(&x, &y);
	m_SelectorMouse += vec2(x,y);
	return true;
}
	
void CBuildmenu::OnRender()
{
	if(!m_Active)
	{
		if(m_WasActive && m_SelectedBuilding != -1)
			Build(m_SelectedBuilding);
		m_WasActive = false;
		return;
	}
	
	if(m_pClient->m_Snap.m_SpecInfo.m_Active)
	{
		m_Active = false;
		m_WasActive = false;
		return;
	}
	
	m_WasActive = true;
	
	int dx = 96;

	if(m_SelectorMouse.x < -191-dx)
		m_SelectorMouse.x = -191-dx;
		
	if(m_SelectorMouse.x > 191-dx)
		m_SelectorMouse.x = 191-dx;
		
	if(abs(m_SelectorMouse.y) > 143)
		m_SelectorMouse.y = sign(m_SelectorMouse.y)*143;
	
//	if (length(selector_mouse) > 140)
//		selector_mouse = normalize(selector_mouse) * 140;

	int col = (m_SelectorMouse.x + 192+dx) / 96;
	int row = (m_SelectorMouse.y + 144) / 96;

//	float selected_angle = get_angle(selector_mouse) + 2*pi/24;
//	if (selected_angle < 0)
//		selected_angle += 2*pi;

	//if (length(selector_mouse) > 100)
	//	selected_emote = (int)(selected_angle / (2*pi) * 12.0f);
	m_SelectedBuilding = row*4+col;

	float tw = TextRender()->TextWidth(0, 24, "Tech 3 ", -1);

    CUIRect Screen = *UI()->Screen();

	Graphics()->MapScreen(Screen.x, Screen.y, Screen.w, Screen.h);

	Graphics()->BlendNormal();

	Graphics()->TextureSet(-1);
	Graphics()->QuadsBegin();
	Graphics()->SetColor(0,0,0,0.5f);
	
	int height = 96;
	int width = 96;
	int left = Screen.w/2-192-dx;
	
	RenderTools()->DrawRoundRect(left, Screen.h/2-height-height/2, 4*width, height-1, 17.0f);
	if(m_pClient->m_Tech[m_pClient->m_Team] < 2)
		Graphics()->SetColor(1.0f,0,0,0.5f);
	RenderTools()->DrawRoundRect(left, Screen.h/2-height/2, 4*width, height-2, 17.0f);
	if(m_pClient->m_Tech[m_pClient->m_Team] < 3)
		Graphics()->SetColor(1.0f,0,0,0.5f);
	RenderTools()->DrawRoundRect(left, Screen.h/2+height/2, 4*width, height, 17.0f);

	Graphics()->SetColor(0,0,0,0.5f);

	
	
	RenderTools()->DrawRoundRect(left+4*width+2, Screen.h/2-height-height/2, 2*width+width/2, 3*height, 17.0f);
	Graphics()->QuadsEnd();
	
	TextRender()->Text(0, left-tw, Screen.h/2-144+48-18, 24, "Tech 1", -1);
	TextRender()->Text(0, left-tw, Screen.h/2-18, 24, "Tech 2", -1);
	TextRender()->Text(0, left-tw, Screen.h/2+48+48-18, 24, "Tech 3", -1);

	TextRender()->Text(0, left+4*width+15, Screen.h/2-144+7, 24, buildings_info[m_SelectedBuilding].name, -1);
	char buf[512];
	str_format(buf, sizeof(buf), "Price: %d BP\n\n%s", buildings_info[m_SelectedBuilding].price, buildings_info[m_SelectedBuilding].desc);
	TextRender()->Text(0, left+4*width+15, Screen.h/2-144+38, 14, buf, -1);
	

	Graphics()->TextureSet(g_pData->m_aImages[IMAGE_BUILDMENU].m_Id);
	Graphics()->QuadsBegin();

/*
	for (int i = 0; i < 12; i++)
	{
		float angle = 2*pi*i/12.0;
		if (angle > pi)
			angle -= 2*pi;

		int row = i / 4;
		int col = i % 4;
		
		bool selected = m_SelectedBuilding == i && (row < g_GameClient.m_Tech[g_GameClient.m.Team]);

		//float size = selected ? 96 + 10*sin((client_tick()%75)/75.0f *2*pi) : 64;
		
		//float size = selected ? 96 : 64;
		//dbg_msg("nodes", "%f", size);

		//float nudge_x = 120 * cos(angle);
		//float nudge_y = 120 * sin(angle);
		
		float nudge_x = -144-dx+96*col;
		float nudge_y = -96+96*row;
		
		select_sprite(SPRITE_E_REACTOR + i);
		gfx_quads_draw(screen.w/2 + nudge_x, screen.h/2 + nudge_y, size, size);
	}
*/
	for (int i = 0; i < NUM_BUILDINGS; i++)
	{
		float Angle = 2*pi*i/NUM_BUILDINGS;
		if (Angle > pi)
			Angle -= 2*pi;

		int row = i / 4;
		int col = i % 4;
		
		bool Selected = m_SelectedBuilding == i && (row < m_pClient->m_Tech[m_pClient->m_Team]);

		float Size = Selected ? 80.0f : 50.0f;

		float NudgeX = -144-dx+96*col;;
		float NudgeY = -96+96*row;
		
		RenderTools()->SelectSprite(SPRITE_E_REACTOR + i);
		IGraphics::CQuadItem QuadItem(Screen.w/2 + NudgeX, Screen.h/2 + NudgeY, Size, Size);
		Graphics()->QuadsDraw(&QuadItem, 1);
	}

	Graphics()->QuadsEnd();

	Graphics()->TextureSet(g_pData->m_aImages[IMAGE_CURSOR].m_Id);
	Graphics()->QuadsBegin();
	Graphics()->SetColor(1,1,1,1);
	IGraphics::CQuadItem QuadItem(m_SelectorMouse.x+Screen.w/2,m_SelectorMouse.y+Screen.h/2,24,24);
	Graphics()->QuadsDrawTL(&QuadItem, 1);
	Graphics()->QuadsEnd();
}

bool CBuildmenu::IsActive() {
	return m_Active;
}

void CBuildmenu::Build(int Building)
{
	CNetMsg_Cl_Building Msg;
	Msg.m_Building = Building;
	Client()->SendPackMsg(&Msg, MSGFLAG_VITAL);
}
