#01. DeviceRunning
#02. StartLittleCircle
#03. DoLittleCircle
#04. LittleSequenceCounterOverflow
#05. BigSequenceCounterOverflow
#06. EndLeftTerminateSensor
#07. EndRightTerminateSensor
#08. TurnLeftTerminateSensor
#09. TurnRightTerminateSensor
#10. TurnNormalTerminateSensor
#11. TimerClockOverflow
#12. TimerClockOverflow2
#13. TimerClockOverflow3
#14. PositionSensor
#15. PositionSensor2
#16. PositionSensor3
#17. PositionSensor4
#18. OpenGateSensor
#19. OpenGateSensor2
#20. OpenGateSensor3

#01.02.03.04.05.06.07.08.09.10.11.12.13.14.15.16.17.18.19.20
  1  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0
#Поиск начального положения
  1  0  0  0  0  1  0  0  0  0  0  0  0  0  0  0  0  0  0  0
  1  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0
  1  0  0  0  0  0  0  0  0  0  0  0  0  1  0  0  0  0  0  0
  1  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0
  1  0  0  0  0  0  0  0  0  0  0  0  0  0  1  0  0  0  0  0
  1  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0
  1  0  0  0  0  0  0  0  0  0  0  0  0  0  0  1  0  0  0  0
  1  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0
  1  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  1  0  0  0
  1  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0
  1  0  0  0  0  0  1  0  0  0  0  0  0  0  0  0  0  0  0  0
  1  1  0  0  0  0  1  0  0  0  0  0  0  0  0  0  0  0  0  0
  1  0  1  0  0  0  1  0  0  0  0  0  0  0  0  0  0  0  0  0
  1  0  0  0  0  0  1  0  0  0  0  0  0  0  0  0  0  0  0  0
#Начинаем движение
  1  0  0  0  0  0  1  0  0  0  0  0  0  0  0  0  0  0  0  0
  1  0  0  0  0  0  1  0  0  0  0  0  0  0  0  0  0  0  0  0
  1  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0
  1  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0
  1  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0
  1  0  0  0  0  0  0  0  0  0  0  0  0  1  0  0  0  0  0  0
#Приехали к первому бункеру
  1  0  0  0  0  0  0  0  0  0  0  0  0  1  0  0  0  0  0  0
  1  0  0  0  0  0  0  0  0  0  0  0  0  1  0  0  0  0  0  0
  1  0  0  0  0  0  0  0  0  0  0  0  0  1  0  0  0  0  0  0
#Открывается залонка первого бункера
  1  0  0  0  0  0  0  0  0  0  0  0  0  1  0  0  0  1  0  0
  1  0  0  0  0  0  0  0  0  0  0  0  0  1  0  0  0  1  0  0
  1  0  0  0  0  0  0  0  0  0  0  0  0  1  0  0  0  1  0  0
  1  0  0  0  0  0  0  0  0  0  0  0  0  1  0  0  0  1  0  0
  1  0  0  0  0  0  0  0  0  0  0  0  0  1  0  0  0  1  0  0
  1  0  0  0  0  0  0  0  0  0  0  0  0  1  0  0  0  1  0  0
#Сработал сигнал таймера первого бункера
  1  0  0  0  0  0  0  0  0  0  1  0  0  1  0  0  0  1  0  0
#Сброс таймера в исходную и ожидание закрытия заслонки
  1  0  0  0  0  0  0  0  0  0  0  0  0  1  0  0  0  1  0  0
  1  0  0  0  0  0  0  0  0  0  0  0  0  1  0  0  0  1  0  0
  1  0  0  0  0  0  0  0  0  0  0  0  0  1  0  0  0  1  0  0
#Заслонка первого бункера закрыта = первый малый цикл прошел
  1  0  0  0  0  0  0  0  0  0  0  0  0  1  0  0  0  0  0  0
  1  0  0  0  0  0  0  0  0  0  0  0  0  1  0  0  0  0  0  0
#Окончание
  1  0  0  0  0  0  0  0  0  0  0  0  0  1  0  0  0  0  0  0
  1  1  0  0  0  0  0  0  0  0  0  0  0  1  0  0  0  0  0  0
  1  1  0  0  0  0  0  0  0  0  0  0  0  1  0  0  0  0  0  0
  1  1  0  0  0  0  0  0  0  0  0  0  0  1  0  0  0  0  0  0
  1  1  0  0  0  0  0  0  0  0  0  0  0  1  0  0  0  0  0  0
#Позиционирование 2
  1  1  0  0  0  0  0  0  0  0  0  0  0  1  0  0  0  0  0  0
  1  1  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0
  1  1  0  0  0  0  0  0  0  0  0  0  0  0  1  0  0  0  0  0
  1  1  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0
  1  1  0  0  0  0  0  0  0  0  0  0  0  0  0  1  0  0  0  0
#Отгрузка
  1  1  0  0  0  0  0  0  0  0  0  0  0  0  0  1  0  0  0  0
  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0
#01.02.03.04.05.06.07.08.09.10.11.12.13.14.15.16.17.18.19.20
