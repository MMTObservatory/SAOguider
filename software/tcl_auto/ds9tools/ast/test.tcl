
load ./libast.so

update

for { } { 1 } { } {
	set here [iis query . root]
	update
	eval iis warp $here
	update
	puts $here
	after 1000
	update
}


