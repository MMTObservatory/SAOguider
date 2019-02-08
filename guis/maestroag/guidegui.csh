#!/bin/csh
if ("`printenv SAOGUIDER_ROOT`" == "") then
    setenv SAOGUIDER_ROOT /mmt/saoguider
endif
source $SAOGUIDER_ROOT/servers/camaz/servers.csh
if ($SAOGUIDER_ROOT == /mmt/saoguider) then
    setenv CCDHOST `/mmt/bin/mmtservdata maestroguider-ccdhost | tr " " ":"`
else
    setenv CCDHOST 127.0.0.1:2410
endif
setenv inst az
grep '^[a-z]' $SAOGUIDER_ROOT/guis/maestroag/guidserv.unique.rc | cut -d'#' -f1 | cat -n | nc `echo $GUIDSERV | tr ":" " "` > /dev/null
echo "1 sethost `echo $CCDHOST | cut -d: -f1`" | nc `echo $GUIDECAM | tr ":" " "` > /dev/null
echo "1 setport `echo $CCDHOST | cut -d: -f2`" | nc `echo $GUIDECAM | tr ":" " "` > /dev/null
setenv MSGINITDIR $SAOGUIDER_ROOT/guis/maestroag
/bin/rm -f ~/.guidegui;sleep 2
exec $SAOGUIDER_ROOT/software/bin$TARGET_ARCH/guidegui -xpa no -port 0 -unix none -fifo none -source $SAOGUIDER_ROOT/software/tcl/guidegui.tcl
