make --trace -j 8 		собрать
make --trace clean 		очистить
make flash			загрузить прошивку

При отправке строки "S@FFF00000@E" через терминал COM порта,
если приходит ответ "memtest_ok", значит память Flash и USB работоспособны.
