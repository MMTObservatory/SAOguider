
proc try { args } {
    if { [catch {
	uplevel "eval $args"
	exit 0
    } error] } {
	puts stderr $error
	exit 1
    }
}

