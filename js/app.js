var $ = this.u$;

var trac = new Trac();
var rpn = new Rpn();

function go() {
    $('#content').html(trac.codeFrom(trac.run($('#source').get().value+trac.codeTo('#(test)`'))));
};
