// RPN - Rear Poland Notation helpers 
// internal form as like Forth but comma separated

function Rpn() {
    // перевод обратной польской нотации в вычисленное значение
    var toValue = function (str, arr) {
        var i, j, stack = [], a, curr, arg, p;
        if (arr) {
            for (i = 0; i < arr.length; i++) {
                str.replace(new RegExp(arr[i].key, 'g'), arr[i].value);
            }
        }
        a = str.split(',');
        for (i = 0; i < a.length; i++) {
            curr = a[i];
            if (curr !== '') {
                if (curr.indexOf('#') < 0) {
                    stack.push(parseFloat(curr));
                } else {
                    arg = curr.split('#');
                    p = [];
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
    };
    // перевод обратной польской нотации в текст на lisp
    var toLisp = function (str) {
        var i, j, a = str.split(','), stack = [], curr, arg, p, res;
        for (i = 0; i < a.length; i++) {
            curr = a[i];
            if (curr !== '') {
                if (curr.indexOf('#') < 0) {
                    stack.push(curr);
                } else {
                    arg = curr.split('#');
                    p = [];
                    for (j = 0; j < parseInt(arg[1], 10) ; j++) {
                        p[j] = stack.pop();
                    }
                    res = '(' + arg[0];
                    for (j = p.length - 1; j >= 0; j--) {
                        res += ' ' + p[j];
                    }
                    stack.push(res + ')');
                }
            }
        }
        return stack[0];
    };
    // перевод обратной польской нотации в мою версию trac
    var toTrac = function (str) {
        var i, j, a = str.split(','), stack = [], curr, arg, p, res;
        for (i = 0; i < a.length; i++) {
            curr = a[i];
            if (curr !== '') {
                if (curr.indexOf('#') < 0) {
                    stack.push(curr);
                } else {
                    arg = curr.split('#');
                    p = [];
                    for (j = 0; j < parseInt(arg[1], 10) ; j++) {
                        p[j] = stack.pop();
                    }
                    res = '#(' + arg[0];
                    for (j = p.length - 1; j >= 0; j--) {
                        res += ',' + p[j];
                    }
                    stack.push(res + ')');
                }
            }
        }
        return stack[0];
    };
    // на входе сторока в скобочной нотации на выходе в обратной польской нотации разделенная запятыми
    var fromBrackets = function (str) {
        var i, a = str, s = [], fs = [], o = '', c = '', p = 0, cur = a[0], prv = '', arc = 0;
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
                        while (s.length !== 0 && (c = s.pop()) !== 'f') { o += (((o[o.length - 1] !== ',') ? ',' : '') + c + '#2,'); }
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
                        if (isChar(cur)) {
                            // один из разрешенных символов имени идентификатора или число
                            o += cur;
                        } else {
                            if (cur === '(') {
                                if (isOp(cur) || prv === '') {
                                    // открывающая скобка операции
                                    s.push(cur);
                                    o += (o[o.length - 1] !== ',') ? ',' : '';
                                    p++;
                                } else {
                                    // открывающая скобка функции
                                    arc = 0;
                                    s.push('f');
                                    fs.push(o.substr(lastOp(o) + 1).trim());
                                    o = o.substr(0, lastOp(o) + 1).trim();
                                    p++;
                                }
                            }
                            if (isOp(cur)) {
                                // это унарный знак
                                if ((cur === '+' || cur === '-') && (isOp(prv) || prv === '(')) {
                                    o += cur;
                                } else {
                                    // это операция
                                    if (s.length === 0) {
                                        // это первая встреченная операция
                                        s.push(cur);
                                        o += (o[o.length - 1] !== ',') ? ',' : '';
                                    } else {
                                        if (prior(s[s.length - 1]) <= prior(cur)) {
                                            // приоритет этой операции больше приоритета операции в стеке
                                            s.push(cur);
                                            o += (o[o.length - 1] !== ',') ? ',' : '';
                                        } else {
                                            // приоритет этой операции меньше приоритета операции в стеке
                                            while (s.length !== 0 && prior(s[s.length - 1]) > prior(cur)) {
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
    var lastOp = function (s) {
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
    var isOp = function (cur) {
        return (cur === '+' || cur === '-' || cur === '*' || cur === '/' || cur === '%');
    };
    // проверяет допустимый ли это символ
    var isChar = function (cur) {
        return (cur === '.' || (cur >= '0' && cur <= '9') || (cur >= 'a' && cur <= 'z') ||
               (cur >= 'A' && cur <= 'Z') || (cur >= 'а' && cur <= 'я') || (cur >= 'А' && cur <= 'Я'));
    };
    // дает приоритет текущей операции
    var prior = function (c) {
        switch (c) {
            case '*': case '/': case '%': return 3;
            case '+': case '-': return 2;
            case '(': return 1;
            default: return 0;
        }
    };
    //
    return Object.frizze({toValue:toValue,toLisp:toLisp,toTrac:toTrac,fromBrackets:fromBrackets});
}





