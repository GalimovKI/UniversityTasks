# proc-chain


## Описание

    При работе в терминале иногда возникает потребность в передаче выхода одной команды на вход другой. Добиться такого поведения можно с помощью пайпов:

    ```bash
    cat students.csv | grep Musk | ...
    ```

    Если этого требует логика обработка данных, то такие пайплайны могут быть достаточно длинными, при этом они не испытывают трудностей при обработке большого потока данных, который целиком не влезает в буфер обычного пайпа.
    
    В данной задаче вам предстоит реализовать программу, которая с некоторыми упрощениями будет повторять поведение пайпов в терминале.


## Задание

    От вас требуется написать программу, принимающую в качестве аргумента строку, состоящую из команд, которые нужно выполнить:

    ```bash
    ./chainy "pwd | wc -c"
    ```

    Особенности реализации:

    - Запрещается использовать функцию `system` или другие способы сразу выполнить строку из команд
    - Запрещается изменять размер пайпов
    - Общение между выполняемыми командами должно происходить через пайпы
    - Реализация должна поддерживать передачу данных размером больше размера пайпа
    - В тестах могут быть пайплайны, состоящие не более чем из `MAX_CHAIN_LINKS_COUNT` команд, каждая из которых принимает не более `MAX_ARGS_COUNT` аргументов

    В файле `chainy.c` находится шаблон решения, но вам необязательно использовать именно его, поскольку в этой задаче проверяется не интерфейс, а результат работы вашей программы.


## Тесты

    Протестировать своё решение можно с помощью публичных тестов, описанных в `tester.py`.


## Дополнительная информация 

    - [exec](https://man7.org/linux/man-pages/man3/exec.3.html)
    - [pipe](https://man7.org/linux/man-pages/man2/pipe.2.html)
    - [dup2](https://man7.org/linux/man-pages/man2/dup.2.html)
