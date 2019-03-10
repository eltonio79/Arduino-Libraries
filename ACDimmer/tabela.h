//***********************************************************************************************************
/*
TABELA WARTOSCI PRZESUNIECIA MOMENTU ZALACZANIA TRIAKA WZGLEDEM ZERA SIECI
Tabela umieszczona w pami�ci programu (program space) za pomoc� dyrektywy const.
autor: Tomasz Sklenarski
e-mail: wy-konam@biznespoczta.pl
platforma: PIC16F690
taktowanie: 8MHz - wewnetrzny oscylator
srodowisko: MPLAB IDE v8.40 + kompilator C HI-TECH PICC 9.70 trial
*/
//***********************************************************************************************************
const int wspolczynnik[]= {
0xFFCB, 			//pelna moc 
0xFD2C,//2
0xFBC3,//3
0xFAB7,
0xF9D7,
0xF913,
0xF862,
0xF7C0,
0xF728,
0xF69A,//10
0xF613,
0xF593,
0xF518,
0xF4A3,
0xF431,
0xF3C3,
0xF359,
0xF2F2,
0xF28E,
0xF22C,//20
0xF1CC,
0xF16F,
0xF114,
0xF0BA,
0xF063,
0xF00D,
0xEFB8,
0xEF65,
0xEF14,
0xEEC3,//30
0xEE74,
0xEE26,
0xEDD9,
0xED8D,
0xED42,
0xECF8,
0xECAF,
0xEC67,
0xEC1F,
0xEBD8,//40
0xEB92,
0xEB4D,
0xEB08,
0xEAC5,
0xEA81,
0xEA3E,
0xE9FC,
0xE9BB,
0xE97A,
0xE939,//50
0xE8F9,
0xE8BA,
0xE87B,
0xE83C,
0xE7FE,
0xE7C0,
0xE783,
0xE746,
0xE709,
0xE6CD,//60
0xE691,
0xE655,
0xE61A,
0xE5DF,
0xE5A4,
0xE56A,
0xE530,
0xE4F6,
0xE4BD,
0xE484,//70
0xE44B,
0xE412,
0xE3DA,
0xE3A2,
0xE36A,
0xE332,
0xE2FA,
0xE2C3,
0xE28C,
0xE255,//80
0xE21E,
0xE1E8,
0xE1B1,
0xE17B,
0xE145,
0xE10F,
0xE0D9,
0xE0A4,
0xE06E,
0xE039,//90
0xE004,
0xDFCF,
0xDF9A,
0xDF65,
0xDF30,
0xDEFC,
0xDEC7,
0xDE93,
0xDE5F,
0xDE2B,//100
0xDDF7,
0xDDC3,
0xDD8F,
0xDD5B,
0xDD27,
0xDCF3,
0xDCC0,
0xDC8C,
0xDC59,
0xDC25,//110
0xDBF2,
0xDBBF,
0xDB8B,
0xDB58,
0xDB25,
0xDAF2,
0xDABF,
0xDA8C,
0xDA58,
0xDA25,//120
0xD9F2,
0xD9BF,
0xD98C,
0xD959,
0xD926,
0xD8F3,
0xD8C1,
0xD88E,
0xD85B,
0xD828,//130
0xD7F5,
0xD7C2,
0xD78F,
0xD75B,
0xD728,
0xD6F5,
0xD6C2,
0xD68F,
0xD65C,
0xD628,//140
0xD5F5,
0xD5C2,
0xD58E,
0xD55B,
0xD527,
0xD4F4,
0xD4C0,
0xD48C,
0xD459,
0xD425,//150
0xD3F1,
0xD3BD,
0xD388,
0xD354,
0xD320,
0xD2EB,
0xD2B7,
0xD282,
0xD24D,
0xD219,//160
0xD1E4,
0xD1AE,
0xD179,
0xD144,
0xD10E,
0xD0D8,
0xD0A3,
0xD06D,
0xD036,
0xD000,//170
0xCFC9,
0xCF93,
0xCF5C,
0xCF25,
0xCEED,
0xCEB6,
0xCE7E,
0xCE46,
0xCE0E,
0xCDD6,//180
0xCD9D,
0xCD64,
0xCD2B,
0xCCF2,
0xCCB8,
0xCC7E,
0xCC44,
0xCC09,
0xCBCF,
0xCB93,//190
0xCB58,
0xCB1C,
0xCAE0,
0xCAA3,
0xCA66,
0xCA29,
0xC9EB,
0xC9AD,
0xC96F,
0xC930,//200
0xC8F0,
0xC8B0,
0xC870,
0xC82F,
0xC7EE,
0xC7AC,
0xC769,
0xC726,
0xC6E2,
0xC69D,//210
0xC658,
0xC612,
0xC5CC,
0xC584,
0xC53C,
0xC4F3,
0xC4A9,
0xC45F,
0xC413,
0xC3C6,//220
0xC378,
0xC329,
0xC2D9,
0xC288,
0xC235,
0xC1E1,
0xC18B,
0xC134,
0xC0DB,
0xC080,//230
0xC023,
0xBFC4,
0xBF63,
0xBEFF,
0xBE98,
0xBE2F,
0xBDC2,
0xBD51,
0xBCDC,
0xBC62,//240
0xBBE3,
0xBB5E,
0xBAD1,
0xBA3C,
0xB99C,
0xB8EE,
0xB82E,
0xB755,
0xB653,
0xB505	//250		//minimalna moc
};