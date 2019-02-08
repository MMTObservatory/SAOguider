proc updateserverstat { var index op } {
    upvar $var S

    array set color {
        Up    green
        Down  red
        Hung  yellow
    }
    $S(statwindow) configure -text "$S(statlabel) $S(connection)" \
                             -bg    $color($S(connection))
}
 
proc msg_stat { w server label timeout updaterate } {
    upvar #0 $server S
    button $w -command ${label}_Restart 		\
		-justify l -anchor w -width [expr 5 + [string length $label]]
    set S(statlabel) $label
    set S(statwindow) $w
    msg_keepalive $server $timeout $updaterate
    trace variable S(connection) w updateserverstat
    updateserverstat S connection w
}

proc remote_start { server program } {
	exec rsh -n $server "$program restart < /dev/null >& /dev/null" < /dev/null > /dev/null 2>&1 &
}

proc restart { program } {
	exec $program restart
}

#proc Hctserv_Restart { } { remote_start hecto /sbin/hecto    }
