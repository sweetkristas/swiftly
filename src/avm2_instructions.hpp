#pragma once

namespace avm2
{
	enum instructions {
		add				= 160,
		add_i			= 197,
		astype			= 134,
		astypelate		= 135,
		bitand			= 168,
		bitnot			= 151,
		bitor			= 169,
		bitxor			= 170,
		call			= 65,
		callmethod		= 67,
		callproperty	= 70,
		callproplex		= 76,
		callpropvoid	= 79,
		callstatic		= 68,
		callsuper		= 69,
		callsupervoid	= 78,
		checkfilter		= 120,
		coerce			= 128,
		coerce_a		= 130,
		coerce_s		= 133,
		construct		= 66,
		constructprop	= 74,
		constructsuper	= 73,
		convert_b		= 118,
		convert_i		= 115,
		convert_d		= 117,
		convert_o		= 119,
		convert_u		= 116,
		convert_s		= 112,
		debug			= 239,
		debugfile		= 241,
		debugline		= 240,
		declocal		= 148,
		declocal_i		= 195,
		decrement		= 147,
		decrement_i		= 193,
		deleteproperty	= 106,
		divide			= 163,
		dup				= 42,
		dxns			= 6,
		dxnslate		= 7,
		equals			= 171,
		esc_xattr		= 114,
		esc_xelem		= 113,
		findproperty	= 94,
		findpropstrict  = 93,
		getdescendents  = 89,
		getglobalscope	= 100,
		getglobalslot	= 110,
		getlex			= 96,
		getlocal		= 98,
		getlocal_0		= 208,
		getlocal_1		= 209,
		getlocal_2		= 210,
		getlocal_3		= 211,
		getproperty		= 102,
		getscopeobject	= 101,
		getslot			= 108,
		getsuper		= 4,
		greaterequals	= 176,
		greaterthan		= 175,
		hasnext			= 31,
		hasnext2		= 50,
		ifeq			= 19,
		iffalse			= 18,
		ifge			= 24,
		ifgt			= 23,
		ifle			= 22,
		iflt			= 21,
		ifnge			= 15,
		ifngt			= 14,
		ifnle			= 13,
		ifnlt			= 12,
		ifne			= 20,
		ifstricteq		= 24,
		ifstrictne		= 25,
		iftrue			= 17,
		in				= 180,
		inclocal		= 146,
		inclocal_i		= 194,
		increment		= 145,
		increment_i		= 192,
		initproperty	= 104,
		instanceof		= 177,
		istype			= 178,
		istypelate		= 179,
		jump			= 16,
		kill			= 8,
		label			= 9,
		lessequals		= 174,
		lessthan		= 173,
		lookupswitch	= 27,
		lshift			= 165,
		modulo			= 164,
		multiply		= 162,
		multiply_i		= 199,
		negate			= 144,
		negate_i		= 196,
		newactivation	= 87,
		newarray		= 86,
		newcatch		= 90,
		newclass		= 88,
		newfunction		= 64,
		newobject		= 85,
		nextname		= 30,
		nextvalue		= 35,
		nop				= 2,
		not				= 150,
		pop				= 41,
		popscope		= 29,
		pushbyte		= 36,
		pushdouble		= 46,
		pushfalse		= 39,
		pushint			= 45,
		pushnamespace	= 49,
		pushnan			= 40,
		pushnull		= 32,
		pushscope		= 48,
		pushshort		= 37,
		pushstring		= 44,
		pushtrue		= 38,
		pushuint		= 46,
		pushundefined	= 33,
		pushwith		= 28,
		returnvalue		= 72,
		returnvoid		= 71,
		rshift			= 166,
		setlocal		= 99,
		setlocal_0		= 212,
		setlocal_1		= 213,
		setlocal_2		= 214,
		setlocal_3		= 215,
		setglobalslot	= 111,
		setproperty		= 97,
		setslot			= 109,
		setsuper		= 5,
		strictequals	= 172,
		subtract		= 161,
		subtract_i		= 198,
		swap			= 43,
		throw_ex		= 3,
		typeof			= 149,
		urshift			= 167,
	};
}
