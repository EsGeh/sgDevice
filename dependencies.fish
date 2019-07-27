#!/bin/fish

########################################
# dependencies of this repository
########################################

set dependencies \
	fishshell_cmd_opts \
	ttymidi \
	usbmidiklik \
	structuredData

# for every entry in $dependencies:
# 	name uri version [init-command]
set fishshell_cmd_opts \
	fishshell-cmd-opts  \
	https://github.com/EsGeh/fishshell-cmd-opts.git \
	e446daf1741b416ecb83e4741b4cb7f99645bc11

set ttymidi \
	ttymidi \
	https://github.com/cjbarnes18/ttymidi.git \
	ee6749f39ef2bf2081f1c1cad5cf80b29a8ab046 \
	"make"

set usbmidiklik \
	USBMidiKliK \
	https://github.com/EsGeh/USBMidiKliK.git \
	5e651a7f983da5c316862e2b3869a73cc0e75cc3 \
	"scripts/init.fish; and make mega"

set structuredData \
	structuredData \
	https://github.com/EsGeh/structuredData.git \
	435a3829241d091b5350201e796502ab1e426fac \
	"scripts/init.fish --deps-dir \$DEP_DIR"
