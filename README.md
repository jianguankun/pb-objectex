# pb-objectexd

#### 介绍
PowerBuilder工程的一般由pbl、pbt文件构成。项目中的窗体对象、函数对象、数据窗口等pb对象是存储在pbl文件中。PowerBuilder IDE 可以把pbl文件中的pb对象导出成独立的对象文件，但批量导出的功能不太友好。powerbuilder-objectex提供的工具能把pbl文件批量导出pb对象的问题。除了批量导出，还是批量导入对象到pbl文件。

#### 安装使用
bin_vc10目录中已准备好可执行程序pbex.exe，把bin_vc10目录添加到系统PATH环境变量中，则可在命令行中使用pbex。
![](https://gitee.com/jianguankun/pb-objectex/raw/master/images/1-1.png)


#### pbex export 批量导出pbl文件的pb对象文件
用法 : **pbex export \<pbl\> \<target>**
参数\<pbl\>必须：pbl文件，或pbl文件所在的目录
参数\<target\>必须：导出的pb对象文件的存放目标目录

例子1，把icclient.pbl所有对象导出到当前目录下
```
pbex export icclient.pbl .
```
![](https://gitee.com/jianguankun/pb-objectex/raw/master/images/1-2.png)

5个对象文件存放在icclient_pbl目录中

![](https://gitee.com/jianguankun/pb-objectex/raw/master/images/1-3.png)

例子2，把F:\example目录下所有pbl文件所有对象导出F:\exports目录下
```
pbex export f:\example f:\exports
```
![](https://gitee.com/jianguankun/pb-objectex/raw/master/images/1-4.png)

f:\exports有两个文件夹icclient_pbl与msgtest_pbl，分别存放f:\example的icclient.pbl与msgtest.pbl导出的对象文件

#### pbex import 批量把pb对象文件导入到pbl文件中
用法 : **pbex import \<pbobjects\> \<pbt> \<pbl\>**
参数\<pbobjects\>必须：PB对象文件目录，必须是目录，目录中所有pb对象文件为准备操作导入的文件。目录若含有子目录，子目录的文件不在导入范围。
参数\<pbt\>必须：pbt文件，pb对象文件导入到pbl文件过程需要编译，所以需要配搭一个pbt文件
参数\<pbl\>必须：目标pbl文件。注意，参数2的pbt文件需要依赖此pbl文件

pbex import意味着目标pbl有改动，为保障pbl文件，每次执行pbex import前，pbl自动备份

例子1，把F:\example\icclient_pbl的对象导入到F:\example\icclient.pbt
```
pbex import icclient_pbl icclient.pbt icclient.pbl
```
![](https://gitee.com/jianguankun/pb-objectex/raw/master/images/1-5.png)

F:\example目录下将产生新的icclient.pbl,及两个目录import-backup与import-logs

![](https://gitee.com/jianguankun/pb-objectex/raw/master/images/1-6.png)

import-backup目录是pbl文件备份，万一导入操作不当，可以找回原来的pbl文件。
import-logs目录是导入的日志，记录导入结果，与编译信息。

例子2，关于导入过程编译出错
打开F:\example\icclient_pbl\msg.srf，这是一个函数对象,制造一个错误语法，使用一个未声明的变量a
![](https://gitee.com/jianguankun/pb-objectex/raw/master/images/1-7.png)

再执行导入
```
pbex import icclient_pbl icclient.pbt icclient.pbl
```
![](https://gitee.com/jianguankun/pb-objectex/raw/master/images/1-8.png)

msg.srf函数对象导入成功，但提示语法错误。具体错误信息可查看
import-logs\icclient.log文件

![](https://gitee.com/jianguankun/pb-objectex/raw/master/images/1-9.png)

#### pbex createlib 创建pbl文件
用法 : **pbex createlib \<pblfile\> [comments]**
参数\<pblfile\>必须，将要创建的pbl文件，如果pbl文件已存在，将创建失败
参数[comments]可选，pbl说明。
```
pbex createlib icclient.pbl
```

#### pbex rebuild 编译项目
用法 : **pbex rebuild \<pbt\> [mode]**
参数\<pbt\>必须：pbt文件。注意pbt文件所依赖的pbl文件需要齐全
参数[mode]可选，默认值为FULL表示全量编译；INC表示增量编译。
例子1，编译F:\example\icclient.pbt项目
```
pbex rebuild icclient.pbt
```
pbex rebuild 作用不太，建议直接使用PowerBuilder IDE编译

#### 编译pbex
本工期使用VC++2010构建，解决方案与工程文件在builds\vc10目录下
![](https://gitee.com/jianguankun/pb-objectex/raw/master/images/1-10.png)

使用VC++2010打开sln文件，即可修改源码与编译，编译产生的pbex文件在bin_vc10目录下。
Debug版的可执行文件名为pbex_d; Release版的可执行文件名为pbex；
注意bin_vc10目录下有许多dll文件，必须与pbex_d.exe/pbex.exe一起方能运行。
