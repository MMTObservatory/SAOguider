#!/bin/csh
if ("`printenv SAOGUIDER_ROOT`" == "") then
    setenv SAOGUIDER_ROOT /mmt/saoguider
endif
source $SAOGUIDER_ROOT/servers/videoserver/servers.csh
setenv inst oldiccd85sm
grep '^[a-z]' $SAOGUIDER_ROOT/guis/iccd85/guidserv.unique.rc | cut -d'#' -f1 | cat -n | nc `echo $GUIDSERV | tr ":" " "` > /dev/null
setenv MSGINITDIR $SAOGUIDER_ROOT/guis/iccd85
/bin/rm -f ~/.guidegui;sleep 2
exec $SAOGUIDER_ROOT/software/bin$TARGET_ARCH/guidegui -xpa no -port 0 -unix none -fifo none -source $SAOGUIDER_ROOT/software/tcl/guidegui.tcl
