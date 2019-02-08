#!/bin/csh
if ("`printenv SAOGUIDER_ROOT`" == "") then
    setenv SAOGUIDER_ROOT /mmt/saoguider
endif
source $SAOGUIDER_ROOT/servers/videoserver/servers.csh
setenv inst f5wfs_stella
grep '^[a-z]' $SAOGUIDER_ROOT/guis/f5wfs/guidserv.unique.rc | cut -d'#' -f1 | cat -n | nc `echo $GUIDSERV | tr ":" " "` > /dev/null
setenv MSGINITDIR $SAOGUIDER_ROOT/guis/f5wfs
/bin/rm -f ~/.guidegui;sleep 2
exec $SAOGUIDER_ROOT/software/bin$TARGET_ARCH/guidegui -xpa no -port 0 -unix none -fifo none -source $SAOGUIDER_ROOT/software/tcl/guidegui.tcl
