if ($SAOGUIDER_ROOT == /mmt/saoguider) then
    setenv GUIDECAMAZ `/mmt/bin/mmtservdata camera-camaz | tr " " ":"`
    setenv GUIDSERV `/mmt/bin/mmtservdata guider-camaz | tr " " ":"`
    setenv TELESCOPE `/mmt/bin/mmtservdata telserver | tr " " ":"`
else
# testing
    setenv GUIDECAMAZ 127.0.0.1:3008
    setenv GUIDSERV 127.0.0.1:3001
    setenv TELESCOPE 127.0.0.1:5403
endif
setenv GUIDECAM $GUIDECAMAZ
setenv FOCUS $TELESCOPE;setenv GUIDE $GUIDSERV;unsetenv UPARM;unsetenv MSGDEBUG
source $SAOGUIDER_ROOT/servers/run_arch.csh
