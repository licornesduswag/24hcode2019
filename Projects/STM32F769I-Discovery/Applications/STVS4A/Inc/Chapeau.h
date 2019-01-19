
// screen max size
#define MAX_X 800
#define MAX_Y 480
#define MIN_X 0
#define MIN_Y 0

/** LEDs Settings *********************************/
#define LED_NUMBER  20
#define LED_ALL     0xFF
/***********************************************/

/** UART Settings *********************************************/

UART_HandleTypeDef UartHandleChapeau;
#define TXBUFFERSIZE  30
#define RXBUFFERSIZE  TXBUFFERSIZE /* Both Rx & Tx buffers have the same fixed size */
#define MSG_END_CHAR  0x0A /* LF */
#define MSG_STUF_CHAR ' '  /* space */
uint8_t aTxBuffer[TXBUFFERSIZE];
uint8_t aRxBuffer[RXBUFFERSIZE];

#define USARTChapeau                           USART6
#define USARTChapeau_CLK_ENABLE()              __USART6_CLK_ENABLE()
#define USARTChapeau_RX_GPIO_CLK_ENABLE()      __GPIOC_CLK_ENABLE()
#define USARTChapeau_TX_GPIO_CLK_ENABLE()      __GPIOC_CLK_ENABLE()

#define USARTChapeau_FORCE_RESET()             __USART6_FORCE_RESET()
#define USARTChapeau_RELEASE_RESET()           __USART6_RELEASE_RESET()

/* Definition for USARTx Pins */
#define USARTChapeau_TX_PIN                    GPIO_PIN_6
#define USARTChapeau_TX_GPIO_PORT              GPIOC
#define USARTChapeau_TX_AF                     GPIO_AF8_USART6
#define USARTChapeau_RX_PIN                    GPIO_PIN_7
#define USARTChapeau_RX_GPIO_PORT              GPIOC
#define USARTChapeau_RX_AF                     GPIO_AF8_USART6

/* Definition for USARTx's NVIC */
#define USARTChapeau_IRQn                      USART6_IRQn
#define USARTChapeau_IRQHandler                USART6_IRQHandler

/***********************************************/


void service_Chapeau_task(void  const * argument);
void service_ChapeauLed_task(void  const * argument);
void service_ChapeauUart_task(void  const * argument);



