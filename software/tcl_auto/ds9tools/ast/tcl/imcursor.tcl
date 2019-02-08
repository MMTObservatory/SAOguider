
global CursorMode
set CursorMode(in) 0

bind CursorMode <Button-1> { CursorVal %x %y "" }
bind CursorMode <KeyPress> { CursorVal %x %y %A }

proc CursorMode { onoff { callback {} } } {
	global CursorMode canvas

    if { $CursorMode(in) == $onoff } { return }

    set CursorMode(in) $onoff

    if { $onoff == 1 } {
        set CursorMode(callback) $callback
	set CursorMode(cursor) [$canvas(name) cget -cursor]
	set CursorMode(bindtg) [bindtags $canvas(name)]

	bindtags $canvas(name) "CursorMode $CursorMode(bindtg)"
	$canvas(name) configure -cursor target
	focus $canvas(name)
    } else {
	$canvas(name) configure -cursor $CursorMode(cursor)
	bindtags $canvas(name) $CursorMode(bindtg)
    }
}

proc CursorVal { x y key } {
	global CursorMode

    if { [string compare $key {}] } {
	eval $CursorMode(callback) $x $y $key
	CursorMode 0
    }
}

