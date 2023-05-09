# Embeded_LCD_with_touch
有STM32控制的TFLCD屏显示，单点触控（之后可能修改）
## 硬件部分
* 控制芯片STM32F103ZET6
* TFLCD：NE35510
* 触摸板：gt911（电容）
## 软件部分
* STM32CubeMX
* NOR Flash/LCD1：
  * Chip Select：NE4
  * LCD Resgister Select：A10 
* GPIO
  * 两个LED：PB5,PE5
  * LCD背光：PB0
  * 按键：PE0,PE4,PA0（未定义功能）
* 软件IIC
