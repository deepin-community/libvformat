# No automatisation found. Write me if any idea.

install-data-hook:
	c2man -P "gcc -E -C -I.." ../vformat/vf_iface.h

	$(mkinstalldirs) $(mandir)/man3

	for i in *.3; do \
	  if test -f $(srcdir)/$$i; then file=$(srcdir)/$$i; \
	  else file=$$i; fi; \
	  ext=`echo $$i | sed -e 's/^.*\\.//'`; \
	  inst=`echo $$i | sed -e 's/\\.[0-9a-z]*$$//'`; \
	  inst=`echo $$inst | sed '$(transform)'`.$$ext; \
	  echo " $(INSTALL_DATA) $$file $(mandir)/man3/$$inst"; \
	  $(INSTALL_DATA) $$file $(mandir)/man3/$$inst; \
	done
