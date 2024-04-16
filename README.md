# Умная лампа
>Данный проект показывает работу устройства на базе ESP8266, управляемую с помощью Яндекс Алисы.
## Введение


Уже несколько лет Яндекс активно работает над развитием системы управления умным домом с помощью голосового помощника Алиса. Эта система позволяет управлять различными устройствами в вашем доме, такими как свет, телевизор, кофемашина, умные пылесосы и термостаты.

Для создания экосистемы умных устройств не требуется огромных затрат и поиска особенных гаджетов. Яндекс предоставляет открытый интерфейс приложений (API), позволяющий интегрировать ваши собственные решения в платформу умного дома через набор навыков.

Одним из таких навыков является "[Домовёнок Кузя](https://dialogs.yandex.ru/store/skills/3229e372-domovenok-kuzya)". Его можно использовать для интеграции произвольных устройств в систему умного дома и управления ими с помощью Алисы. Это позволяет создать полностью настраиваемую и персонализированную систему автоматизации, отвечающую именно вашим потребностям.

Процесс интеграции собственного устройства с помощью навыка "Домовёнок Кузя" несложен и может быть выполнен относительно быстро. Он включает в себя следующие шаги:


1. Создание навыка в Яндекс Диалогах. Здесь вы определяете команды, которые будут использоваться для взаимодействия с вашим устройством, а также действия, которые будут выполняться при их вводе.
2. Интеграция навыка с вашим устройством. Это включает в себя создание приложения или сервиса, который будет взаимодействовать с вашим устройством и выполнять команды, получаемые от навыка.
3. Публикация навыка в Яндекс Диалогах. После интеграции навык должен быть опубликован, чтобы его можно было использовать в системе умного дома.


Таким образом, используя навык "Домовёнок Кузя" и API Яндекса, вы можете без труда интегрировать свои собственные устройства в систему умного дома и управлять ими с помощью голосового помощника Алиса. Это открывает широкие возможности для создания уникальных и удобных решений для автоматизации вашего дома.



## Сборка тестового устройства и описание.
### Схема:
![Untitled Sketch 2_bb](https://github.com/BNB77/smartlamp/assets/81864906/54b69d58-caaf-4dca-b773-568de5cc8dc2)
Используемые компоненты: плата ESP8266 D1 mini, OLED-дисплей, RGB лента, модуль зарядки, холдер 18650, акб 18650, кнопка.
>С точки зрения пайки проект достаточно не сложный, но большой объем.
### Корпус:
Каркас моего проекта был разработан в программе Autodesk Fusion 360.
![Рисунок1](https://github.com/BNB77/smartlamp/assets/81864906/95dde8df-e212-4e14-9cd2-3e2f42ee1860)

>Корпус состоит из 2 деталей: основание, рассеивающий купол. Основание напечатано из PLA пластика непрозрачного белого цвета. Рассеивающий купол – полупрозрачный белый цвет.
>Корпус перепечатывался несколько раз из-за неточности 3-D печати. Сборка устройства тоже не сложная, достаточно поставить всё в сови пазы. Но вот RGB ленту пришлось горячим клеем.
### Программа:
_Весь код написан на языке C++_

_Код получился достаточно объмным более 500 строк._

Прежде всего я решил написать программу для работы RGB ленты.
![image](https://github.com/BNB77/smartlamp/assets/81864906/c3dffb8b-e43e-4a61-8bb8-b453b0608786)

***После я приступил к написанию программы для работы часов. Так же я решил добавит не только время, но и информацию о погоде.***
![image](https://github.com/BNB77/smartlamp/assets/81864906/0c87dc08-a0e3-441c-b30b-d1c8e43ecec9)
Для вывода погоды в реальном времени мне помог сайт [openWeatherMap](https://openweathermap.org/). С помощью токена мы можем брать в нашем коде информацию о погоде.

**Далее я интегрировал к этому всему Яндекс Алису и её навык "Домовёнок Кузя".**

Принцип работы получился такой:![image](https://github.com/BNB77/smartlamp/assets/81864906/08ea4b50-0042-4106-85f8-d445272244e6)

С помощью сервиса [Blynk](https://blynk.io/) устанавливается связь между платой и навыком.
В Blynk создаем виртуальный пин которым можно управлять как с помощью приложежния Blynk, так и Яндекс Алисой. Виртуальный пин выдает только 2 значения: 1 и 0. Далее в "void loop" идет проверка изменения значения и взависимотси от него лампа включена или выключена.








