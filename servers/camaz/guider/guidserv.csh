#!/bin/csh
if ("`printenv SAOGUIDER_ROOT`" == "") then
    setenv SAOGUIDER_ROOT /mmt/saoguider
endif
source $SAOGUIDER_ROOT/servers/camaz/servers.csh
setenv inst az
setenv MSGINITDIR $SAOGUIDER_ROOT/servers/camaz/guider
exec $SAOGUIDER_ROOT/servers/camaz/guider/bin$TARGET_ARCH/guidserv
