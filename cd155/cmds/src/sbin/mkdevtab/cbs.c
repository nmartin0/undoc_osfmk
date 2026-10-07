/*
 * 
 * $Copyright
 * Copyright 1993, 1994, 1995  Intel Corporation
 * INTEL CONFIDENTIAL
 * The technical data and computer software contained herein are subject
 * to the copyright notices; trademarks; and use and disclosure
 * restrictions identified in the file located in /etc/copyright on
 * this system.
 * Copyright$
 * 
 */
 


extern int debug;
extern int paragon_mesh_x;
extern int paragon_mesh_y;

root2cbs(root)
	int	root;
{
	int	x;
	int	y;
	int	cab;
	int	bp;
	int	slot;

	x = paragon_mesh_x - (root % paragon_mesh_x) - 1;
	y = paragon_mesh_y - (root / paragon_mesh_x) - 1;
	if (y < 0) {
#ifdef DEBUG
if (debug) printf("root2cbs(%d)  x %d  y %d  return -1\n", root, x, y);
#endif DEBUG
		return -1;
	}

	cab = x / 4;
	bp = y / 4;
	slot = "ahipbgjocfkndelm"[(y % 4)*4 + (x % 4)] - 'a';
#ifdef DEBUG
if (debug)
printf("root2cbs(%d)  x %d  y %d   cab %d  bp %d  slot %d   cbs %d\n",
	root, x, y, cab, bp, slot, (cab << 6) | (bp << 4) | slot);
#endif DEBUG

	return (cab << 6) | (bp << 4) | slot;
}

char	cbs_separator = '?';

cbs2root(cbs)
	int	cbs;
{
	int	cab;
	int	bp;
	int	slot;
	int	x;
	int	y;

	if (cbs == -1) {
		return -1;
	}

	cab = cbs >> 6;
	bp = (cbs >> 4) & 3;
	slot = cbs & 15;

	x = paragon_mesh_x - (cab * 4 + slot / 4) - 1;
	y = paragon_mesh_y - (bp * 4 + ("0123321001233210"[slot] - '0')) - 1;

	return	paragon_mesh_x * y + x;
}

char *
cbstos(cbs)
	int	cbs;
{
	static char s[16];
	int	cab;
	int	bp;
	int	slot;
	char	separator;

	if (cbs == -1) {
		return "***";
	}
	if (cbs_separator == '?') {
		separator = 'A';
	} else {
		separator = cbs_separator;
	}

	cab = cbs >> 6;
	bp = (cbs >> 4) & 3;
	slot = cbs & 15;

	if (separator >= 'A' && separator <= 'Z' ||
	    separator >= 'a' && separator <= 'z') {
		sprintf(s, "%d%c%d", cab, bp + separator, slot);
	} else {
		sprintf(s, "%d%c%d%c%d",
			cab, separator, bp, separator, slot);
	}
	return s;
}

iscbs(s)
	char	*s;
{
	char	separator;

	if (*s < '0' || *s > '9') {
		return 0;
	}
	while (*s >= '0' && *s <= '9') {
		s++;
	}
	if (*s == 0) {
		return 0;
	}

	if (cbs_separator == '?' &&
	    *s >= 'A' && *s <= 'Z') {
		separator = 'A';
	} else if (cbs_separator == '?' &&
	           *s >= 'a' && *s <= 'z') {
		separator = 'a';
	} else {
		separator = cbs_separator;
	}

	if (separator >= 'A' && separator <= 'Z' ||
	    separator >= 'a' && separator <= 'z') {
		s++;
		if (*s < '0' || *s > '9') {
			return 0;
		}

	} else {

		if (separator == '?') {
			s++;
		} else {
			if (*s++ != separator) {
				return 0;
			}
		}
		if (*s < '0' || *s > '9') {
			return 0;
		}
		while (*s >= '0' && *s <= '9') {
			s++;
		}
		if (*s == 0) {
			return 0;
		}
		if (separator == '?') {
			s++;
		} else {
			if (*s++ != separator) {
				return 0;
			}
		}
		if (*s < '0' || *s > '9') {
			return 0;
		}
	}

	return 1;
}

atocbs(s)
	char	*s;
{
	int	cab;
	int	bp;
	int	slot;
	char	separator;

	cab = 0;
	if (*s < '0' || *s > '9') {
		return -1;
	}
	while (*s >= '0' && *s <= '9') {
		cab = 10*cab + *s++ - '0';
	}
	if (*s == 0) {
		return -1;
	}

	if (cbs_separator == '?' &&
	    *s >= 'A' && *s <= 'Z') {
		separator = 'A';
	} else if (cbs_separator == '?' &&
	           *s >= 'a' && *s <= 'z') {
		separator = 'a';
	} else {
		separator = cbs_separator;
	}

	if (separator >= 'A' && separator <= 'Z' ||
	    separator >= 'a' && separator <= 'z') {
		bp = *s++ - separator;
		if (bp < 0 || bp >= 4) {
			return -1;
		}
		if (*s < '0' || *s > '9') {
			return -1;
		}
		slot = 0;
		while (*s >= '0' && *s <= '9') {
			slot = 10*slot + *s++ - '0';
		}

	} else {

		if (separator == '?') {
			s++;
		} else {
			if (*s++ != separator) {
				return -1;
			}
		}
		if (*s < '0' || *s > '9') {
			return -1;
		}
		bp = 0;
		while (*s >= '0' && *s <= '9') {
			bp = 10*bp + *s++ - '0';
		}
		if (*s == 0) {
			return -1;
		}
		if (separator == '?') {
			s++;
		} else {
			if (*s++ != separator) {
				return -1;
			}
		}
		if (*s < '0' || *s > '9') {
			return -1;
		}
		slot = 0;
		while (*s >= '0' && *s <= '9') {
			slot = 10*slot + *s++ - '0';
		}
	}

	return (cab << 6) | (bp << 4) | slot;
}
