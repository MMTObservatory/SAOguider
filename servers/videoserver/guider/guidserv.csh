#!/bin/csh
if ("`printenv SAOGUIDER_ROOT`" == "") then
    setenv SAOGUIDER_ROOT /mmt/saoguider
endif
source $SAOGUIDER_ROOT/servers/videoserver/servers.csh
setenv inst stellacam
setenv MSGINITDIR $SAOGUIDER_ROOT/servers/videoserver/guider
exec $SAOGUIDER_ROOT/servers/videoserver/guider/bin$TARGET_ARCH/guidserv
