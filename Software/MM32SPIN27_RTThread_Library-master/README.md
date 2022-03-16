![逐飞LOGO](https://images.gitee.com/uploads/images/2019/0924/114256_eaf16bad_1699060.png "逐飞科技logo 中.png")
# 逐飞科技MM32SPIN2Xps开源库
#### 简介
该项目为逐飞科技基于灵动微电子“MM32SPIN2Xps_SDK”制作的MM32SPIN2Xps开源库。简化了部分SDK库函数功能的使用步骤，便于使用MM32SPIN2Xps参加竞赛以及进行产品开发。

#### 该开源库的优势
1.  **重新封装了常用功能的驱动函数：** 
- 在官方SDK的基础上做了二次封装，步骤简洁，更加方便使用各种内部外设功能。
2.  **集成了常用模块的驱动函数：** 
- 开源库内已集成多种外置外设模块驱动，如ICM20602陀螺仪驱动，IPS液晶屏驱动，总钻风摄像头驱动，可以直接通过调用函数的方式使用外设模块，节省开发时间。

#### 环境准备
1.  **MM32SPIN2Xps硬件环境：** 
- 推荐使用本公司MM32SPIN2Xps核心板，[点击此处购买](https://item.taobao.com/item.htm?spm=a1z10.5-c-s.w4002-22508770847.20.6e211e6cS8XdJx&id=633253463170)。
2.  **软件开发环境：** 
（IAR或MDK可任选其一）
- IAR 推荐使用版本：IAR Embedded Workbench for ARM V8.32.4。（以下简称IAR）
- MDK 推荐使用版本：MDK v5.24及以上。（5.26版本后加入了对DAP仿真器V2版本的支持，可以使用本公司DAP仿真器的WinUSB模式进行更高速的下载）
3.  **仿真器：** 
（DAP仿真器及J-Link仿真器可任选其一）
- DAP仿真器：推荐使用本公司DAP仿真器，双下载模式，可以在支持的环境下实现更高下载速度。
- J-Link仿真器：请确保您的J-Link仿真器硬件版本为V9或更高（不支持J-Link OB）。且J-Link驱动版本为V6.40或更高。

#### 使用说明

1.  **下载开源库：** 点击页面右侧的克隆/下载按钮，将工程文件保存到本地。您可以使用git克隆（Clone）或下载ZIP压缩包的方式来下载。推荐使用git将工程目录克隆到本地，这样可以使用git随时与我们的开源库保持同步。关于码云与git的使用教程可以参考以下链接 [https://gitee.com/help](https://gitee.com/help)。
2.  **打开工程：** 将下载好的工程文件夹打开（若下载的为ZIP文件，请先解压压缩包）。在打开工程前，请务必确保您的IDE满足环境准备章节的要求。否则可能出现打开工程时报错，提示丢失目录信息等问题。
- 若您使用的IDE为IAR，工程文件保存在Project/IAR文件夹下。
- 若您使用的IDE为MDK，工程文件保存在Project/MDK文件夹下。

#### 逐飞科技MM32SPIN2Xps核心板

![逐飞科技MM32SPIN2Xps核心板 V1.0](https://images.gitee.com/uploads/images/2020/1223/110951_701b1f8d_848799.jpeg)
