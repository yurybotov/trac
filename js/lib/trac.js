
function _Trac() {
    var _ibuf = '';
    var _abuf = '';
    var _nbuf = '';
    var _form = [];
    var _obuf = '';
    var _worked = true;
    const _idle = '#(ps,#(rs))';
    var _Z = false;
    var _meta = '`';
    var _funcvalue = '';
    
    // сокращенное обращение к первым трем символам активной строки с заменой 
    // результата на пустой если строка короче чем нужно
    var _c1 = function () { return (_abuf.length > 0)? _abuf[0]:''; };
    var _c2 = function () { return (_abuf.length > 1)? _abuf[1]:''; };
    var _c3 = function () { return (_abuf.length > 2)? _abuf[2]:''; };
    // удаление лидирующего символа активной строки
    var _next = function (_n) { _n = _n? _n : 1; if (_abuf.length > 0) _abuf = _abuf.substr(_n); };
    // если аргумент не представлен заменить его на ''
    var _get = function (_strarr, _i) { if (_i > _strarr.length - 1) return ''; else return _strarr[_i]; };
    // вычисляет хэш для формы
    var _hash = function (_s) { var _a = 0; for (var _i = 0; _i < _s.length; _i++) { _a = (_a << 3) ^ _i; } return _a; };
    // добавляет форму
    var _formadd = function (_id, _fname, _fform, _fptr, _fcss) {
        _formdel([_fname]);
        _form.push({ _id: _id, _hash: _hash(_fname), _name: _fname, _form: _fform, _ptr: _fptr, _css: _fcss });
    };
    // возвращает количество схраненных форм
    var _formlen = function () { return _form.length; };
    // сегментирует форму
    var _formseg = function (_name, _sarr) {
        var _h = _hash(_name), _i, _j;
        for (_i = 0; _i < _form.length; _i++) {
            if (_h === _form[_i]._hash && _name === _form[_i]._name) {
                for (_j = 0; _j < _sarr.length; _j++) {
                    _form[_i]._form = _form[_i]._form.replace(new RegExp(_sarr[_j], 'g'), '\ufffc' + _j);
                }
                break;
            }
        }
    };
    // вызывает функцию из формы
    var _formcall = function (_name, _sarr) {
        var _h = _hash(_name), _s = '', _i, _j, _f;
        for (_i = 0; _i < _form.length; _i++) {
            if (_h === _form[_i]._hash && _name === _form[_i]._name) {
                _f = _form[_i]._form;
                if (_sarr.length < _s._css) {
                    for (_j = 0; _j < _sarr.length; _j++) {
                        _f = _f.replace(new RegExp('\ufffc' + _j, 'g'), _sarr[_j]);
                    }
                    for (_j = _sarr.length; _j < _s._css; _j++) {
                        _f = _f.replace(new RegExp('\ufffc' + _j, 'g'), '');
                    }
                } else {
                    for (_j = 0; _j < _sarr.length; _j++) {
                        _f = _f.replace(new RegExp('\ufffc' + _j, 'g'), _sarr[_j]);
                    }
                }
                return _f;
            }
        }
    };
    // достает из формы сегмент
    var _formcallseg = function (_name, _z) {
        var _h = _hash(_name), _i, _f, _j, _ptr;
        for (_i = 0; _i < _form.length; _i++) {
            if (_h === _form[_i]._hash && _name === _form[_i]._name) {
                _f = _form[_i]._form;
                if (_form[_i]._ptr >= _f.length) { _Z = true; return _z; }
                _j = _f.indexOf('\ufffc', _form[_i]._ptr);
                if (_j >= 0) {
                    _ptr = _form[_i]._ptr;
                    _form[_i]._ptr = _j + 2;
                    return _f.substr(_ptr, _j - _ptr);
                } else {
                    _ptr = _form[_i]._ptr;
                    _form[_i]._ptr = _f.length;
                    return _f.substr(_ptr);
                }
            }
        }
    };
    // достает из формы символ
    var _formcallchr = function (_name, _z) {
        var _h = _hash(_name), _i, _j;
        for (_i = 0; _i < _form.length; _i++) {
            if (_h === _form[_i]._hash && _name === _form[_i]._name) {
                if (_form[_i]._ptr >= _form[_i]._form.length) { _Z = true; return _z; }
                _j = _form[_i]._ptr;
                if (_form[_i]._form[_j] === '\ufffc') { _form[_i]._ptr += 2; _j = _form[_i]._ptr; }
                _form[_i]._ptr++;
                return _form[_i]._form[_j];
            }
        }
    };
    // достает из формы символы
    var _formcallchrs = function (_name, _d, _z) {
        var _h = _hash(_name), _i, _j, _out = '';
        for (_i = 0; _i < _form.length; _i++) {
            if (_h === _form[_i]._hash && _name === _form[_i]._name) {
                if (_d > 0) {
                    for (_j = 0; _j < _d; _j++) {
                        if (_form[_i]._ptr >= _form[_i]._form.length) { _Z = true; return _z; }
                        if (_form[_i]._form[_form[_i]._ptr] === '\ufffc') { _form[_i]._ptr += 2; if (_form[_i]._ptr >= _form[_i]._form.length) { _Z = true; return _z; } }
                        _out += _form[_i]._form[_form[_i]._ptr];
                        _form[_i]._ptr++;
                    }
                } else {
                    for (_j = 0; _j > _d; _j--) {
                        if (_form[_i]._ptr <= 0) { _Z = true; return _z; }
                        if (_form[_i]._form[_form[_i]._ptr - 1] === '\ufffc') { _form[_i]._ptr -= 2; if (_form[_i]._ptr <= 0) { _Z = true; return _z; } }
                        _out = _form[_i]._form[_form[_i]._ptr] + _out;
                        _form[_i]._ptr--;
                    }
                }
                return _out;
            }
        }
    };
    // сбрасывает указатель формы
    var _formcallrest = function (_name) {
        var _h = _hash(_name), _i = 0;
        for (_i = 0; _i < _form.length; _i++) {
            if (_h === _form[_i]._hash && _name === _form[_i]._name) {
                _form[_i]._ptr = 0;
                break;
            }
        }
    };
    // ищет подстроку в форме и ставит на нее указатель
    var _formsearch = function(_name,_substr,_z) {
        var _h = _hash(_name), _i = 0, _ptr;
        for (_i = 0; _i < _form.length; _i++) {
            if (_h === _form[_i]._hash && _name === _form[_i]._name) {
                _ptr = _form[_i]._form.indexOf(_substr);
                if (_ptr === -1) { _Z = true; return _z; }
                _form[_i]._ptr = _ptr;
                break;
            }
        }
    };
    // удаляет формы
    var _formdel = function (_sarr) {
        var _i, _j, _f, _t;
        for (_i = 0; _i < _form.length; _i++) {
            _f = _form.shift();
            _t = true;
            for (_j = 0; _j < _sarr.length; _j++) {
                if (_f._name === _sarr[_j]) _t = false;
            }
            if (_t) _form.push(_f);
        }
    };
    // удаляет все формы
    var _formdelall = function () { _form = []; };
        // список форм в памяти через разделитель
    var _formslist = function(_delimiter) {
        var _list = '', _i, _dlm = (_delimiter !== '')? _delimiter:' ';
        for (_i = 0; _i < _form.length; _i++) {
            _list += ((_i===0)? '':_dlm) + _form[_i]._name;
        }
        return _list;
    };
    // возвращает содержимое формы с указанным именем (отладочный json)
    var _formbody = function(_name) {
        var _h = _hash(_name), _i;
        for (_i = 0; _i < _form.length; _i++) {
            if (_h === _form[_i]._hash && _name === _form[_i]._name) {
                return '{"name":"'+ _form[_i]._name + 
                        '"\ufff3"hash":"'+ _form[_i]._hash +
                        '"\ufff3"id":"' + _form[_i]._id +
                        '"\ufff3"ptr":"' + _form[_i]._ptr +
                        '"\ufff3"css":"' + _form[_i]._css +
                        '"\ufff3"form":"' + _form[_i]._form + '"}';
            }
        }
        return '{"error":"form absent"}';
    };
    // переместить формы во внешнюю память    
    var _blockstore = function (_name, _sarr) {
        var _i, _j, _f, _t, _out = [];
        for (_i = 0; _i < _form.length; _i++) {
            _f = _form.shift();
            _t = true;
            for (_j = 0; _j < _sarr.length; _j++) {
                if (_f._name === _sarr[_j]) _t = false;
            }
            if (_t) _form.push(_f); else _out.push(_f);
        }
        localStorage.setItem('trac_'+_name, JSON.stringify(_out));
    };
    // загрузить формы из внешней памяти
    var _blockfetch = function (_name) {
        var _s = JSON.parse( localStorage.getItem('trac_'+_name));
        if (_s) { for (var _i = 0; _i < _s.length; _i++) { _form.push(_s[_i]); }}
    };
    // удалить блок во внешней памяти
    var _blockerase = function (_name) { localStorage.removeItem('trac_'+_name); };
    // ищет парную скобку
    var _searchparent = function () {
        var _cnt = 0, _go = true, _i = 0;
        while (_go) {
            if (_abuf[_i] === '(') { _cnt++; } else { if (_abuf[_i] === ')') { if (_cnt === 0) return _i; else _cnt--; } }
            _i++;
            if (_i === _abuf.length) { _go = false; }
        }
    };
    // загружает символ
    var _loadc = function (_z) {
        if(_ibuf.length === 0) { _Z = true; return _z; }
        var _res = _ibuf[0];
        _ibuf = _ibuf.substr(1);
        return _res;
    };
    // загружает строку ограниченную метасимволом или концом строки
    var _loads = function (_z) {
        if(_ibuf.length === 0) { _Z = true; return _z; }
        var _end = _ibuf.indexOf(_meta), _out;
        if (_end === -1) {
            _end = _ibuf.length;
            _out = _ibuf.substr(0, _end);
            _ibuf = '';
        } else {
            _out = _ibuf.substr(0, _end);
            _ibuf = _ibuf.substr(_end + 1);
        }
        return _out;
    };
    // обработчик встроенных функций
    var _start = function (_fstart) {
        var _s = _nbuf.substr(_fstart + 1).split('\u2551');
        _s.pop();
        switch (_s[0]) {
            // общий ввод вывод
            case 'rs': case 'читать': _funcvalue = _loads(_get(_s, 1)); return;
            case 'rc': case 'читатьсимвол': case 'читать символ': _funcvalue = _loadc(_get(_s, 1)); return;
            case 'cm': _funcvalue = ''; _meta = _get(_s, 1)[0]; return;
            case 'gm': _funcvalue = _meta; return;
            case 'ps': case 'печать': _funcvalue = ''; _obuf += _get(_s, 1); return;
                // работа с формами: пользовательские функции, массивы и т.п.
            case 'ds': case 'определить': _formadd(_formlen(), _get(_s, 1), _get(_s, 2), 0, 0); _funcvalue = ''; return;
            case 'ss': case 'разбить': var _n = _get(_s, 1); var _ss = _s; _ss.shift(); _ss.shift(); _formseg(_n, _ss); _funcvalue = ''; return;
            case 'cl': case 'вызвать': var _n = _get(_s, 1); var _ss = _s; _ss.shift(); _ss.shift(); _funcvalue = _formcall(_n, _ss); return;
	    case 'cs': case 'сегмент': _funcvalue = _formcallseg( _get(_s, 1), _get(_s, 2)); return;
            case 'cc': case 'символ': _funcvalue = _formcallchr( _get(_s, 1), _get(_s, 2)); return;
            case 'cn': case 'символы': _funcvalue = _formcallchrs( _get(_s, 1), _get(_s, 2), _get(_s, 3)); return;
            case 'cr': case 'сначала': _formcallrest( _get(_s, 1)); _funcvalue = ''; return;
            case 'fd': case 'искать': _formsearch(_get(_s, 1),_get(_s, 2),_get(_s, 3)); _funcvalue = ''; return;
            case 'dd': case 'стереть': var _ss = _s; _ss.shift(); _formdel(_ss); _funcvalue = ''; return;
            case 'da': case 'стеретьвсе': case 'стереть все': _formdelall(); _funcvalue = ''; return;
                // арифметика
            case 'ad': case '+': var _a1 = parseFloat(_get(_s, 1)), _a2 = parseFloat(_get(_s, 2)); if (typeof _a1 === 'number' && typeof _a2 === 'number') _funcvalue = (_a1 + _a2).toString(); else { _funcvalue = _get(_s, 3); _Z = true; } return;
            case 'su': case '-': var _a1 = parseFloat(_get(_s, 1)), _a2 = parseFloat(_get(_s, 2)); if (typeof _a1 === 'number' && typeof _a2 === 'number') _funcvalue = (_a1 - _a2).toString(); else { _funcvalue = _get(_s, 3); _Z = true; } return;
            case 'ml': case '*': var _a1 = parseFloat(_get(_s, 1)), _a2 = parseFloat(_get(_s, 2)); if (typeof _a1 === 'number' && typeof _a2 === 'number') _funcvalue = (_a1 * _a2).toString(); else { _funcvalue = _get(_s, 3); _Z = true; } return;
            case 'dv': case '/': var _a1 = parseFloat(_get(_s, 1)), _a2 = parseFloat(_get(_s, 2)); if (typeof _a1 === 'number' && typeof _a2 === 'number' && _a2 !== 0) _funcvalue = (_a1 / _a2).toString(); else { _funcvalue = _get(_s, 3); _Z = true; } return;
            case 'целое': _funcvalue = Math.floor(parseFloat(_get(_s, 1))); return;
                // битовые операции
            case 'bu': _funcvalue = ''; return;
            case 'bi': _funcvalue = ''; return;
            case 'bc': _funcvalue = ''; return;
            case 'bs': _funcvalue = ''; return;
            case 'br': _funcvalue = ''; return;
                // управление выполнением
            case 'eq': case '=': if (_get(_s, 1) == _get(_s, 2)) { _funcvalue = _get(_s, 3); } else { _funcvalue = _get(_s, 4); } return;
            case 'gr': case '>': if (_get(_s, 1) > _get(_s, 2)) { _funcvalue = _get(_s, 3); } else { _funcvalue = _get(_s, 4); } return;
            case 'lt': case '<': if (_get(_s, 1) < _get(_s, 2)) { _funcvalue = _get(_s, 3); } else { _funcvalue = _get(_s, 4); } return;
                // интерфейс с внешним хранилищем
            case 'sb': case 'сохранить': var _n = _get(_s, 1); var _ss = _s; _ss.shift(); _ss.shift(); _blockstore(_n, _ss); _funcvalue = ''; return;
            case 'fb': case 'загрузить': _blockfetch(_get(_s, 1)); _funcvalue = ''; return;
            case 'eb': case 'удалить': _blockerase(_get(_s, 1)); _funcvalue = ''; return;
                // стоп и ноп
            case 'hl': case 'стоп': _funcvalue = ''; _worked = false; return;
                // отладка
            case 'np': _funcvalue = ''; return;
            case 'nl': case 'формы': _funcvalue = _formslist(_get(_s,1)); return;
            case 'pb': case 'форма': _funcvalue = _formbody(_get(_s,1)); return;
                // экстра
           // case 'сегодня': var t = new Date(); funcvalue = t.getFullYear() + '-' + Lib.add0(t.getMonth() + 1) + '-' + Lib.add0(t.getDate()); return;
           // case 'сейчас': var t = new Date(); funcvalue = t.getHours() + ':' + Lib.add0(t.getMinutes().toString()) + ':' + Lib.add0(t.getSeconds().toString()); return;
// cn
// 


            default: var _n = _s[0]; var _ss = _s; _ss.shift(); _funcvalue = _formcall(_n, _ss); return;
        }
    };
    // распознаватель типа функции
    var _gofunc = function () {
        var _fstart = Math.max(_nbuf.lastIndexOf('\u2642'), _nbuf.lastIndexOf('\u2640'));
        _start(_fstart);
        if (_fstart >= 0) {
            if (_nbuf[_fstart] === '\u2642') {
                _abuf = _funcvalue + _abuf;
                _nbuf = _nbuf.substr(0, _fstart);
                _Z = false;
            } else {
                if (_nbuf[_fstart] === '\u2640' && _Z === true) {
                    _abuf = _funcvalue + _abuf;
                    _nbuf = _nbuf.substr(0, _fstart);
                    _Z = false;
                } else {
                    if (_nbuf[_fstart] === '\u2640' && _Z === false) {
                        _nbuf = _nbuf.substr(0, _fstart) + _funcvalue;
                        _Z = false;
                    } else {
                        _nbuf = _nbuf.substr(0, _fstart);
                    }
                }
            }
        }
    };
    // интерпретатор
    var _interpret = function () {
        while (_worked) {
            if (_abuf.length === 0) {
                _abuf = _idle;
                _Z = false;
            } else {
                if (_c1() === '(') {
                    _next();
                    var _parent = _searchparent();
                    _nbuf += _abuf.substr(0, _parent);
                    _abuf = _abuf.substr(_parent + 1);
                } else {
                    if (_c1() === '\r' || _c1() === '\n' || _c1() === '\t') {
                        _next();
                    } else {
                        if (_c1() === ',') {
                            _next();
                            _nbuf += '\u2551';
                        } else {
                            if (_c1() === '#' && _c2() === '#' && _c3() === '(') {
                                _next(3);
                                _nbuf += '\u2640';
                            } else {
                                if (_c1() === '#' && _c2() === '(') {
                                    _next(2);
                                    _nbuf += '\u2642';
                                } else {
                                    if (_c1() === ')') {
                                        _next();
                                        _nbuf += '\u2551\ufd3f';
                                        _gofunc();   
                                    } else {
                                        _nbuf += _c1();
                                        _next();
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
    // после его работы созданные и измененные формы по прежнему хранятся и
    // могут использоваться при следующих запусках интерфейса
    var _run = function (_input) {
        _ibuf = _input + '#(hl)';
        _abuf = _idle;
        _nbuf = '';
        _obuf = '';
        _worked = true;
        _interpret();
        return _obuf;
    };
    // инициалиизация если надо перезапустить машину полностью, сбросить формы и т.д.
    var _reset = function() {
        _ibuf = '';
        _abuf = '';
        _nbuf = '';
        _form = [];
        _obuf = '';
        _worked = true;
        _idle = '#(ps,#(rs))';
        _Z = false;
        _meta = '`';
        _funcvalue = '';
    };
    // перекодировка входных строк так чтобы их содержимое не воспринималось как trac код
    var _codeTo = function (_s) {
        return _s.replace('#','\ufff0').replace('(','\ufff1').replace(')','\ufff2').replace(',','\ufff3').replace(_meta,'\ufff4');
    };
    // перекодировка выходной строки, возвращающая на место используемые trac символы
    var _codeFrom = function (_s) {
        return _s.replace('\ufff0','#').replace('\ufff1','(').replace('\ufff2',')').replace('\ufff3',',').replace('\ufff4',_meta);
    };
    
    // фиксируем доступные снаружи методы
    return Object.freeze({run:_run,reset:_reset,codeTo:_codeTo,codeFrom:_codeFrom});
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

 #(ds,name,value)`(присваивание константы: var name = value)`

 значение переменной  name
 #(name)

 инкремент переменной  name++
 #(ds,name,#(ad,#(name),1)`

 присваивание результата выражения    name <- expression
 #(ds,name,expression)`

 определение функции func name(param1, param2) <- expression
 #(ds,name,(expression))#(ss,name,param1,param2)`

 вызов функции    name(param1,param2)
 #(name,param1,param2)`

 условное ветвление if(condition==0,trueexpression,falseexpression)
 #(eq,condition,0,falseexpression,trueexpression)`

 цикл по условию while(condition==0, expression)
 #(ds,tempname,(#(eq,condition,0,expression#(tempname),)))`#(tempname)`#(dd,tempname)`

 цикл по счетчику for(startcount,endcount, expression)
 #(ds,tempvar,(startcount))#(ds,tempname,(#(eq,tempvar,endcount,,expression#(ds,tempvar,#(ad,#(tempvar),1)))#(tempname))))`#(tempname)`#(dd,tempvar,tempname)`
 
 рекурсия
 #(ds,Factorial,(#(eq,1,X,1,(#(ml,X,#(Factorial,#(ad,X,-1)))))))#(ss,Factorial,X)`#(Factorial,5)`
 
 html шаблон
 #(ds,div,(#(ps,<div style='background-color:#0A0;width:30px;height:30px;'>text</div>)))`#(ss,div,text)` #(div,123)`(comment)'

 последовательное посимвольное чтение из строки
 #(ds,line,0123456789abcdef)#(cr,line)`#(ps,#(cc,line))`#(ps,#(cc,line))`#(ps,#(cc,line))`#(ps,#(cn,line,5))`
    
 последовательное построчное чтение из массива строк
 #(ds,line,012/34567/89abcd/ef)#(cr,line)#(ss,line,/)`#(ps,#(cs,line))#(ps,-))#(ps,#(cs,line))#(ps,-))#(ps,#(cs,line))`
    
 

 */