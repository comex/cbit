import mconfig
settings = mconfig.settings_root
settings.package_unix_name.value = 'test'

mconfig.parse_args()

emitter = settings.emitter

emitter.add_command(settings, ['(out)/foo'], [], [
    'touch (out)/foo',
])

emitter.add_command(settings, ['all'], ['(out)/foo'], [], phony=True)
emitter.set_default_rule('all')

mconfig.finish_and_emit()
