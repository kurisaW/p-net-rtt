# P-NET

> 此存储库是对开源 P-Net 评估版本在 RT-Thread 上的移植，用于 Profinet 设备实现的 P-Net 协议栈。

## 1.前期准备

软件环境

* [CODESYS](https://us.store.codesys.com/)（profinet主站模拟）
  * CODESYS
  * CODESYS Gateway（网关设备）
  * CODESYS Control Win SysTray（PLC设备）
* [PRONETA](https://support.industry.siemens.com/cs/attachments/67460624/proneta_3_8_0_0.zip)

硬件环境

* 附带ETH的微控制器（eth驱动需支持 RT-Thread 以太网驱动框架）

## 2.软件包配置

打开 bsp 下的 env界面，进入 **->RT-Thread online packages->IoT**，找到 **[*] P-Net stack for Profinet device implementation  --->** 使能，同时留出配置提供给用户：

```md
-*- Default netif name for P-NET  --->
	-> (e00) default ethernet interface name for p-net app, default as 'e00
-*- Enable P-NET sample board config  --->
	-> (0x0209) p-ent user led pin
	-> (0x0005) p-net user key pin
-*- Default root filesystem path for P-NET  --->
    -> [*] p-net using ramfs filesystem by default, or you can turn this off and choose another way to enable the filesystem
    -> (8192)  default memory size for ramfs
-*- P-NET sample slave network ip config  --->
    -> (192.168.137.196) set static ip address for profinet slaver
    -> (192.168.137.1) set static gateway address for profinet slaver
    -> (255.255.255.0) set static mask address for profinet slaver
    version (latest)  --->
```

* **Default netif name for p-net**：p-net 网卡设备接口名称，默认为 e00 ；
* **Enable pnet sample board config**：p-net app 用户LED及按键配置；
* **Default root filesystem path for p-net**：p-net 文件系统配置，默认使用 ramfs ，默认分配 8K 内存空间；
* **P-NET sample slave network ip config**：p-net 从站设备静态IP配置（**请关闭 RT_LWIP_DHCP 功能，使用静态IP**）

完成上述配置后，将程序编译下载至开发板。

## 3.网络配置

我们使用一根网线连接开发板与PC，同时在PC端配置静态IP：

![image-20241108100915339](figures/image-20241108100915339.png)

检查开发板端的 IP 信息，并测试联通性：

![image-20241108101125803](figures/image-20241108101125803.png)

## 4.软PLC主站启动

> CODESYS简介：CODESYS是德国3S公司开发的PLC软件，集成了PLC逻辑、运动控制、组态显示等功能。CODESYS，全称为“Controller Development System”，是一种基于 IEC 61131-3 标准的工业自动化编程工具。它不仅支持多种编程语言（如梯形图、结构化文本、功能块图等），还提供了丰富的库和功能模块，帮助工程师快速开发和调试 PLC（可编程逻辑控制器）和工业控制系统。CODESYS 的灵活性和强大功能使其成为工业自动化领域广泛使用的开发平台。

### 4.1 CODESYS创建标准工程

请确保已安装CODESYS软件，安装之后下面这三个是我们需要用到的软件：

![image-20241108112037518](figures/image-20241108112037518.png)

* CODESYS V3.5 SP20 Patch 3：Profinet 主站模拟
* CODESYS Gateway V3：网关设备
* CODESYS Control Win V3 -x64 SysTray：软PLC设备

首先打开 **CODESYS V3.5 SP20 Patch 3**，依次选择 -> 新建工程 -> Projects -> Standard project ，配置工程名称及位置后点击确定：

![image-20241108112525402](figures/image-20241108112525402.png)

弹出下面这个弹窗后保持默认配置点击确定：

![image-20241108113312173](figures/image-20241108113312173.png)

> 注意：如果您购买了**[CODESYS Control RTE SL](http://store.codesys.cn/codesys/store/detail.html?productId=58)**，可选择设备：CODESYS Control RTE V3 (CODESYS) / x64 (CODESYS)，正常评估用途可选择不安装此扩展包，选择 CODESYS Control Win V3 (CODESYS) / x64 (CODESYS) 设备创建即可。

创建成功后就可以看到主界面了：

![image-20241108114000212](figures/image-20241108114000212.png)

### 4.2 Gateway 及 软PLC 启动

依次打开下面两个软件：

* CODESYS Gateway V3（右键 Start Gateway）
* CODESYS Control Win V3 -x64 SysTray（右键 Start PLC）

![image-20241108114211620](figures/image-20241108114211620.png)

回到 CODESYS 主站软件，双击 Device(CODESYS Control Win V3 x64) -> 通信设置 -> 扫描网络

![image-20241108114527294](figures/image-20241108114527294.png)

 弹出设备用户登录窗口后，配置用户名和密码（用户自定义）：

![image-20241108114642325](figures/image-20241108114642325.png)

检查网关设备及软PLC设备是否在线：

![image-20241108133523711](figures/image-20241108133523711.png)

### 4.3 profinet GSDML文件添加

> GSD(Generic Station Description file)：即通用站点描述文件，主要用于PROFIBUS DP（GSD文件）和PROFINET IO（GSDML文件）通信，作为描述文件，是PLC系统中CPU模块和IO模块之间的桥梁，通常包括通道数据、参数数据、诊断数据以及用户自定义数据。

本项目的 GSDML文件位于如下路径：

* ..\src\ports\rtthread\pn_dev

选择设备存储库安装描述文件，选择上述路径下的 **GSDML-V2.4-RT-Labs-P-Net-Sample-App-20220324.xml** 文件。

![image-20241108133947330](figures/image-20241108133947330.png)

安装成功后可以看到 p-net 从站描述文件：

![image-20241108134832574](figures/image-20241108134832574.png)

### 4.4 设备添加

* Ethernet添加：左侧导航栏点击Device并右键添加设备，选择以太网适配器

![image-20241108153525467](figures/image-20241108153525467.png)

* PROFINET IO主站添加：右键左侧导航栏中的Ethernet，选择PN-Controller

![image-20241108153722094](figures/image-20241108153722094.png)

* PROFINET IO从站添加：右键左侧导航栏中的 PN-Controller，选择 P-Net-multiple-module sample app

![image-20241108153920925](figures/image-20241108153920925.png)

### 4.5 任务响应

* Main Tasks 配置：左侧导航栏选择 Application -> 任务配置 -> 双击MainTask(IEC-Tasks)，优先级设置为1，类型选择循环，周期选择 4ms；

![image-20241111101451572](figures/image-20241111101451572.png)

* Profinet_CommunicationTask 配置：双击 Profinet_CommunicationTask(IEC-Tasks)，优先级设置为14，类型选择循环，周期设置为 10ms。

![image-20241111101828252](figures/image-20241111101828252.png)

### 4.6 网络配置

* Ethernet 配置：双击左侧导航栏中的Ethernet(Ethernet) -> 通用，修改网络接口为连接到开发板的以太网端口（这里由于我开启了PRONETA，所以在同一网段下分配了两个主站IP，这里需要注意选择正确的那一个）

![image-20241111094138076](figures/image-20241111094138076.png)

* PN_Controller 配置：双击左侧导航栏 PN_Controller(PN-Controller) -> 通用，并正确修改默认从站IP参数的区间，根据提示修改即可。
* P-Net 从站网络配置：双击左侧导航栏 P-Net-multiple-module sample app -> 通用， 修改IP参数为开发板IP

![image-20241111095351359](figures/image-20241111095351359.png)

![image-20241111095924136](figures/image-20241111095924136.png)

### 4.7 工程编译并启动调试

* step1：工程上方导航栏选择 编译-> 生成代码
* step2：选择 在线 -> 登录
* step3：点击 调试 -> 启动

此时就可以看到 PN 主站已经上线成功

![image-20241111102052203](figures/image-20241111102052203.png)

## 5.profinet 从站应用启动

开发板端启动 PN 从站，执行命令：pnet_app：

![image-20241111102801463](figures/image-20241111102801463.png)

![image-20241111102909056](figures/image-20241111102909056.png)

## 6.PN协议栈运行demo

> 这里我们分别使用 CODESYS 和 PRONETA 两款软件来测试 PN 的主从站交互及网络拓扑状态。

### 6.1 LED闪烁

回到 CODESYS 软件，左侧导航栏选择 PN_Controller，右键点击扫描设备，单击设备名后点击闪烁LED：

![image-20241111103530633](figures/image-20241111103530633.png)

此时的开发板端（PN从站IO）可以看到日志输出，并伴随板载User LED闪烁：

![image-20241111103913906](figures/image-20241111103913906.png)

### 6.2 从站 I&M 数据修改

依然是扫描设备界面，我们点击左下角的 I&M，修改信息并写入 I&M：

![image-20241111104223774](figures/image-20241111104223774.png)

同时 PNIO 会更新从站配置信息：

![image-20241111104354793](figures/image-20241111104354793.png)

我们再次点击 I&M，即可发现 I&M 修改成功！

### 6.3 PN网络拓扑

> PRONETA 介绍：PRONETA Basic 是一款简易工具，用于快速分析和组态 PROFINET 网络，且可对 ET 200 分布式 IO 系统和其它组件进行简单测试。

#### 6.3.1 安装GSDML文件

我们打开下载的 PRONETA 软件，添加 GSDML文件：

![image-20241111105332226](figures/image-20241111105332226.png)

安装成功后可以看到状态信息：

![image-20241111105406072](figures/image-20241111105406072.png)

#### 6.3.2 选择网络适配器

点击 设置 -> 网络适配器，选择 PN 对应的以太网口：

![image-20241111105548779](figures/image-20241111105548779.png)

#### 6.3.3 在线组网状态查看

选择左上角导航栏 网络分析-> 在线，点击刷新，稍等片刻便可看到 PN组网状态，同时在右侧栏可以查看具体的PNIO信息，由于我们刚刚在CODESYS中修改了PNIO的信息，在这里也可以看到更新之后的信息

![image-20241111110153537](figures/image-20241111110153537.png)

由于 pnet默认仅支持一个AR，所以我们需要关闭CODESYS的主站的网关设备及软PLC设备：

![image-20241111110443169](figures/image-20241111110443169.png)

然后刷新设备后，右键 rt-labs-dev，测试 LED 及I&M数据修改：

![image-20241111110804487](figures/image-20241111110804487.png)