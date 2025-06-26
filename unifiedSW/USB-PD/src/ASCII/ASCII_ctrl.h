#ifndef __ASCII_CTRL_H__
#define __ASCII_CTRL_H__

/* *********************************************
 * ASCII control characters 0x00-0x1f and 0x7f
 */
 																				
//		Name 	Value		description

#define	NUL		(0x00)	// null
#define	SOH		(0x01)	// (^A) start of heading
#define	STX		(0x02)	// (^B) start of text
#define	ETX		(0x03)	// (^C) end of text
#define	EOT		(0x04)	// (^D) end of transmission
#define	ENQ		(0x05)	// (^E) enquiry
#define	ACK		(0x06)	// (^F) acknowledge
#define	BEL		(0x07)	// (^G) bell
#define	BS		(0x08)	// (^H) backspace
#define	HT		(0x09)	// (^I) horizontal tabulation
#define	LF		(0x0a)	// (^J) line feed
#define VT		(0x0b)	// (^K) vertical tabulation
#define	FF		(0x0c)	// (^L) form feed
#define	CR		(0x0d)	// (^M) carriage return
#define	SO		(0x0e)	// (^N) shift out
#define	SI		(0x0f)	// (^O) shift in
#define	DLE		(0x10)	// (^P) data link escape
#define	DC1		(0x11)	// (^Q) device control 1 (X-ON)
#define	DC2		(0x12)	// (^R) device control 2 (tape)
#define	DC3		(0x13)	// (^S) device control 3 (X-OFF)
#define	DC4		(0x14)	// (^T) device control 4 (tape)
#define NAK		(0x15)	// (^U) negative acknowledge
#define SYN		(0x16)	// (^V) synchronous idle
#define ETB		(0x17)	// (^W) end of transmission block
#define CAN		(0x18)	// (^X) cancel
#define	EM		(0x19)	// (^Y) end of medium
#define	SUB		(0x1a)	// (^Z) substitute
#define	CPM_EOF	(0x1a)	// (^Z) end of file CPM / DOS / UNIX
#define	ESC		(0x1b)	// escape
#define	FS		(0x1c)	// file separator
#define	GS		(0x1d)	// group separator
#define	RS		(0x1e)	// record separator
#define US		(0x1f)	// unit separator
#define SPACE   (0x20)	// ASCII char Space
				
#define DEL		(0x7f)	// delete (robout)


#endif // __ASCII_CTRL_H__
