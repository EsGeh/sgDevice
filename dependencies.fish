#!/bin/fish

########################################
# dependencies of this repository
########################################

set dependencies \
	fishshell_cmd_opts \
	ttymidi \
	usbmidiklik \
	pd_lib_builder \
	structuredData \
	zexy

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
	5e651a7f983da5c316862e2b3869a73cc0e75cc3

set pd_lib_builder \
	pd-lib-builder \
	https://github.com/pure-data/pd-lib-builder.git \
	77525265694bac50ed94c5ef62ebbae680c72ab0

set structuredData \
	structuredData \
	https://github.com/EsGeh/structuredData.git \
	3ab006533caf9f723fae844272f9bcf6ad4b07fa \
	"scripts/init.fish --deps-dir \$DEP_DIR"

set zexy \
	zexy \
	https://git.iem.at/pd/zexy \
	b078a0ed5f32444b65d2fd6235127d72e4129646
