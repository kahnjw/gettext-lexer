var fs = require('fs');
var chai = require('chai');
var gettext = require('gettext-parser');
var gettextLexer = require('../build/Release/gettext-lexer');


describe('gettext lexer', function() {
  var po = fs.readFileSync(__dirname + '/fake.po', 'utf-8');
  var expected = gettext.po.parse(po);

  it('parses the po', function() {
    gettextLexer.parse(po).to.equal(expected);
  });
});