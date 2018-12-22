// MainDlg.cpp : implementation of the CMainDlg class
//
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "MainDlg.h"	

CMainDlg::CMainDlg() : SHostWnd(_T("LAYOUT:XML_MAINWND"))
{
}

CMainDlg::~CMainDlg()
{
}

int CMainDlg::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	SetMsgHandled(FALSE);
	return 0;
}

BOOL CMainDlg::OnInitDialog(HWND hWnd, LPARAM lParam)
{
	SListView *pLvImg = FindChildByID2<SListView>(R.id.lv_thumbnail);
	m_imgAdapter.Attach(new CImageAdapter);
	pLvImg->SetAdapter(m_imgAdapter);

	CHttpDownloader::getSingletonPtr()->download("http://cdn.apc.360.cn/index.php?c=WallPaper&a=getAllCategoriesV2&from=360chrome",URL_CATEGORIES,-1);
	return 0;
}

//TODO:消息映射
void CMainDlg::OnClose()
{
	CSimpleWnd::DestroyWindow();
}

void CMainDlg::OnMaximize()
{
	SendMessage(WM_SYSCOMMAND, SC_MAXIMIZE);
}
void CMainDlg::OnRestore()
{
	SendMessage(WM_SYSCOMMAND, SC_RESTORE);
}
void CMainDlg::OnMinimize()
{
	SendMessage(WM_SYSCOMMAND, SC_MINIMIZE);
}

void CMainDlg::OnSize(UINT nType, CSize size)
{
	SetMsgHandled(FALSE);
	
	SWindow *pBtnMax = FindChildByName(L"btn_max");
	SWindow *pBtnRestore = FindChildByName(L"btn_restore");
	if(!pBtnMax || !pBtnRestore) return;
	
	if (nType == SIZE_MAXIMIZED)
	{
		pBtnRestore->SetVisible(TRUE);
		pBtnMax->SetVisible(FALSE);
	}
	else if (nType == SIZE_RESTORED)
	{
		pBtnRestore->SetVisible(FALSE);
		pBtnMax->SetVisible(TRUE);
	}
}

void CMainDlg::OnDownloadFinish(EventArgs *e)
{
	EventDownloadFinish *e2 = sobj_cast<EventDownloadFinish>(e);
	if(e2->type == URL_CATEGORIES)
	{//
		Json::Reader reader;
		Json::Value  value;
		if(reader.parse(e2->data,value))
		{
			if(value["errno"].asString() == "0")
			{
				SListBox *pLbCategory = FindChildByID2<SListBox>(R.id.lb_catetory);
				SASSERT(pLbCategory);


				Json::Value categories = value["data"];
				for(int i=0;i<categories.size();i++)
				{
					string id = categories[i]["id"].asString();
					string name = categories[i]["name"].asString();

					pLbCategory->AddString(S_CA2T(name.c_str(),CP_UTF8),0,atoi(id.c_str()));
				}
			}else
			{
				SLOG_ERROR("fatch categories error code:"<<value["error"].asString().c_str());
			}
		}else
		{
			SLOG_ERROR("parse categories failed");
		}
	}else
	{
		m_imgAdapter->OnDownloadFinish(e2->url,e2->data,e2->type,e2->catetory,e2->pImg);
	}
}

void CMainDlg::OnCategorySelChange(EventArgs *e)
{
	EventLBSelChanged *e2 = sobj_cast<EventLBSelChanged>(e);
	SListBox *pLbCategory = sobj_cast<SListBox>(e->sender);
	int category = (int)pLbCategory->GetItemData(e2->nNewSel);
	
	m_imgAdapter->setCategory(category);
}

