proc stripargs {arglist argkey} {
 
    if {[lsearch -exact $arglist "-help"] != -1} {
        foreach k $argkey {
            puts stderr $k
        }
        exit
    }
    
    foreach key $argkey {
        set flag [lindex $key 0]
        set index [lsearch -exact $arglist $flag]
        set index1 [expr $index + 1]
        if {$index >= 0} {
            set varname [lindex $key 1]
            upvar $varname var
            set var [lindex $arglist $index1]
            set arglist [lreplace $arglist $index $index1]
	}
    }
    return $arglist
}
 
proc saveargs {wname arglist} {
    upvar #0 $wname w
    foreach arg $arglist {
	set n [lindex $arg 0]
	set varname [lindex $arg 1]
	upvar $varname v
	set w($n) $v
    }
}

proc msg_checkbutton { args } {

    set w [lindex $args 0]

    # Get the allowed params for a checkbutton
    checkbutton $w
    set origargs [$w configure]
    destroy $w
    
    foreach arg $origargs {
	set a1 [lindex $arg 0]
	set a2 [string range $a1 1 end]
	lappend arglist [list $a1 $a2]
	set $a2 [lindex $arg 4]
    }
    
    set onlabel  "On "
    set offlabel "Off"
    lappend arglist {-server server}
    lappend arglist {-onlabel onlabel}
    lappend arglist {-offlabel offlabel}
    stripargs $args $arglist
    saveargs $w $arglist

    frame $w
    
    button $w.b -text $text -command "checkbutton_toggle $w" -pady $pady -padx $padx
    global $variable
    if {![info exists $variable]} {
	set $variable $offvalue
    }
    label  $w.l -text $offlabel
    grid $w.b $w.l
    msg_subscribe $server $variable $variable "checkbutton_update $w"
    return $w

}

proc checkbutton_update { w name index op } {
    upvar $name var
    upvar #0 $w W
    if { $var == $W(-offvalue) } {
	$w.l configure -text $W(-offlabel)
    } else {
	$w.l configure -text $W(-onlabel)
    }
}


proc checkbutton_toggle { w } {
    upvar $w W
    set value [$w.l cget -text]

    if { $value == $W(-offlabel) } {
	set value $W(-onvalue)
    } else  {
	set value $W(-offvalue)
    }
	
    set server $W(-server)
    set variable $W(-variable)
    msg_set $server $variable $value
    global $variable
    set $variable $value
}




