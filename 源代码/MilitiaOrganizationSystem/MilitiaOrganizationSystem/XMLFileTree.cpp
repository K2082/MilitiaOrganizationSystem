
// MilitiaOrganizationSystemDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "MilitiaOrganizationSystem.h"
#include "XMLFileTree.h"
#include "afxdialogex.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
public:
//	afx_msg void OnIdbtnImportgroupxml();
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
//	ON_COMMAND(IDBTN_IMPORTGROUPXML, &CAboutDlg::OnIdbtnImportgroupxml)
END_MESSAGE_MAP()


// CMilitiaOrganizationSystemDlg 对话框



XMLFileTree::XMLFileTree(CWnd* pParent /*=NULL*/)
	: CDialogEx(XMLFileTree::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void XMLFileTree::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_GROUPTREE, m_Groups);
}

BEGIN_MESSAGE_MAP(XMLFileTree, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_SIZE()
//	ON_WM_DESTROY()
ON_WM_CLOSE()
//ON_WM_RBUTTONDOWN()
//ON_WM_RBUTTONUP()
ON_NOTIFY(NM_RCLICK, IDC_GROUPTREE, &XMLFileTree::OnNMRClickGrouptree)
ON_COMMAND(ID_MENU_VIEW, &XMLFileTree::OnMenuView)
ON_COMMAND(ID_MENU_DELETE, &XMLFileTree::OnMenuDelete)
ON_COMMAND(ID_MENU_ADD, &XMLFileTree::OnMenuAdd)
ON_COMMAND(ID_MENU_MODIFY, &XMLFileTree::OnMenuModify)
ON_NOTIFY(TVN_ENDLABELEDIT, IDC_GROUPTREE, &XMLFileTree::OnTvnEndlabeleditGrouptree)
END_MESSAGE_MAP()


// CMilitiaOrganizationSystemDlg 消息处理程序

BOOL XMLFileTree::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
		}
	}

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码

	//初始化树形控件
	m_imageList.Create(32, 32, ILC_COLOR8|ILC_MASK, 0, 1);
	m_imageList.Add(AfxGetApp()->LoadIconW(IDI_ICON1));
	m_imageList.Add(AfxGetApp()->LoadIconW(IDI_ICON1));

	m_Groups.SetImageList(&m_imageList, TVSIL_NORMAL);//树形控件绑定imageList
	HTREEITEM m_TreeRoot = m_Groups.InsertItem(L"本基层编组任务");//插入根节点
	loadXMLFile(L"C:\\Users\\Hzq\\Desktop\\软工民兵组\\硬性说明\\militia.xml", m_TreeRoot);

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void XMLFileTree::OnSysCommand(UINT nID, LPARAM lParam)
{
		CDialogEx::OnSysCommand(nID, lParam);
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void XMLFileTree::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR XMLFileTree::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

//显示一个元素的所有子节点到文件树
void XMLFileTree::showXMLElementInFileTree(TiXmlElement* root_Element, HTREEITEM tree_Root) {
	TiXmlElement* firstElement = root_Element->FirstChildElement();
	if(firstElement == NULL) {
		return;
	}
	HTREEITEM treeTemp = m_Groups.InsertItem(CodeTranslater::UTF8ToGBK(firstElement->FirstAttribute()->Value()), 1, 0, tree_Root);
	showXMLElementInFileTree(firstElement, treeTemp);
	TiXmlElement* nextElement;
	while( (nextElement = firstElement->NextSiblingElement()) != NULL) {
		firstElement = nextElement;
		treeTemp = m_Groups.InsertItem(CodeTranslater::UTF8ToGBK(nextElement->FirstAttribute()->Value()), 1, 1, tree_Root);
		showXMLElementInFileTree(nextElement, treeTemp);
	}
}

//显示文件树
void XMLFileTree::loadXMLFile(CString str_FilePath, HTREEITEM tree_Root) {

	USES_CONVERSION;
	TiXmlDocument doc(T2A(str_FilePath));
	bool loadOk = doc.LoadFile();
	if(!loadOk) {
		m_Groups.InsertItem(L"没有此文件！加载失败！\n", 0, 0, tree_Root);
		return;
	}
	TiXmlElement* rootElement = doc.RootElement();
	
	showXMLElementInFileTree(rootElement, tree_Root);

}



//void CAboutDlg::OnIdbtnImportgroupxml()
//{
//	// TODO: 在此添加命令处理程序代码
//}


void XMLFileTree::OnSize(UINT nType, int cx, int cy)
{//让组件随窗口变化而自适应
	CDialogEx::OnSize(nType, cx, cy);

	// TODO: 在此处添加消息处理程序代码
	if(m_Groups.GetSafeHwnd()) {
		m_Groups.MoveWindow(0, 0, cx, cy);
	}
}


//void XMLFileTree::OnDestroy()
//{
//	CDialogEx::OnDestroy();
//
//	// TODO: 在此处添加消息处理程序代码
//}


void XMLFileTree::OnClose()
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	ShowWindow(SW_HIDE);
}

HTREEITEM XMLFileTree::getSelectItem() {
	CPoint pt;
    GetCursorPos(&pt);//得到当前鼠标的位置
	m_Groups.ScreenToClient(&pt);//将屏幕坐标转换为客户区坐标
    HTREEITEM tree_Item = m_Groups.HitTest(pt);//调用HitTest找到对应点击的树节点
    return tree_Item;
}



void XMLFileTree::OnNMRClickGrouptree(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: 在此添加控件通知处理程序代码
	//临时鼠标的屏幕坐标，用来弹出menu
    CPoint ScreenPt;
    GetCursorPos(&ScreenPt);

    //获取到当前鼠标选择的树节点
	HTREEITEM selectItem = getSelectItem();
    if (selectItem != NULL)
    {
		m_Groups.SelectItem(selectItem); //使右键单击的树节点被选中
			
        CMenu menu;
        menu.LoadMenuW(IDR_MENU3);
        CMenu* pPopup = menu.GetSubMenu(0);//装载第一个子菜单，即我们菜单的第一列
        pPopup->TrackPopupMenu(TPM_LEFTALIGN, ScreenPt.x, ScreenPt.y, this);//弹出菜单
        }
	*pResult = 0;
}


void XMLFileTree::OnMenuView()
{
	// TODO: 在此添加命令处理程序代码
}


void XMLFileTree::OnMenuDelete()
{
	// TODO: 在此添加命令处理程序代码
	HTREEITEM selectItem = m_Groups.GetSelectedItem();
	if(m_Groups.GetChildItem(selectItem) == NULL) {//如果本结点是叶结点，直接删除（还应确认里面是否有民兵，之后添加）
		m_Groups.DeleteItem(selectItem);
	} else {//让用户确认是否删除
		MessageBox(_T("hello world!"),_T("提示!"));
	}
}


void XMLFileTree::OnMenuAdd()
{
	// TODO: 在此添加命令处理程序代码
	HTREEITEM selectItem = m_Groups.GetSelectedItem();
	HTREEITEM addItem = m_Groups.InsertItem(_T("新建组"), 1, 1, selectItem);
	m_Groups.EditLabel(addItem);//编辑
}


void XMLFileTree::OnMenuModify()
{
	// TODO: 在此添加命令处理程序代码
	HTREEITEM selectItem = m_Groups.GetSelectedItem();
	m_Groups.EditLabel(selectItem);//编辑
	
}


void XMLFileTree::OnTvnEndlabeleditGrouptree(NMHDR *pNMHDR, LRESULT *pResult)
{//当标签编辑完成时
	LPNMTVDISPINFO pTVDispInfo = reinterpret_cast<LPNMTVDISPINFO>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	CString strText;

	m_Groups.GetEditControl()->GetWindowTextW(strText);

	m_Groups.SetItemText(pTVDispInfo->item.hItem, strText);

	*pResult = 0;
}
