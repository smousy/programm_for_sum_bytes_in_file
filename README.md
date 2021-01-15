# multithreading programm for counting bytes in chosen directory
 
# Основные задания: #
* Необходимо реализовать программу для подсчета суммы значений байт каждого файла в папке.
* Программа должна иметь возможность задать/выбрать папку для подсчета.
* Подсчет суммы должен выполняться не в контексте основного потока.
* Результат подсчета должен быть сохранен в эту же папку в виде xml-файла с указанием имени каждого файла и его суммы байт.

# Дополнительные задания: #
* По возможности максимально использовать все ядра процессора.
* Результаты подсчетов должны выводиться на экран по мере получения результатов.
* Обрабатывать все файлы во всех подпапках.

## Дополнительные библиотеки: ##
* boost :: asio - https://www.boost.org/ .
* TinyXML - https://sourceforge.net/projects/tinyxml/ .
