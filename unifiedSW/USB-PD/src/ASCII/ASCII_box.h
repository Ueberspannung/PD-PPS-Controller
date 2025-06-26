#ifndef __ASCII_BOX_CHARS_H__
#define __ASCII_BOX_CHARS_H__

/*
 * nameing:
 * H: 	Horizontal line
 * V:	Veritcal line
 * I:	intersecting lines
 * TL:	top left corner
 * TR:	top right corner
 * BL:	bottom left corner
 * BR:	bottom right corner
 * VL:	vertical line with tap, left egde	
 * VR:	vertical line with tap, right edge	
 * HT:	horizontal line with tap, top edge
 * HB:	horizontal line with tap, bottom edge
 * 
 * Example:
 * 
 * 	TL - H - HT - H - TR
 *	 !		  !		   !
 * 	V         V        V
 *	 !		  !		   !
 * 	VL - H - I  - H - VR
 *	 !		  !		   !
 * 	V         V        V
 *	 !		  !		   !
 * 	BL - H - HB - H - BR
 * 
 * S: dingle line
 * D: double line
 * 
 * 
 * *********************************************************
 * Windows Charset: CP437 (DOS Charset)
 * *********************************************************
 */
 

#define BOX_HS		(196)		// single line, horizontal 
#define BOX_VS		(179)		// single line, vertical 
#define BOX_IS		(197)		// single line, interescting 
#define BOX_TLS		(218)		// single line, top left corner
#define BOX_TRS		(191)		// single line, top right corner
#define BOX_BLS		(192)		// single line, bottom left corner	
#define BOX_BRS 	(217)		// single line, bottom right corner	
#define BOX_VLS 	(195)		// single vertical line with tep, lrft egde
#define BOX_VRS 	(180)		// single vertical line with tap, right edge	
#define BOX_HTS 	(194)		// single horizontal line with tap, top edge
#define BOX_HBS 	(193)		// single horizontal line with tap, bottom edge
#define BOX_HD		(205)		// double line, horizontal 
#define BOX_VD		(186)		// double line, vertical 
#define BOX_ID		(206)		// double line, intersecting 
#define BOX_TLD		(201)		// double line, top left corner
#define BOX_TRD		(187)		// double line, top right corner
#define BOX_BLD		(200)		// double line, bottom left corner	
#define BOX_BRD 	(188)		// double line, bottom right corner	
#define BOX_VLD 	(204)		// double vertical line with tep, lrft egde
#define BOX_VRD 	(185)		// double vertical line with tap, right edge	
#define BOX_HTD 	(203)		// double horizontal line with tap, top edge
#define BOX_HBD 	(202)		// double horizontal line with tap, bottom edge
#define BOX_TLHSVD	(214)		// top left corner, horizontal single, verticle doubele
#define BOX_TRHSVD	(183)		// top right corner, horizontal single, verticle doubele
#define BOX_BLHSVD	(211)		// top left corner, horizontal single, verticle doubele
#define BOX_BRHSVD	(189)      	// top right corner, horizontal single, verticle doubele
#define BOX_TLHDVS	(213)		// top left corner, horizontal doubele, verticle single, 
#define BOX_TRHDVS	(184)		// top right corner, horizontal doubele, verticle single,
#define BOX_BLHDVS	(212)		// top left corner, horizontal doubele, verticle single, 
#define BOX_BRHDVS	(190)      	// top right corner, horizontal doubele, verticle single,
#define BOX_VSLD 	(198)		// single vertical line with double line tap, left egde
#define BOX_VSRD 	(181)		// single vertical line with double line tap, right edge	
#define BOX_HSTD 	(210)		// single horizontal line with double line tap, top edge
#define BOX_HSBD 	(208)		// single horizontal line with double line tap, bottom edge
#define BOX_VDLS 	(199)		// double vertical line with single line tap, lrft egde
#define BOX_VDRS 	(182)		// double vertical line with single line tap, right edge	
#define BOX_HDTS 	(209)		// double horizontal line with single line tap, top edge
#define BOX_HDBS 	(207)		// double horizontal line with single line tap, bottom edge
#define BOX_IHSVD	(215)		// horizontal single line intersecting with vertical double line
#define BOX_IHDVS	(216)		// horizontal double line intersecting with vertical single line


#endif // __ASCII_BOX_CHARS_H__
