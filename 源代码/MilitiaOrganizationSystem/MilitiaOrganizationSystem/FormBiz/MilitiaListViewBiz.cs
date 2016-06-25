﻿using System;
using System.Collections;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.Xml;

namespace MilitiaOrganizationSystem
{
    class MilitiaListViewBiz
    {
        private static MilitiaEditDialog militiaEditDlg = new MilitiaEditDialog();//编辑民兵对话框,应该只有主界面才会调用
        private ListView militia_ListView;
        private XmlNodeList parameters;//民兵类的参数
        private bool sort = false;
        private SqlBiz sqlBiz;//数据库业务逻辑层

        private System.Linq.Expressions.Expression<Func<Militia, bool>> lambdaContition { get; set; }//此页面的查询条件

        public int pageSize { get; set; }//每页显示多少民兵
        public int page { get; set; }//第几页
        public int maxPage { get; set; }//在加载第一页的时候初始化，为最大页数


        public MilitiaListViewBiz(ListView listView, SqlBiz sBz, System.Linq.Expressions.Expression<Func<Militia, bool>> condition)
        {
            militia_ListView = listView;
            parameters = MilitiaXmlConfig.parameters();
            sqlBiz = sBz;

            this.lambdaContition = condition;//查询条件

            page = 1;
            pageSize = 20;

            bindEvent();

            refreshCurrentPage();

            FormBizs.mListBizs.Add(this);//添加到biz池中
        }

        ~MilitiaListViewBiz()
        {
            FormBizs.mListBizs.Remove(this);
        }

        private void bindEvent()
        {
            militia_ListView.ColumnClick += Militia_ListView_ColumnClick;//点击排序
        }

        private void Militia_ListView_ColumnClick(object sender, ColumnClickEventArgs e)
        {
            // 创建一个ListView排序类的对象，并设置militia_ListView的排序器
            ListViewColumnSorter lvwColumnSorter = new ListViewColumnSorter();


            if (sort == false)
            {
                sort = true;
                lvwColumnSorter.Order = SortOrder.Descending;
            }
            else
            {
                sort = false;
                lvwColumnSorter.Order = SortOrder.Ascending;
            }
            lvwColumnSorter.SortColumn = e.Column;
            militia_ListView.ListViewItemSorter = lvwColumnSorter;

            // 用新的排序方法对ListView排序
            //this.listView1.Sort();
        }

        private void addColumnHeader()
        {
            
            foreach(XmlNode parameter in parameters)
            {
                ColumnHeader ch = new ColumnHeader();
                ch.Text = parameter.Attributes["name"].Value;   //设置列标题 
                ch.Width = 120;    //设置列宽度 
                ch.TextAlign = HorizontalAlignment.Left;   //设置列的对齐方式 
                militia_ListView.Columns.Add(ch);    //将列头添加到ListView控件。 
            }

            militia_ListView.Columns.Add("分组");
            
        }

        public void updateItem(ListViewItem lvi)
        {//用tag更新显示
            Militia militia = (Militia)lvi.Tag;
            MilitiaReflection mr = new MilitiaReflection(militia);//反射
            lvi.ImageIndex = 0;
            XmlNode firstNode = parameters[0];
            string value = "";
            try
            {
                value = mr.getProperty(parameters[0].Attributes["property"].Value).ToString();
            } catch(Exception e)
            {
                
            }
            XmlNode selectNode = null;
            if (parameters[0].Attributes["type"].Value == "enum")
            {
                selectNode = firstNode.SelectSingleNode("selection[@value='" + value + "']");
                if (selectNode != null)
                {
                    value = selectNode.Attributes["name"].Value;
                }
            }
            lvi.Text = value;
            if(lvi.SubItems.Count != parameters.Count)
            {//此lvi是新建的,则将lvi的subItems添上
                string[] items = new string[parameters.Count];
                for(int i = 0; i < items.Length; i++)
                {
                    items[i] = "";
                }
                lvi.SubItems.AddRange(items);
            }
            for (int i = 1; i < parameters.Count; i++)
            {
                XmlNode node = parameters[i];
                value = "";
                try
                {
                    value = mr.getProperty(parameters[i].Attributes["property"].Value).ToString();
                }
                catch (Exception e)
                {

                }

                if(parameters[i].Attributes["type"].Value == "enum")
                {
                    selectNode = node.SelectSingleNode("selection[@value='" + value + "']");
                    if (selectNode != null)
                    {
                        value = selectNode.Attributes["name"].Value;
                    }
                }

                lvi.SubItems[i].Text = value;
            }

            lvi.SubItems[parameters.Count].Text = militia.Group;   
        }

        public void loadMilitiaList(List<Militia> mList)
        {
            militia_ListView.Clear();//先清除所有

            addColumnHeader();

            militia_ListView.BeginUpdate();   //数据更新，UI暂时挂起，直到EndUpdate绘制控件，可以有效避免闪烁并大大提高加载速度 

            foreach (Militia militia in mList)
            {
                ListViewItem lvi = new ListViewItem();

                lvi.Tag = militia;//设置Tag

                lvi.Name = militia.Id;//设置查询的Key

                updateItem(lvi);//更新显示

                militia_ListView.Items.Add(lvi);
            }

            militia_ListView.EndUpdate();  //结束数据处理，UI界面一次性绘制。 

        }

        public void loadAllMilitiaInDB()
        {//加载数据库中所有民兵到ListView
            
            loadMilitiaList(sqlBiz.getAllMilitias());
        }

        public void loadNotGroupedMilitiasInDb()
        {//加载未分组民兵到ListView
            militia_ListView.Clear();
            int sum;
            loadMilitiaList(sqlBiz.getMilitiasByGroup("未分组", 0, 1000, out sum));
        }

        public void addOneMilitia(Militia militia)
        {//添加一个item
            ListViewItem lvi = new ListViewItem();
            lvi.Tag = militia;
            lvi.Name = militia.Id;//之前一定是存了数据库的
            updateItem(lvi);
            militia_ListView.Items.Add(lvi);
            militia_ListView.SelectedItems.Clear();
            lvi.Selected = true;
        }

        public void addOne()
        {//添加一个民兵
            Militia militia = new Militia();
            
            if(militiaEditDlg.showEditDlg(militia) == DialogResult.OK)
            {
                sqlBiz.addMilitia(militia);//先加入数据库，这样才有了Id，才能使用

                MessageBox.Show(militia.Id);

                addOneMilitia(militia);
            }
           
        }

        public void editOne(ListViewItem lvi, int focusIndex = 0)
        {//编辑一个民兵,focusIndex是弹出编辑对话框需要focus到哪个编辑框
            Militia militia = (Militia)lvi.Tag;

            if(militiaEditDlg.showEditDlg(militia, focusIndex) == DialogResult.OK)
            {
                updateItem(lvi);
                sqlBiz.updateMilitia(militia);
            }

            //通知GroupForm刷新民兵
            //((XMLGroupTaskForm)Program.formDic["GroupForm"]).updateMilitiaNode(militia);
            FormBizs.updateMilitiaItem(militia);
        }

        public void editSelectedItems()
        {//编辑所有选中的item
            foreach(ListViewItem lvi in militia_ListView.SelectedItems)
            {
                editOne(lvi);
            }
        }

        public void deleSelectedItems()
        {//删除所有选中的item
            foreach(ListViewItem lvi in militia_ListView.SelectedItems)
            {
                Militia militia = (Militia)lvi.Tag;
                FormBizs.removeMilitiaItem(militia);
                FormBizs.groupBiz.reduceCount(militia);
                militia_ListView.Items.Remove(lvi);
                sqlBiz.deleteMilitia(militia);   
            }
        }

        /*public ListViewItem findItemWithMilitia(Militia militia)
        {//根据身份证号查找民兵
            if (militia_ListView.Items.Count == 0)
            {//必须判断，否则ListView为空时会报错
                return null;
            }
            ListViewItem lvi = null;
            int startIndex = 0;
            do
            {
                lvi = militia_ListView.FindItemWithText(militia.InfoDic["CredentialNumber"], true, startIndex);//根据身份证号寻找
                startIndex = militia_ListView.Items.IndexOf(lvi) + 1;

            } while (lvi != null && !((Militia)lvi.Tag).isEqual(militia) && startIndex < militia_ListView.Items.Count);

            return lvi;
        }*/

        public ListViewItem findItemWithMilitia(Militia militia)
        {//根据民兵对象，查找此界面的民兵
            ListViewItem[] lvis = militia_ListView.Items.Find(militia.Id, false);
            foreach(ListViewItem lvi in lvis)
            {
                if(((Militia)lvi.Tag).Place == militia.Place)
                {//Place即为数据库，如果相等，说明是要找的
                    return lvi;
                }
            }
            return null;//没找到
        }


        public void updateMilitiaItem(Militia militia)
        {//刷新一个民兵的显示，（可能在分组界面更改了分组），函数被分组界面调用
            ListViewItem lvi = findItemWithMilitia(militia);
            
            if (lvi != null)
            {
                lvi.Tag = militia;
                updateItem(lvi);

            } else
            {
                MessageBox.Show("why?");
            }

        }

        public void removeMilitiaItem(Militia militia)
        {
            ListViewItem lvi = findItemWithMilitia(militia);

            if (lvi != null)
            {
                lvi.Remove();

            }
        }

        public void refreshCurrentPage()
        {
            int sum;
            List<Militia> mList = sqlBiz.queryByContition(lambdaContition, (page - 1) * pageSize, pageSize, out sum);
            maxPage = sum / pageSize + (sum % pageSize == 0 ? 0 : 1);//最大页数
            if(page > maxPage)
            {
                page = maxPage;
            }
            loadMilitiaList(mList);
        }

        public void lastPage()
        {
            if(page > 1)
            {
                page--;
            }
            refreshCurrentPage();
        }

        public void nextPage()
        {
            if(page < maxPage)
            {
                page++;
            }
            refreshCurrentPage();
        }

        public void finalPage()
        {
            page = maxPage;
            refreshCurrentPage();
        }

        public void toPage(int p)
        {
            if(p >= 1 && p <= maxPage)
            {
                page = p;
            }
            refreshCurrentPage();
        }



        class ListViewColumnSorter : IComparer
        {

            /// <summary>
            /// 指定按照哪个列排序
            /// </summary>
            private int ColumnToSort;

            /// <summary>
            /// 指定排序的方式
            /// </summary>
            private SortOrder OrderOfSort;

            /// <summary>
            /// 声明CaseInsensitiveComparer类对象，
            /// 参见ms-help://MS.VSCC.2003/MS.MSDNQTR.2003FEB.2052/cpref/html/frlrfSystemCollectionsCaseInsensitiveComparerClassTopic.htm
            /// </summary>
            private CaseInsensitiveComparer ObjectCompare;


            /// <summary>
            /// 构造函数
            /// </summary>
            public ListViewColumnSorter()
            {
                // 默认按第一列排序
                ColumnToSort = 0;

                // 排序方式为不排序
                OrderOfSort = SortOrder.None;

                // 初始化CaseInsensitiveComparer类对象
                ObjectCompare = new CaseInsensitiveComparer();
            }


            /// <summary>
            /// 重写IComparer接口.
            /// </summary>
            /// <param name="x">要比较的第一个对象</param>
            /// <param name="y">要比较的第二个对象</param>
            /// <returns>比较的结果.如果相等返回0，如果x大于y返回1，如果x小于y返回-1</returns>
            public int Compare(object x, object y)
            {
                int compareResult;
                ListViewItem listviewX, listviewY;

                // 将比较对象转换为ListViewItem对象
                listviewX = (ListViewItem)x;
                listviewY = (ListViewItem)y;

                // 比较
                compareResult = ObjectCompare.Compare(listviewX.SubItems[ColumnToSort].Text, listviewY.SubItems[ColumnToSort].Text);

                // 根据上面的比较结果返回正确的比较结果
                if (OrderOfSort == SortOrder.Ascending)
                {
                    // 因为是正序排序，所以直接返回结果
                    return compareResult;
                }
                else if (OrderOfSort == SortOrder.Descending)
                {
                    // 如果是反序排序，所以要取负值再返回
                    return (-compareResult);
                }
                else
                {
                    // 如果相等返回0
                    return 0;
                }
            }


            /// <summary>
            /// 获取或设置按照哪一列排序.
            /// </summary>
            public int SortColumn
            {
                set
                {
                    ColumnToSort = value;
                }
                get
                {
                    return ColumnToSort;
                }
            }


            /// <summary>
            /// 获取或设置排序方式.
            /// </summary>
            public SortOrder Order
            {

                set
                {
                    OrderOfSort = value;
                }
                get
                {
                    return OrderOfSort;
                }
            }
        }
    }
}
