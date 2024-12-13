# P-NET-RTT

> This repository is a port of the open-source P-Net evaluation version to RT-Thread, used for implementing the P-Net protocol stack for Profinet devices.

## 1. Preliminary Setup

### Software Environment

* [CODESYS](https://us.store.codesys.com/) (Profinet master simulation)
  * CODESYS
  * CODESYS Gateway (Gateway device)
  * CODESYS Control Win SysTray (SoftPLC device)
* [Npcap](https://npcap.com/dist/npcap-1.80.exe) (This software is required for running CODESYS and must be installed in advance!)
* [PRONETA](https://support.industry.siemens.com/cs/attachments/67460624/proneta_3_8_0_0.zip)

### Hardware Environment

* Microcontroller with ETH (Ethernet driver must support RT-Thread Ethernet driver framework)

## 2. Software Package Configuration

Open the `bsp`'s `env` interface, go to **->RT-Thread online packages->IoT**, and find **[*] P-Net stack for Profinet device implementation --->** to enable it. Also, leave configurations for the user:

```md
-*- Default netif name for P-NET  ---> 
	-> (e00) default ethernet interface name for p-net app, default as 'e00'
-*- Enable P-NET sample board config  ---> 
	-> (0x0209) p-net user led pin
	-> (0x0005) p-net user key pin
-*- Default root filesystem path for P-NET  ---> 
    -> [*] p-net using ramfs filesystem by default, or you can turn this off and choose another way to enable the filesystem
    -> (8192) default memory size for ramfs
-*- P-NET sample slave network ip config  ---> 
    -> (192.168.137.196) set static ip address for Profinet slave
    -> (192.168.137.1) set static gateway address for Profinet slave
    -> (255.255.255.0) set static mask address for Profinet slave
    version (latest)  ---> 
```

* **Default netif name for p-net**: P-net network interface device name, default as e00.
* **Enable pnet sample board config**: Configuration for P-net app user LED and button.
* **Default root filesystem path for p-net**: P-net filesystem configuration, default using ramfs with 8KB memory space.
* **P-NET sample slave network ip config**: Static IP configuration for Profinet slave device (**Make sure to disable RT_LWIP_DHCP feature and use static IP**).

Next, configure DHCP to be disabled and use a static IP. Go to **→ RT-Thread Components → Network → LwIP: lightweight TCP/stack**, and disable DHCP:

![image-20241213150409268](figures/image-20241213150409268.png)

After completing the configuration, compile and download the program to the development board.

## 3. Network Configuration

Connect the development board to the PC using an Ethernet cable, and configure a static IP on the PC side:

![image-20241108100915339](figures/image-20241108100915339.png)

Check the IP information on the development board and test the connectivity:

![image-20241108101125803](figures/image-20241108101125803.png)

## 4. SoftPLC Master Startup

> CODESYS Overview: CODESYS is a PLC software developed by the German company 3S. It integrates functions like PLC logic, motion control, and configuration displays. CODESYS, short for "Controller Development System," is an industrial automation programming tool based on the IEC 61131-3 standard. It supports multiple programming languages (e.g., ladder diagram, structured text, function block diagram) and offers a wide range of libraries and functional modules, helping engineers quickly develop and debug PLCs and industrial control systems. Its flexibility and powerful features make it widely used in industrial automation.

### 4.1 Create a Standard CODESYS Project

Ensure that the CODESYS software is installed. After installation, the following three pieces of software will be used:

![image-20241108112037518](figures/image-20241108112037518.png)

* CODESYS V3.5 SP20 Patch 3: Profinet master simulation
* CODESYS Gateway V3: Gateway device
* CODESYS Control Win V3 -x64 SysTray: SoftPLC device

First, open **CODESYS V3.5 SP20 Patch 3**, select -> New Project -> Projects -> Standard Project, configure the project name and location, then click OK:

![image-20241108112525402](figures/image-20241108112525402.png)

After this popup window appears, click OK to keep the default configuration:

![image-20241108113312173](figures/image-20241108113312173.png)

> Note: If you purchased the **[CODESYS Control RTE SL](http://store.codesys.cn/codesys/store/detail.html?productId=58)**, you can choose the device: CODESYS Control RTE V3 (CODESYS) / x64 (CODESYS). For normal evaluation purposes, you can skip installing this extension package and select CODESYS Control Win V3 (CODESYS) / x64 (CODESYS) device to create the project.

After the creation is successful, you will see the main interface:

![image-20241108114000212](figures/image-20241108114000212.png)

### 4.2 Gateway and SoftPLC Startup

Open the following two software applications:

* CODESYS Gateway V3 (Right-click Start Gateway)
* CODESYS Control Win V3 -x64 SysTray (Right-click Start PLC)

![image-20241213150426614](figures/image-20241213150426614.png)

Go back to the CODESYS master software, double-click **Device (CODESYS Control Win V3 x64)** -> Communication Settings -> Scan Network:

![image-20241108114527294](figures/image-20241108114527294.png)

After the device user login window appears, configure the username and password (user-defined):

![image-20241108114642325](figures/image-20241108114642325.png)

Check if the gateway device and softPLC device are online:

![image-20241108133523711](figures/image-20241108133523711.png)

### 4.3 Add Profinet GSDML File

> GSD (Generic Station Description) files: These are used in PROFIBUS DP (GSD files) and PROFINET IO (GSDML files) communication, acting as a bridge between the CPU module and IO modules in a PLC system. They typically include channel data, parameter data, diagnostic data, and user-defined data.

The GSDML file for this project is located at:

* ..\src\ports\rtthread\pn_dev

Install the GSDML file from the device repository by selecting **GSDML-V2.4-RT-Labs-P-Net-Sample-App-20220324.xml** under the above path:

![image-20241108133947330](figures/image-20241108133947330.png)

After the installation is successful, you will see the P-net slave description file:

![image-20241108134832574](figures/image-20241108134832574.png)

### 4.4 Add Devices

* Add Ethernet: Right-click **Device** on the left navigation panel and select **Ethernet Adapter**:

![image-20241108153525467](figures/image-20241108153525467.png)

* Add PROFINET IO Master: Right-click **Ethernet** on the left navigation panel and select **PN-Controller**:

![image-20241108153722094](figures/image-20241108153722094.png)

* Add PROFINET IO Slave: Right-click **PN-Controller** on the left navigation panel and select **P-Net-multiple-module sample app**:

![image-20241108153920925](figures/image-20241108153920925.png)

### 4.5 Task Configuration

* **Main Tasks** configuration: On the left navigation panel, select **Application -> Task Configuration**, double-click **MainTask (IEC-Tasks)**, set the priority to 1, set the type to cyclic, and set the cycle to 4ms:

![image-20241111101451572](figures/image-20241111101451572.png)

* **Profinet_CommunicationTask** configuration: Double-click **Profinet_CommunicationTask (IEC-Tasks)**, set the priority to 14, set the type to cyclic, and set the cycle to 10ms:

![image-20241111101828252](figures/image-20241111101828252.png)

### 4.6 Network Configuration

* **Ethernet configuration**: Double-click **Ethernet (Ethernet)** on the left navigation panel -> General, and change the network interface to the Ethernet port connected to the development board (since I enabled PRONETA, two master IPs are assigned within the same subnet, so make sure to choose the correct one):

![image-20241111094138076](figures/image-20241111094138076.png)

* **PN_Controller configuration**: Double-click **PN_Controller (PN-Controller)** on

 the left navigation panel -> General, and modify the default slave IP parameters according to the prompts.
* **P-Net Slave network configuration**: Double-click **P-Net-multiple-module sample app** on the left navigation panel -> General, and change the IP parameters to the development board's IP:

![image-20241111095351359](figures/image-20241111095351359.png)

![image-20241111095924136](figures/image-20241111095924136.png)

### 4.7 Compile, Debug, and Start the Project

* Step 1: In the project navigation bar, select **Build -> Generate Code**.
* Step 2: Select **Online -> Login**.
* Step 3: Click **Debug -> Start**.

You should now see that the PN master has successfully come online:

![image-20241111102052203](figures/image-20241111102052203.png)

## 5. Profinet Slave Application Startup

Start the PN slave on the development board by executing the command: `pnet_app`:

![image-20241111102801463](figures/image-20241111102801463.png)

![image-20241111102909056](figures/image-20241111102909056.png)

## 6. PN Protocol Stack Demo

> In this section, we will use CODESYS and PRONETA software to test the interaction between PN master and slave as well as network topology status.

### 6.1 LED Blinking

Go back to the CODESYS software, select **PN_Controller** from the left navigation panel, right-click and scan for devices. After clicking on the device name, click **Blink LED**:

![image-20241111103530633](figures/image-20241111103530633.png)

At this point, you will see the log output on the development board (PN slave IO), and the onboard User LED will blink:

![image-20241111103913906](figures/image-20241111103913906.png)

### 6.2 Modify Slave I&M Data

In the device scanning interface, click the **I&M** button at the bottom left, modify the information, and write the new I&M data:

![image-20241111104223774](figures/image-20241111104223774.png)

PNIO will update the slave configuration information:

![image-20241111104354793](figures/image-20241111104354793.png)

Click **I&M** again to see that the I&M modification has been successfully applied!

### 6.3 PN Network Topology

> PRONETA Overview: PRONETA Basic is a simple tool for quickly analyzing and configuring PROFINET networks. It also supports basic testing of ET 200 distributed IO systems and other components.

#### 6.3.1 Install GSDML Files

Open the PRONETA software and add the GSDML file:

![image-20241111105332226](figures/image-20241111105332226.png)

After installation is successful, you can see the status information:

![image-20241111105406072](figures/image-20241111105406072.png)

#### 6.3.2 Select Network Adapter

Click **Settings -> Network Adapter** and choose the PN corresponding Ethernet port:

![image-20241111105548779](figures/image-20241111105548779.png)

#### 6.3.3 View Online Network Status

Select **Network Analysis -> Online** from the top left navigation panel, click **Refresh**, and after a moment, you should see the PN network status. The specific PNIO information can be viewed on the right sidebar. Since we modified the PNIO information in CODESYS, you can also see the updated information here.

![image-20241111110153537](figures/image-20241111110153537.png)

Since pnet only supports a single AR by default, we need to disable the gateway device and softPLC device on the CODESYS master:

![image-20241111110443169](figures/image-20241111110443169.png)

Then, refresh the device and right-click **rt-labs-dev** to test the LED and I&M data modification:

![image-20241111110804487](figures/image-20241111110804487.png)