## utils/cache_gc

cache_gc_src = files([
  'categories.c',
  'db.c',
  'kr_cache_gc.c',
  'main.c',
])
c_src_lint += cache_gc_src

if build_utils
  cache_gc = executable(
    'kres-cache-gc',
    cache_gc_src,
    dependencies: [
      contrib_dep,
      libkres_dep,
      libknot,
      luajit_inc,
    ],
    install: true,
    install_dir: get_option('sbindir'),
  )
endif
