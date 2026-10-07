include ../Config.mk
include ../Bld/include/StdDefines.mk
include ../Bld/include/OS.mk
include ../Bld/include/Path.mk
UUCP = Devices Dialcodes Dialers Permissions Poll Systems

BIN = Maxuuscheds Maxuuxqts ct cu lock login newgrp passwd  \
      remote.unknown su uucp uulog uuname uupick uusend uuto uux

LBIN = makekey uucheck uucico uucleanup uudemon.admin uudemon.cleanu \
       uudemon.hour uudemon.poll uusched uustat uuxqt

NLS = ftpd.cat lock.cat login.cat newgrp.cat passwd.cat rexecd.cat su.cat \
      uucp.cat uudecode.cat uuencode.cat uusend.cat

SBIN = ftpd rexecd uucpd

prebuilt: $(UUCP) $(BIN) $(LBIN) $(NLS) $(SBIN)
install: prebuilt
	@echo "Installing Prebuilt commands"
	@[ -d $(ParagonExportsArea)/usr/lib/nls/msg/en_US.88591 ] || mkdir -p $(ParagonExportsArea)/usr/lib/nls/msg/en_US.88591
	@[ -d $(ParagonExportsArea)/etc/uucp ] || mkdir -p $(ParagonExportsArea)/etc/uucp
	@$(Chown) uucp $(ParagonExportsArea)/etc/uucp
	@chgrp uucp $(ParagonExportsArea)/etc/uucp

#
	@for i in $(NLS) ; do \
	   echo "[usr/lib/nls/msg/en_US.88591/$$i]" ; \
	   if [ ! -f $(ParagonExportsArea)/usr/lib/nls/msg/en_US.88591/$$i ]; then \
	      cp $$i $(ParagonExportsArea)/usr/lib/nls/msg/en_US.88591/$$i ;\
	      $(Chown) 0.0 $(ParagonExportsArea)/usr/lib/nls/msg/en_US.88591/$$i ;\
	      chmod 0644 $(ParagonExportsArea)/usr/lib/nls/msg/en_US.88591/$$i ;\
	   fi; \
	done
#
	@echo "[usr/lbin/makekey]"
	@if [ ! -f $(ParagonExportsArea)/usr/lbin/makekey ]; then \
	    cp makekey $(ParagonExportsArea)/usr/lbin/makekey ; \
	    $(Chown) 0.0 $(ParagonExportsArea)/usr/lbin/makekey ; \
	    chmod 0755 $(ParagonExportsArea)/usr/lbin/makekey ; \
	fi
#
	@for i in login newgrp passwd su ; do \
	   echo "[usr/bin/$$i]" ; \
	   if [ ! -f $(ParagonExportsArea)/usr/bin/$$i ]; then \
	      cp $$i $(ParagonExportsArea)/usr/bin/$$i ; \
	      $(Chown) 0.0 $(ParagonExportsArea)/usr/bin/$$i ; \
	      chmod 4711 $(ParagonExportsArea)/usr/bin/$$i ; \
	   fi ; \
	done
	@if [ ! -f $(ParagonExportsArea)/usr/bin/chfn ]; then  \
	   ln $(ParagonExportsArea)/usr/bin/passwd $(ParagonExportsArea)/usr/bin/chfn ; \
	fi
	@if [ ! -f $(ParagonExportsArea)/usr/bin/chsh ]; then  \
	   ln $(ParagonExportsArea)/usr/bin/passwd $(ParagonExportsArea)/usr/bin/chsh ; \
	fi
#
	@for i in ct cu uucp uuname uusend uux uuto ; do \
	   echo "[usr/bin/$$i]" ; \
	   if [ ! -f $(ParagonExportsArea)/usr/bin/$$i ]; then \
	      cp $$i $(ParagonExportsArea)/usr/bin/$$i ; \
	      $(Chown) 2.2 $(ParagonExportsArea)/usr/bin/$$i ; \
	      chmod 4755 $(ParagonExportsArea)/usr/bin/$$i ; \
	   fi ; \
	done
#
	@echo "[usr/bin/lock]"
	@if [ ! -f $(ParagonExportsArea)/usr/bin/lock ]; then \
	   cp lock $(ParagonExportsArea)/usr/bin/lock ; \
	   $(Chown) 0.0 $(ParagonExportsArea)/usr/bin/lock ; \
	   chmod 0755 $(ParagonExportsArea)/usr/bin/lock ; \
	fi
#
	@for i in uucheck uucleanup ; do \
	   echo "[usr/lbin/$$i]" ; \
	   if [ ! -f $(ParagonExportsArea)/usr/lbin/$$i ]; then \
	      cp $$i $(ParagonExportsArea)/usr/lbin/$$i ; \
	      $(Chown) 2.2 $(ParagonExportsArea)/usr/lbin/$$i ; \
	      chmod 0550 $(ParagonExportsArea)/usr/lbin/$$i ; \
	   fi ; \
	done
#
	@for i in uucico uustat uusched uuxqt ; do \
	   echo "[usr/lbin/$$i]" ; \
	   if [ ! -f $(ParagonExportsArea)/usr/lbin/$$i ]; then \
	      cp $$i $(ParagonExportsArea)/usr/lbin/$$i ; \
	      $(Chown) 2.2 $(ParagonExportsArea)/usr/lbin/$$i ; \
	      chmod 4755 $(ParagonExportsArea)/usr/lbin/$$i ; \
	   fi ; \
	done
#
	@echo "[usr/sbin/uucpd]"
	@if [ ! -f $(ParagonExportsArea)/usr/sbin/uucpd ]; then \
	   cp uucpd $(ParagonExportsArea)/usr/sbin/uucpd ; \
	   $(Chown) 2.2 $(ParagonExportsArea)/usr/sbin/uucpd ; \
	   chmod 0755 $(ParagonExportsArea)/usr/sbin/uucpd ; \
	fi
#
	@for i in Systems Permissions ; do \
	   echo "[etc/uucp/$$i]" ; \
	   if [ ! -f $(ParagonExportsArea)/etc/uucp/$$i ]; then \
	      cp $$i $(ParagonExportsArea)/etc/uucp/$$i ; \
	      $(Chown) 2.2 $(ParagonExportsArea)/etc/uucp/$$i ; \
	      chmod 0440 $(ParagonExportsArea)/etc/uucp/$$i ; \
	   fi ; \
	done
#
	@for i in Devices Dialcodes Dialers Poll ; do \
	   echo "[etc/uucp/$$i]" ; \
	   if [ ! -f $(ParagonExportsArea)/etc/uucp/$$i ]; then \
	      cp $$i $(ParagonExportsArea)/etc/uucp/$$i ; \
	      $(Chown) 2.2 $(ParagonExportsArea)/etc/uucp/$$i ; \
	      chmod 0755 $(ParagonExportsArea)/etc/uucp/$$i ; \
	   fi ; \
	done
#
	@for i in uupick uulog Maxuuscheds Maxuuxqts remote.unknown ; do \
	   echo "[usr/bin/$$i]" ; \
	   if [ ! -f $(ParagonExportsArea)/usr/bin/$$i ]; then \
	      cp $$i $(ParagonExportsArea)/usr/bin/$$i ; \
	      $(Chown) 2.2 $(ParagonExportsArea)/usr/bin/$$i ; \
	      chmod 0755 $(ParagonExportsArea)/usr/bin/$$i ; \
	   fi ; \
	done
#
	@for i in uudemon.admin uudemon.cleanu uudemon.hour uudemon.poll ; do \
	   echo "[usr/lbin/$$i]" ; \
	   if [ ! -f $(ParagonExportsArea)/usr/lbin/$$i ]; then \
	      cp $$i $(ParagonExportsArea)/usr/lbin/$$i ; \
	      $(Chown) 2.2 $(ParagonExportsArea)/usr/lbin/$$i ; \
	      chmod 0755 $(ParagonExportsArea)/usr/lbin/$$i ; \
	   fi ; \
	done
#
	@for i in ftpd rexecd ; do \
	   echo "[usr/sbin/$$i]" ; \
	   if [ ! -f $(ParagonExportsArea)/usr/sbin/$$i ]; then \
	      cp $$i $(ParagonExportsArea)/usr/sbin/$$i ; \
	      $(Chown) 0.0 $(ParagonExportsArea)/usr/sbin/$$i ; \
	      chmod 0755 $(ParagonExportsArea)/usr/sbin/$$i ; \
	   fi ; \
	done
