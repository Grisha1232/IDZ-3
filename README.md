# Селезнев Григорий
# Вариант 5

## Условие
Задача об обедающих философах. Пять философов сидят возле круглого стола. Они проводят жизнь, чередуя приемы пищи и
размышления. В центре стола находится большое блюдо спагетти.
Спагетти длинные и запутанные, философам тяжело управляться с ними, поэтому каждый из них, что бы съесть порцию, должен пользоваться двумя вилками. К несчастью, философам дали
только пять вилок. Между каждой парой философов лежит одна вилка, поэтому эти высококультурные и предельно вежливые
люди договорились, что каждый будет пользоваться только теми
вилками, которые лежат рядом с ним (слева и справа). Написать
программу, моделирующую поведение философов Программа должна избегать фатальной ситуации, в которой все философы
голодны, но ни один из них не может взять обе вилки (например,
каждый из философов держит по одной вилки и не хочет отдавать
ее). Решение должно быть симметричным, то есть все
процессы–философы должны выполнять один и тот же
код. Философы являются отдельными клиентами, синхронизируемыми посредством сервера.

## Запуск
Сервер:
```
g++ server.c -o server
./server
```
> После запуска сервера будет предложено выбрать порт, после ввода порта будет выведено сообщение о том, что сервер запущен (или же выведет ошибку) и перейдет в ожидание 5 клиентов (философов), после начинается обед

Клиенты:
```
g++ client.c -o client
./client
```
> После запуска очередного клиента (философа) будет запрошено ввести IP и порт, а также номер философа (от 1 до 5). После подключения всех философов начинается обед.

> Запуск клиентов осуществляется с пяти разных терминалов


# Отчет на 4-5:

[Код сервера и клиента](https://github.com/Grisha1232/IDZ-3/tree/main/4-5)

Сценарий:
1. Запускается сервер, указывается порт серера. После Сервер уходит в ожидание 5 клиентов.
2. Запускается 5 клиентов, указывается IP и порт, а также id философа.
3. После того как 5 клиентов подключится к серверу, начинается обед: (1) сервер отправляет всем клиентам сообщение о начале обеда, (2) клиенты в случайное время (сделано специально, чтобы можно было проследить, что происходит) отправляют сообщение о "бронирование" вилок серверу, (3) сервер проверяет может ли философ взять вилки, если нет отправляет соответсвующее сообщение клиенту, иначе отправляет сообщение о удачном взятие вилок.
4. Клиент принимает результат бронирование, обедает и отправляет сообщение серверу о возвращении вилок.
5. Каждый клиент должен "поесть" 5 раз. После чего клиент завершает свою программу
6. Сервер работает, что называется, до последнего клиента (Каждый клиент должен поесть 5 раз)
7. После чего сервер завершает свое выполнение.

К сожалению не смог проверить на двух компьютерах в одной сети (Но уверен это работает)

[Примеры выводов](https://github.com/Grisha1232/IDZ-3/blob/main/4-5/test.md)

# Отчет на 6-7
[Код сервера, клиента и наблюдателя](https://github.com/Grisha1232/IDZ-3/tree/main/6-7)

Добавлен файл наблюдатель [viewer](https://github.com/Grisha1232/IDZ-3/blob/main/6-7/viewer.cpp), который будет выводить всю информацию о происходящем

Запуск:
1. Сначала обязательно запустить viewer:
```
g++ viewer.cpp -o viewer
./viewer
```
2. Запустить сервер (все тоже самое что и в отчете выше)
3. Запустить 5 клиентов

[Примеры выводов]()
