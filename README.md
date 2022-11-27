make --trace -j 8 		собрать

make --trace clean 		очистить

make flash			загрузить прошивку


При отправке строки "S@FFF00000@E" через терминал COM порта,
если приходит ответ "memtest_ok", значит память Flash и USB работоспособны.


![STM32F205_template_diagramm](https://user-images.githubusercontent.com/52402483/204126306-ed2a4b8a-ce93-4a37-adcb-43e90b236d92.png)
![пример_теста_CAN_FLASH_USB](https://user-images.githubusercontent.com/52402483/204126307-9f65fced-c76d-463a-a2b6-d8bf731d0e67.png)
