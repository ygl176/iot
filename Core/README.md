# 远程控制实现

## 移植层

- common.c

```c

```

> **注意：**
>
> usart_printf 需要设置最大发送缓冲区
> 
> PRINTF_BUFFERS_LEN 默认为 1024

## ESP8266

- 通过 at 指令实现对 ESP8266 控制

```c
/** 头文件配置 **/

//配置 at 指令最大等待回复时间
#define AT_CMD_TIMEOUT_MS
//at 接收缓冲区最大长度
#define AT_BUFF_LEN                     1024
//命令回答响应间隔
#define AT_CMD_RESPONSE_INTERVAL_MS        100


// esp8266 模块抽象


bsp_esp8266 esp8266; //全局变量

/** 功能函数 **/

//控制函数
esp8266_mode();
esp8266_wifi_connect();

//消息处理函数
at_parper();

//消息接收方式: 串口空闲中断接收
```

## MQTT 协议

- 生成、解析 mqtt 报文
  - 通过发送、接收消息队列处理消息

```c
/** 头文件配置 **/

// MQTT报文最大长度
#define MQTT_MAX_LEN

// MQTT结构抽象


/** 功能实现 **/

//发送mqtt报文
mqtt_send();

//mqtt报文分析、打包
mqtt_parper();
```

## 腾讯云服务器连接

- 服务器功能操作
- 物理层协议解析、生成
- 设备属性注册表
- 订阅主题注册表

```c
/** 头文件配置 **/


/** 功能实现 **/

```

## 设备属性控制和功能上报

- 根据报文执行对应操作
- 记录修改属性并上报

```c
/** down **/

/** up **/
```
