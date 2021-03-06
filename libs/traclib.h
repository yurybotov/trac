#ifndef __TRACLIB_H__
#define __TRACLIB_H__
//// управление выполнением
void rt(litera*);  // ресет
void hl(litera*);  // остановка
void eq(litera*);  // сравнение если равно
void gr(litera*);  // сравнение если больше
void lt(litera*);  // сравнение если меньше

//// Ввод-вывод
void gm(litera*);  // возвращает текущий символ мета
void cm(litera*);  // заменяет текущий символ мета. Возвращает пустую строку
void rs(litera*);  // читает из входного буфера строку от начала до символа мета
                   // и переносит ее в конец активной цепочки. Мету из буфера
                   // выкидываем. Если все нормально возвращает пустую строку,
                   // в случае ошибки возвращает Z
void rc(litera*);  // читает из входного буфера один символ. Любой, включая мету.
                   // возвращает этот символ.
void ps(litera*);  // отправляет строку на текущее устройство вывода
void si(litera*);  // подключить подсистему ввода к файлу с именем N, если #(si)
                   // - стандартный ввод из терминала. Возвращает пустую строку.
void so(litera*);  // подключить подсистему вывода к файлу с именем N, если #(so)
                   // - стандартный вывод на терминал. Возвращает пустую строку

//// Арифметика

void rx(litera*);  // установить r-значную систему счисления
void ad(litera*);  // сложение
void su(litera*);  // вычитание
void ml(litera*);  // умножение
void dv(litera*);  // деление

//// Побитовые операции
void an(litera*);  // возвращает побитовое И
void or_(litera*);  // возвращает побитовое ИЛИ
void xr(litera*);  // возвращает побитовое XOR
void no(litera*);  // возвращает побитовое NOT
void bs(litera*);  // возвращает D1 сдвинутое влево на D2 бит
void br(litera*);  // возвращает D1 сдвинутое вправо на D2 бит

//// Отладка
void np(litera*);  // нет операции, можно использовать для временного скрытия кода
void nl(litera*);  // возвращает список имен форм, S - делимитер между именами
void pb(litera*);  // возвращает содержимое формы с именем S
void tr(litera*);  // начать трассировку
void tf(litera*);  // закончить трассировку

//// Формы
void ds(litera*);  // создает форму с именем N и значением S.
                   // Возвращает пустую строку
void ss(litera*);  // нарезка формы с именем N на сегменты ограниченные цепочками
                   // dx. Возвращает пустую строку
void cl(litera*);  // #(cl,n,d1,...,dn) вызывает форму n как функцию с параметрами dx
void cs(litera*);  // вызов формы с именем N как список. Возвращает одну
                   // "следующую" запись из списка. Если записей нет или они
                   // закончились возвращает Z
void cc(litera*);  // вызов формы N как массив символов. Возвращает один
                   // "следующий" символ. Если вернуть нечего возвращает Z
void cn(litera*);  // вызов формы N как массив полей равной длины D. Возвращает D
                   // символов, при отрицательном D выдаются символы слева от
                   // маркера при положительном - справа. При ошибке возвращает Z
void fd(litera*);  // перемещение внутреннего указателя в форме N на начало
                   // найденной в форме подстроки S. Возвращает пустую строку,
                   // а при ошибке - Z
void cr(litera*);  // обнуление внутреннего указателя в форме N. Возвращает
                   // пустую строку
void dd(litera*);  // удаление формы N. Возвращает пустую строку.
void da(litera*);  // удаление всех форм. Возвращает пустую строку.
void sb(litera*);  // сохранение форм n1.. в блок на диске с именем N.
                   // Возвращает пустую строку
void fb(litera*);  // загрузить все формы из блока на диске с именем N.
                   // Возвращает пустую строку
void eb(litera*);  // удалить блок на диске с именем N. Возвращает пустую строку

// другое

litera* loads(void);          // для rs
litera* formcall(litera*, litera*); // для cl
int findform(litera*);
int formadd(litera*, litera*); // для ds 
uint16_t hash(litera*);


#endif // __TRACLIB_H__
