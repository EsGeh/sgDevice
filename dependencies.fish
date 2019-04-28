#!/bin/fish

########################################
# dependencies of this repository
########################################

set dependencies \
	fishshell_cmd_opts \
	ttymidi \
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
set structuredData \
	structuredData \
	https://github.com/EsGeh/structuredData.git \
	32d2347677589ca062679f4af31dfdb09eb9f0ac \
	"scripts/init.fish --deps-dir \$DEP_DIR"
