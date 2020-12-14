说明：
这是一个轻量级程序，它不是为处理大量数据设计的，目前抓包或处理数据限制60万包，可以在ui或者脚本里面过滤。
分析数据是它的强项，lua或者c语言作为脚本，虚拟机注册了方便的调试接口。

2015.9.24：
1.增加lua函数：dbgview输出到 dbgview.exe
2.trace函数测试通过：trace("stringxxx")
3.bug：切换插件数据安全性操作

2015.9.15：
1.只需要PowerSniff.exe就可以运行，其它文件为插件和数据，三方工具
2.当前版本使用windows raw socket, 文件格式为.cap（兼容wireshark）
3.免费软件（不开源），版权所有，用户使用造成不便与损失和作者无关

2015.9.30:
1.内嵌tcc（作为插件，没有tcc也可以正常运行），支持c语言脚本


后续计划（乱写的）：
(1)exception捕捉到gui
(2)winpcap driver支持
(3)更多协议目前只有tcp/udp/icmp
(4)python脚本
(5)数据导出html				优先
(6)托盘显示
(7)命令行支持
(8)更多插件编写				优先
(9)admin权限下自动添加防火墙		优先
(10)theme颜色待调
(11)发送数据
(12)listview排序
(13)进程抓包
(14)sock5代理
(15)支持tcc和python			优先
