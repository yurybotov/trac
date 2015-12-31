
var Trac = (function () {

    // конструктор
    function Trac() {
        this.ibuf = '';
        this.abuf = '';
        this.nbuf = '';
        this.form = [];
        this.obuf = '';
        this.pc = 0;
        this.value = '';
        this.worked = true;
        this.io = '';
        this.idle = '#(ps,#(rs))';
        this.z = false;
        this.meta = '`';
        this.funcvalue = '';
    }

    // сокращенное обращение к первым трем символам активной строки с заменой результата на пустой если строка короче чем нужно
    Trac.prototype.c1 = function () { if (this.abuf.length > 0) return this.abuf[0]; else return ''; };
    Trac.prototype.c2 = function () { if (this.abuf.length > 1) return this.abuf[1]; else return ''; };
    Trac.prototype.c3 = function () { if (this.abuf.length > 2) return this.abuf[2]; else return ''; };
    // удаление лидирующего символа активной строки
    Trac.prototype.pop = function () { if (this.abuf.length > 0) this.abuf = this.abuf.substr(1); };
    // если аргумент не представлен заменить его на ''
    Trac.prototype.get = function (strarr, i) { if (i > strarr.length - 1) return ''; else return strarr[i]; };
    // вычисляет хэш для формы
    Trac.prototype.hash = function (s) { var a = 0; for (var i = 0; i < s.length; i++) { a = (a << 3) ^ i; } return a; };
    // добавляет форму
    Trac.prototype.formadd = function (id, fname, fform, fptr, fcss) {
        var p = [];
        p.push(fname);
        this.formdel(p);
        this.form.push({ id: id, hash: this.hash(fname), name: fname, form: fform, ptr: fptr, css: fcss });
    };
    // возвращает количество схраненных форм
    Trac.prototype.formlen = function () { return this.form.length; };
    // сегментирует форму
    Trac.prototype.formseg = function (name, sarr) {
        var h = this.hash(name);
        var i = 0, j = 0;
        for (i = 0; i < this.form.length; i++) {
            if (h === this.form[i].hash && name === this.form[i].name) {
                for (j = 0; j < sarr.length; j++) {
                    this.form[i].form = this.form[i].form.replace(new RegExp(sarr[j], 'g'), '\ufffc' + j);
                }
                break;
            }
        }
    };
    // вызывает функцию из формы
    Trac.prototype.formcall = function (name, sarr) {
        var h = this.hash(name);
        var s = '', i = 0, j = 0;
        for (i = 0; i < this.form.length; i++) {
            if (h === this.form[i].hash && name === this.form[i].name) {
                var f = this.form[i].form;
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
    Trac.prototype.formcallseg = function (name, z) {
        var h = this.hash(name);
        var i = 0;
        for (i = 0; i < this.form.length; i++) {
            if (h === this.form[i].hash && name === this.form[i].name) {
                var f = this.form[i].form;
                if (this.form[i].ptr >= f.length) { this.z = true; return z; }
                var j = f.indexOf('\ufffc', this.form[i].ptr);
                if (j >= 0) {
                    var ptr = this.form[i].ptr;
                    this.form[i].ptr = j + 2;
                    return f.substr(ptr, j-ptr);
                } else {
                    var ptr = this.form[i].ptr;
                    this.form[i].ptr = f.length;
                    return f.substr(ptr);
                }
            }
        }
    };
    // достает из формы символ
    Trac.prototype.formcallchr = function (name, z) {
        var h = this.hash(name);
        var i = 0;
        for (i = 0; i < this.form.length; i++) {
            if (h === this.form[i].hash && name === this.form[i].name) {
                if (this.form[i].ptr >= this.form[i].form.length) { this.z = true; return z; }
                var j = this.form[i].ptr;
                if (this.form[i].form[j] === '\ufffc') { this.form[i].ptr += 2; j = this.form[i].ptr; }
                this.form[i].ptr++;
                return this.form[i].form[j];
            }
        }
    };
    // достает из формы символы
    Trac.prototype.formcallchrs = function (name, d, z) {
        var h = this.hash(name);
        var i = 0, j = 0;
        var out = '';
        for (i = 0; i < this.form.length; i++) {
            if (h === this.form[i].hash && name === this.form[i].name) {
                if (d > 0) {
                    for (j = 0; j < d; j++) {
                        if (this.form[i].ptr >= this.form[i].form.length) { this.z = true; return z; }
                        if (this.form[i].form[this.form[i].ptr] === '\ufffc') { this.form[i].ptr += 2; if (this.form[i].ptr >= this.form[i].form.length) { this.z = true; return z; } }
                        out += this.form[i].form[this.form[i].ptr];
                        this.form[i].ptr++;
                    }
                    return out;
                } else {
                    for (j = 0; j > d; j--) {
                        if (this.form[i].ptr <= 0) { this.z = true; return z; }
                        if (this.form[i].form[this.form[i].ptr - 1] === '\ufffc') { this.form[i].ptr -= 2; if (this.form[i].ptr <= 0) { this.z = true; return z; } }
                        out = this.form[i].form[this.form[i].ptr] + out;
                        this.form[i].ptr--;
                    }
                    return out;
                }
            }
        }
    };
    // сбрасывает указатель формы
    Trac.prototype.formcallrest = function (name) {
        var h = this.hash(name);
        var i = 0;
        for (i = 0; i < this.form.length; i++) {
            if (h === this.form[i].hash && name === this.form[i].name) {
                this.form[i].ptr = 0;
                break;
            }
        }
    };
    // удаляет формы
    Trac.prototype.formdel = function (sarr) {
        var h = this.hash(name);
        var i = 0, j = 0;
        for (i = 0; i < this.form.length; i++) {
            var f = this.form.shift();
            var t = true;
            for (j = 0; j < sarr.length; j++) {
                if (f.name === sarr[j]) t = false;
            }
            if (t) this.form.push(f);
        }
    };
    // удаляет все формы
    Trac.prototype.formdelall = function () {
        this.form = [];
    };
    // переместить формы во внешнюю память    
    Trac.prototype.blockstore = function (name, sarr) {
        var h = this.hash(name);
        var i = 0, j = 0;
        var out = [];
        for (i = 0; i < this.form.length; i++) {
            var f = this.form.shift();
            var t = true;
            for (j = 0; j < sarr.length; j++) {
                if (f.name === sarr[j]) t = false;
            }
            if (t) this.form.push(f); else out.push(f);
        }
        localStorage.setItem(name, JSON.stringify(out));
    };
    // загрузить формы из внешней памяти
    Trac.prototype.blockfetch = function (name) {
        var s = JSON.parse(localStorage.getItem(name));
        if (s) {
            for (var i = 0; i < s.length; i++) {
                this.form.push(s[i]);
            }
        }
    };
    // удалить блок во внешней памяти
    Trac.prototype.blockerase = function (name) { localStorage.removeItem(name); };
    // ищет парную скобку
    Trac.prototype.searchparent = function () {
        var cnt = 0, ans = -1, go = true, i = 0;
        while (go) {
            if (this.abuf[i] === '(') { cnt++; } else { if (this.abuf[i] === ')') { if (cnt === 0) return i; else cnt--; } }
            i++;
            if (i === this.abuf.length) { go = false; }
        }
    };
    // загружает символ
    Trac.prototype.loadc = function () {
        var res = this.ibuf[0];
        this.ibuf = this.ibuf.substr(1);
        return res;
    };
    // загружает строку ограниченную метасимволом или концом строки
    Trac.prototype.loads = function () {
        var end = this.ibuf.indexOf(this.meta);
        if (end === -1) {
            end = this.ibuf.length;
            var out = this.ibuf.substr(0, end);
            this.ibuf = '';
        } else {
            var out = this.ibuf.substr(0, end);
            this.ibuf = this.ibuf.substr(end + 1);
        }
        return out;
    };
    // обработчик встроенных функций
    Trac.prototype.start = function (fstart) {
        var s = this.nbuf.substr(fstart + 1).split('\u2551');
        s.pop();
        switch (s[0]) {
            // общий ввод вывод
            case 'rs': this.funcvalue = this.loads(); return;
            case 'rc': this.funcvalue = this.loadc(); return;
            case 'cm': this.funcvalue = ''; this.meta = this.get(s, 1)[0]; return;
            case 'ps': this.funcvalue = ''; this.obuf += this.get(s, 1); return;
                // работа с формами: пользовательские функции, массивы и т.п.
            case 'ds': this.formadd(this.formlen(), this.get(s, 1), this.get(s, 2), 0, 0); this.funcvalue = ''; return;
            case 'ss': var n = this.get(s, 1); var ss = s; ss.shift(); ss.shift(); this.formseg(n, ss); this.funcvalue = ''; return;
            case 'cl': var n = this.get(s, 1); var ss = s; ss.shift(); ss.shift(); this.funcvalue = this.formcall(n, ss); return;
            case 'd': var n = this.get(s, 1); var ss = s; ss.shift(); ss.shift(); this.funcvalue = this.formcall(n, ss); return;
            case 'cs': this.funcvalue = this.formcallseg(this.get(s, 1), this.get(s, 2)); return;
            case 'cc': this.funcvalue = this.formcallchr(this.get(s, 1), this.get(s, 2)); return;
            case 'cn': this.funcvalue = this.formcallchrs(this.get(s, 1), this.get(s, 2), this.get(s, 3)); return;
            case 'cr': this.formcallrest(this.get(s, 1)); this.funcvalue = ''; return;
            case 'dd': var ss = s; ss.shift(); this.formdel(ss); this.funcvalue = ''; return;
            case 'da': this.formdelall(); this.funcvalue = ''; return;
                // арифметика
            case 'ad': var a1 = parseInt(this.get(s, 1)), a2 = parseInt(this.get(s, 2)); if (typeof a1 === 'number' && typeof a2 === 'number') this.funcvalue = (a1 + a2).toString(); else { this.funcvalue = this.get(s, 3); this.z = true; } return;
            case 'su': var a1 = parseInt(this.get(s, 1)), a2 = parseInt(this.get(s, 2)); if (typeof a1 === 'number' && typeof a2 === 'number') this.funcvalue = (a1 - a2).toString(); else { this.funcvalue = this.get(s, 3); this.z = true; } return;
            case 'ml': var a1 = parseInt(this.get(s, 1)), a2 = parseInt(this.get(s, 2)); if (typeof a1 === 'number' && typeof a2 === 'number') this.funcvalue = (a1 * a2).toString(); else { this.funcvalue = this.get(s, 3); this.z = true; } return;
            case 'dv': var a1 = parseInt(this.get(s, 1)), a2 = parseInt(this.get(s, 2)); if (typeof a1 === 'number' && typeof a2 === 'number' && a2 !== 0) this.funcvalue = Math.floor(a1 / a2).toString(); else { this.funcvalue = this.get(s, 3); this.z = true; } return;
                // битовые операции
            case 'bu': this.funcvalue = ''; return;
            case 'bi': this.funcvalue = ''; return;
            case 'bc': this.funcvalue = ''; return;
            case 'bs': this.funcvalue = ''; return;
            case 'br': this.funcvalue = ''; return;
                // управление выполнением
            case 'eq': if (this.get(s, 1) === this.get(s, 2)) { this.funcvalue = this.get(s, 3); } else { this.funcvalue = this.get(s, 4); } return;
            case 'gr': if (this.get(s, 1) > this.get(s, 2)) { this.funcvalue = this.get(s, 3); } else { this.funcvalue = this.get(s, 4); } return;
                // интерфейс с внешним хранилищем
            case 'sb': var n = this.get(s, 1); var ss = s; ss.shift(); ss.shift(); this.blockstore(n, ss); this.funcvalue = ''; return;
            case 'fb': this.blockfetch(this.get(s, 1)); this.funcvalue = ''; return;
            case 'eb': this.blockerase(this.get(s, 1)); this.funcvalue = ''; return;
                // стоп и ноп
            case 'hl': this.funcvalue = ''; this.worked = false; return;
            case 'np': this.funcvalue = this.get(s, 1); return;
            default: var n = s[0]; var ss = s; ss.shift(); this.funcvalue = this.formcall(n, ss); return;
        }
    };
    // распознаватель типа функции
    Trac.prototype.gofunc = function () {
        var fstart = Math.max(this.nbuf.lastIndexOf('\u2642'), this.nbuf.lastIndexOf('\u2640'));
        this.start(fstart);
        if (fstart >= 0) {
            if (this.nbuf[fstart] === '\u2642') {
                this.abuf = this.funcvalue + this.abuf;
                this.nbuf = this.nbuf.substr(0, fstart);
                this.z = false;
            } else {
                if (this.nbuf[fstart] === '\u2640' && this.z === true) {
                    this.abuf = this.funcvalue + this.abuf;
                    this.nbuf = this.nbuf.substr(0, fstart);
                    this.z = false;
                } else {
                    if (this.nbuf[fstart] === '\u2640' && this.z === false) {
                        this.nbuf = this.nbuf.substr(0, fstart) + this.funcvalue;
                        this.z = false;
                    } else {
                        this.nbuf = this.nbuf.substr(0, fstart);
                    }
                }
            }
        }
    };
    // интерпретатор
    Trac.prototype.interpret = function () {
        var step = 0;
        while (this.worked) {
            if (this.abuf.length === 0) {
                this.nbuf = '';
                this.abuf = this.idle;
                this.pc = 0;
                this.z = false;
            } else {
                if (this.c1() === '(') {
                    this.pop();
                    var parent = this.searchparent();
                    this.nbuf += this.abuf.substr(0, parent);
                    this.abuf = this.abuf.substr(parent + 1);
                } else {
                    if (this.c1() === '\r' || this.c1() === '\n' || this.c1() === '\t') {
                        this.pop();
                    } else {
                        if (this.c1() === ',') {
                            this.pop();
                            this.nbuf += '\u2551';
                        } else {
                            if (this.c1() === '#' && this.c2() === '#' && this.c3() === '(') {
                                this.pop(); this.pop(); this.pop();
                                this.nbuf += '\u2640';
                            } else {
                                if (this.c1() === '#' && this.c2() === '(') {
                                    this.pop(); this.pop();
                                    this.nbuf += '\u2642';
                                } else {
                                    if (this.c1() === ')') {
                                        this.pop();
                                        this.nbuf += '\u2551\ufd3f';
                                        this.gofunc();
                                    } else {
                                        this.nbuf += this.c1();
                                        this.pop();
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
    Trac.prototype.run = function (input) {
        this.ibuf = input + '#(hl)';
        this.obuf = '';
        this.worked = true;
        this.interpret();
        return this.obuf;
    };

    return Trac;
})();

var $ = this.u$;

var trac = new Trac();

function go() {
    $('#content').html(trac.run($('#source').get().value));
};



// программа тестирования интерпретатора

//+ #(ps,#(rs))`прочитали строку rs<br>`#(ps,#(rc))`Прочитали символ rc<br>`#(cm,^)`#(ps,#(rs))^Поменяли мету<br>#(cm,`)^#(ps,#(rs))`вернули мету на место<br>`
//+ #(ds,arr,1:2:3:4:5)#(ss,arr,:)`#(ps,#(cs,arr))`#(ps,#(cs,arr))`#(ps,#(cs,arr))`#(ps,#(cs,arr))`#(ps,#(cs,arr))`#(ps,#(cs,arr,err))`
//+ #(ds,+,(#(ad,p1,p2)))#(ss,+,p1,p2)`#(ps,#(cl,+,2,3))`#(ps,#(+,2,3))`
//+ #(ds,str,string)#(ss,str,i)`#(ps,#(cc,str))`#(ps,#(cc,str))`#(ps,#(cc,str))`#(ps,#(cc,str))`#(ps,#(cc,str))`#(ps,#(cc,str))`#(ps,#(cc,str,err))`
//+ #(ds,Factorial,(#(eq,1,X,1,(#(ml,X,#(cl,Factorial,#(ad,X,-1)))))))#(ss,Factorial,X)`#(cl,Factorial,5)`
//+ 1+1=`#(ad,1,1)` 2-1=`#(su,2,1)` 2-4=`#(su,2,4)` 2*4=`#(ml,2,4)` 6/2=`#(dv,6,2,?)` 5/2=`#(dv,5,2,?)` 6/0=`#(dv,6,0,?)`
//+ проверка условий:`#(eq,11,11,ok,not)` #(eq,11,12,not,ok)` #(gr,11,12,not,ok)`  #(gr,12,11,ok,not)`  
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
/* раазбор выражения 

если начинается с :
    func - определение функции
    var - определение переменной
    иначе - выражение
разделитель ';'

при обработке функции проверять ее имя и если оно равно
    if
    while
    for
обрабатывать дополнительно по шаблону

*/

var Jit = (function () {
    function Jit() {
        this.o = '';
    }
    Jit.prototype.run = function (s) { return this.parse(s); };
    Jit.prototype.parse = function (s) {
        var inp = s, token = '';
        while (inp.length > 0) {
            // достаем выражение
            if (inp.indexOf(';') > 0) {
                token = inp.substr(0, inp.indexOf(';'));
                inp = inp.substr(inp.indexOf(';'))
            } else {
                token = inp;
                inp = '';
            }
            // это определение функции?
            if (/^\s*func\s+[A-Za-zА-Яа-я]+[A-Za-zА-Яа-я0-9]*\s*\(.*\)\s*=\s*.+$/.test(token)) {
                this.functionDefinition(token);
            } else {
                // это присваивание?
                if (/^\s*var\s+[A-Za-zА-Яа-я]+[A-Za-zА-Яа-я0-9]*\s*=\s*.*$/.test(token)) {
                    this.assignment(token);
                } else {
                // это выражение...
                    this.expression(token);
                }
            }
        }
    };
    Jit.prototype.functionDefinition = function (s) {
        var res = s.match(/^\s*func\s+([A-Za-zА-Яа-я]+[A-Za-zА-Яа-я0-9]*)\s*\((.*)\)\s*=\s*(.+)$/gi);
        this.o += '#(ds,'+res[1]+',('+this.expression(res[3])+'))#(ss,'+res[2]+')`';
    };
    Jit.prototype.assignment = function (s) {
        var res = s.match(/^\s*var\s+([A-Za-zА-Яа-я]+[A-Za-zА-Яа-я0-9]*)\s*=\s*(.*)$/gi);
        this.o += '#(ds,'+res[1]+','+this.expression(res[2])+')`';
    };
    Jit.prototype.expression = function (s) {

    };
    return Jit;
})();

var jit = new Jit();

/* Формальное описание языка

программа = строка+
строка = token+  (токены разделяются ;)
token = определение||присваивание||выражение
определение = func + name + ( + аргументы* + ) = + выражение
присваивание = var + name + = + выражение

*/