# 远程控制实现

## 通用移植层

- common.c
  - 实现线程创建、串口打印、非精确阻塞延时、互斥信号量等功能

> **注意：**
>
> usart_printf 需要设置最大发送缓冲区,一次发送字符长度不能超出缓冲区长度
> 
> PRINTF_BUFFERS_LEN 默认为 1024

## 串口环形接收缓冲区

```c
//环形缓冲区
sRingbuff esp_ring_buff;

//接收
//stm32f1xx_it.c
//接收字符写入缓冲区， 控制缓冲区end++，释放对应信号量， 通过空闲接收标志写入结束标志
void USART1_IRQHandler(void);

//获取
//bsp_at_esp8266.c
//根据 begin 获取缓冲区字符
static bool esp_recv_ReadMessage(tbsp_esp8266 p_esp);

```

## BSP

### ESP

- 通过 at 指令实现对 ESP8266 控制
  - 同时时间只能有一条命令等待回复

```c
// bsp_esp8266

/** 头文件配置 **/

//配置 at 指令最大等待回复时间
#define AT_CMD_TIMEOUT_MS               
//at 接收缓冲区最大长度
#define AT_BUFF_LEN                     1024
//命令回答响应间隔
#define AT_CMD_RESPONSE_INTERVAL_MS        100


// esp8266 模块抽象
static bsp_esp8266 esp;
```

## MQTT 层

- 将物理层报文转换为MQTT传输报文，通过ESP8266发送
  - 由mqtt.c实现，只支持 qos 0
  - 同一时间只发送一条主动请求回复报文，判断发送结果

```c
//mqtt.c

//初始化mqtt层
bool mqtt_init();

//发起mqtt连接
bool mqtt_connect(uint8_t* client_id, uint8_t* device_name, uint8_t* device_key, uint16_t keep_alive_s);

//断开连接
void mqtt_disconnect();

//订阅指定主题
bool mqtt_subscribe(uint8_t* sub_topic, uint16_t req_qos, uint16_t msgid);

//取消订阅主题
bool mqtt_unsubscribe(uint8_t* unsub_topic, uint16_t msgid);

//发布消息
bool mqtt_publish(uint8_t* pub_topic, uint8_t* payload);

//心跳
bool mqtt_ping();
```

## 功能实现

```c
void data_template_demo_task(void *arg);
```
