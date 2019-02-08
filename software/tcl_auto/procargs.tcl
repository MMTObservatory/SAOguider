
interp alias {} _proc {} proc

proc _procargs { __name __params __args } {
    set __nargs [llength $__args]
    set __npars [llength $__params]

    # Set the defaults
    #
    foreach __par $__params {
	set __param	[lindex $__par 0]
	set __value	[lindex $__par 1]

	if { ![string compare $__par args] } {
	    set __acat 1
	    break
	}

	upvar $__param $__param
	set   $__param $__value

	lappend __paramnames $__param
    }

    # Set the args
    #
    set __p   0
    set __eoo 0
    for { set __i 0 } { $__i < $__nargs } { incr __i } {
	set __arg [lindex $__args $__i]

	if { $__eoo } {
	    set __pnum $__p
	    incr __p
	} else {
	    switch -regexp -- $__arg {
	     ^$		-
	     ^-[0-9.]+$ -
	     ^[^-].*$  {
		set __pnum $__p
		incr __p
	     }
	     default {
		if { ![string compare $__arg "--"] } {
		    set __eoo 1
		    continue
		}
		set __pnum [lsearch $__paramnames [string range $__arg 1 end]]
		if { $__pnum < 0 } {
		    error "$name: no option $__arg: must be one of: $__paramnames"
		}
		set __arg [lindex $__args [incr __i]]
	     }
	    }
	}

	set __param [lindex $__paramnames $__pnum]
	set $__param $__arg
    }
}

proc  procargs { name { params {} } { body {} } } {
	#_procargs procargs { name params body } $args

    if { ![string compare $name proc] } {
	rename _proc	 {}
	rename  proc	 _proc
	rename  procargs  proc

	return
    }

    if { [llength [info commands $name]] && ![string compare $body {}] } {
	set args [info args $name]
	set body [info body $name]

	# rebuild params
	#
	foreach arg $args {
	    if { [info default $name $arg value] } {
		lappend params "$arg $value"
	    } else {
		lappend params "$arg"
	    }
	}
    }

    _proc $name { args } "_procargs $name [list $params] \$args; $body"
}
