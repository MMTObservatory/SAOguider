/* tclobj.h
 */


/* argc, argv interface
 */

#define TclCmdDel(data)		free(data)

#define TclCmdRetStr(interp, string, type, ret) 			\
    {									\
	Tcl_SetResult(interp, string, type);				\
	return ret;							\
    }

#define TclCmdErr(interp, string, type)					\
    TclCmdRetStr(interp, string, type, TCL_ERROR)

#define TclAppendRetStr(interp, result)					\
	{								\
		Tcl_AppendResult(interp, result, NULL);			\
	}

#define TclNArgs(interp, string, argc, n)				\
    if ( argc != n ) {							\
	TclAppendRetStr(interp, string); 				\
	return TCL_ERROR;						\
    } else

#define TclCmdNew(interp, name, object, command, delete, argc, argv, code) \
    if ( !strcmp("new", argv[1]) ) {					\
	object *name;							\
	if ( argc < 3 ) {						\
	    TclCmdErr(interp, "Too few arguments: "#name" new ..."	\
		, TCL_STATIC);						\
	}								\
        name = (object *) calloc(sizeof(object), 1);			\
									\
        Tcl_CreateCommand (interp, argv[2], command,			\
			 (ClientData) name,				\
			 (Tcl_CmdDeleteProc *) delete);			\
									\
	code								\
									\
	TclCmdRetStr(interp, argv[2], TCL_VOLITILE, TCL_OK)		\
    }


#define TclCmdSet(interp, inst, name, field, type, argc, argv		\
		, get, set) 						\
    if ( objc == 1 ) TclAppendRetStr(interp, "	" #inst " " name " ?value?\n")	\
    else								\
    if ( !strcmp(name, argv[1]) ) {					\
	/*  */ if ( argc == 2 ) {					\
		if ( 1 ) { get }					\
		return TCL_OK;						\
	} else if ( argc == 3 ) {					\
		if ( 1 ) { set }					\
		return TCL_OK;						\
	} else {							\
	    TclCmdErr(interp						\
		, "wrong number of args: "#inst" "#field" ..." 		\
		, TCL_STATIC);						\
	}								\
    }


#define TclCmdVXX(interp, inst, name, field, type, argc, argv, get, set)\
        TclCmdSet(interp, inst, name, field, type, argc, argv		\
  	    , get							\
	    , return Get_##type(interp, inst->field)			\
	    , if ( Set_##type(interp, argv[2], &inst->field) == TCL_OK ) {\
		set							\
	      } else return TCL_ERROR;					\
	)

#define TclCmdVX_(interp, inst, name, field, type, argc, argv, get)\
        TclCmdSet(interp, inst, name, field, type, argc, argv		\
	    , get							\
	      return Get_##type(interp, inst->field)			\
	    , return Set_##type(interp, argv[2], &inst->field);		\
	)

#define TclCmdV_X(interp, inst, name, field, type, argc, argv, set)\
        TclCmdSet(interp, inst, name, field, type, argc, argv		\
	    , return Get_##type(interp, inst->field);			\
	    , if ( Set_##type(interp, argv[2], &inst->field) == TCL_OK ) {\
		set							\
	      } else return TCL_ERROR;					\
	)

#define TclCmdVXO(interp, inst, name, field, type, argc, argv, get)  	\
        TclCmdSet(interp, inst, name, field, type, argc, argv		\
	    , get							\
	      return Get_##type(interp, inst->field);			\
	    , TclCmdErr(interp						\
		, "can't set "#inst" "#field": its read only" 		\
		, TCL_STATIC);						\
	)

#define TclCmdVRO(interp, inst, name, field, type, argc, argv)  	\
        TclCmdSet(interp, inst, name, field, type, argc, argv		\
	    , return Get_##type(interp, inst->field);			\
	    , TclCmdErr(interp						\
		, "can't set "#inst" "#field": its read only" 		\
		, TCL_STATIC);						\
	)

#define TclCmdV__(interp, inst, name, field, type, argc, argv)  	\
        TclCmdSet(interp, inst, name, field, type, argc, argv		\
	    , return Get_##type(interp, inst->field);			\
	    , return Set_##type(interp, argv[2], &inst->field);		\
	)

#define TclCmdCmd(interp, inst, name, args, expr, objc, objv, code)	\
    if ( objc == 1 )							\
    	    Tcl_AppendResult(interp, "	" #inst " ", name, " ", args, "\n", NULL); \
    else								\
    if ( !strcmp(name, Tcl_GetStringFromObj(objv[1], NULL)) ) {		\
	if ( !(expr) ) 							\
    	    Tcl_AppendResult(interp, "	" #inst " ", name, " ", args, "\n", NULL); \
	else {								\
	    code							\
	}								\
    }

/* objc, objv interface
 */

#define TclObjCmdDel(data)		free(data)

#define TclObjCmdRetObj(interp, obj, ret) 				\
    {									\
	Tcl_SetObjResult(interp, obj);					\
	return ret;							\
    }

#define TclObjCmdRetBool(interp, i, ret) 				\
    {									\
	Tcl_Obj *obj = Tcl_GetObjResult(interp);			\
									\
	Tcl_SetBooleanObj(obj, i);						\
	return ret;							\
    }

#define TclObjCmdRetInt(interp, i, ret) 				\
    {									\
	Tcl_Obj *obj = Tcl_GetObjResult(interp);			\
									\
	Tcl_SetIntObj(obj, i);						\
	return ret;							\
    }

#define TclObjCmdRetDbl(interp, dbl, ret) 				\
    {									\
	Tcl_Obj *obj = Tcl_GetObjResult(interp);			\
									\
	Tcl_SetDoubleObj(obj, dbl);					\
	return ret;							\
    }

#define TclObjCmdRetStr(interp, string, ret) 				\
    {									\
	Tcl_Obj *obj = Tcl_GetObjResult(interp);			\
									\
	Tcl_SetStringObj(obj, string, -1);				\
	return ret;							\
    }

#define TclObjCmdErr(interp, string)					\
    TclObjCmdRetStr(interp, string, TCL_ERROR)

#define TclObjCmdMinArgs(interp, name, argc, n) 			\
    if ( argc < n ) {							\
	TclObjCmdErr(interp, "Too few args: "#name); 			\
    }									\


#define TclObjCmdNew(interp, name, object, command, delete, objc, objv, code) 	\
    if ( !strcmp("new", Tcl_GetStringFromObj(objv[1], NULL)) ) {	\
	object *name;							\
        name = (object *) calloc(sizeof(object), 1);			\
									\
        Tcl_CreateObjCommand (interp, Tcl_GetStringFromObj(objv[2], NULL)\
			 , command					\
			 , (ClientData) name				\
			 , (Tcl_CmdDeleteProc *) delete);		\
									\
	code								\
									\
	TclObjCmdRetObj(interp, objv[2], TCL_OK);			\
    }


#define TclObjCmdSet(interp, inst, name, field, type, objc, objv	\
		, get, set) 						\
    if ( objc == 1 ) TclAppendRetStr(interp, "	" #inst " " name " ?value?\n")	\
    else								\
    if ( !strcmp(name, Tcl_GetStringFromObj(objv[1], NULL)) ) {		\
	/*  */ if ( objc == 2 ) {					\
		if ( 1 ) { get }					\
		return TCL_OK;						\
	} else if ( objc == 3 ) {					\
		if ( 1 ) { set }					\
		return TCL_OK;						\
	} else {							\
	    TclObjCmdErr(interp						\
		, "Too many args:" #inst #field " ...");		\
	}								\
    }


#define TclObjCmdVXX(interp, inst, name, field, type, objc, objv, get, set)\
        TclObjCmdSet(interp, inst, name, field, type, objc, objv	\
	    , get;							\
	      return GetObj_##type(interp, inst->field)			\
	    , if ( SetObj_##type(interp, objv[2], &inst->field) == TCL_OK ) {\
		set							\
	      } else return TCL_ERROR;					\
	)

#define TclObjCmdVX_(interp, inst, name, field, type, objc, objv, get)	\
        TclObjCmdSet(interp, inst, name, field, type, objc, objv	\
	    , get;							\
	      return GetObj_##type(interp, inst->field)			\
	    , return SetObj_##type(interp, objv[2], &inst->field);	\
	)

#define TclObjCmdV_X(interp, inst, name, field, type, objc, objv, set)	\
        TclObjCmdSet(interp, inst, name, field, type, objc, objv	\
	    , return GetObj_##type(interp, inst->field);		\
	    , if ( SetObj_##type(interp, objv[2], &inst->field) == TCL_OK ) {\
		set							\
	      } else return TCL_ERROR;					\
	)

#define TclObjCmdVRO(interp, inst, name, field, type, objc, objv)  	\
        TclObjCmdSet(interp, inst, name, field, type, objc, objv	\
	    , return GetObj_##type(interp, inst->field);		\
	    , TclObjCmdErr(interp					\
		, "can't set "#inst" "#field": its read only");		\
	)

#define TclObjCmdV__(interp, inst, name, field, type, objc, objv)  	\
        TclObjCmdSet(interp, inst, name, field, type, objc, objv		\
	    , return GetObj_##type(interp, inst->field);		\
	    , return SetObj_##type(interp, objv[2], &inst->field);	\
	)

#define TclObjCmdCmd(interp, inst, name, args, expr, objc, objv, code)	\
	   TclCmdCmd(interp, inst, name, args, expr, objc, objv, code)
