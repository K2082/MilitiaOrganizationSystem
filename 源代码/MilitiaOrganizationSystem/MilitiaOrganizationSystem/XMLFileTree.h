
// MilitiaOrganizationSystemDlg.h : 头文件
//

#pragma once
#include "afxcmn.h"
#include "tinyxml\tinystr.h"
#include "tinyxml\tinyxml.h"
#include "tinyxml\CodeTranslater.h"


// CMilitiaOrganizationSystemDlg 对话框
class XMLFileTree : public CDialogEx
{
// 构造
public:
	XMLFileTree(CWnd* pParent = NULL);	// 标准构造函数
	
// 对话框数据
	enum { IDD = IDD_XMLFileTree };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	CTreeCtrl m_Groups;
	CImageList m_imageList;
	
	void loadXMLFile(CString str_Dir, HTREEITEM tree_Root);
	HTREEITEM getSelectItem();
private:
	void showXMLElementInFileTree(TiXmlElement* root_Element, HTREEITEM tree_Root);
public:
	afx_msg void OnSize(UINT nType, int cx, int cy);
//	afx_msg void OnDestroy();
	afx_msg void OnClose();
//	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
//	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnNMRClickGrouptree(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnMenuView();
	afx_msg void OnMenuDelete();
	afx_msg void OnMenuAdd();
	afx_msg void OnMenuModify();
	afx_msg void OnTvnEndlabeleditGrouptree(NMHDR *pNMHDR, LRESULT *pResult);
};
