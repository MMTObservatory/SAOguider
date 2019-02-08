# Labeled Entry Thingy
#
set EntryValue  0
set EntryWidget .


# Change the case of the first letter of the variable name to lowercase 
#  to use as the  widget name
proc entFixWidgetName { w } {
    set var [lindex [split $w .] end]
    set wlist [split $w .]
    set lvar "[string tolower [string range $var 0 0]][string range $var 1 end]"
    set wlist [lreplace $wlist end end $lvar]
    set w [join $wlist .]
}

proc retcopy { name indx op } {
    upvar $name value
    global ${name}.buffer
    set ${name}.buffer $value
}

proc retentry { w args } { 
    set var [lindex [split $w .] end]
    set w [entFixWidgetName $w]
    global ${w}-varname
    set ${w}-varname $var

    global $var ${var}.buffer
 
    catch {
	upvar #0 $var value
	set ${var}.buffer $value
    }

    trace variable ${var} w { retcopy }
    
    eval entry $w -textvariable ${var}.buffer -justify right $args

    # This forces a trace on the return key
    #
    bind $w <Key-Return> {
	set ${%W-varname} [%W get]
    }
    bind $w <FocusOut> {

	upvar #0 ${%W-varname}        val
	upvar #0 ${%W-varname}.buffer buf

	if { [string compare $val $buf] != 0 } {
	    %W delete 0 end
	    %W insert 0 $val
	}
    }

    return $w
}

proc msgentry { server lab w { def 0 } { init {} } { code {} } { timeout {} } { sync {} } args } {
    set var [lindex [split $w .] end]
    set w [entFixWidgetName $w]

    msg_variable $server $var $var w $def $init $code $timeout $sync

    label ${w}_l -text $lab -anchor w
    set predash ""
    set pstdash ""
    while { [lindex $args 0] == "-"  } {
	set args [lrange $args 1 end]
	set predash "$predash -"
    }
    while { [lindex $args 0] == "+"  } {
	set args [lrange $args 1 end]
	set pstdash "$pstdash -"
    }
    
    eval retentry $w $args

    return "${w}_l $predash $w $pstdash"
}

proc msgentry_async { server lab w { def 0 } { init {} } { code {} } { timeout {} } { sync {} } args } {
    set var [lindex [split $w .] end]
    set neww [entFixWidgetName $w]
 
    msg_variable $server $var $var w $def $init $code $timeout $sync
 
    label ${neww}_l -text $lab -anchor w
    set predash ""
    set pstdash ""
    while { [lindex $args 0] == "-"  } {
        set args [lrange $args 1 end]
        set predash "$predash -"
    }
    while { [lindex $args 0] == "+"  } {
        set args [lrange $args 1 end]
        set pstdash "$pstdash -"
    }
    
    eval retentry $w $args

    return "${neww}_l $predash $neww $pstdash"
}

proc labentry { lab w args } {
    set var [lindex [split $w .] end]
    set w [entFixWidgetName $w]

    global $var

    label ${w}_l -text $lab -anchor w
    set predash ""
    set pstdash ""
    while { [lindex $args 0] == "-"  } {
	set args [lrange $args 1 end]
	set predash "$predash -"
    }
    while { [lindex $args 0] == "+"  } {
	set args [lrange $args 1 end]
	set pstdash "$pstdash -"
    }

    eval entry $w -textvariable $var -justify right $args

    return "${w}_l $predash $w $pstdash"
}

