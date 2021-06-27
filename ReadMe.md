## 仓库说明

> 此仓库基于edk2-stable202105，手动导入了依赖的submodule(github挂代理克隆edk2的代码和init submodule速度还是很慢，可以借助gitee中转)，日常记录追code的一些注释和理解

## 基本开发环境配置

- Git
- Python
- Visual Studio 
- VSCode + Vim插件

## 编译Tool部署

- [NASM]( https://www.nasm.us/ )
- [IASL编译器](https://acpica.org/downloads/binary-tools)

> 推荐都放在C盘根目录，并设置环境变量(NASM_PREFIX)，不然需要修改tools_def.txt，edksetup.bat的Tool路径(ASL_PATH、NASM_PREFIX)

## 仓库使用

```c
edksetup.bat rebuild  //重新获取配置的环境变量,会在Conf目录下建target.txt、Tools_def.txt、AutoGenIdFile.txt、build_rule.txt等文件 
build -t VS2017 -a IA32 -y Report.txt //-t 指定ToolChain -a 指定TargetArch -y 生成格式化的Report.txt
```

## 调试相关设置

> 在需要调试代码处加上CpuBreakpoint(),并设置注册表,即会触发中断,调出VS2017跟进源码

![](http://tc.holychen.cn/小书匠/1619518365699.png)

![](http://tc.holychen.cn/小书匠/1619519544086.png)

## VScode + Visual Studio联调

![](http://tc.holychen.cn/小书匠/1619520470753.png)



## UEFI BIOS启动流程

> EFI系统表是UEFI 规范定义的最重要的数据结构，它的指针会被当作入口参数传递给每个驱动和应用程序， 进而驱动和应用程序就可以从这个数据结构得到系统的配置信息和丰富的系统服务

```c
///
/// EFI System Table
///
typedef struct {
  ///
  /// The table header for the EFI System Table.
  ///
  EFI_TABLE_HEADER                  Hdr;
  ///
  /// A pointer to a null terminated string that identifies the vendor
  /// that produces the system firmware for the platform.
  ///
  CHAR16                            *FirmwareVendor;
  ///
  /// A firmware vendor specific value that identifies the revision
  /// of the system firmware for the platform.
  ///
  UINT32                            FirmwareRevision;
  ///
  /// The handle for the active console input device. This handle must support
  /// EFI_SIMPLE_TEXT_INPUT_PROTOCOL and EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL.
  ///
  EFI_HANDLE                        ConsoleInHandle;
  ///
  /// A pointer to the EFI_SIMPLE_TEXT_INPUT_PROTOCOL interface that is
  /// associated with ConsoleInHandle.
  ///
  EFI_SIMPLE_TEXT_INPUT_PROTOCOL    *ConIn;
  ///
  /// The handle for the active console output device.
  ///
  EFI_HANDLE                        ConsoleOutHandle;
  ///
  /// A pointer to the EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL interface
  /// that is associated with ConsoleOutHandle.
  ///
  EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL   *ConOut;
  ///
  /// The handle for the active standard error console device.
  /// This handle must support the EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL.
  ///
  EFI_HANDLE                        StandardErrorHandle;
  ///
  /// A pointer to the EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL interface
  /// that is associated with StandardErrorHandle.
  ///
  EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL   *StdErr;
  ///
  /// A pointer to the EFI Runtime Services Table.
  ///
  EFI_RUNTIME_SERVICES              *RuntimeServices;
  ///
  /// A pointer to the EFI Boot Services Table.
  ///
  EFI_BOOT_SERVICES                 *BootServices;
  ///
  /// The number of system configuration tables in the buffer ConfigurationTable.
  ///
  UINTN                             NumberOfTableEntries;
  ///
  /// A pointer to the system configuration tables.
  /// The number of entries in the table is NumberOfTableEntries.
  ///
  EFI_CONFIGURATION_TABLE           *ConfigurationTable;
} EFI_SYSTEM_TABLE;
```



![image-20210620011215626](http://tc.holychen.cn/小书匠/image-20210620011215626.png)



- EFI 系统表中的启动时服务只在操作系统运行时之前是可用的， 而运行时服务则在操作系统运行时之前和操作系统运行时都是可用的

- UEFI 规范定义的Handle 数据库是UEFI 需要维护的最重要的对象库，由Handle 和协议(Protocol)组成。 这个Handle 数据库是所有的UEFI Handle 的列表， 每个Handle 上可以挂载一个或者多个协议
-  协议是GUID（Globally Unique Identifier， 全局唯一标识符）来命名并唯一标识的结构体， 可能包含一些函数指针和数据结构体，即用面向对象的C Struct模拟Class实现OOP，最小的协议可以只定义GUID，而不包含任何函数指针或者数据结构体。 换句话说， GUID 是一个协议必须要定义的
- 协议通过挂载在Handle 上来进行使用， 跟Handle 一起存放在Handle 数据库中。 系统中的其他函数通过GUID 在Handle 数据库中查找对应的协议
- Handle和Protocol的数据结构实质表征是双向环链表， UEFI 的可扩展特性很大程度上就体现在协议上



**基于UEFI和PI规范实现的UEFI BIOS启动流程如下：**

![image-20210620011020584](http://tc.holychen.cn/小书匠/image-20210620011020584.png)



按照启动先后顺序， 该流程分为以下7个阶段：

- 1） 安全检测(SEC， Security)阶段
- 2） EFI初始化准备(PEI， Pre-EFI Initialization)阶段
- 3） 驱动程序执行环境(DXE， Driver Execution Environment)阶段
- 4） 启动设备选择(BDS， Boot Device Selection)阶段
- 5） 瞬时系统加载(TSL， Transient System Load)阶段
- 6） 运行时(RT， Run Tim e)阶段
- 7） 生命周期后(AL， After Life)阶段



### 安全检测(SEC)阶段

- 在X86的计算机系统上， 系统开机，硬件电路重置寄存器的值(ResetVector) ，CPU从4GB的最高端地址FFFFFFF0h开始执行指令(映射到Nor Flash上的BIOS芯片)

- ResetVector组件会被打包成一个固件文件系统(FFS， Firmware File System)高文件卷文件(VTF， Volume Top File)，它负责找到和执行SecCore(SECURITY CORE)模块 

- SecCore模块会进行基本的系统初始化、 为CPU打微码补丁、 初始化CAR(Cache As RAM)、 切换到保护模式、 获取启动固件卷(BFV， Boot Firmware Volume)的地址和找到PeiCore(PEI_CORE)及其入口地址以准备进入PEI阶段

- 在CAR被初始化好之前， 系统没有内存来作为堆栈使用， ResetVector组件和SecCore模块的代码采用汇编实现； 在CAR被初始化好之后， 一小块CPU cache空间将被当作临时内存(temporary memory)来使用， SecCore模块会进入C语言执行环境 

- SecCore 模块利用获取的启动固件卷地址和初始化了的CAR 来给EFI_SEC_PEI_HAND_OFF中的启动固件卷地址和大小、 临时内存地址和大小、PEI 临时内存地址和大小以及栈地址和大小域赋值， 最后把EFI_SEC_PEI_HAND_OFF与SEC阶段提供的PpiList一起传递给PeiCore， 从而进入PEI阶段

- SEC阶段提供的PpiList 可能会包含安全或者验证服务， 如TCG(Trusted Computing Group)访问服务， 因为在一个遵循TCG的系统中， SEC就是可信度量根的核心(CRTM， Core Root-of-Trust Measurement)

### EFI 初始化准备(PEI)阶段

> PEI阶段的主要任务是决定系统的启动路径（正常启动、 S3唤醒或者Recovery等)，初始化各种平台硬件， 包括CPU和芯片组等，检测和初始化系统内存，报告和处理DXE固件卷，最后找到DxeCore(DXE_CORE)及其入口地址以进入DXE阶段

- PeiCore(PEI_CORE)首先会对PEI的基础服务（如内存、 HOB、 PPI和FV服务等）进行初始化

- 接着PEI调度器开始利用FV服务根据固件文件系统的格式从启动固件卷查找PEI模块， 检查它们的依赖关系(dependency expression)是否已经满足， 进而执行PEI模块， PEI模块可能会产生PPI为其它PEI模块提供服务， DxeIpl PEI模块会产生DxeIpl PPI

- 内存初始化模块在检测和初始化系统内存之后会通过PEI的基础服务InstallPeiMemory来向PeiCore报告PEI阶段可用的常驻内存(permanent memory)和建立Resource HOB来报告系统内存

- 接着PeiCore将会迁移PEI正在使用的临时内存（包括栈(stack)和堆(heap)， HOB列表就在堆中）里面的所有数据到常驻内存中去， 以及产生MemoryDiscovered PPI来通知其它所有依赖于系统内存的PEI模块， 让它们知道系统内存可以使用了

- 然后其它PEI模块通过PEI基础服务NotifyPpi注册的MemoryDiscovered PPI回调函数(callback)就会被执行， 如平台PEI模块注册的MemoryDiscovered PPI回调函数可能会设置系统内存的缓存属性和通过产生FvInfo PPI来报告DXE固件卷

 **PEI阶段的内存映射和使用如下：**

![image-20210620012406859](http://tc.holychen.cn/小书匠/image-20210620012406859.png)



- PEI调度器在执行完所有的PEI模块之后会调用DxeIpl PPI， 接着DxeIpl PPI就会从DXE固件卷中找到DxeCore及其入口地址， 最后把PEI阶段建立的HOB列表传递给DxeCore， 从而进入DXE阶段

- 在X86的计算机系统上， 如果DXE阶段为64位， DxeIpl PPI在执行DxeCore之前还需要建立页表(page table)和切换CPU到IA32e模式(long mode)

- PEI阶段建立的HOB列表如下图所示， 开始的一个HOB必须是PHIT(Phase Handoff Information Table) HOB， 最后一个HOB必须是end of list HOB

![image-20210620011130677](http://tc.holychen.cn/小书匠/image-20210620011130677.png)



### 驱动程序执行环境(DXE)阶段

- DXE阶段包含的组件： DXE Foundation、 DXE调度器和DXE驱动程序，DXE阶段的主要任务是生成一套完整的UEFI接口， 和后续的BDS阶段一起工作， 建立控制台并尝试启动操作系统

- DXE阶段是UEFI BIOS最重要的阶段DxeCore(DXE_CORE)首先会初始化启动时服务表、 运行时服务表和DXE服务表， 利用PEI阶段传递过来的HOB列表中的Resource HOB初始化DXE服务表中的GCD(Global Coherency Domain)服务以及启动时服务表中的内存服务，基于HOB列表中的FV HOB产生FVB协议及FV协议 

- DxeCore还会把DXE服务表和HOB列表安装到EFI系统表的配置表(ConfigurationTable)中。接着DXE调度器会开始利用FV协议根据固件文件系统的格式从DXE固件卷查找DXE驱动，检查它们的依赖关系是否已经满足， 进而执行DXE驱动

- DXE驱动有两类： 较早执行的DXE驱动和UEFI驱动（符合UEFI驱动模型的DXE驱动）。 较早执行的DXE驱动程序会初始化平台硬件， 包括CPU和芯片组等， 安装配置表（如ACPI Table和SMBIOS Table就会被相应的DXE驱动安装到配置表中）和产生架构协议（例如DXE Variable驱动就会产生Variable架构协议和Variable Write架构协议， BDS驱动会产生BDS架构协议）等 

- 如下图所示DXE阶段产生的架构协议， 架构协议基本上与EFI系统表中的启动时服务和运行时服务相对应。 UEFI驱动在所有的架构协议产生之后才会被执行， 它们会产生Driver Binding协议， 这些Driver Binding协议在BDS阶段连接控制器（利用启动时服务表中的ConnectController接口）的时候会被连接， 进而为控制台和启动设备提供软件抽象

![image-20210620011449280](http://tc.holychen.cn/小书匠/image-20210620011449280.png)



- DXE阶段会生成一套完整的UEFI接口， DXE调度器在执行完所有的DXE驱动之后会调用BDS架构协议， 从而进入BDS阶段， DXE阶段和BDS阶段一起工作来建立可供操作系统启动的平台。 注意， 只有EFI系统表中的运行时服务和由运行时DXE驱动程序提供的服务在操作系统运行阶段还继续存在

### 启动设备选择(BDS)阶段

- BDS阶段的主要功能就是为平台选择一个合适的启动设备， 并尝试加载它

- BDS架构协议是DXE阶段的BDS驱动产生的， DXE调度器在执行完所有的DXE驱动之后会调用BDS架构协议， 从而进入BDS阶段

- 在BDS阶段， BDS架构协议在连接控制器（利用启动时服务表中的ConnectController接口）的时候连接DXE阶段UEFI驱动产生的Driver Binding协议， 进而为控制台和启动设备提供软件抽象。 

- BDS阶段是一个独特的启动管理阶段， 根据平台的启动策略，UEFI规范定义的全局启动选项Variable L"Boot####"和启动选项顺序VariableL "BootOrder"会被建立

- 一个启动选项对应于相应启动设备中的一个UEFI应用程序(APPLICATION)， 如UEFI BIOS的SETUP、 启动菜单、 扩展的诊断工具和操作系统加载器等等

- UEFI启动管理器会按照启动顺序来引导启动选项， 启动选项对应的UEFI应用程序会被加载和执行 

- 如果是操作系统加载器被加载和执行， 操作系统加载器就会获得控制权， 然后进入接下来的TSL阶段来加载操作系统

### 瞬时系统加载(TSL)阶段

- 操作系统加载器在BDS 阶段被加载和执行， EFI 系统表的指针会被当作入口参数传递给操作系统加载器 

- 在TSL 阶段， 操作系统加载器会使用UEFI接口加载操作系统

- TSL 阶段结束及操作系统运行时阶段开始的标志是操作系统加载器调用启动时服务表中的ExitBootServices 接口

- ExitBootServices 接口会触发ExitBootServices 事件来通知其它模块启动时服务将要被终止了

### 运行时(RT)阶段

- 操作系统加载器在TSL 阶段调用启动时服务表中的ExitBootServices 接口而进入操作系统运行时阶段
-  启动时服务表中的ExitBootServices 接口一旦被调用， EFI 系统表中所有的启动时服务都会被终止， 结果就只有EFI 系统表中的配置表和运行时服务在操作系统运行时阶段可用 
- 操作系统可以从操作系统加载器得到EFI 系统表的指针， 通过这个指针， 操作系统可以获得各种平台配置信息（如从配置表得到ACPI 表和SMBIOS 表等）和访问运行时服务

### 生命周期后(AL)阶段

- 生命周期后阶段代表的是平台的控制权从操作系统端重新回到了UEFI BIOS 端

- 这个阶段可以说是操作系统运行时阶段的延续， 可能是重启系统（如操作系统调用运行时服务表ResetSystem 接口）、 进入到ACPI 睡眠状态（如进入到S3， S5）或者进入SMM（在X86 CPU 检查到一种特殊的系统中断SMI的时候， 处理器会进入一种特殊的操作模式， 这种操作模式叫做SMM， 它对于OS 来说是完全透明的)等

