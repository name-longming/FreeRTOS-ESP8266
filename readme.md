# 项目简介

基于stm32F407+FreeRTOS+ESP8266的实时气象站系统，通过物联网技术实时读取天气情况，温度以及自带了一个计时功能。

## 所需设备

stm32F407，淘晶驰串口屏，ESP8266；

串口屏连接串口3，ESP8266连接串口2，串口1用于打印状态。

## 实现过程

通过对ESP8266发送AT指令，从服务器读取天气的json数据，然后通过cJSON解码数据，最后FreeRTOS对任务进行管理（FreeRTOS和cJSON有冲突，需要将cJSON申请内存空间的函数替换成FreeRTOS申请内存的函数，每次解码后，**一定要释放内存**，否则解码会卡死，而且需要把Heap_size设置稍微大一点，推荐设置为4096）

## 实现效果

![实现效果](https://img-blog.csdnimg.cn/20200625171702401.JPG?x-oss-process=image/watermark,type_ZmFuZ3poZW5naGVpdGk,shadow_10,text_aHR0cHM6Ly9ibG9nLmNzZG4ubmV0L25hbWVfbG9uZ21pbmc=,size_16,color_FFFFFF,t_70)