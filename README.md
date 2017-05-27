# MQ2-MQTT-Ethernet-Majordomo
Скетч для считывания и отправки через Ethernet соединение по протоколу MQTT в домашнюю систему автоматизации Majordomo

В скетче необходмо указать свой адресс MQTT брокера, порт, логин и пароль.Так же необходимо указать валидный IP адресс Ethernet Shild'a Arduino.

После привязки полученных значений в Majordomo для проверки срабатывания превышения порога концентрации природного газа необходимо зайти Объекты=>Timer=>onNewHour и вставить код:
https://github.com/Gelezako/MajorDomo-Scenarios/blob/master/onNewHour/AutoAlertMQ2.php

Реализация озвучивания команды
https://github.com/Gelezako/MajorDomo-Scenarios/blob/master/MQ2.php

Демонстрация применения и видео инструкция по настройке
https://www.youtube.com/watch?v=mB0fdDYdvEI
