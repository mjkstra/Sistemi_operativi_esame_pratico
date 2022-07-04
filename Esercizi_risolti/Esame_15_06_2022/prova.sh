
if [[ -e ${DEST} ]]; then
		cd ${DEST} 
		gcc main.c -o coda
	else 
		echo "?ERROR" >&2
	fi