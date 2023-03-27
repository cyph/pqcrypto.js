var fastSHA512;(function(){var __webpack_modules__=[function(module,__unused_webpack_exports,__webpack_require__){var isNode="object"==typeof process&&"object"!=typeof window&&"function"!=typeof importScripts,naclSHA512=__webpack_require__(1),sodiumUtil=__webpack_require__(2),nodeCrypto,Buffer;isNode&&(nodeCrypto=eval("require")("crypto"),Buffer=eval("global.Buffer"));var fastSHA512={bytes:64,baseHash:function(e,r){return Promise.resolve().then((function(){if(void 0===e&&(e=new Uint8Array(0)),!ArrayBuffer.isView(e))throw new Error("Cannot hash invalid input.");if(isNode){var r=nodeCrypto.createHash("sha512");return r.update(Buffer.from(e)),r.digest()}return crypto.subtle.digest({name:"SHA-512"},e)})).then((function(e){return new Uint8Array(e)})).catch((function(){return naclSHA512(e)})).then((function(n){return r&&sodiumUtil.memzero(e),n}))},hash:function(e,r){return Promise.resolve().then((function(){return fastSHA512.baseHash(sodiumUtil.from_string(e),"string"==typeof e)})).then((function(e){return r?e:{binary:e,hex:sodiumUtil.to_hex(e)}}))}};fastSHA512.fastSHA512=fastSHA512,module.exports=fastSHA512},function(e){var r=[1116352408,3609767458,1899447441,602891725,3049323471,3964484399,3921009573,2173295548,961987163,4081628472,1508970993,3053834265,2453635748,2937671579,2870763221,3664609560,3624381080,2734883394,310598401,1164996542,607225278,1323610764,1426881987,3590304994,1925078388,4068182383,2162078206,991336113,2614888103,633803317,3248222580,3479774868,3835390401,2666613458,4022224774,944711139,264347078,2341262773,604807628,2007800933,770255983,1495990901,1249150122,1856431235,1555081692,3175218132,1996064986,2198950837,2554220882,3999719339,2821834349,766784016,2952996808,2566594879,3210313671,3203337956,3336571891,1034457026,3584528711,2466948901,113926993,3758326383,338241895,168717936,666307205,1188179964,773529912,1546045734,1294757372,1522805485,1396182291,2643833823,1695183700,2343527390,1986661051,1014477480,2177026350,1206759142,2456956037,344077627,2730485921,1290863460,2820302411,3158454273,3259730800,3505952657,3345764771,106217008,3516065817,3606008344,3600352804,1432725776,4094571909,1467031594,275423344,851169720,430227734,3100823752,506948616,1363258195,659060556,3750685593,883997877,3785050280,958139571,3318307427,1322822218,3812723403,1537002063,2003034995,1747873779,3602036899,1955562222,1575990012,2024104815,1125592928,2227730452,2716904306,2361852424,442776044,2428436474,593698344,2756734187,3733110249,3204031479,2999351573,3329325298,3815920427,3391569614,3928383900,3515267271,566280711,3940187606,3454069534,4118630271,4000239992,116418474,1914138554,174292421,2731055270,289380356,3203993006,460393269,320620315,685471733,587496836,852142971,1086792851,1017036298,365543100,1126000580,2618297676,1288033470,3409855158,1501505948,4234509866,1607167915,987167468,1816402316,1246189591];function n(e,r,n,t){e[r]=n>>24&255,e[r+1]=n>>16&255,e[r+2]=n>>8&255,e[r+3]=255&n,e[r+4]=t>>24&255,e[r+5]=t>>16&255,e[r+6]=t>>8&255,e[r+7]=255&t}function t(e,n,t,o){for(var a,i,f,u,c,s,_,p,l,d,h,y,g,w,m,v,A,b,x,U,S,H,k,C,T,E,q=new Int32Array(16),B=new Int32Array(16),I=e[0],z=e[1],N=e[2],O=e[3],j=e[4],D=e[5],P=e[6],R=e[7],M=n[0],V=n[1],Z=n[2],$=n[3],F=n[4],G=n[5],J=n[6],K=n[7],L=0;o>=128;){for(x=0;x<16;x++)U=8*x+L,q[x]=t[U+0]<<24|t[U+1]<<16|t[U+2]<<8|t[U+3],B[x]=t[U+4]<<24|t[U+5]<<16|t[U+6]<<8|t[U+7];for(x=0;x<80;x++)if(a=I,i=z,f=N,u=O,c=j,s=D,_=P,R,l=M,d=V,h=Z,y=$,g=F,w=G,m=J,K,k=65535&(H=K),C=H>>>16,T=65535&(S=R),E=S>>>16,k+=65535&(H=(F>>>14|j<<18)^(F>>>18|j<<14)^(j>>>9|F<<23)),C+=H>>>16,T+=65535&(S=(j>>>14|F<<18)^(j>>>18|F<<14)^(F>>>9|j<<23)),E+=S>>>16,k+=65535&(H=F&G^~F&J),C+=H>>>16,T+=65535&(S=j&D^~j&P),E+=S>>>16,S=r[2*x],k+=65535&(H=r[2*x+1]),C+=H>>>16,T+=65535&S,E+=S>>>16,S=q[x%16],C+=(H=B[x%16])>>>16,T+=65535&S,E+=S>>>16,T+=(C+=(k+=65535&H)>>>16)>>>16,k=65535&(H=b=65535&k|C<<16),C=H>>>16,T=65535&(S=A=65535&T|(E+=T>>>16)<<16),E=S>>>16,k+=65535&(H=(M>>>28|I<<4)^(I>>>2|M<<30)^(I>>>7|M<<25)),C+=H>>>16,T+=65535&(S=(I>>>28|M<<4)^(M>>>2|I<<30)^(M>>>7|I<<25)),E+=S>>>16,C+=(H=M&V^M&Z^V&Z)>>>16,T+=65535&(S=I&z^I&N^z&N),E+=S>>>16,p=65535&(T+=(C+=(k+=65535&H)>>>16)>>>16)|(E+=T>>>16)<<16,v=65535&k|C<<16,k=65535&(H=y),C=H>>>16,T=65535&(S=u),E=S>>>16,C+=(H=b)>>>16,T+=65535&(S=A),E+=S>>>16,z=a,N=i,O=f,j=u=65535&(T+=(C+=(k+=65535&H)>>>16)>>>16)|(E+=T>>>16)<<16,D=c,P=s,R=_,I=p,V=l,Z=d,$=h,F=y=65535&k|C<<16,G=g,J=w,K=m,M=v,x%16==15)for(U=0;U<16;U++)S=q[U],k=65535&(H=B[U]),C=H>>>16,T=65535&S,E=S>>>16,S=q[(U+9)%16],k+=65535&(H=B[(U+9)%16]),C+=H>>>16,T+=65535&S,E+=S>>>16,A=q[(U+1)%16],k+=65535&(H=((b=B[(U+1)%16])>>>1|A<<31)^(b>>>8|A<<24)^(b>>>7|A<<25)),C+=H>>>16,T+=65535&(S=(A>>>1|b<<31)^(A>>>8|b<<24)^A>>>7),E+=S>>>16,A=q[(U+14)%16],C+=(H=((b=B[(U+14)%16])>>>19|A<<13)^(A>>>29|b<<3)^(b>>>6|A<<26))>>>16,T+=65535&(S=(A>>>19|b<<13)^(b>>>29|A<<3)^A>>>6),E+=S>>>16,E+=(T+=(C+=(k+=65535&H)>>>16)>>>16)>>>16,q[U]=65535&T|E<<16,B[U]=65535&k|C<<16;k=65535&(H=M),C=H>>>16,T=65535&(S=I),E=S>>>16,S=e[0],C+=(H=n[0])>>>16,T+=65535&S,E+=S>>>16,E+=(T+=(C+=(k+=65535&H)>>>16)>>>16)>>>16,e[0]=I=65535&T|E<<16,n[0]=M=65535&k|C<<16,k=65535&(H=V),C=H>>>16,T=65535&(S=z),E=S>>>16,S=e[1],C+=(H=n[1])>>>16,T+=65535&S,E+=S>>>16,E+=(T+=(C+=(k+=65535&H)>>>16)>>>16)>>>16,e[1]=z=65535&T|E<<16,n[1]=V=65535&k|C<<16,k=65535&(H=Z),C=H>>>16,T=65535&(S=N),E=S>>>16,S=e[2],C+=(H=n[2])>>>16,T+=65535&S,E+=S>>>16,E+=(T+=(C+=(k+=65535&H)>>>16)>>>16)>>>16,e[2]=N=65535&T|E<<16,n[2]=Z=65535&k|C<<16,k=65535&(H=$),C=H>>>16,T=65535&(S=O),E=S>>>16,S=e[3],C+=(H=n[3])>>>16,T+=65535&S,E+=S>>>16,E+=(T+=(C+=(k+=65535&H)>>>16)>>>16)>>>16,e[3]=O=65535&T|E<<16,n[3]=$=65535&k|C<<16,k=65535&(H=F),C=H>>>16,T=65535&(S=j),E=S>>>16,S=e[4],C+=(H=n[4])>>>16,T+=65535&S,E+=S>>>16,E+=(T+=(C+=(k+=65535&H)>>>16)>>>16)>>>16,e[4]=j=65535&T|E<<16,n[4]=F=65535&k|C<<16,k=65535&(H=G),C=H>>>16,T=65535&(S=D),E=S>>>16,S=e[5],C+=(H=n[5])>>>16,T+=65535&S,E+=S>>>16,E+=(T+=(C+=(k+=65535&H)>>>16)>>>16)>>>16,e[5]=D=65535&T|E<<16,n[5]=G=65535&k|C<<16,k=65535&(H=J),C=H>>>16,T=65535&(S=P),E=S>>>16,S=e[6],C+=(H=n[6])>>>16,T+=65535&S,E+=S>>>16,E+=(T+=(C+=(k+=65535&H)>>>16)>>>16)>>>16,e[6]=P=65535&T|E<<16,n[6]=J=65535&k|C<<16,k=65535&(H=K),C=H>>>16,T=65535&(S=R),E=S>>>16,S=e[7],C+=(H=n[7])>>>16,T+=65535&S,E+=S>>>16,E+=(T+=(C+=(k+=65535&H)>>>16)>>>16)>>>16,e[7]=R=65535&T|E<<16,n[7]=K=65535&k|C<<16,L+=128,o-=128}return o}e.exports=function(e){var r=new Uint8Array(64);return function(e,r,o){var a,i=new Int32Array(8),f=new Int32Array(8),u=new Uint8Array(256),c=o;for(i[0]=1779033703,i[1]=3144134277,i[2]=1013904242,i[3]=2773480762,i[4]=1359893119,i[5]=2600822924,i[6]=528734635,i[7]=1541459225,f[0]=4089235720,f[1]=2227873595,f[2]=4271175723,f[3]=1595750129,f[4]=2917565137,f[5]=725511199,f[6]=4215389547,f[7]=327033209,t(i,f,r,o),o%=128,a=0;a<o;a++)u[a]=r[c-o+a];for(u[o]=128,u[(o=256-128*(o<112?1:0))-9]=0,n(u,o-8,c/536870912|0,c<<3),t(i,f,u,o),a=0;a<8;a++)n(e,8*a,i[a],f[a])}(r,e,e.length),r}},function(e){var r=function(){function e(r){if("function"==typeof TextDecoder)return new TextDecoder("utf-8",{fatal:!0}).decode(r);var n=8192,t=Math.ceil(r.length/n);if(t<=1)try{return decodeURIComponent(escape(String.fromCharCode.apply(null,r)))}catch(e){throw new TypeError("The encoded data was not valid.")}for(var o="",a=0,i=0;i<t;i++){var f=Array.prototype.slice.call(r,i*n+a,(i+1)*n+a);if(0!=f.length){var u,c=f.length,s=0;do{var _=f[--c];_>=240?(s=4,u=!0):_>=224?(s=3,u=!0):_>=192?(s=2,u=!0):_<128&&(s=1,u=!0)}while(!u);for(var p=s-(f.length-c),l=0;l<p;l++)a--,f.pop();o+=e(f)}}return o}return{from_base64:function(e){return"string"==typeof e?function(e,r){for(var n,t,o,a=e.replace(/[^A-Za-z0-9\+\/]/g,""),i=a.length,f=3*i+1>>2,u=new Uint8Array(f),c=0,s=0,_=0;_<i;_++)if(t=3&_,c|=((o=a.charCodeAt(_))>64&&o<91?o-65:o>96&&o<123?o-71:o>47&&o<58?o+4:43===o?62:47===o?63:0)<<18-6*t,3===t||i-_==1){for(n=0;n<3&&s<f;n++,s++)u[s]=c>>>(16>>>n&24)&255;c=0}return u}(e):e},from_base64url:function(e){return"string"==typeof e?this.from_base64(e.replace(/-/g,"+").replace(/_/g,"/")):e},from_hex:function(e){return"string"==typeof e?function(e){if(!function(e){return"string"==typeof e&&/^[0-9a-f]+$/i.test(e)&&e.length%2==0}(e))throw new TypeError("The provided string doesn't look like hex data");for(var r=new Uint8Array(e.length/2),n=0;n<e.length;n+=2)r[n>>>1]=parseInt(e.substr(n,2),16);return r}(e):e},from_string:function(e){return"string"==typeof e?function(e){if("function"==typeof TextEncoder)return new TextEncoder("utf-8").encode(e);e=unescape(encodeURIComponent(e));for(var r=new Uint8Array(e.length),n=0;n<e.length;n++)r[n]=e.charCodeAt(n);return r}(e):e},memcmp:function(e,r){if(!(e instanceof Uint8Array&&r instanceof Uint8Array))throw new TypeError("Only Uint8Array instances can be compared");if(e.length!==r.length)throw new TypeError("Only instances of identical length can be compared");for(var n=0,t=0,o=e.length;t<o;t++)n|=e[t]^r[t];return 0===n},memzero:function(e){e instanceof Uint8Array?function(e){if(!e instanceof Uint8Array)throw new TypeError("Only Uint8Array instances can be wiped");for(var r=0,n=e.length;r<n;r++)e[r]=0}(e):"undefined"!=typeof Buffer&&e instanceof Buffer&&e.fill(0)},to_base64:function(e){return"string"==typeof e?e:function(e,r){function n(e){return e<26?e+65:e<52?e+71:e<62?e-4:62===e?43:63===e?47:65}if(void 0===r&&(r=!0),"string"==typeof e)throw new Error("input has to be an array");for(var t=2,o="",a=e.length,i=0,f=0;f<a;f++)t=f%3,f>0&&4*f/3%76==0&&!r&&(o+="\r\n"),i|=e[f]<<(16>>>t&24),2!==t&&e.length-f!=1||(o+=String.fromCharCode(n(i>>>18&63),n(i>>>12&63),n(i>>>6&63),n(63&i)),i=0);return o.substr(0,o.length-2+t)+(2===t?"":1===t?"=":"==")}(e).replace(/\s+/g,"")},to_base64url:function(e){return this.to_base64(e).replace(/\+/g,"-").replace(/\//g,"_")},to_hex:function(e){return"string"==typeof e?e:function(e){for(var r,n,t,o="",a=0;a<e.length;a++)t=87+(n=15&e[a])+(n-10>>8&-39)<<8|87+(r=e[a]>>>4)+(r-10>>8&-39),o+=String.fromCharCode(255&t)+String.fromCharCode(t>>>8);return o}(e).replace(/\s+/g,"")},to_string:function(r){return"string"==typeof r?r:e(r)}}}();e.exports?(r.sodiumUtil=r,e.exports=r):self.sodiumUtil=r}],__webpack_module_cache__={};function __webpack_require__(e){var r=__webpack_module_cache__[e];if(void 0!==r)return r.exports;var n=__webpack_module_cache__[e]={exports:{}};return __webpack_modules__[e](n,n.exports,__webpack_require__),n.exports}var __webpack_exports__=__webpack_require__(0);fastSHA512=__webpack_exports__})(),"undefined"!=typeof module&&module.exports?module.exports=fastSHA512:self.fastSHA512=fastSHA512;