/**
 ******************************************************************************
 * @file    Chapeau.c
 * @author  MCD Application Team
 * @version V1.0.0
 * @date    04-December-2018
 * @brief   Ademo file for 24h code 2019 contest
 ******************************************************************************
 * @attention
 *
 * <h2><center>&copy; COPYRIGHT(c) 2018 STMicroelectronics</center></h2>
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *   1. Redistributions of source code must retain the above copyright notice,
 *      this list of conditions and the following disclaimer.
 *   2. Redistributions in binary form must reproduce the above copyright notice,
 *      this list of conditions and the following disclaimer in the documentation
 *      and/or other materials provided with the distribution.
 *   3. Neither the name of STMicroelectronics nor the names of its contributors
 *      may be used to endorse or promote products derived from this software
 *      without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 ******************************************************************************
 */

#include "service.h"
#include "platform_init.h"
#include "cmsis_os.h"
#include "Chapeau.h"
#include "stdlib.h"

#define NB_RETRY_RELEASE_DETECTION 10 /* 10 * 5ms = 50ms */
#define SPIx SPI2

/* Size of buffer */
#define BUFFERSIZE (COUNTOF(aTxBuffer) - 1)

/**
  * @brief  System Clock Configuration
  *         The system Clock is configured as follow :
  *            System Clock source            = PLL (HSE)
  *            SYSCLK(Hz)                     = 216000000
  *            HCLK(Hz)                       = 216000000
  *            AHB Prescaler                  = 1
  *            APB1 Prescaler                 = 4
  *            APB2 Prescaler                 = 2
  *            HSE Frequency(Hz)              = 25000000
  *            PLL_M                          = 25
  *            PLL_N                          = 432
  *            PLL_P                          = 2
  *            PLL_Q                          = 9
  *            PLL_R                          = 7
  *            VDD(V)                         = 3.3
  *            Main regulator output voltage  = Scale1 mode
  *            Flash Latency(WS)              = 7
  * @param  None
  * @retval None
  */
void SystemClock_Config_spi(void)
{
  RCC_ClkInitTypeDef RCC_ClkInitStruct;
  RCC_OscInitTypeDef RCC_OscInitStruct;
  HAL_StatusTypeDef ret = HAL_OK;

  /* Enable HSE Oscillator and activate PLL with HSE as source */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 25;
  RCC_OscInitStruct.PLL.PLLN = 432;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 9;
  RCC_OscInitStruct.PLL.PLLQ = 7;

  ret = HAL_RCC_OscConfig(&RCC_OscInitStruct);
  if(ret != HAL_OK)
  {
    while(1) { ; }
  }

  /* Activate the OverDrive to reach the 216 MHz Frequency */
  ret = HAL_PWREx_EnableOverDrive();
  if(ret != HAL_OK)
  {
    while(1) { ; }
  }

  /* Select PLL as system clock source and configure the HCLK, PCLK1 and PCLK2 clocks dividers */
  RCC_ClkInitStruct.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2);
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  ret = HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_7);
  if(ret != HAL_OK)
  {
    while(1) { ; }
  }
}

/**
  * @brief  CPU L1-Cache enable.
  * @param  None
  * @retval None
  */
static void CPU_CACHE_Enable(void)
{
  /* Enable I-Cache */
  SCB_EnableICache();

  /* Enable D-Cache */
  SCB_EnableDCache();
}


/**
  * @brief  This function is executed in case of error occurrence.
  * @param  None
  * @retval None
  */
static void Error_Handler(void)
{
  /* Configure LED1 which is shared with SPI2_SCK signal */
  BSP_LED_Init(LED1);
  BSP_LED_Off(LED1);
  while(1)
  {
    /* Toogle LED1 for error */
    BSP_LED_Toggle(LED1);
    HAL_Delay(1000);
  }
}


enum {
	TRANSFER_WAIT,
	TRANSFER_COMPLETE,
	TRANSFER_ERROR
};
TS_StateTypeDef  TS_State = {0};

uint16_t maxX, minX, maxY, minY;

typedef struct point_t
{
	uint16_t x;
	uint16_t y;
}point;

#define MAX_POINTS 200
#define MAX_SERIES 20
typedef struct series_t
{
	point points [MAX_POINTS];
	uint16_t points_nb;
}series;

typedef struct image_t
{
	series point_series [MAX_SERIES];
	uint16_t series_nb;
}image;

void resetTouchInfos(){
	maxX=0;
	maxY=0;
	minY=480;
	minX=800;
}

void drawline(int x0, int y0, int x1, int y1, int width, int height, char **imageWithLine)
{
	point p0; point p1;

	int32_t dx, sx, dy, sy, err, e2;
	p0.x=x0;
	p0.y=y0;
	p1.x=x1;
	p1.y=y1;
	dx	= abs(p1.x - p0.x);
	sx	= p0.x < p1.x ? 1 : -1;
	dy	= abs(p1.y - p0.y);
	sy	= p0.y < p1.y ? 1 : -1;
	err	= (dx > dy ? dx : -dy) / 2;

	for (;;) {
		/* check that we don't overflow*/
		if( (p0.x < width) && (p0.y < height)) {
			imageWithLine[p0.x][p0.y]=0;

			/* For debug only */
			/*BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
			BSP_LCD_FillCircle(p0.x+10,p0.y+10,1);*/
		}

		if (p0.x == p1.x && p0.y == p1.y)
			break;
		e2 = err;
		if (e2 > -dx) {
			err -= dy;
			p0.x += sx;
		}
		if (e2 <  dy) {
			err += dx;
			p0.y += sy;
		}
	}
}


void create_bitmap(image *imageNew, char **bitmap28x28)
{
	float facteur = 20;
	int Width, Height;
	int Width_Rescaled=28, Height_Rescaled=28;

	Width = maxX - minX + 2;
	Height = maxY - minY + 2;

	float factx = (Width)/facteur;
	float facty = (Height) /facteur;

	/* Update point series for 20x20 Image */
	for (int i = 0; i < imageNew->series_nb; i++) {
		for (int j = 0; j < imageNew->point_series[i].points_nb; j++) {
			imageNew->point_series[i].points[j].x = (int) floor((imageNew->point_series[i].points[j].x-minX)/factx);
			imageNew->point_series[i].points[j].y = (int) floor((imageNew->point_series[i].points[j].y-minY)/facty);
		}
	}

	for (int w =0; w < Width_Rescaled; w++)
		for (int h= 0 ; h < Height_Rescaled ; h++)  bitmap28x28[w][h]  = 0xff;

	for (int i = 0; i < imageNew->series_nb; i++) {
		for (int j = 0; j < imageNew->point_series[i].points_nb - 1; j++) {
			drawline(imageNew->point_series[i].points[j].x+4,
					imageNew->point_series[i].points[j].y+4,
					imageNew->point_series[i].points[j+1].x+4,
					imageNew->point_series[i].points[j+1].y+4,
					Width_Rescaled,
					Height_Rescaled,
					(char **)bitmap28x28);
		}
	}
}

void rotate_bitmap(char **bitmap28x28, char *bitmap28x28_rotated)
{
	int Width_Rescaled=28, Height_Rescaled=28;

	/* rotate image 90�, compensate LCD rotation => input for AI algo */
	for (int x=0; x<Width_Rescaled; x++) {
		for (int y=0; y<Height_Rescaled; y++) {
			bitmap28x28_rotated[y*Width_Rescaled + x] = bitmap28x28[x][y];
		}
	}
}

void printplot(char **bitmap28x28){
	int Width_Rescaled=28, Height_Rescaled=28;

	/* Draw rescaled image */
	for (int j =0; j < Width_Rescaled; j++)
	{
		for (int i= 0 ; i < Height_Rescaled ; i++)
		{
			if (bitmap28x28[i][j]==0x0)
			{
				BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
			}
			else BSP_LCD_SetTextColor(LCD_COLOR_WHITE);

			BSP_LCD_FillCircle(i+10,j+10,1);
		}
	}
	/* Restore back the default color */
	BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
}

extern const void *              pSoundWav;

/** UART  ***************************************/

void service_ChapeauUart_task(void  const * argument)
{
	/* you can use this thread to handle UART communication */

	AVS_TRACE_INFO("start Harry Potter Uart thread, are you talking to me ?");

	/* init code */

	while (1) {
		/* loop. Don't forget to use osDelay to allow other tasks to be scedulled */
		osDelay(10);
	}
}
/**********************************************/


/** LEDs ***************************************/

void service_ChapeauLed_task(void  const * argument)
{
	AVS_TRACE_INFO("start Harry Potter Led thread, and the light goes on");
	/* you can use this thread to handle LEDs */
	SPI_HandleTypeDef SpiHandle;

	/* Buffer used for transmission */
	uint8_t aTxBuffer[] = "****SPI - Two Boards communication based on DMA **** SPI Message ******** SPI Message ******** SPI Message ****";

	/* Buffer used for reception */
	uint8_t aRxBuffer[BUFFERSIZE];
	__IO uint32_t wTransferState = TRANSFER_WAIT;


	/* init code */
	CPU_CACHE_Enable();
	HAL_Init();

	/* Configure the system clock to 216 MHz */
	SystemClock_Config_spi();

	/*##-1- Configure the SPI peripheral #######################################*/
	/* Set the SPI parameters */
	SpiHandle.Instance               = SPIx;
	SpiHandle.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_32;
	SpiHandle.Init.Direction         = SPI_DIRECTION_2LINES;
	SpiHandle.Init.CLKPhase          = SPI_PHASE_1EDGE;
	SpiHandle.Init.CLKPolarity       = SPI_POLARITY_HIGH;
	SpiHandle.Init.DataSize          = SPI_DATASIZE_8BIT;
	SpiHandle.Init.FirstBit          = SPI_FIRSTBIT_MSB;
	SpiHandle.Init.TIMode            = SPI_TIMODE_DISABLE;
	SpiHandle.Init.CRCCalculation    = SPI_CRCCALCULATION_DISABLE;
	SpiHandle.Init.CRCPolynomial     = 7;
	SpiHandle.Init.NSS               = SPI_NSS_SOFT;

#ifdef MASTER_BOARD
	SpiHandle.Init.Mode = SPI_MODE_MASTER;
#else
	SpiHandle.Init.Mode = SPI_MODE_SLAVE;

	/* Slave board must wait until Master Board is ready. This to guarantee the
		 correctness of transmitted/received data */
	HAL_Delay(5);
#endif /* MASTER_BOARD */

	if(HAL_SPI_Init(&SpiHandle) != HAL_OK)
	{
		/* Initialization Error */
		Error_Handler();
	}

#ifdef MASTER_BOARD
	/* Configure User push-button button */
	BSP_PB_Init(BUTTON_USER,BUTTON_MODE_GPIO);
	/* Wait for User push-button press before starting the Communication */
	while (BSP_PB_GetState(BUTTON_USER) != GPIO_PIN_SET)
	{
		HAL_Delay(100);
	}
#endif /* MASTER_BOARD */

	while (1) {
		/*##-2- Start the Full Duplex Communication process ########################*/
		/* While the SPI in TransmitReceive process, user can transmit data through
		     "aTxBuffer" buffer & receive data through "aRxBuffer" */
		if(HAL_SPI_TransmitReceive_DMA(&SpiHandle, (uint8_t*)aTxBuffer, (uint8_t *)aRxBuffer, BUFFERSIZE) != HAL_OK)
		{
			/* Transfer error in transmission process */
			Error_Handler();
		}

		/*##-3- Wait for the end of the transfer ###################################*/
		/*  Before starting a new communication transfer, you must wait the callback call
		      to get the transfer complete confirmation or an error detection.
		      For simplicity reasons, this example is just waiting till the end of the
		      transfer, but application may perform other tasks while transfer operation
		      is ongoing. */
		while (wTransferState == TRANSFER_WAIT)
		{
		}

		/* loop. Don't forget to use osDelay to allow other tasks to be scedulled */
		switch (AVS_Get_State(hInstance))
		{
		case AVS_STATE_START_CAPTURE:

			break;
		case AVS_STATE_STOP_CAPTURE:
			break;
		default:
			break;
		}

		osDelay(10);
	}
}

/**********************************************/


/** User Interface *******************************/

void redraw(){
	/* Clear the LCD */
	BSP_LCD_Clear(LCD_COLOR_WHITE);

	//****************************************************************
	// Top panel
	//****************************************************************

	// "Title" box
	BSP_LCD_SetTextColor(LCD_COLOR_DARKBLUE);
	BSP_LCD_FillRect(0, 0, 200, 80);
	BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
	BSP_LCD_SetBackColor(LCD_COLOR_DARKBLUE);
	BSP_LCD_SetFont(&Font24);
	BSP_LCD_DisplayStringAt(10, 40, (uint8_t *)"ST Team", LEFT_MODE);

	// Display it
	BSP_LCD_SetTextColor(LCD_COLOR_GREEN);
	BSP_LCD_FillRect(200, 0, 600, 80);
	BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
	BSP_LCD_SetBackColor(LCD_COLOR_GREEN);
	BSP_LCD_SetFont(&Font24);
	BSP_LCD_DisplayStringAt(210, 40, (uint8_t *)"wait msg", RIGHT_MODE);

	//****************************************************************
	// Left panel
	//****************************************************************

	// "Alexa" box
	BSP_LCD_SetTextColor(LCD_COLOR_CYAN);
	BSP_LCD_FillRect(0, 80, 200, 100);
	BSP_LCD_SetBackColor(LCD_COLOR_CYAN);
	BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
	BSP_LCD_SetFont(&Font24);
	BSP_LCD_DisplayStringAt(10, 130, (uint8_t *)"Alexa", LEFT_MODE);

	// "Send" box
	BSP_LCD_SetTextColor(LCD_COLOR_LIGHTBLUE);
	BSP_LCD_FillRect(0, 180, 200, 100);
	BSP_LCD_SetBackColor(LCD_COLOR_LIGHTBLUE);
	BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
	BSP_LCD_SetFont(&Font24);
	BSP_LCD_DisplayStringAt(10, 230, (uint8_t *)"Send", LEFT_MODE);

	// "Clear" box
	BSP_LCD_SetTextColor(LCD_COLOR_LIGHTBLUE);
	BSP_LCD_FillRect(0, 380, 200, 100);
	BSP_LCD_SetBackColor(LCD_COLOR_LIGHTBLUE);
	BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
	BSP_LCD_SetFont(&Font24);
	BSP_LCD_DisplayStringAt(10, 430, (uint8_t *)"Clear", LEFT_MODE);


	//****************************************************************
	// Right panel
	//****************************************************************
	// ...

	// Lines
	BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
	BSP_LCD_DrawLine(0, 80, 800, 80);
	BSP_LCD_DrawLine(200, 0, 200, 480);
	BSP_LCD_DrawLine(600, 80, 600, 480);
	BSP_LCD_DrawLine(0, 180, 200, 180);
	BSP_LCD_DrawLine(0, 280, 200, 280);
	BSP_LCD_DrawLine(0, 380, 200, 380);

}


void service_Chapeau_task(void  const * argument)
{
	uint16_t x1, y1;
	uint16_t compteur = 0;
	uint8_t AVSrunning = 0;
	image* imageNew;
	uint16_t points_nb;
	char **bitmap;
	char *bitmap_rotated;

	AVS_TRACE_INFO("start Harry Potter thread, welcome in magic school !");

	BSP_LCD_LayerDefaultInit(0, LCD_FB_START_ADDRESS);

	/* Touchscreen initialization */
	if (BSP_TS_Init(BSP_LCD_GetXSize(), BSP_LCD_GetYSize()) == TS_ERROR) {
		AVS_TRACE_INFO("BSP_TS_Init error\n");
	}
	redraw();
	resetTouchInfos();

	int n_retry = NB_RETRY_RELEASE_DETECTION;
	int point_series_on_going = 0;
	int release_nb = 0;

	imageNew = malloc(sizeof(image));
	if(!imageNew)
		AVS_TRACE_ERROR("Fails to allocate image");

	bitmap = (char **)malloc(28 * sizeof(char *));
	if(!bitmap)
		AVS_TRACE_ERROR("Fails to allocate bitmap");

	for (int i=0; i<28; i++) {
		bitmap[i] = (char *)malloc(28 * sizeof(char));
		if(!bitmap[i])
			AVS_TRACE_ERROR("Fails to allocate bitmap column");
	}

	bitmap_rotated = (char *)malloc(28 * 28 * sizeof(char *));
	if(!bitmap_rotated)
		AVS_TRACE_ERROR("Fails to allocate bitmap_rotated");

	memset(imageNew, 0, sizeof(image));

	BSP_LCD_SetTextColor(LCD_COLOR_BLACK);

	while (1) {

		BSP_TS_GetState(&TS_State);
		/* if no touch detected, check for release state */
		if(!TS_State.touchDetected && point_series_on_going) {
			if (--n_retry == 0) {
				point_series_on_going = 0;
				AVS_TRACE_INFO("Release", release_nb);
				n_retry = NB_RETRY_RELEASE_DETECTION;
			}
		}

		if(TS_State.touchDetected) {
			n_retry = NB_RETRY_RELEASE_DETECTION;
			/* One or dual touch have been detected          */

			/* Get X and Y position of the first touch post calibrated */
			x1 = TS_State.touchX[0];
			y1 = TS_State.touchY[0];

			//AVS_TRACE_INFO("Touch Detected x=%d y=%d  %d\n", x1, y1, compteur);

			if ((x1 > 0) && ( x1 < 200) && (y1 > 80) && ( y1 < 480)){
				// Left panel
				if ((x1 > 0) && ( x1 < 200) && (y1 > 80) && ( y1 < 180))
				{
					AVS_TRACE_INFO("Touch detected : 'Alexa' button\n");

					AVS_Play_Sound(hInstance, AVS_PLAYSOUND_PLAY, (void *)(uint32_t)pSoundWav, 100);

					/* Starts the capture only if northing occurs on the system ( ie BUSY etc...) */
					if(AVS_Set_State(hInstance, AVS_STATE_START_CAPTURE) == AVS_OK){
						/* Capture started and button pressed */
						//buttonState = TRUE;
					}

					osDelay(100); // to avoid multiple detections
				} else if ((x1 > 0) && ( x1 < 200) && (y1 > 180) && ( y1 < 280))
				{
					AVS_TRACE_INFO("Touch detected : 'Send' button\n");

					/* Save only if point series is finishedn, users has released the touch
					 * before pressing the Save area */
					if (point_series_on_going == 1)
						continue;

					/* Print only if at least a touch was detected */
					if(imageNew->series_nb) {

						create_bitmap(imageNew, bitmap);
						rotate_bitmap(bitmap, (char *)bitmap_rotated);
					}

					memset(imageNew, 0, sizeof(image));
					point_series_on_going = 0;
					resetTouchInfos();

					// redraw the full UI and draw the last drawn symbol
					redraw();
					printplot(bitmap);

					osDelay(100); // to avoid multiple detections

				} else if ((x1 > 0) && ( x1 < 200) && (y1 > 380) && ( y1 < 480))
				{
					AVS_TRACE_INFO("Touch detected : 'Clear' button\n");

					/* Clear only if point series is finished, users has released the touch
					 * before pressing the Save area */
					if (point_series_on_going == 1)
						continue;

					redraw();
					BSP_LCD_SetTextColor(LCD_COLOR_BLACK);

					memset(imageNew, 0, sizeof(image));
					point_series_on_going = 0;

					osDelay(100); // to avoid multiple detections

				}
			} else if ((x1 > 200) && ( x1 < 600) && (y1 > 80) && ( y1 < 480))
			{
				//AVS_TRACE_INFO("Touch detected : 'Middle' button\n");
				if(!point_series_on_going) {
					/* Create new point series */
					point_series_on_going = 1;
					imageNew->series_nb++;
					if (imageNew->series_nb > MAX_SERIES) {
						AVS_TRACE_INFO("MAX series reached, clear Image");
						memset(imageNew, 0, sizeof(image));
						point_series_on_going = 0;
					}
				}

				/* Get X and Y position of the first touch post calibrated */
				imageNew->point_series[imageNew->series_nb-1].points_nb++;
				points_nb = imageNew->point_series[imageNew->series_nb-1].points_nb;
				imageNew->point_series[imageNew->series_nb-1].points[points_nb-1].x = x1;
				imageNew->point_series[imageNew->series_nb-1].points[points_nb-1].y = y1;

				/* if we reach the maximum pointpoint_series_on_goings number of a serie, create a new one */
				if(points_nb >= MAX_POINTS) {
					point_series_on_going = 0;
					AVS_TRACE_INFO(" Max Points reached, create new serie");
				}

				if (x1>maxX) maxX= x1;
				if (y1>maxY) maxY= y1;
				if (x1<minX) minX= x1;
				if (y1<minY) minY = y1;
				//AVS_TRACE_INFO("max x=%d y=%d\n", maxX, maxY);
				//AVS_TRACE_INFO("min x=%d y=%d\n", minX, minY);
				BSP_LCD_FillCircle(x1, y1, 10);

			}
			osDelay(10);
		}else {
			osDelay(5);
		}
	}
	free(imageNew);
	for (int i=0; i<28; i++)
		free(bitmap[i]);
	free(bitmap);
	free(bitmap_rotated);
}





