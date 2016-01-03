// $$macro.js
var Macro = (function () {

    // конструктор
    function Macro() {
        this.ibuf = '';
        this.abuf = '';
        this.nbuf = '';
        this.form = [];
        this.obuf = '';
        this.pc = 0;
        this.value = '';
        this.worked = true;
        this.io = '';
        this.idle = '#(печать,#(читать))';
        this.z = false;
        this.meta = '`';
        this.funcvalue = '';
    }

    // сокращенное обращение к первым трем символам активной строки с заменой результата на пустой если строка короче чем нужно
    Macro.prototype.c1 = function () { if (this.abuf.length > 0) return this.abuf[0]; else return ''; };
    Macro.prototype.c2 = function () { if (this.abuf.length > 1) return this.abuf[1]; else return ''; };
    Macro.prototype.c3 = function () { if (this.abuf.length > 2) return this.abuf[2]; else return ''; };
    // удаление лидирующего символа активной строки
    Macro.prototype.pop = function () { if (this.abuf.length > 0) this.abuf = this.abuf.substr(1); };
    // если аргумент не представлен заменить его на ''
    Macro.prototype.get = function (strarr, i) { if (i > strarr.length - 1) return ''; else return strarr[i]; };
    // вычисляет хэш для формы
    Macro.prototype.hash = function (s) { var a = 0; for (var i = 0; i < s.length; i++) { a = (a << 3) ^ i; } return a; };
    // добавляет форму
    Macro.prototype.formadd = function (id, fname, fform, fptr, fcss) {
        var p = [];
        p.push(fname);
        this.formdel(p);
        this.form.push({ id: id, hash: this.hash(fname), name: fname, form: fform, ptr: fptr, css: fcss });
    };
    // возвращает количество схраненных форм
    Macro.prototype.formlen = function () { return this.form.length; };
    // сегментирует форму
    Macro.prototype.formseg = function (name, sarr) {
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
    Macro.prototype.formcall = function (name, sarr) {
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
    Macro.prototype.formcallseg = function (name, z) {
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
                    return f.substr(ptr, j - ptr);
                } else {
                    var ptr = this.form[i].ptr;
                    this.form[i].ptr = f.length;
                    return f.substr(ptr);
                }
            }
        }
    };
    // достает из формы символ
    Macro.prototype.formcallchr = function (name, z) {
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
    Macro.prototype.formcallchrs = function (name, d, z) {
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
    Macro.prototype.formcallrest = function (name) {
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
    Macro.prototype.formdel = function (sarr) {
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
    Macro.prototype.formdelall = function () {
        this.form = [];
    };
    // переместить формы во внешнюю память    
    Macro.prototype.blockstore = function (name, sarr) {
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
    Macro.prototype.blockfetch = function (name) {
        var s = JSON.parse(localStorage.getItem(name));
        if (s) {
            for (var i = 0; i < s.length; i++) {
                this.form.push(s[i]);
            }
        }
    };
    // удалить блок во внешней памяти
    Macro.prototype.blockerase = function (name) { localStorage.removeItem(name); };
    // ищет парную скобку
    Macro.prototype.searchparent = function () {
        var cnt = 0, ans = -1, go = true, i = 0;
        while (go) {
            if (this.abuf[i] === '(') { cnt++; } else { if (this.abuf[i] === ')') { if (cnt === 0) return i; else cnt--; } }
            i++;
            if (i === this.abuf.length) { go = false; }
        }
    };
    // загружает символ
    Macro.prototype.loadc = function () {
        var res = this.ibuf[0];
        this.ibuf = this.ibuf.substr(1);
        return res;
    };
    // загружает строку ограниченную метасимволом или концом строки
    Macro.prototype.loads = function () {
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
    Macro.prototype.start = function (fstart) {
        var s = this.nbuf.substr(fstart + 1).split('\u2551');
        s.pop();
        switch (s[0]) {
            // общий ввод вывод
            case 'rs': case 'читать': this.funcvalue = this.loads(); return;
            case 'rc': case 'читатьсимвол': case 'читать символ': this.funcvalue = this.loadc(); return;
            case 'cm': case 'мета': this.funcvalue = ''; this.meta = this.get(s, 1)[0]; return;
            case 'ps': case 'печать': this.funcvalue = ''; this.obuf += this.get(s, 1); return;
                // работа с формами: пользовательские функции, массивы и т.п.
            case 'ds': case 'определить': this.formadd(this.formlen(), this.get(s, 1), this.get(s, 2), 0, 0); this.funcvalue = ''; return;
            case 'ss': case 'разбить': var n = this.get(s, 1); var ss = s; ss.shift(); ss.shift(); this.formseg(n, ss); this.funcvalue = ''; return;
            case 'cl': case 'вызвать': var n = this.get(s, 1); var ss = s; ss.shift(); ss.shift(); this.funcvalue = this.formcall(n, ss); return;
	    case 'cs': case 'сегмент': this.funcvalue = this.formcallseg(this.get(s, 1), this.get(s, 2)); return;
            case 'cc': case 'символ': this.funcvalue = this.formcallchr(this.get(s, 1), this.get(s, 2)); return;
            case 'cn': case 'символы': this.funcvalue = this.formcallchrs(this.get(s, 1), this.get(s, 2), this.get(s, 3)); return;
            case 'cr': case 'сначала': this.formcallrest(this.get(s, 1)); this.funcvalue = ''; return;
            case 'dd': case 'стереть': var ss = s; ss.shift(); this.formdel(ss); this.funcvalue = ''; return;
            case 'da': case 'стеретьвсе': case 'стереть все': this.formdelall(); this.funcvalue = ''; return;
                // арифметика
            case 'ad': case '+': var a1 = parseFloat(this.get(s, 1)), a2 = parseFloat(this.get(s, 2)); if (typeof a1 === 'number' && typeof a2 === 'number') this.funcvalue = (a1 + a2).toString(); else { this.funcvalue = this.get(s, 3); this.z = true; } return;
            case 'su': case '-': var a1 = parseFloat(this.get(s, 1)), a2 = parseFloat(this.get(s, 2)); if (typeof a1 === 'number' && typeof a2 === 'number') this.funcvalue = (a1 - a2).toString(); else { this.funcvalue = this.get(s, 3); this.z = true; } return;
            case 'ml': case '*': var a1 = parseFloat(this.get(s, 1)), a2 = parseFloat(this.get(s, 2)); if (typeof a1 === 'number' && typeof a2 === 'number') this.funcvalue = (a1 * a2).toString(); else { this.funcvalue = this.get(s, 3); this.z = true; } return;
            case 'dv': case '/': var a1 = parseFloat(this.get(s, 1)), a2 = parseFloat(this.get(s, 2)); if (typeof a1 === 'number' && typeof a2 === 'number' && a2 !== 0) this.funcvalue = (a1 / a2).toString(); else { this.funcvalue = this.get(s, 3); this.z = true; } return;
            case 'целое': this.funcvalue = Math.floor(parseFloat(this.get(s, 1))); return;
                // битовые операции
            case 'bu': this.funcvalue = ''; return;
            case 'bi': this.funcvalue = ''; return;
            case 'bc': this.funcvalue = ''; return;
            case 'bs': this.funcvalue = ''; return;
            case 'br': this.funcvalue = ''; return;
                // управление выполнением
            case 'eq': case 'еслиравно': case 'если равно': if (this.get(s, 1) === this.get(s, 2)) { this.funcvalue = this.get(s, 3); } else { this.funcvalue = this.get(s, 4); } return;
            case 'gr': case 'еслибольше': case 'если больше': if (this.get(s, 1) > this.get(s, 2)) { this.funcvalue = this.get(s, 3); } else { this.funcvalue = this.get(s, 4); } return;
                // интерфейс с внешним хранилищем
            case 'sb': case 'сохранить': var n = this.get(s, 1); var ss = s; ss.shift(); ss.shift(); this.blockstore(n, ss); this.funcvalue = ''; return;
            case 'fb': case 'загрузить': this.blockfetch(this.get(s, 1)); this.funcvalue = ''; return;
            case 'eb': case 'удалить': this.blockerase(this.get(s, 1)); this.funcvalue = ''; return;
                // стоп и ноп
            case 'hl': case 'стоп': this.funcvalue = ''; this.worked = false; return;
            case 'np': case 'пусто': this.funcvalue = this.get(s, 1); return;
                // экстра
            case 'сегодня': var t = new Date(); this.funcvalue = t.getFullYear() + '-' + Lib.add0(t.getMonth() + 1) + '-' + Lib.add0(t.getDate()); return;
            case 'сейчас': var t = new Date(); this.funcvalue = t.getHours() + ':' + Lib.add0(t.getMinutes().toString()) + ':' + Lib.add0(t.getSeconds().toString()); return;

            default: var n = s[0]; var ss = s; ss.shift(); this.funcvalue = this.formcall(n, ss); return;
        }
    };
    // распознаватель типа функции
    Macro.prototype.gofunc = function () {
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
    Macro.prototype.interpret = function () {
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
    Macro.prototype.run = function (input) {
        this.ibuf = input + '#(стоп)';
        this.obuf = '';
        this.worked = true;
        this.interpret();
        return this.obuf;
    };

    return Macro;
})();

var Calc = (function () {
    function Calc() {
    }
    // интерфейс к бибилиотеке снаружи, при необходимости можно поправить
    Calc.prototype.run = function (str) {
        //var rpn = this.torpn(str)
        //var trac = this.rpn2trac(rpn);
        //var lisp = this.rpn2lisp(rpn);
        //var value = this.rpn2value(rpn);
        //var res = 'source: ' + str + '<br>' + 'forth: ' + rpn + '<br>' + 'trac: ' + trac + '<br>' + 'lisp: ' + lisp + '<br>' + 'value: ' + value + '<br>';
        //return res;
        return this.rpn2trac(this.torpn(str));
    }
    // перевод обратной польской нотации в вычисленное значение
    Calc.prototype.rpn2value = function (str, arr) {
        var i = 0, stack = [], out = '';
        if (arr) {
            for (i = 0; i < arr.length; i++) {
                str.replace(new RegExp(arr[i].key, 'g'), arr[i].value);
            }
        }
        var a = str.split(',');
        for (i = 0; i < a.length; i++) {
            var curr = a[i];
            if (curr !== '') {
                if (curr.indexOf('#') < 0) {
                    stack.push(parseFloat(curr));
                } else {
                    var arg = curr.split('#');
                    var p = [];
                    for (j = 0; j < parseInt(arg[1], 10) ; j++) {
                        p[j] = stack.pop();
                    }
                    switch (arg[0]) {
                        case '+': stack.push(p[1] + p[0]); break;
                        case '-': stack.push(p[1] - p[0]); break;
                        case '*': stack.push(p[1] * p[0]); break;
                        case '/': stack.push(p[1] / p[0]); break;
                        case '%': stack.push(p[1] % p[0]); break;
                        case 'abs': stack.push(Math.abs(p[0])); break;
                        case 'floor': stack.push(Math.floor(p[0])); break;
                        case 'ceil': stack.push(Math.ceil(p[0])); break;
                        case 'sin': stack.push(Math.sin(p[0])); break;
                        case 'cos': stack.push(Math.cos(p[0])); break;
                        case 'tan': stack.push(Math.tan(p[0])); break;
                        case 'asin': stack.push(Math.asin(p[0])); break;
                        case 'acos': stack.push(Math.acos(p[0])); break;
                        case 'sqrt': stack.push(Math.sqrt(p[0])); break;
                        case 'min': stack.push(Math.min(p[0])); break;
                        case 'max': stack.push(Math.max(p[0])); break;
                        case 'if': stack.push((p[2] === true) ? p[1] : p[0]); break;
                        case 'exp': stack.push(Math.exp(p[0])); break;
                        case 'log': stack.push(Math.log(p[0])); break;
                        default: break;
                    }
                }
            }
        }
        return stack[0];
    }
    // перевод обратной польской нотации в текст на lisp
    Calc.prototype.rpn2lisp = function (str) {
        var i = 0, j = 0, a = str.split(','), stack = [], out = '', curr;
        for (i = 0; i < a.length; i++) {
            curr = a[i];
            if (curr !== '') {
                if (curr.indexOf('#') < 0) {
                    stack.push(curr);
                } else {
                    var arg = curr.split('#');
                    var p = [];
                    for (j = 0; j < parseInt(arg[1], 10) ; j++) {
                        p[j] = stack.pop();
                    }
                    var res = '(' + arg[0];
                    for (j = p.length - 1; j >= 0; j--) {
                        res += ' ' + p[j];
                    }
                    stack.push(res + ')');
                }
            }
        }
        return stack[0];
    }
    // перевод обратной польской нотации в мою версию trac
    Calc.prototype.rpn2trac = function (str) {
        var i = 0, j = 0, a = str.split(','), stack = [], out = '', curr;
        for (i = 0; i < a.length; i++) {
            curr = a[i];
            if (curr !== '') {
                if (curr.indexOf('#') < 0) {
                    stack.push(curr);
                } else {
                    var arg = curr.split('#');
                    var p = [];
                    for (j = 0; j < parseInt(arg[1], 10) ; j++) {
                        p[j] = stack.pop();
                    }
                    var res = '#(' + arg[0];
                    for (j = p.length - 1; j >= 0; j--) {
                        res += ',' + p[j];
                    }
                    stack.push(res + ')');
                }
            }
        }
        return stack[0];
    }
    // на входе сторока в скобочной нотации на выходе в обратной польской нотации разделенная запятыми
    Calc.prototype.torpn = function (str) {
        var i = 0, a = str, s = [], fs = [], o = '', c = '', p = 0, cur = a[0], prv = '', arc = 0;
        while (a.length > 0 && i < a.length && cur !== '=') {
            // выкинуть незначащие символы
            if (cur === ' ' || cur === '\n' || cur === '\t' || cur === '') {
                i++;
                cur = a[i];
                continue;
            } else {
                if (cur === ')') {
                    if (s.lastIndexOf('(') > s.lastIndexOf('f')) {
                        // это закрывающая скобка - операция
                        o += (o[o.length - 1] !== ',') ? ',' : '';
                        while (s.length !== 0 && (c = s.pop()) !== '(') { o += (((o[o.length - 1] !== ',') ? ',' : '') + c + '#2,'); }
                        p--;
                    } else {
                        // это закрывающая скобка функции
                        while (s.length !== 0 && (c = s.pop()) !== 'f') { o += (((o[o.length - 1] !== ',') ? ',' : '') + c + '#2,') }
                        o += ((o[o.length - 1] !== ',') ? ',' : '') + fs.pop() + '#' + ((prv === '(') ? 0 : (arc + 1)) + ',';
                        p--;
                    }
                } else {
                    if (cur === ',') {
                        // еще один агрумент функции
                        arc++;
                        while (c !== 'f') { o += (((c = s.pop()) === '(' || c === 'f') ? '' : (((o[o.length - 1] !== ',') ? ',' : '') + c + '#2,')); }
                        s.push('f');
                    } else {
                        if (this.isChar(cur)) {
                            // один из разрешенных символов имени идентификатора или число
                            o += cur;
                        } else {
                            if (cur === '(') {
                                if (this.isOp(cur) || prv === '') {
                                    // открывающая скобка операции
                                    s.push(cur);
                                    o += (o[o.length - 1] !== ',') ? ',' : '';
                                    p++;
                                } else {
                                    // открывающая скобка функции
                                    arc = 0;
                                    s.push('f');
                                    fs.push(o.substr(this.lastOp(o) + 1).trim());
                                    o = o.substr(0, this.lastOp(o) + 1).trim();
                                    p++;
                                }
                            }
                            if (this.isOp(cur)) {
                                // это унарный знак
                                if ((cur === '+' || cur === '-') && (this.isOp(prv) || prv === '(')) {
                                    o += cur;
                                } else {
                                    // это операция
                                    if (s.length === 0) {
                                        // это первая встреченная операция
                                        s.push(cur);
                                        o += (o[o.length - 1] !== ',') ? ',' : '';
                                    } else {
                                        if (this.prior(s[s.length - 1]) <= this.prior(cur)) {
                                            // приоритет этой операции больше приоритета операции в стеке
                                            s.push(cur);
                                            o += (o[o.length - 1] !== ',') ? ',' : '';
                                        } else {
                                            // приоритет этой операции меньше приоритета операции в стеке
                                            while (s.length !== 0 && this.prior(s[s.length - 1]) > this.prior(cur)) {
                                                o += (((c = s.pop()) === '(') ? '' : (((o[o.length - 1] !== ',') ? ',' : '') + c + '#2,'));
                                            }
                                            s.push(cur);
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
            i++;
            prv = cur;
            cur = a[i];
        }
        if (p !== 0) alert('скобки не парные');
        // доочищаем что осталось в стэке
        while (s.length !== 0) { o += (((c = s.pop()) === '(') ? '' : (((o[o.length - 1] !== ',') ? ',' : '') + c + '#2,')); }
        return o;
    };
    // вычисляет положение последней операции
    Calc.prototype.lastOp = function (s) {
        var s1 = s.lastIndexOf('+');
        var s2 = s.lastIndexOf('-');
        var s3 = s.lastIndexOf('*');
        var s4 = s.lastIndexOf('/');
        var s5 = s.lastIndexOf('%');
        var s6 = s.lastIndexOf('(');
        var s7 = s.lastIndexOf(',');
        return s = Math.max(s1, s2, s3, s4, s5, s6, s7);
    };
    // проверяет операция ли это
    Calc.prototype.isOp = function (cur) {
        return (cur === '+' || cur === '-' || cur === '*' || cur === '/' || cur === '%')
    };
    // проверяет допустимый ли это символ
    Calc.prototype.isChar = function (cur) {
        return (cur === '.' || (cur >= '0' && cur <= '9') || (cur >= 'a' && cur <= 'z') ||
               (cur >= 'A' && cur <= 'Z') || (cur >= 'а' && cur <= 'я') || (cur >= 'А' && cur <= 'Я'));
    };
    // дает приоритет текущей операции
    Calc.prototype.prior = function (c) {
        switch (c) {
            case '*': case '/': case '%': return 3;
            case '+': case '-': return 2;
            case '(': return 1;
            default: return 0;
        }
    };
    //
    return Calc;
})();

var $ = this.u$;

var macro = new Macro();
var calc = new Calc();

function go() {
    $('#content').html(macro.run($('#source').get().value));
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