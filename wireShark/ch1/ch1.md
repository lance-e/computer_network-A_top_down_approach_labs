# wireshark : warm-up

### 尝试wireshark抓包
- 打开浏览器访问http://gaia.cs.umass.edu/wireshark-labs/INTRO-wireshark-file1.html 课程网址
- 输入http过滤词，过滤抓取的包


![效果](./image/1.png)

### 需要提交的内容

第一个实验的目的主要是向你介绍Wireshark。以下问题将证明您已经能够使Wireshark启动并运行，并且已经探索了其中一些功能。根据您的Wireshark实验回答以下问题：

1. 列出上述步骤7中出现在未过滤的分组列表窗口的协议列中的3种不同的协议。
- 答：TCP，DNS，TLSv1.2
2. 从HTTP GET消息发送到HTTP OK回复需要多长时间？ (默认情况下，分组列表窗口中的时间列的值是自Wireshark开始捕获以来的时间(以秒为单位)。要想以日期格式显示时间，请选择Wireshark的“视图”下拉菜单，然后选择“时间显示格式”，然后选择“日期和时间”。)
- 答：20:29:18.241296-20:29:17.925291 = 0.316005
3. gaia.cs.umass.edu(也称为wwwnet.cs.umass.edu)的Internet地址是什么？您的计算机的Internet地址是什么？
- gaia.cs.umass.edu地址：128.119.245.12
- 我的地址；192.168.28.5
4. 打印问题2提到的两个HTTP消息(GET和OK)。要这样做，从Wireshark的“文件”菜单中选择“打印”，然后选择“仅选中分组”和“按当前显示”按钮，然后单击确定。
- 略