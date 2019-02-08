proc srvinit { who what } {
    set host [string toupper [lindex [split $::env([string toupper $who]) :] 0]]

    # puts "who: $who what: $what host: $host"
    if { ![info exists ::$host] } {
        set ::env($host)         [string tolower $host]:7500
        msg_client $host
    } else {
        # puts "srvinit already open host: $host"
    }

    msg_cmd $host "$who $what" 60000
}



