var mceliece = (function () {

/* Workaround for https://bugs.chromium.org/p/chromium/issues/detail?id=736602 */

var mobileChromeVersion	= typeof navigator === 'undefined' || /Edge/.test(navigator.userAgent) ?
	NaN :
	parseInt((navigator.userAgent.match(/Chrome\/(\d+).*Mobile/) || [])[1], 10)
;

var GlobalWebAssembly	= typeof WebAssembly === 'undefined' ? undefined : WebAssembly;

return (function () {

var WebAssembly			= mobileChromeVersion !== 58 ? GlobalWebAssembly : undefined;
