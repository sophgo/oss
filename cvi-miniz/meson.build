project('cvi_miniz', 'c')

cvi_miniz_includes = include_directories('.')

cfg = configuration_data()
cfg.set('MINIZ_EXPORT', '')
cvi_miniz_export_h = configure_file(output: 'cvi_miniz_export.h',
                                configuration: cfg)

libcvi_miniz = static_library('cvi_miniz',
                          cvi_miniz_export_h, 'cvi_miniz.c', 'cvi_miniz_zip.c', 'cvi_miniz_tinfl.c', 'cvi_miniz_tdef.c',
                          include_directories: cvi_miniz_includes)

cvi_miniz_dependency = declare_dependency(link_with: libcvi_miniz,
                                      include_directories: cvi_miniz_includes)
                                      
cvi_miniz_dep = cvi_miniz_dependency # Compatibility for WrapDB users

if meson.version().version_compare('>= 0.54.0')
    meson.override_dependency('cvi_miniz', cvi_miniz_dep)
endif
