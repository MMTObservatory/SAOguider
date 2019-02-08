
proc cal { mjd { format "%b %e %T %Y" } } {
	return [_cal $mjd $format]
}

