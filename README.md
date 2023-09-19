# Զ�̿���ʵ��

## ͨ����ֲ��

- common.c
  - ʵ���̴߳��������ڴ�ӡ���Ǿ�ȷ������ʱ�������ź����ȹ���

> **ע�⣺**
>
> usart_printf ��Ҫ��������ͻ�����,һ�η����ַ����Ȳ��ܳ�������������
> 
> PRINTF_BUFFERS_LEN Ĭ��Ϊ 1024

## ���ڻ��ν��ջ�����

```c
//���λ�����
sRingbuff esp_ring_buff;

//����
//stm32f1xx_it.c
//�����ַ�д�뻺������ ���ƻ�����end++���ͷŶ�Ӧ�ź����� ͨ�����н��ձ�־д�������־
void USART1_IRQHandler(void);

//��ȡ
//bsp_at_esp8266.c
//���� begin ��ȡ�������ַ�
static bool esp_recv_ReadMessage(tbsp_esp8266 p_esp);

```

## BSP

### ESP

- ͨ�� at ָ��ʵ�ֶ� ESP8266 ����
  - ͬʱʱ��ֻ����һ������ȴ��ظ�

```c
// bsp_esp8266

/** ͷ�ļ����� **/

//���� at ָ�����ȴ��ظ�ʱ��
#define AT_CMD_TIMEOUT_MS               
//at ���ջ�������󳤶�
#define AT_BUFF_LEN                     1024
//����ش���Ӧ���
#define AT_CMD_RESPONSE_INTERVAL_MS        100


// esp8266 ģ�����
static bsp_esp8266 esp;
```

## MQTT ��

- ������㱨��ת��ΪMQTT���䱨�ģ�ͨ��ESP8266����
  - ��mqtt.cʵ�֣�ֻ֧�� qos 0
  - ͬһʱ��ֻ����һ����������ظ����ģ��жϷ��ͽ��

```c
//mqtt.c

//��ʼ��mqtt��
bool mqtt_init();

//����mqtt����
bool mqtt_connect(uint8_t* client_id, uint8_t* device_name, uint8_t* device_key, uint16_t keep_alive_s);

//�Ͽ�����
void mqtt_disconnect();

//����ָ������
bool mqtt_subscribe(uint8_t* sub_topic, uint16_t req_qos, uint16_t msgid);

//ȡ����������
bool mqtt_unsubscribe(uint8_t* unsub_topic, uint16_t msgid);

//������Ϣ
bool mqtt_publish(uint8_t* pub_topic, uint8_t* payload);

//����
bool mqtt_ping();
```
