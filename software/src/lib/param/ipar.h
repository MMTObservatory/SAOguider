typedef union {
	ParamList	*plist;
	Parameter	*param;
	char		*stri;
} YYSTYPE;
#define	T_comment	258
#define	T_struct	259
#define	T_word	260
#define	T_line	261
#define	T_nl	262


extern YYSTYPE iparlval;
