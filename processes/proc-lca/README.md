# proc-lca


## Описание

Представим классическую ситуацию: в вашей системе есть два ненавистных вам процесса `x` и `y` и вы бы хотели избавиться от них, потратив всего один системный вызов `kill`. С одной стороны, вы настолько не хотите их видеть, что готовы принести в жертву и другие процессы, а с другой - хотите эти жертвы минимизировать. Оптимальное решение - найти общего предка `p` процессов `x` и `y`, а затем отправить сигнал группе процессов:

```cpp
kill(-p, SIGKILL);
```

Осталась всего одна преграда - почему-то никто не добавил в ядро функцию для поиска LCA двух процессов.

(Если читателю требуется ещё один пример из реальной жизни, то, зная pid двух процессов, находящихся внутри одного контейнера, можно найти pid корневого процесса этого контейнера)


## Задание

В файле `lca.c` находится функция `find_lca`, которую вам и требуется реализовать. 

Особенности реализации:

- Запрещается использовать функцию `system`
- Глубина дерева процессов в тестах составляет не более `MAX_TREE_DEPTH`
- Алгоритм поиска LCA не должен быть алгоритмически эффективным, то есть подойдёт реализация, работающая за линейное время от глубины дерева процессов

## Дополнительная информация 

- [procfs](https://man7.org/linux/man-pages/man5/proc.5.html)
