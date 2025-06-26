#ifndef __MESSAGES_H__
#define __MESSAGES_H__

#define Stringize( L )     #L 
#define MakeString( M, L ) M(L)
#define $Line MakeString( Stringize, __LINE__ )

#define DO_PRAGMA(x) _Pragma (#x)
#define TODO(x) DO_PRAGMA(message (__FILE__ "(" $Line ") TODO - " #x))


#endif // __MESSAGES_H__
