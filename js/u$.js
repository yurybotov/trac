// Микро библиотека работы с селекторами взамен jQuery или Zepto

(function (exports) {

    // отработка запуска после загрузки контента
    var ready;
    var onready = [];
    function readytest() {
        ready = false;
        if (document.addEventListener) {
            document.addEventListener("DOMContentLoaded", completed, false);
            window.addEventListener("load", completed, false);
        } else {
            document.attachEvent("onreadystatechange", completed);
            window.attachEvent("onload", completed);
        }
    }
    function complited() {
        ready = true;
        for (var i = 0; i < onready.length; i++) {
            onready[i]();
        }
    }

    var u$ = (function u$() {

        // конструктор - собственно создание объекта селектора
        // понимет селекторы:
        //    #xxxxx - найти объект с этим id
        //    .xxxxx - найти все объекты с этим классом
        //    ххххх - найти все такие тэги
        // строчные селекторы могут компоноваться через знак '&' (пробелы игнорируются) $('div & .classname & .otherclassname') 
        // тогда поиск ведется по первому селектору, а из массива затем удаляются элементы не соответствующие следующим селекторам.
        // то есть, по сути селекторы объединяются по 'И'
        //    объектDOM - выбрать этот объект
        //    функция - запустить эту функцию без параметров только по окончании загрузки всей страницы
        function U$(id) {
            var i = 0;
            this.res = [];
            this.u$ = true;

            switch (typeof id) {
                case 'object':
                    this.res.push(id);
                    break;
                case 'string':
                    switch (id.substr(0, 1)) {
                        case '#':
                            var obj = window.document;
                            this.res.push(obj.getElementById(id.substr(1)));
                            break;
                        case '.':
                            var obj = window.document;
                            this.res = obj.getElementsByClassName(id.substr(1));
                            break;
                        default:
                            var obj = window.document;
                            this.res = obj.getElementsByTagName(id);
                            break;
                    }
                    break;
                case 'function':
                    if (ready) id(); else onready.push(id);
                    break;
                default:
                    break;
            }
            return this;
        }

        // сокращенная запись
        U$.fn = U$.prototype;

        // создание экземпляра объекта селектора
        function u$(id) {
            return new U$(id);
        }


        // эти свойства можно выстраивать в цепочку

        // выполнить И по отношении к существующим записям и записям нового селектора
        U$.fn.and = function (id) {
            var arr = [], i = 0, j = 0, newres = [];
            switch (typeof id) {
                case 'object':
                    arr.push(id);
                    break;
                case 'string':
                    switch (id.substr(0, 1)) {
                        case '#':
                            var obj = window.document;
                            arr.res.push(obj.getElementById(id.substr(1)));
                            break;
                        case '.':
                            var obj = window.document;
                            arr.res = obj.getElementsByClassName(id.substr(1));
                            break;
                        default:
                            var obj = window.document;
                            arr.res = obj.getElementsByTagName(id);
                            break;
                    }
                    break;
                default:
                    break;
            }
            for (i = 0; i < this.res.length; i++) {
                for (j = 0; i < arr.length; i++) {
                    if (this.res[i].isSameNode(arr[j])) newres.push(this.res[i]);
                }
            }
            this.res = newres;
            return this;
        };

        // выполнить ИЛИ по отношении к существующим записям и записям нового селектора
        U$.fn.or = function (id) {
            var arr = [], i = 0, j = 0, newres = [], exist = false;
            switch (typeof id) {
                case 'object':
                    arr.push(id);
                    break;
                case 'string':
                    switch (id.substr(0, 1)) {
                        case '#':
                            var obj = window.document;
                            arr.res.push(obj.getElementById(id.substr(1)));
                            break;
                        case '.':
                            var obj = window.document;
                            arr.res = obj.getElementsByClassName(id.substr(1));
                            break;
                        default:
                            var obj = window.document;
                            arr.res = obj.getElementsByTagName(id);
                            break;
                    }
                    break;
                default:
                    break;
            }
            for (i = 0; i < arr.length; i++) {
                exist = false;
                for (j = 0; i < this.res[j].length; i++) {
                    if (this.res[j].isSameNode(arr[i])) exist = true;
                }
                if (!exist) this.res.push(arr[j]);
            }
            return this;
        };

        // выбрать из существующих по наличию или значению аттрибута
        U$.fn.attr = function (attr) {
            var attribute, op, value, idx;
            if ((idx = attr.indexOf('$=')) > 0) {
                attribute = attr.substr(0, idx);
                op = 'end';
                value = attr.substr(idx + 2);
            } else if ((idx = attr.indexOf('^=')) > 0) {
                attribute = attr.substr(0, idx);
                op = 'bgn';
                value = attr.substr(idx + 2);
            } else if ((idx = attr.indexOf('*=')) > 0) {
                attribute = attr.substr(0, idx);
                op = 'has';
                value = attr.substr(idx + 2);
            } else if ((idx = attr.indexOf('=')) > 0) {
                attribute = attr.substr(0, idx);
                op = 'eq';
                value = attr.substr(idx + 1);
            } else {
                attribute = attr;
                op = 'def';
                value = '';
            }
            this.filter(function (o) {
                if (o[attribute] == null) return false;
                switch (op) {
                    case 'def': return true;
                    case 'eq': return (o[attribute] === value);
                    case 'bgn': return (o[attribute].indexOf(value) === 0);
                    case 'end': return (o[attribute].indexOf(value) === o[attribute].length - value.length);
                    case 'has': return (o[attribute].indexOf(value) > -1);
                    default: return false;
                }
            });
            return this;
        };

        // что то сделать со всеми выбранными объектами (у функции должен быть параметр через который передается каждый из объектов - function(item){}
        // но можно заодно получить и его номер и весь массив, то есть  function(item, number, array){}
        U$.fn.each = function (func) {
            if (this.u$ && this.res.length > 0) {
                for (var i = 0; i < this.res.length; i++)
                    if (this.res[i]) func(this.res[i], i, this.res);
            }
            return this;
        };

        // фильтрация - выбор подмассива из массива выбранных объектов. аргументы func как у each и она должна возвращать значение true для
        // выбора элемента и false для его отбрасывания
        U$.fn.filter = function (func) {
            var arr = []
            if (this.u$ && this.res.length > 0) {
                for (var i = 0; i < this.res.length; i++) {
                    if (this.res[i]) {
                        if (func(this.res[i], i, this.res)) {
                            arr.push(this.res[i]);
                        }
                    }
                }
                this.res = arr;
            }
            return this;
        };

        // прицепить к выбранным объектам обработчик события
        U$.fn.on = function (e, callback) {
            this.each(function (o) { o.addEventListener(e, callback, false); })
            return this;
        };
        U$.fn.bind = U$.fn.on;

        // отсоединить обработчик события
        U$.fn.off = function (e, callback) {
            this.each(function (o) { o.removeEventListener(e, callback, false); })
            return this;
        };
        U$.fn.unbind = U$.fn.off;

        // сделать объект невидимым
        U$.fn.hide = function () {
            this.each(function (o) { o.style.display = 'none'; });
            return this;
        };

        // сделать объект видимым
        U$.fn.show = function () {
            this.each(function (o) { o.style.display = ''; });
            return this;
        };

        // подцепить HTML к выбранным объектам
        U$.fn.html = function (text) {
            this.each(function (o) { o.innerHTML = text; });
            return this;
        };

        // очистить поле HTML у выбранных объектов
        U$.fn.empty = function () {
            this.each(function (o) { o.innerHTML = ''; });
            return this;
        };

        // изменить стиль выбранных объектов
        U$.fn.css = function (name, value) {
            this.each(function (o) { o.style[name] = value; });
            return this;
        };

        // прицепить класс к выбранным объектам
        U$.fn.addClass = function (className) {
            var r = new Regexp("(^|\\s)" + className + "(\\s|$)", "g");
            this.each(function (o) { if (!r.test(o.className)) { o.className = (o.className + " " + className).replace(/\s+/g, " ").replace(/(^ | $)/g, ""); } });
            return this;
        };

        // удалить класс из выбранных объектов
        U$.fn.removeClass = function (className) {
            var r = new RegExp("(^|\\s)" + className + "(\\s|$)", "g");
            this.each(function (o) { o.className = o.className.replace(re, "$1").replace(/\s+/g, " ").replace(/(^ | $)/g, ""); });
            return this;
        };


        // эти свойства - последние в цепочке

        // вернуть массив выбранных объектов для дальнейшей обработки
        U$.fn.values = function () {
            if (this.u$ && this.res.length > 0) {
                return this.res;
            } else
                return [];
        };

        // вернуть один объект для последующей обработки (если index нет то нулевой объект)
        U$.fn.get = function (index) {
            if (this.u$ && this.res.length > 0) {
                if (index) {
                    return this.res[index];
                } else {
                    return this.res[0];
                }
            } else return null;
        };
        U$.fn.item = function (index) {
            if (this.u$ && this.res.length > 0) {
                if (index) {
                    return this.res[index];
                } else {
                    return this.res[0];
                }
            } else return null;
        };

        return u$;

    })();

    exports.u$ = u$;

})(this);

// Использование снаружи в стиле jQuery:
//
//// прицепить к знаку $ (можно это и не делать, а использовать u$('...') )
//  var $ = this.u$;                                                
//// эта функция будет выполнени только после загрузки всей страницы
//  $(function() {    
//// элементы с id "tegid" и "othertegid" спрятать, подцепить к ним новый html и css код и показать
//      $('#tegid').or('#othertegid').hide().html("<div>...</div>").css('color','white').show();           
//// для каждого элемента с классом "tegclass" у которого есть 'innerHTML' подцепить соответсвующий HTML код хрянащийся в localstorage
//      $('.tagclass').attr('innerHTML').each( function(i) { i.innerHTML = localStorage(i.id)} );  
//  }