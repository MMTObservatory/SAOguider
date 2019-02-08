
#include "putil.h"
#include "pfile.h"

char *getenv();

char *ExpandMacro();

char *lookupparam(name, pf)
	char	  *name;
	ParamFile  pf;
{
	int		 mode = HHMODE;
    return (char *) ParamGetX(pf, name, StringType, &mode);
}

ParamList *ParamArgsBase(pf, argv, argc)
	ParamFile	  pf;
	char		**argv;
	int	  	  argc;
{
	int		  n;
	char	 	 *pbase;
	FILE		 *db;

    pbase = getenv("PARAMRETR");

    if ( pbase && *pbase ) {
	char	*cmd;
	char	arg[1024];

	char *keys[] = { "pset", "key" };
	char *vals[2];
	vals[0] = pf->psets->psetname;
	vals[1] = &argv[0][1];

	cmd = ExpandMacro(pbase, keys, vals, 2, lookupparam, pf);
	db = popen(cmd, "r");

	n = fread(arg, 1, 1024, db);
	arg[n-1] = '\0';

	pclose(db);

	argv[0] = arg;

        return ParamFileArgs(pf,  argv, argc);
    } else {
	return ParamFileArgs(pf, &argv[1], argc -1);
    }
}

ParamDataBase(pf, id)
	ParamFile	 pf;
	char		*id;
{
	char	 	 *pbase;
	Parameter     	**params;
	FILE		 *db;

	int		 mode = HHMODE;

    pbase = getenv("PARAMSAVE");

    if ( pbase && *pbase ) {
	char	*cmd;
	char *keys[] = { "pset" };
	char *vals[1];
	vals[0] = pf->psets->psetname;

	cmd = ExpandMacro(pbase, keys, vals, 1, lookupparam, pf);

	db = popen(cmd, "w");

	for ( params = pf->psets->param; *params; params++ ) {
		    char	*str;

	    if ( !strcmp((*params)->pname, "mode") ) continue;
	    if ( !strcmp((*params)->pname, "#") ) continue;

	    str = (char *) ParamGetX(pf, (*params)->pname, StringType, &mode);
	    fprintf(db,   "%s	%s\n", (*params)->pname, str);
	}

	pclose(db);
    }
}
