var path = require('path');
var chai = require('chai');
var gt = require('../build/Release/nodegettext');

var expect = chai.expect;


describe('gettext', function() {

  var dirName = path.join(__dirname, 'locale');
  var domainName = 'messages';
  var locale = 'pt_BR.UTF-8';
  var lcType = '';
  var expected;

  it('binds the text domain', function() {
    expected = dirName;
    expect(gt.bindtextdomain(domainName, dirName)).to.equal(expected);
  });

  it('sets the domain', function() {
    expected = domainName;
    expect(gt.textdomain(domainName)).to.equal(expected);
  });

  it('gets the domain', function() {
    expected = domainName;
    expect(gt.textdomain(null)).to.equal(expected);
  });

  it('sets the locale', function() {
    expected = locale;
    expect(gt.setlocale(gt.LC_MESSAGES, locale)).to.equal(expected);
  });

  it('gets the locale', function() {
    expected = locale;
    expect(gt.setlocale(gt.LC_MESSAGES, null)).to.equal(expected);
  });

  it('gets translated text', function() {
    expected = 'Endereço';

    gt.bindtextdomain(domainName, dirName);
    gt.textdomain(domainName);
    gt.setlocale(gt.LC_MESSAGES, locale);

    expect(gt.gettext('Address')).to.equal(expected);
  });
});
