
function Trac() {
    var ibuf = '';
    var abuf = '';
    var nbuf = '';
    var form = [];
    var obuf = '';
    //var pc = 0;
    var worked = true;
    var idle = '#(ps,#(rs))';
    var Z = false;
    var meta = '`';
    var funcvalue = '';
    
    // инициалиизация если надо будет перезапустить машину полностью
    var reset = function() {
        ibuf = '';
        abuf = '';
        nbuf = '';
        form = [];
        obuf = '';
        //var pc = 0;
        worked = true;
        idle = '#(ps,#(rs))';
        Z = false;
        meta = '`';
        funcvalue = '';
    };

    // сокращенное обращение к первым трем символам активной строки с заменой 
    // результата на пустой если строка короче чем нужно
    var c1 = function () { return (abuf.length > 0)? abuf[0]:''; };
    var c2 = function () { return (abuf.length > 1)? abuf[1]:''; };
    var c3 = function () { return (abuf.length > 2)? abuf[2]:''; };
    // удаление лидирующего символа активной строки
    var next = function (n) { n = n? n: 1; if (abuf.length > 0) abuf = abuf.substr(n); };
    // если аргумент не представлен заменить его на ''
    var get = function (strarr, i) { if (i > strarr.length - 1) return ''; else return strarr[i]; };
    // вычисляет хэш для формы
    var hash = function (s) { var a = 0; for (var i = 0; i < s.length; i++) { a = (a << 3) ^ i; } return a; };
    // добавляет форму
    var formadd = function (id, fname, fform, fptr, fcss) {
        formdel([fname]);
        form.push({ id: id, hash: hash(fname), name: fname, form: fform, ptr: fptr, css: fcss });
    };
    // возвращает количество схраненных форм
    var formlen = function () { return form.length; };
    // сегментирует форму
    var formseg = function (name, sarr) {
        var h = hash(name), i, j;
        for (i = 0; i < form.length; i++) {
            if (h === form[i].hash && name === form[i].name) {
                for (j = 0; j < sarr.length; j++) {
                    form[i].form = form[i].form.replace(new RegExp(sarr[j], 'g'), '\ufffc' + j);
                }
                break;
            }
        }
    };
    // вызывает функцию из формы
    var formcall = function (name, sarr) {
        var h = hash(name), s = '', i, j, f;
        for (i = 0; i < form.length; i++) {
            if (h === form[i].hash && name === form[i].name) {
                f = form[i].form;
                if (sarr.length < s.css) {
                    for (j = 0; j < sarr.length; j++) {
                        f = f.replace(new RegExp('\ufffc' + j, 'g'), sarr[j]);
                    }
                    for (j = sarr.length; j < s.css; j++) {
                        f = f.replace(new RegExp('\ufffc' + j, 'g'), '');
                    }
                } else {
                    for (j = 0; j < sarr.length; j++) {
                        f = f.replace(new RegExp('\ufffc' + j, 'g'), sarr[j]);
                    }
                }
                return f;
            }
        }
    };
    // достает из формы сегмент
    var formcallseg = function (name, z) {
        var h = hash(name), i, f;
        for (i = 0; i < form.length; i++) {
            if (h === form[i].hash && name === form[i].name) {
                f = form[i].form;
                if (form[i].ptr >= f.length) { Z = true; return z; }
                var j = f.indexOf('\ufffc', form[i].ptr);
                if (j >= 0) {
                    var ptr = form[i].ptr;
                    form[i].ptr = j + 2;
                    return f.substr(ptr, j - ptr);
                } else {
                    var ptr = form[i].ptr;
                    form[i].ptr = f.length;
                    return f.substr(ptr);
                }
            }
        }
    };
    // достает из формы символ
    var formcallchr = function (name, z) {
        var h = hash(name), i, j;
        for (i = 0; i < form.length; i++) {
            if (h === form[i].hash && name === form[i].name) {
                if (form[i].ptr >= form[i].form.length) { Z = true; return z; }
                j = form[i].ptr;
                if (form[i].form[j] === '\ufffc') { form[i].ptr += 2; j = form[i].ptr; }
                form[i].ptr++;
                return form[i].form[j];
            }
        }
    };
    // достает из формы символы
    var formcallchrs = function (name, d, z) {
        var h = hash(name), i, j, out = '';
        for (i = 0; i < form.length; i++) {
            if (h === form[i].hash && name === form[i].name) {
                if (d > 0) {
                    for (j = 0; j < d; j++) {
                        if (form[i].ptr >= form[i].form.length) { Z = true; return z; }
                        if (form[i].form[form[i].ptr] === '\ufffc') { form[i].ptr += 2; if (form[i].ptr >= form[i].form.length) { Z = true; return z; } }
                        out += form[i].form[form[i].ptr];
                        form[i].ptr++;
                    }
                } else {
                    for (j = 0; j > d; j--) {
                        if (form[i].ptr <= 0) { Z = true; return z; }
                        if (form[i].form[form[i].ptr - 1] === '\ufffc') { form[i].ptr -= 2; if (form[i].ptr <= 0) { Z = true; return z; } }
                        out = form[i].form[form[i].ptr] + out;
                        form[i].ptr--;
                    }
                }
                return out;
            }
        }
    };
    // сбрасывает указатель формы
    var formcallrest = function (name) {
        var h = hash(name), i = 0;
        for (i = 0; i < form.length; i++) {
            if (h === form[i].hash && name === form[i].name) {
                form[i].ptr = 0;
                break;
            }
        }
    };
    // удаляет формы
    var formdel = function (sarr) {
        var i, j, f, t;
        for (i = 0; i < form.length; i++) {
            f = form.shift();
            t = true;
            for (j = 0; j < sarr.length; j++) {
                if (f.name === sarr[j]) t = false;
            }
            if (t) form.push(f);
        }
    };
    // удаляет все формы
    var formdelall = function () { form = []; };
    // переместить формы во внешнюю память    
    var blockstore = function (name, sarr) {
        var i, j, f, t, out = [];
        for (i = 0; i < form.length; i++) {
            f = form.shift();
            t = true;
            for (j = 0; j < sarr.length; j++) {
                if (f.name === sarr[j]) t = false;
            }
            if (t) form.push(f); else out.push(f);
        }
        localStorage.setItem('trac_'+name, JSON.stringify(out));
    };
    // загрузить формы из внешней памяти
    var blockfetch = function (name) {
        var s = JSON.parse(localStorage.getItem('trac_'+name));
        if (s) { for (var i = 0; i < s.length; i++) { form.push(s[i]); }}
    };
    // удалить блок во внешней памяти
    var blockerase = function (name) { localStorage.removeItem('trac_'+name); };
    // ищет парную скобку
    var searchparent = function () {
        var cnt = 0, go = true, i = 0;
        while (go) {
            if (abuf[i] === '(') { cnt++; } else { if (abuf[i] === ')') { if (cnt === 0) return i; else cnt--; } }
            i++;
            if (i === abuf.length) { go = false; }
        }
    };
    // загружает символ
    var loadc = function () {
        var res = ibuf[0];
        ibuf = ibuf.substr(1);
        return res;
    };
    // загружает строку ограниченную метасимволом или концом строки
    var loads = function () {
        var end = ibuf.indexOf(meta), out;
        if (end === -1) {
            end = ibuf.length;
            out = ibuf.substr(0, end);
            ibuf = '';
        } else {
            out = ibuf.substr(0, end);
            ibuf = ibuf.substr(end + 1);
        }
        return out;
    };
    // обработчик встроенных функций
    var start = function (fstart) {
        var s = nbuf.substr(fstart + 1).split('\u2551');
        s.pop();
        switch (s[0]) {
            // общий ввод вывод
            case 'rs': case 'читать': funcvalue = loads(); return;
            case 'rc': case 'читатьсимвол': case 'читать символ': funcvalue = loadc(); return;
            case 'cm': case 'мета': funcvalue = ''; meta = get(s, 1)[0]; return;
            case 'ps': case 'печать': funcvalue = ''; obuf += get(s, 1); return;
                // работа с формами: пользовательские функции, массивы и т.п.
            case 'ds': case 'определить': formadd(formlen(), get(s, 1), get(s, 2), 0, 0); funcvalue = ''; return;
            case 'ss': case 'разбить': var n = get(s, 1); var ss = s; ss.shift(); ss.shift(); formseg(n, ss); funcvalue = ''; return;
            case 'cl': case 'вызвать': var n = get(s, 1); var ss = s; ss.shift(); ss.shift(); funcvalue = formcall(n, ss); return;
	    case 'cs': case 'сегмент': funcvalue = formcallseg( get(s, 1), get(s, 2)); return;
            case 'cc': case 'символ': funcvalue = formcallchr( get(s, 1), get(s, 2)); return;
            case 'cn': case 'символы': funcvalue = formcallchrs( get(s, 1), get(s, 2), get(s, 3)); return;
            case 'cr': case 'сначала': formcallrest( get(s, 1)); funcvalue = ''; return;
            case 'dd': case 'стереть': var ss = s; ss.shift(); formdel(ss); funcvalue = ''; return;
            case 'da': case 'стеретьвсе': case 'стереть все': formdelall(); funcvalue = ''; return;
                // арифметика
            case 'ad': case '+': var a1 = parseFloat(get(s, 1)), a2 = parseFloat(get(s, 2)); if (typeof a1 === 'number' && typeof a2 === 'number') funcvalue = (a1 + a2).toString(); else { funcvalue = get(s, 3); Z = true; } return;
            case 'su': case '-': var a1 = parseFloat(get(s, 1)), a2 = parseFloat(get(s, 2)); if (typeof a1 === 'number' && typeof a2 === 'number') funcvalue = (a1 - a2).toString(); else { funcvalue = get(s, 3); Z = true; } return;
            case 'ml': case '*': var a1 = parseFloat(get(s, 1)), a2 = parseFloat(get(s, 2)); if (typeof a1 === 'number' && typeof a2 === 'number') funcvalue = (a1 * a2).toString(); else { funcvalue = get(s, 3); Z = true; } return;
            case 'dv': case '/': var a1 = parseFloat(get(s, 1)), a2 = parseFloat(get(s, 2)); if (typeof a1 === 'number' && typeof a2 === 'number' && a2 !== 0) funcvalue = (a1 / a2).toString(); else { funcvalue = get(s, 3); Z = true; } return;
            case 'целое': funcvalue = Math.floor(parseFloat(get(s, 1))); return;
                // битовые операции
            case 'bu': funcvalue = ''; return;
            case 'bi': funcvalue = ''; return;
            case 'bc': funcvalue = ''; return;
            case 'bs': funcvalue = ''; return;
            case 'br': funcvalue = ''; return;
                // управление выполнением
            case 'eq': case '=': if (get(s, 1) === get(s, 2)) { funcvalue = get(s, 3); } else { funcvalue = get(s, 4); } return;
            case 'gr': case '>': if (get(s, 1) > get(s, 2)) { funcvalue = get(s, 3); } else { funcvalue = get(s, 4); } return;
                // интерфейс с внешним хранилищем
            case 'sb': case 'сохранить': var n = get(s, 1); var ss = s; ss.shift(); ss.shift(); blockstore(n, ss); funcvalue = ''; return;
            case 'fb': case 'загрузить': blockfetch(get(s, 1)); funcvalue = ''; return;
            case 'eb': case 'удалить': blockerase(get(s, 1)); funcvalue = ''; return;
                // стоп и ноп
            case 'hl': case 'стоп': funcvalue = ''; worked = false; return;
            case 'np': funcvalue = get(s, 1); return;
                // экстра
           // case 'сегодня': var t = new Date(); funcvalue = t.getFullYear() + '-' + Lib.add0(t.getMonth() + 1) + '-' + Lib.add0(t.getDate()); return;
           // case 'сейчас': var t = new Date(); funcvalue = t.getHours() + ':' + Lib.add0(t.getMinutes().toString()) + ':' + Lib.add0(t.getSeconds().toString()); return;

            default: var n = s[0]; var ss = s; ss.shift(); funcvalue = formcall(n, ss); return;
        }
    };
    // распознаватель типа функции
    var gofunc = function () {
        var fstart = Math.max(nbuf.lastIndexOf('\u2642'), nbuf.lastIndexOf('\u2640'));
        start(fstart);
        if (fstart >= 0) {
            if (nbuf[fstart] === '\u2642') {
                abuf = funcvalue + abuf;
                nbuf = nbuf.substr(0, fstart);
                Z = false;
            } else {
                if (nbuf[fstart] === '\u2640' && Z === true) {
                    abuf = funcvalue + abuf;
                    nbuf = nbuf.substr(0, fstart);
                    Z = false;
                } else {
                    if (nbuf[fstart] === '\u2640' && Z === false) {
                        nbuf = nbuf.substr(0, fstart) + funcvalue;
                        Z = false;
                    } else {
                        nbuf = nbuf.substr(0, fstart);
                    }
                }
            }
        }
    };
    // интерпретатор
    var interpret = function () {
        while (worked) {
            if (abuf.length === 0) {
                nbuf = '';
                abuf = idle;
                pc = 0;
                Z = false;
            } else {
                if (c1() === '(') {
                    next();
                    var parent = searchparent();
                    nbuf += abuf.substr(0, parent);
                    abuf = abuf.substr(parent + 1);
                } else {
                    if (c1() === '\r' || c1() === '\n' || c1() === '\t') {
                        next();
                    } else {
                        if (c1() === ',') {
                            next();
                            nbuf += '\u2551';
                        } else {
                            if (c1() === '#' && c2() === '#' && c3() === '(') {
                                next(3);
                                nbuf += '\u2640';
                            } else {
                                if (c1() === '#' && c2() === '(') {
                                    next(2);
                                    nbuf += '\u2642';
                                } else {
                                    if (c1() === ')') {
                                        next();
                                        nbuf += '\u2551\ufd3f';
                                        gofunc();
                                    } else {
                                        nbuf += c1();
                                        next();
                                    }
                                }
                            }
                        }
                    }
                }

            }
        }
    };
    // внешний интерфейс, съедает строку программы возвращает строку результата
    var run = function (input) {
        ibuf = input + '#(hl)';
        obuf = '';
        worked = true;
        interpret();
        return obuf;
    };
    // перекодировка входных строк так чтобы их содержимое не воспринималось как trac код
    var codeTo = function (s) {
        return s.replace('#','\ufff0').replace('(','\ufff1').replace(')','\ufff2').replace(',','\ufff3').replace(meta,'\ufff4');
    };
    // перекодировка выходной строки, возвращающая на место используемые trac символы
    var codeFrom = function (s) {
        return s.replace('\ufff0','#').replace('\ufff1','(').replace('\ufff2',')').replace('\ufff3',',').replace('\ufff4',meta);
    };
    
    return Object.freeze({run:run,reset:reset,codeTo:codeTo,codeFrom:codeFrom});
}
    
// программа тестирования интерпретатора

//+ #(ps,#(rs))`прочитали строку rs<br>`#(ps,#(rc))`Прочитали символ rc<br>`#(cm,^)`#(ps,#(rs))^Поменяли мету<br>#(cm,`)^#(ps,#(rs))`вернули мету на место<br>`
//+ #(ds,arr,1:2:3:4:5)#(ss,arr,:)`#(ps,#(cs,arr))`#(ps,#(cs,arr))`#(ps,#(cs,arr))`#(ps,#(cs,arr))`#(ps,#(cs,arr))`#(ps,#(cs,arr,err))`
//+ #(ds,+,(#(ad,p1,p2)))#(ss,+,p1,p2)`#(ps,#(cl,+,2,3))`#(ps,#(+,2,3))`
//+ #(ds,str,string)#(ss,str,i)`#(ps,#(cc,str))`#(ps,#(cc,str))`#(ps,#(cc,str))`#(ps,#(cc,str))`#(ps,#(cc,str))`#(ps,#(cc,str))`#(ps,#(cc,str,err))`
//+ #(ds,Factorial,(#(eq,1,X,1,(#(ml,X,#(cl,Factorial,#(ad,X,-1)))))))#(ss,Factorial,X)`#(cl,Factorial,5)`
//+ 1+1=`#(ad,1,1)` 2-1=`#(su,2,1)` 2-4=`#(su,2,4)` 2*4=`#(ml,2,4)` 6/2=`#(dv,6,2,?)` 5/2=`#(dv,5,2,?)` 6/0=`#(dv,6,0,?)`
//+ проверка условий:`#(ps,#(eq,11,11,ok,not))` #(ps,#(eq,11,12,not,ok))`  #(ps,#(gr,11,12,not,ok))`  #(ps,#(gr,12,11,ok,not))`  
//+ управление` #(np)#(np,123)#(np,#(ad,1,1))`#(hl)`это не должно быть видно`
//+ #(ds,test,(#(ad,1,1)))`функции` #(ps,(#(cl,test)))` #(ps,#(cl,test))` #(ps,##(cl,test))` 
//+ #(ds,test,(#(ad,1,1)))`сохранение` #(sb,local,test)` #(ps,##(cl,test))`#(fb,local)` #(ps,##(cl,test))`#(eb,local)` #(da)`#(fb,local)` #(ps,##(cl,test))`

/* Типовые операции

 присваивание константы   var name = value
 #(ds,name,value)`

 использование переменной  name
 #(name)

 инкремент переменной  name++
 #(ds,name,#(ad,#(name),1)`

 присваивание результата выражения    name <- expression
 #(ds,name,expression)`

 определение функции func name(param1, param2) <- expression
 #(ds,name,(expression))#(ss,param1,param2)`

 вызов функции    name(param1,param2)
 #(name,param1,param2)`

 условное ветвление if(condition,trueexpression,falseexpression)
 #(eq,condition,0,falseexpression,trueexpression)`

 цикл по условию while(condition, expression)
 #(ds,tempname,(#(eq,condition,0,expression#(tempname),)))`#(tempname)`#(dd,tempname)`

 цикл по счетчику for(startcount,endcount, expression)
 #(ds,tempvar,(startcount))#(ds,tempname,(#(eq,tempvar,endcount,,expression#(ds,tempvar,#(ad,#(tempvar),1)))#(tempname))))`#(tempname)`#(dd,tempvar,tempname)`

 */


