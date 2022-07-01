	return {
		from_base64: function (data) {
			return typeof data === 'string' ?
				from_base64(data) :
				data
			;
		},
		from_base64url: function (data) {
			return typeof data === 'string' ?
				this.from_base64(data.replace(/-/g, '+').replace(/_/g, '/')) :
				data
			;
		},
		from_hex: function (data) {
			return typeof data === 'string' ?
				from_hex(data) :
				data
			;
		},
		from_string: function (message) {
			return typeof message === 'string' ?
				from_string(message) :
				message
			;
		},
		memcmp: memcmp,
		memzero: function (data) {
			if (data instanceof Uint8Array) {
				memzero(data);
			}
			else if (typeof Buffer !== 'undefined' && data instanceof Buffer) {
				data.fill(0);
			}
		},
		to_base64: function (data) {
			return typeof data === 'string' ?
				data :
				to_base64(data).replace(/\s+/g, '')
			;
		},
		to_base64url: function (data) {
			return this.to_base64(data).replace(/\+/g, '-').replace(/\//g, '_');
		},
		to_hex: function (data) {
			return typeof data === 'string' ?
				data :
				to_hex(data).replace(/\s+/g, '')
			;
		},
		to_string: function (message) {
			return typeof message === 'string' ?
				message :
				to_string(message)
			;
		}
	};
}());


if (typeof module !== 'undefined' && module.exports) {
	sodiumUtil.sodiumUtil	= sodiumUtil;
	module.exports			= sodiumUtil;
}
else {
	self.sodiumUtil			= sodiumUtil;
}
