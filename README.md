# Практикум №1 (вариант 18)
###### Даны α и буква x. Найти максимальное k, такое что в L есть слова, начинающиеся с x<sup>k</sup>.  
____  
**Предупреждение:** ответом на первый тест из ТЗ (ab+c.aba.∗.bac.+.+∗ a) является 0, а не 2, т.к. в инфиксной нотации это ((ca+b)(ab)<sup>∗</sup>a + c(b+a))<sup>∗</sup>
____
P.S. Некоторые вещи, такие как std::min, std::max пришлось заменить на тернарный оператор ради увеличения Code Coverage.


